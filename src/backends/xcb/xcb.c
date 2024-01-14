#include <stdint.h>
#include <stdio.h>
#include <wayland-util.h>
#include <xcb/xcb.h>
#include <xcb/dri3.h>
#include <xcb/shm.h>

#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <wayland-server.h>
#include <wayland-server-protocol.h>
#include <wayland-server-core.h>

#include <kwl/backend/backend.h>
#include <kwl/interfaces/kwl_output.h>
#include <kwl/logger/log.h>
#include <xcb/xproto.h>

/*TODO: move this into renderer code*/
#include <kwl_private/allocator/allocator.h>
#include <kwl_private/interfaces/kwl_buffer.h>

typedef struct {
	kwl_output_t generic; /**Generic output*/

	xcb_window_t window; /**window associated with this window*/
	xcb_gcontext_t gc; /**Graphics context*/
	xcb_connection_t *connection; /**Connection associated with output*/

	struct wl_list link;
} kwl_xcb_output_t;

typedef struct {
	kwl_backend_t impl;
	
	struct wl_display *display;
	struct wl_event_source *x_event;

	xcb_connection_t *connection;
	xcb_screen_t *screen;
	
	kwl_allocator_t *allocator;

	struct wl_list outputs;
} kwl_xcb_backend_t;

void kwl_xcb_backend_deinit(kwl_backend_t *backend) {
	kwl_xcb_backend_t *xcb = (void*)backend;
	kwl_xcb_output_t *x11_output, *tmp;

	wl_event_source_remove(xcb->x_event);
	
	wl_list_for_each_safe(x11_output, tmp, &xcb->outputs, link) {
		wl_list_remove(&x11_output->link);
		free(x11_output->generic.name);
		xcb_free_gc(x11_output->connection, x11_output->gc);
		xcb_destroy_window(x11_output->connection, x11_output->window);
		free(x11_output);
	}

	free(xcb->allocator);

	xcb_disconnect(xcb->connection);
	free(xcb);
}

int kwl_xcb_backend_start(kwl_backend_t *backend) {
	kwl_xcb_backend_t *xcb = (void*)backend;
	kwl_xcb_output_t *output;
	wl_list_for_each(output, &xcb->outputs, link) {
		kwl_info("Mapping window %d\n",output->window);
		xcb_map_window(output->connection, output->window);
	

		wl_signal_emit(&xcb->impl.events.new_output, &output->generic);
	}

	xcb_flush(xcb->connection);
	return 0;
}

void kwl_xcb_output_set_mode(kwl_output_t *output, uint32_t height, uint32_t width) {
	output->current_mode.flags = WL_OUTPUT_MODE_CURRENT; /*This is current mode*/
	output->current_mode.refresh = 60;
	output->current_mode.width = width;
	output->current_mode.height = height;
}

void kwl_xcb_output_set_geomerty(kwl_output_t *output, int32_t x, int32_t y,
		int32_t phy_width, int32_t phy_height, int32_t subpixel, 
		int32_t transform, char *make, char *model) {
	output->x = x;
	output->y = y;
	output->subpixel = subpixel;
	output->transform = transform;
	output->make = make;
	output->model = model;
	output->phy_width = phy_width;
	output->phy_height = phy_height;
}

kwl_xcb_output_t *kwl_xcb_find_output_by_window(struct wl_list *list, xcb_window_t window) {
	kwl_xcb_output_t *output;

	wl_list_for_each(output, list, link) {
		if(output->window == window) return output;
	}

	return NULL;
}

void kwl_xcb_configure_notify(kwl_xcb_backend_t *xcb, xcb_configure_notify_event_t *notify) {
	kwl_xcb_output_t *x11_output;

	x11_output = kwl_xcb_find_output_by_window(&xcb->outputs, notify->window);
	kwl_xcb_output_set_mode(&x11_output->generic, notify->height, notify->width);

	x11_output->generic.buffer->entry_points.destroy(x11_output->generic.buffer);
	x11_output->generic.buffer = xcb->allocator->create_buffer(notify->height, notify->width, 0);
}

