#pragma once

#include <stdint.h>
#include <wayland-server.h>

#include <kwl/interfaces/kwl_buffer.h>


typedef struct _kwl_output_mode_s {
	int32_t width;
	int32_t height;
	int32_t refresh;
	enum wl_output_mode flags;
} kwl_output_mode_t;

typedef struct _kwl_output_mode_list_s {
	kwl_output_mode_t mode;

	struct wl_list link;
} kwl_output_mode_list_t;

typedef struct _kwl_output_s {
	/*Version 1.0 Details*/
	char *make;
	char *model;
	
	int32_t x, y;
	int32_t phy_width, phy_height;

	enum wl_output_subpixel subpixel;
	enum wl_output_transform transform;

	kwl_output_mode_t current_mode;

	/*List of modes*/
	kwl_output_mode_list_t modes;

	/*Version 2.0 Details*/
	int32_t scale;

	/*Version 4.0 Details*/
	char *name;
	char *description;


	struct wl_global *global;

	kwl_buffer_t *buffer;

	struct {
		struct wl_signal frame; /*Indicates you should draw a frame*/
	} events;

	struct {
		
	} entry_points;
} kwl_output_t;
