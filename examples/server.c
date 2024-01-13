#include <wayland-server.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include <kwl/logger/log.h>
#include <kwl/backend/backend.h>

int main(int argc, char **argv) {
	struct wl_display *display;

	display = wl_display_create();

	wl_display_add_socket_auto(display);

	kwl_backend_t *backend = kwl_backend_init_env(display);
	if(backend == NULL) {
		return 1;
	}

	backend->entry_points.start(backend);
	
	wl_display_run(display);
	return 0;
}
