#include "kwl/logger/log.h"
#include "kwl_private/allocator/allocator.h"
#include <kwl/backend/backend.h>
#include <kwl/renderer/renderer.h>
#include <kwl_private/interfaces/kwl_buffer.h>
#include <kwl/interfaces/kwl_output.h>

#include <stdint.h>
#include <stdlib.h>

typedef struct _kwl_sw_renderer_s {
	kwl_renderer_t impl;

	kwl_allocator_t *allocator;
	kwl_buffer_t *bound_buffer; /*The buffer we currently are rendering to*/
} kwl_sw_renderer_t;

void kwl_sw_renderer_clear_screen(kwl_renderer_t *renderer, float r, float g, float b) {
	kwl_sw_renderer_t *sw = (void*)renderer;
	uint32_t *data;
	uint32_t height, width;
	uint32_t color;
	

	kwl_debug("Bound buffer: %p\n", sw->bound_buffer);

	color = ((uint32_t)(0xff * r) << 16) | ((uint32_t)(0xff * g) << 8) | (uint32_t)(0xff * b);
	
	height = sw->bound_buffer->entry_points.get_height(sw->bound_buffer);
	width = sw->bound_buffer->entry_points.get_width(sw->bound_buffer);
	data = sw->bound_buffer->entry_points.get_data_ptr(sw->bound_buffer);

	for(uint32_t x = 0; x < width; x++) {
		for(uint32_t y = 0; y < height; y++) {
			data[y * width + x] = color;
		}
	}
}

void kwl_sw_renderer_output_deinit(kwl_renderer_t *renderer, kwl_output_t *output) {
	output->buffer->entry_points.destroy(output->buffer);
}

void kwl_sw_renderer_output_init(kwl_renderer_t *renderer, kwl_output_t *output) {
	kwl_sw_renderer_t *sw = (void *)renderer;
	kwl_output_mode_t *mode = &output->current_mode;

	output->buffer = sw->allocator->create_buffer(mode->height, mode->width, 0);
}

void kwl_sw_renderer_bind_buffer(kwl_renderer_t *renderer, kwl_buffer_t *buffer) {
	kwl_sw_renderer_t *sw = (void*)renderer;

	sw->bound_buffer = buffer;
}

void kwl_sw_renderer_end(kwl_renderer_t *renderer) {

}

void kwl_sw_renderer_begin(kwl_renderer_t *renderer) {

}

kwl_renderer_implementation_t renderer_impl = {
	.clear_screen = kwl_sw_renderer_clear_screen,
	.bind_buffer = kwl_sw_renderer_bind_buffer,
	.output_init = kwl_sw_renderer_output_init,
	.output_deinit = kwl_sw_renderer_output_deinit,
	.end = kwl_sw_renderer_end,
	.begin = kwl_sw_renderer_begin,
};


kwl_renderer_t *kwl_sw_renderer_init(kwl_backend_t *backend) {
	kwl_sw_renderer_t *sw = calloc(1, sizeof(kwl_sw_renderer_t));
	
	sw->impl.renderer = &renderer_impl;

	sw->allocator = kwl_allocator_init();

	return (void*)sw;
}
