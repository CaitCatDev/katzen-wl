#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include <stdio.h>
#include <stdint.h>
#include <string.h>

void wl_output_name(void *data, struct wl_output *output, const char *name) {
	fprintf(stderr, "Monitor Name: %s\n", name);
}

void wl_output_description(void *data, struct wl_output *output, const char *desc) {
	fprintf(stderr, "Monitor Description: %s\n", desc);
}

void wl_output_done(void *data, struct wl_output *output) {
	fprintf(stderr, "Monitor Done\n");
}

void wl_output_scale(void *data, struct wl_output *output, int32_t scale) {
	fprintf(stderr, "Monitor Scale: %d\n", scale);
}

void wl_output_mode(void *data, struct wl_output *output, uint32_t flags,
		int32_t width, int32_t height, int32_t refresh) {
	fprintf(stderr, "Monitor mode:\n\tResolution: %dx%d\n"
			"\tRefresh: %d, Flags: %d\n", width, height, refresh, flags);
}

void wl_output_geometry(void *data, struct wl_output *output, int32_t x,
		int32_t y, int32_t pwidth, int32_t pheight, int32_t subpixel, 
		const char *make, const char *model, int32_t transform) {
	fprintf(stderr, "Monitor geometry:\n"
			"\tMake: %s\n" "\tModel: %s\n" "\tPhysical dimensions: %dx%d\n"
			"\tX & Y POS: %dx%d\n" "\tSubpixel: %d\n" "\tTransform: %d\n", 
			make, model, pwidth, pheight, x, y, subpixel, transform);
}



static const struct wl_output_listener output_listener = {
	.mode = wl_output_mode,
	.scale = wl_output_scale,
	.description = wl_output_description,
	.name = wl_output_name,
	.geometry = wl_output_geometry,
	.done = wl_output_done,
};

void wl_registry_global(void *data, struct wl_registry *registry,
		uint32_t name, const char *interface, uint32_t version) {
	if(strcmp(interface, wl_output_interface.name) == 0) {
		 data = wl_registry_bind(registry, name, &wl_output_interface, version);
		 wl_output_add_listener(data, &output_listener, data);
	}
}

void wl_registry_global_remove(void *data, struct wl_registry *registry,
		uint32_t name) {

}

static const struct wl_registry_listener registry_listener = {
	.global = wl_registry_global,
	.global_remove = wl_registry_global_remove,
};

int main(int argc, char **argv) {
	struct wl_display *display;
	struct wl_registry *registry;
	struct wl_output *output;

	display = wl_display_connect(NULL);
	if(!display) {
		fprintf(stderr, "Failed to connect to wayland display\n");
		return 1;
	}

	registry = wl_display_get_registry(display);
	if(!registry) {
		fprintf(stderr, "Failed to get display registry\n");
		return 1;
	}

	wl_registry_add_listener(registry, &registry_listener, &output);
	
	wl_display_roundtrip(display);

	while(wl_display_dispatch(display)) {

	}

	return 0;
}
