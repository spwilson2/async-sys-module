#include <linux/rbtree.h>
#include <linux/file.h>
#include <linux/mutex.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/pid.h>
#include <linux/stddef.h>

#include "buffer.h"
#include "common.h"

// TODO: Create global map for id->void* of allocated buffers.
struct {
	rwlock_t lock;

	struct rb_root _root;
	buffer_id_t _uid_counter;
} map_wrapper = {
	RW_DEP_MAP_INIT(map_wrapper.lock)
	._root = {NULL},
	._uid_counter = 0
};

static inline buffer_id_t gen_next_map_id(void)
{
	/* FIXME: Make atomic. */
	return map_wrapper._uid_counter++;
}

struct map_key {
	/* Both make up the key, <pid, buffer_uid> -> buffer. */
	pid_t pid;
	buffer_id_t buffer_uid;
};

struct map_entry {
	struct rb_node node;
	struct map_key key;
	struct buffer_slab buffer;
};

/* A linked list of pointers to map enteries. */
struct map_entry_list {
	struct map_entry *entry;
	struct list_head list;
};

static inline int
map_key_cmp(struct map_key *l_key, struct map_key *r_key)
{
	if (l_key->pid < r_key->pid)
		return -1;
	else if (l_key->pid > r_key->pid)
		return 1;
	else {
		if (l_key->buffer_uid < r_key->buffer_uid)
			return -1;
		else if (l_key->buffer_uid > r_key->buffer_uid)
			return 1;
		else
			return 0;
	}
};

/*
 * See linux/Documentation/rbtree.txt for original source.
 */
static struct map_entry *
map_search(struct rb_root *root, struct map_key *key)
{
	struct rb_node *node = root->rb_node;

	/* Traverse the tree looking for our key */
	while (node) {
		struct map_entry *data = container_of(node, struct map_entry, node);
		int result = map_key_cmp(key, &data->key);
		if (result < 0)
			node = node->rb_left;
		else if (result > 0)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

/**
 * map_search_pid() - Search the map for all enteries with pid
 *
 * Return:		A linked list of them of all entries with a given pid.
 */
//static struct map_entry_list *
//map_search_pid(struct rb_root *root, pid_t pid)
//{
//	// TODO: This will be needed for deletion of an entire file's enteries.
//	return NULL;
//}

/*
 * See linux/Documentation/rbtree.txt for original source.
 */
static int
map_insert(struct rb_root *root, struct map_entry *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	/* Figure out where to put new node */
	while (*new) {
		struct map_entry *this = container_of(*new, struct map_entry, node);
		int result = map_key_cmp(&data->key, &this->key);

		parent = *new;
		if (result < 0)
			new = &((*new)->rb_left);
		else if (result > 0 )
			new = &((*new)->rb_right);
		else
			return false;
	}

	/* Add new node and rebalance tree. */
	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return true;
}

/**
 * alloc_buffer() - Allocate a buffer for a given file.
 * @size		Size of the buffer in bytes to allocate.
 * @file		The file context which we are going to
 *			attach the given buffer into.
 *
 * In order to maintain state information for processes the
 * file will use the file.private_data pointer to hold a unique
 * map key id for each buffer it holds. The format for this
 * list of map key ids for a file will be a singly linked list
 * to allow quick removal and additions.
 *
 */
int
alloc_buffer(size_t size, struct file *file, struct buffer_slab **buffer)
{
	/*
	 * 1. Grab reader lock on file owner
	 * 1. malloc size of size for the process that owns file (how do I do
	 * this so user space has access?)
	 * 1. Insert pointer to this space into alloc_map
	 * 1. Grab writer lock on the linked list for allocated ids of file
	 * 1. Insert alloc_map_key into the file->private_data queue of
	 * allocated ids
	 * 1. Unlock writer for file linked list
	 * 1. Unlcok reader lock on file owner
	 */

	struct map_entry *entry;

	/* Allocate space for the map entry*/
	entry = kmalloc(sizeof(struct map_entry), GFP_KERNEL);
	if (!entry) {
		// TODO: Need to try a vmalloc if unable to succeed.
		return false; // Failed to alloc.
	}


	/*
	 * TODO: Need to set up the address space boundaries for the correct
	 * process.
	 */
	/* Allocate space for our shared ring buffer. */
	entry->buffer.buffer = kmalloc(size, GFP_USER);
	if (!entry->buffer.buffer) {
		// TODO: Need to try a vmalloc if unable to succeed.
		return false; // Failed to alloc.
	}

	// Initilize the lock on the new entry's buffer and grab the lock.
	spin_lock_init(&entry->buffer.spinlock);
	spin_lock(&entry->buffer.spinlock);

	/* Grab the read lock on the file so we can find the pid. */
	read_lock(&file->f_owner.lock);
	entry->key = (struct map_key){.buffer_uid = gen_next_map_id(), .pid = pid_nr(file->f_owner.pid)};
	read_unlock(&file->f_owner.lock);

	write_lock(&map_wrapper.lock);
	if (!map_insert(&map_wrapper._root, entry)) {
		// There was a duplicate....?
		write_unlock(&map_wrapper.lock);

		kfree(entry->buffer.buffer);
		kfree(entry);
		return false;
	}
	/*
	 * We are holding the spinlock on the buffer so it cannnot be freed.
	 * TODO: At some point could move to a mutex, although I think it would
	 * be rare that we want to delete a buffer as we are also allocating
	 * one.
	 */
	*buffer = &entry->buffer;
	write_unlock(&map_wrapper.lock);

	return true;
}

/**
 * free_buffer() - Free the buffer of given id
 */
void
free_buffer(buffer_id_t id, struct file *file)
{
	struct map_entry entry;
	struct map_entry *match;

	read_lock(&file->f_owner.lock);
	entry.key = (struct map_key){.buffer_uid = id, .pid = pid_nr(file->f_owner.pid)};
	read_unlock(&file->f_owner.lock);

	write_lock(&map_wrapper.lock);
	if (!(match = map_search(&map_wrapper._root, &entry.key))) {
		// No match found
		write_unlock(&map_wrapper.lock);
		mprintk("Called free_buffer for id '%lu' but no match found.", id);
	}
	rb_erase(&match->node, &map_wrapper._root);

	write_unlock(&map_wrapper.lock);
}
