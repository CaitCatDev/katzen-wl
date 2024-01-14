#pragma once

#include <stdint.h>

#define KWL_BUFFER_DATA_PTR_CAP 0x1 /*It's possible to get data ptr*/
#define KWL_BUFFER_SHM_BUFFER 0x2 /*Buffer can have shm fd*/

typedef struct _kwl_buffer_s kwl_buffer_t;

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
