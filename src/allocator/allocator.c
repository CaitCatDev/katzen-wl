#include <kwl_private/allocator/shm.h>
#include <kwl_private/allocator/allocator.h>

kwl_allocator_t *kwl_allocator_init() {
	return kwl_shm_allocator_create();
}
