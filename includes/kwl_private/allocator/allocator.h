#pragma once

#include <kwl_private/interfaces/kwl_buffer.h>
#include <stdint.h>

typedef struct _kwl_allocator_s kwl_allocator_t;

typedef kwl_buffer_t *(*pfn_kwl_allocator_buffer_create)(uint32_t height, uint32_t widht, uint32_t format);

struct _kwl_allocator_s {
	
	pfn_kwl_allocator_buffer_create create_buffer;
};


kwl_allocator_t *kwl_allocator_init();
