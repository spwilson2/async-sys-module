# Design Doc

## User space API

### Structs

```c
/* Struct which is used to request a particular system call from the user library. */
struct async_cb {
    long number; /* The syscall number. */
    void * vargs[]; /* NULL terminated list of arguments to the syscall of given number. */
};

/* Used to store information about results. */
struct async_event {
    struct async_cb* cbp; /* Pointer to the async_cb where event came from. */
    __s64 res; /* Result of syscall. */
};
```

### Request Async Call Ring

```c
int async_setup(unsigned long nr_events, async_context_t *ctx_idp)
```
Returns: -1 and sets errno if fail else returns NULL.

### Dealloc Async Call Ring Manually

```c
int async_destroy(async_context_t ctx)
```
Returns: -1 and sets errno if fail else returns NULL.

### Get Events (Just a user space check)

```c
int async_check_events(async_context_t ctx, long min_nr, long max_nr, struct async_event events[], 
struct timespec *timeout)
```
Returns the number of events which were handled.

### Add Syscalls (Just a user space placement)

`async_cbps` should be an array of `async_cb` pointers filled with syscall information.
```c
int async_submit(async_context_t ctx, long nr, struct async_cb * async_cbps[])
```
Returns: -1 and sets errno if fail else returns NULL.

### Force block in kernel to get queue updates

```c
int async_getevents(async_context_t ctx, long min_nr, long max_nr, struct
async_event events[], struct timespec *timeout)
```
Returns the number of events which were handled.

## Shared Memory Ring Layout

The shared memory ring should be implemented as a thread safe, multi-producer,
multi-consumer, queue. In order for this to remain safe for both the kernel and
user, this queue will NOT be lock-free (if it were it would be even more
complicated [possibly impossible] for the kernel to remain safe from a user). 

This queue will contain elements with system calls listed to be executed
asynchronously. Elements on this queue will be of max syscall size. (I.E. even
`write(int,void*)` will take up a slot to hold six args that are available for
use on linux.) The alternative would be to determine the next space in a queue
for a system call after inserting the last one and this would difficult the
queue implementation further.


## ioctl API (go between user library and kernel)

### Create a shared memory ring with the kernel to be able to submit async requests

```c
int async_setup(unsigned long nr_events, async_context_t *ctx_idp)
```
Returns: -1 and sets errno if fail else returns NULL.

### Block in the kernel for a set number of events or a timeout.

```c
int async_getevents(async_context_t ctx, long min_nr, long max_nr, struct async_event events[], 
struct timespec *timeout)
```
Returns: the number of events which were handled.

### Destroy the async ring manually

```c
int async_destroy(async_context_t ctx)
```
Returns: -1 and sets errno if fail else returns NULL.

# Thinking space...

Additional System calls:
- Request Space to queue up system calls
- Block to force kernel to handle queue
TODO: Might want info receiving system calls?

Kernel Modifications:
- Create System Calls
- Create kernel thread routine for reading from buffer
 

------------

Q: How do we improve concurrency in the shared memory? We would just block on
a single item in the queue in the kernel. We could instead use a less circular
layout and be much more concurrent?

RE: In order to make the queue more time efficient, we won't use only a single
queue for requests and return values. Instead we will keep a single queue with
syscall requests, return values will be sent to user space to locations
allocated by the user when they submitted a request. 


# Other Design Notes

NOTE: Rather than trying to intercept system calls (sounds like bad practice)
or implement a system call (not portable since is built into the kernel), we'll
be creating our own 'device' file and communicate with our kernel module
through the ioctl call. (Likely will be wrapped by a user library in order to
keep the call structured [it's vararg as defined by header].)

TODO: Might be worth switching from a `struct` which contains a lot of extra
garbage for the `async_submit` to a `va_args` type function.

See [](http://www.makelinux.net/ldd3/) for help.

# Authors & Acknowledgements (Some omitted until more progress is made.)

Sean Wilson <spwilson2@wisc.edu>

This project follows up on [previous work](https://github.com/spwilson2/cs758-project) to improve filesystem performance in
Go.

This project is also heavily inspired by
[FlexSC](http://www.cs.cmu.edu/~chensm/Big_Data_reading_group/papers/flexsc-osdi10.pdf)
written by Livio Soares & Michael Stumm.
