#include <wayland-server.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include <kwl/logger/log.h>
#include <kwl/backend/backend.h>
#include <signal.h>

int server_sigint(int signal_number, void *data) {
	kwl_debug("Sigint\n");
	wl_display_terminate(data);	
	return 0;
}

int main(int argc, char **argv) {
	struct wl_display *display;
	struct wl_event_loop *loop;
	struct wl_event_source *source;
	const char *socket_name;

	kwl_log_set_level(KWL_LOG_DEBUG);

	display = wl_display_create();

	socket_name = wl_display_add_socket_auto(display);
	kwl_debug("%s\n", socket_name);

	kwl_backend_t *backend = kwl_backend_init_env(display);
	if(backend == NULL) {
		return 1;
	}

	loop = wl_display_get_event_loop(display);
	source = wl_event_loop_add_signal(loop, SIGINT, server_sigint, display);

	backend->entry_points.start(backend);
	
	wl_display_run(display);

	backend->entry_points.deinit(backend);

	wl_event_source_remove(source);
	wl_display_destroy(display);
	

	return 0;
}
