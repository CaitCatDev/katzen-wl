#include <kwl/backend/backend.h>
#include <kwl/backend/xcb/backend.h>

#include <stdlib.h>
#include <string.h>

#include <wayland-server.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>


kwl_backend_t *kwl_backend_init_env(struct wl_display *display) {
	if(getenv("DISPLAY")) {
		return kwl_xcb_backend_init(display);
	}

	return KWL_BACKEND_FAILED;
}

kwl_backend_t *kwl_backend_init_name(struct wl_display *display, const char *name) {
	return KWL_BACKEND_FAILED;
}
