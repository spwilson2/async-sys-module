#ifndef __MODULE_SRC_BUFFER_H
#define __MODULE_SRC_BUFFER_H

/* Allocate a buffer for a given file. */
buffer_id_t alloc_buffer(size_t size, struct *file, void *ret);

/* Return a pointer to the buffer of given id. */
void* get_buffer(buffer_id_t id);

/* Free the buffer for the given id. */
void free_buffer(buffer_id_t id, struct *file);

#endif
