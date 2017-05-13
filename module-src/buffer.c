#include "buffer.h"
#include <datastructures/queue.h>

// TODO: Create global map for id->void* of allocated buffers.

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
 * Return: buffer_id_t of allocated buffer if success, INVALID_BUFFER_ID if
 * fail
 */
buffer_id_t alloc_buffer(size_t size, struct *file) {
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
}

/**
 * free_buffer() - Free the buffer of given id
 */
void free_buffer(buffer_id_t id, struct *file) {
}

