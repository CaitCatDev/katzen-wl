#pragma once

#include <stdint.h>
#include <kwl/interfaces/kwl_buffer.h>

typedef void *(*pfn_kwl_buffer_get_data_ptr)(kwl_buffer_t *buffer);
typedef void (*pfn_kwl_buffer_destroy)(kwl_buffer_t *buffer);

typedef struct {
	pfn_kwl_buffer_destroy destroy;
	pfn_kwl_buffer_get_data_ptr get_data_ptr;
} kwl_buffer_entry_points_t;

struct _kwl_buffer_s {
	uint32_t caps;

	kwl_buffer_entry_points_t entry_points;
};
