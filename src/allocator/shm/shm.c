#define _XOPEN_SOURCE 600
#include <kwl_private/allocator/allocator.h>
#include <kwl_private/allocator/shm.h>
#include <kwl_private/interfaces/kwl_buffer.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>
#include <sys/mman.h>


typedef struct _kwl_shm_buffer_s {
	kwl_buffer_t kwl_buffer;
	uint32_t height, width, format;
	size_t size, pitch;

	int fd; /*FD associated with this buffer*/
	void *data;
} kwl_shm_buffer_t;

typedef struct _kwl_shm_allocator_s {
	kwl_allocator_t allocator;
} kwl_shm_allocator_t;

uint32_t kwl_shm_buffer_get_height(kwl_buffer_t *buffer) {
	kwl_shm_buffer_t *shm_buffer = (void*)buffer;

	return shm_buffer->height;
}

uint32_t kwl_shm_buffer_get_width(kwl_buffer_t *buffer) {
	kwl_shm_buffer_t *shm_buffer = (void*)buffer;

	return shm_buffer->width;
}


void *kwl_shm_buffer_get_data(kwl_buffer_t *buffer) {
	kwl_shm_buffer_t *shm_buffer = (void*)buffer;

	return shm_buffer->data;
}

void kwl_shm_buffer_destroy(kwl_buffer_t *buffer) {
	kwl_shm_buffer_t *shm_buffer = (void *)buffer;
	
	munmap(shm_buffer->data, shm_buffer->size);
	close(shm_buffer->fd);
	free(shm_buffer);
}

static const kwl_buffer_entry_points_t kwl_buffer_entry_points = {
	.destroy = kwl_shm_buffer_destroy,
	.get_data_ptr = kwl_shm_buffer_get_data,
	.get_width = kwl_shm_buffer_get_width,
	.get_height = kwl_shm_buffer_get_height,
};

kwl_buffer_t *kwl_shm_allocator_create_buffer(uint32_t height, uint32_t width, uint32_t format) {
	kwl_shm_buffer_t *shm_buffer;
	char template[] = "/tmp/kwl_buffer-XXXXXX";
	/*TODO: Format*/
	shm_buffer = calloc(1, sizeof(kwl_shm_buffer_t));


	shm_buffer->fd = mkstemp(template);
	
	/*Unlink the template name from the fd*/
	unlink(template);

	/*Assume 32bbp format*/
	shm_buffer->size = height * width * 4;
	
	ftruncate(shm_buffer->fd, shm_buffer->size);

	shm_buffer->width = width;
	shm_buffer->height = height;
	shm_buffer->format = format;
	shm_buffer->pitch = width * 4;
	shm_buffer->data = mmap(NULL, shm_buffer->size, PROT_READ | PROT_WRITE,
			MAP_SHARED, shm_buffer->fd, 0);
	
	shm_buffer->kwl_buffer.entry_points = kwl_buffer_entry_points;
	shm_buffer->kwl_buffer.caps = KWL_BUFFER_SHM_BUFFER | KWL_BUFFER_DATA_PTR_CAP;;

	return (void *)shm_buffer;
}

kwl_allocator_t *kwl_shm_allocator_create() {
	kwl_shm_allocator_t *shm;

	shm = calloc(1, sizeof(kwl_shm_allocator_t));
	shm->allocator.create_buffer = kwl_shm_allocator_create_buffer;

	return (void*)shm;
}
