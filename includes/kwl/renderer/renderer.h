#pragma once

#include "kwl/backend/backend.h"
#include <kwl/interfaces/kwl_output.h>
#include <kwl/interfaces/kwl_buffer.h>
#include <stdint.h>

typedef struct _kwl_renderer_s kwl_renderer_t;

typedef void (*pfn_kwl_renderer_clear_screen)(kwl_renderer_t *renderer, float r, float g, float b);
typedef void (*pfn_kwl_renderer_begin)(kwl_renderer_t *renderer);
typedef void (*pfn_kwl_renderer_end)(kwl_renderer_t *renderer);
typedef void (*pfn_kwl_renderer_output_init)(kwl_renderer_t *renderer, kwl_output_t *output);
typedef void (*pfn_kwl_renderer_bind_buffer)(kwl_renderer_t *renderer, kwl_buffer_t *buffer);
typedef void (*pfn_kwl_renderer_output_deinit)(kwl_renderer_t *renderer, kwl_output_t *output);

typedef struct _kwl_renderer_implementation_s {
	pfn_kwl_renderer_end end;
	pfn_kwl_renderer_begin begin;
	pfn_kwl_renderer_bind_buffer bind_buffer;
	pfn_kwl_renderer_output_init output_init;
	pfn_kwl_renderer_output_deinit output_deinit;
	pfn_kwl_renderer_clear_screen clear_screen;
} kwl_renderer_implementation_t;

struct _kwl_renderer_s {
	
	kwl_renderer_implementation_t *renderer;
};

kwl_renderer_t *kwl_renderer_init(kwl_backend_t *backend);
