#pragma once

#include <stdint.h>
#include <kwl/interfaces/kwl_buffer.h>

typedef void *(*pfn_kwl_buffer_get_data_ptr)(kwl_buffer_t *buffer);
typedef void (*pfn_kwl_buffer_destroy)(kwl_buffer_t *buffer);
typedef uint32_t (*pfn_kwl_get_height)(kwl_buffer_t *buffer);
typedef uint32_t (*pfn_kwl_get_width)(kwl_buffer_t *buffer);


typedef struct {
	pfn_kwl_buffer_destroy destroy;
	pfn_kwl_buffer_get_data_ptr get_data_ptr;
	pfn_kwl_get_height get_height;
	pfn_kwl_get_width get_width;
} kwl_buffer_entry_points_t;

struct _kwl_buffer_s {
	uint32_t caps;

	kwl_buffer_entry_points_t entry_points;
};