void kwl_xcb_expose(kwl_xcb_backend_t *xcb, xcb_expose_event_t *expose) {
	kwl_xcb_output_t *x11_output;

	x11_output = kwl_xcb_find_output_by_window(&xcb->outputs, expose->window);

	/*trigger frame events*/
	wl_signal_emit(&x11_output->generic.events.frame, x11_output);
	xcb_put_image(x11_output->connection, XCB_IMAGE_FORMAT_Z_PIXMAP,
			x11_output->window, x11_output->gc, x11_output->generic.current_mode.width, 
			x11_output->generic.current_mode.height, 0, 0, 0, 
			24, x11_output->generic.current_mode.height * x11_output->generic.current_mode.width * 4, 
			x11_output->generic.buffer->entry_points.get_data_ptr(x11_output->generic.buffer));

}

int kwl_xcb_backend_event(int fd, uint32_t mask, void *data) {
	kwl_xcb_backend_t *xcb = (void*)data;
	xcb_generic_event_t *ev;

	while((ev = xcb_poll_for_event(xcb->connection))) {
		switch(ev->response_type & 0x7f) {
			case XCB_EXPOSE:
				kwl_xcb_expose(xcb, (xcb_expose_event_t*)ev);
				break;
			case XCB_CONFIGURE_NOTIFY:
				kwl_xcb_configure_notify(xcb, (xcb_configure_notify_event_t*)ev);
				break;
			default:
				kwl_info("Unhandled Event: %d\n", ev->response_type);
		}
		xcb_flush(xcb->connection);
		free(ev);
	}
	return 0;
}


static const struct wl_output_interface wl_output_implementation  = {
	.release = NULL,
};

void kwl_output_bind(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	struct wl_resource *resource;
	kwl_output_t *output = data;
	kwl_info("%d %d\n", version, wl_output_interface.version);
	resource = wl_resource_create(client, &wl_output_interface,
			wl_output_interface.version, id);
	
	if(!resource) {
		wl_client_post_no_memory(client);
		return;
	} 
	wl_resource_set_implementation(resource,
			&wl_output_implementation, data, NULL);
	
	if(version >= WL_OUTPUT_GEOMETRY_SINCE_VERSION) {
		wl_output_send_geometry(resource, output->x, output->y, 
			output->phy_width, output->phy_height, output->subpixel, 
			output->make, output->model, output->transform);
	}

	if(version >= WL_OUTPUT_MODE_SINCE_VERSION) {
		wl_output_send_mode(resource, output->current_mode.flags, output->current_mode.width,
			output->current_mode.height, output->current_mode.refresh);
	}

	if(version >= WL_OUTPUT_SCALE_SINCE_VERSION) {
		wl_output_send_scale(resource, output->scale);
	}

	if(version >= WL_OUTPUT_NAME_SINCE_VERSION) {
		wl_output_send_name(resource, output->name);
	}

	if(version >= WL_OUTPUT_DESCRIPTION_SINCE_VERSION) {
		wl_output_send_description(resource, output->description);
	}
	
	if(version >= WL_OUTPUT_DONE_SINCE_VERSION) {
		wl_output_send_done(resource);
	}

}



