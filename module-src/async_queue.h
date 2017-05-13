#ifndef __Module_SRC_ASYNC_QUEUE_H
#define __Module_SRC_ASYNC_QUEUE_H

/* Initilize the asynchronous queue with the given buffer and events size. */
void 
init_async_queue(void* buffer, unsigned long nr_events, async_context_t ctx_idp);



#endif
