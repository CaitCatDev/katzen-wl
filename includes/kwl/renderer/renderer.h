#pragma once

#include <kwl/interfaces/kwl_output.h>

typedef struct _kwl_renderer_s kwl_renderer_t;

typedef void (*pfn_kwl_renderer_clear_screen)(kwl_renderer_t *renderer, float r, float g, float b);
typedef void (*pfn_kwl_renderer_begin)(kwl_renderer_t *renderer, kwl_output_t *output);
typedef void (*pfn_kwl_renderer_end)(kwl_renderer_t *renderer);

typedef struct _kwl_renderer_implementation_s {
	pfn_kwl_renderer_end end;
	pfn_kwl_renderer_begin begin;
	pfn_kwl_renderer_clear_screen clear_screen;
} kwl_renderer_implementation_t;

struct _kwl_renderer_s {
	
};