static kwl_xcb_output_t *kwl_xcb_output_create(kwl_xcb_backend_t *xcb, uint32_t output_number) {
	kwl_xcb_output_t *x11_output = calloc(1, sizeof(kwl_xcb_output_t));
	uint32_t mask, values[2];
	xcb_void_cookie_t cookie;
	xcb_generic_error_t *error;
	kwl_output_t *kwl_output;
	int length;

	x11_output->window = xcb_generate_id(xcb->connection);
	x11_output->connection = xcb->connection;
	x11_output->gc = xcb_generate_id(xcb->connection);
	wl_signal_init(&x11_output->generic.events.frame);
	kwl_output = &x11_output->generic;
	kwl_output->scale = 1; /*scale to one*/
	kwl_output->buffer = xcb->allocator->create_buffer(600, 600, 0);

	kwl_xcb_output_set_mode(kwl_output, 600, 600);
	/*There is no physical geomerty to this output*/
	kwl_xcb_output_set_geomerty(kwl_output, 0, 0, 0, 0, 0, 0, 
			"XCB Display", "XCB Window");
	
	length = snprintf(NULL, 0, "X11-%d", output_number);

	char *name = calloc(1, length + 1);
	
	length = snprintf(name, length + 1, "X11-%d", output_number);

	kwl_output->name = name;

	kwl_output->description = "X11 Virtual Output";
	
	kwl_output->global = wl_global_create(xcb->display, &wl_output_interface, 4, kwl_output, kwl_output_bind);

	mask = XCB_CW_EVENT_MASK;
	values[0] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS |
		XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS |
		XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_BUTTON_MOTION |
		XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_POINTER_MOTION |
		XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW;

	cookie = xcb_create_window_checked(xcb->connection, xcb->screen->root_depth, 
			x11_output->window, xcb->screen->root, 0, 0, 600, 600, 1, 
			XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb->screen->root_visual,
			mask, values);
	
	error = xcb_request_check(xcb->connection, cookie);
	if(error) {
		kwl_error("Wind Error code: %d\n\tMaj: %d Min: %d\n", error->error_code,
				error->major_code, error->minor_code);
		free(error);
		return NULL;
	}

	mask = XCB_GC_FOREGROUND;
	values[0] = xcb->screen->black_pixel;

	cookie = xcb_create_gc_checked(xcb->connection, x11_output->gc, 
			x11_output->window, mask, values);
	error = xcb_request_check(xcb->connection, cookie);
	if(error) {
		kwl_error("Error code: %d\n\tMaj: %d Min: %d\n", error->error_code,
				error->major_code, error->minor_code);
		free(error);
		return NULL;
	}

	wl_list_init(&x11_output->link);
	return x11_output;
}

int isdigitstr(const char *str) {
	uint32_t i;

	for(i = 0; str[i] != '\0'; i++) {
		if(isdigit(str[i]) == 0) {
			return 0;
		}
	}
	return 1;

}

kwl_backend_t *kwl_xcb_backend_init(struct wl_display *display) {
	kwl_xcb_backend_t *xcb = calloc(1, sizeof(kwl_xcb_backend_t));
	kwl_xcb_output_t *output;
	struct wl_event_loop *loop;
	xcb_screen_iterator_t iter;
	int screenp, result, override;
	char *xcb_output_count;

	kwl_debug("XCB backend chosen\n");

	wl_list_init(&xcb->outputs);
	wl_signal_init(&xcb->impl.events.new_output);
	xcb->display = display;

	xcb->connection = xcb_connect(NULL, &screenp);
	result = xcb_connection_has_error(xcb->connection);
	if(result) {
		xcb_disconnect(xcb->connection);
		free(xcb);
		return KWL_BACKEND_FAILED;
	}

	iter = xcb_setup_roots_iterator(xcb_get_setup(xcb->connection));

	for(; iter.rem; --screenp, xcb_screen_next(&iter)) {
		if(screenp == 0) {
			xcb->screen = iter.data;
			break;
		}
	}

	override = 1;
	xcb_output_count = getenv("KWL_XCB_OUTPUTS");
	if(xcb_output_count) {
		if(isdigitstr(xcb_output_count) == 0) {
			kwl_warn("KWL_XCB_OUTPUTS: Contains non numeric value or \n"
				"\tNon base 10 value please pass in a value decimal number\n"
				"\tDefaulting to 1 Output\n");
			xcb_output_count = NULL;
		} else {
			errno = 0; /*Makesure errno is already 0*/
			override = strtoul(xcb_output_count, NULL, 10);
			if(errno) {
				kwl_warn("KWL_XCB_OUTPUTS: %s could not be converted please\n"
						"\tinput a valid decimal number.\n"
						"\tThat is less than 64bits. Defaulting to 1 output\n", xcb_output_count);
				override = 1;
			}
		}
	}

	/*TODO: MOVE INTO RENDERER*/
	xcb->allocator = kwl_allocator_init();

	/*create the speficed amount of outputs*/
	for(; override; override--) {
		output = kwl_xcb_output_create(xcb, override);
		wl_list_insert(&xcb->outputs, &output->link); /*Insert the output into list*/
	}

	loop = wl_display_get_event_loop(display);
	xcb->x_event = wl_event_loop_add_fd(loop, xcb_get_file_descriptor(xcb->connection),
			WL_EVENT_READABLE, kwl_xcb_backend_event, xcb);

	xcb->impl.entry_points.start = kwl_xcb_backend_start;
	xcb->impl.entry_points.deinit = kwl_xcb_backend_deinit;

	return (void *)xcb;
}
