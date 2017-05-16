It's imperative to follow the lock ordering presented here in order to avoid deadlock.

1. `file->f_owner.rwlock`
2. `(struct file_ll_head*)file->private_data`
3. `map_wrapper.lock`
4. `buffer_slab.rwlock`
