#include <stdint.h>
#include <stdlib.h>
#include <wayland-server.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#include <kwl/logger/log.h>
#include <kwl/backend/backend.h>
#include <signal.h>

#include <kwl/interfaces/kwl_output.h>
#include <wayland-util.h>

int server_sigint(int signal_number, void *data) {
	kwl_debug("Sigint\n");
	wl_display_terminate(data);	
	return 0;
}

/*Lets just pretend for now this was in some renderer.c file*/
#include <kwl_private/interfaces/kwl_buffer.h>
void clear_screen(kwl_output_t *output, uint32_t rgb) {
	uint32_t *data = output->buffer->entry_points.get_data_ptr(output->buffer);
	
	for(uint32_t x = 0; x < output->current_mode.width; x++) {
		for(uint32_t y = 0; y < output->current_mode.height; y++) {
			data[y * output->current_mode.width + x] = rgb;
		}
	}
}


/*end*/

struct our_output {
	kwl_output_t *kwl_output;
	struct wl_listener frame_listener;
	
	struct wl_list link;
};

struct our_server {
	struct wl_display *display;
	struct wl_event_loop *loop;
	struct wl_event_source *signal;
	struct wl_listener output_listener;

	kwl_backend_t *backend;

	struct wl_list outputs;
};

void output_frame(struct wl_listener *listener, void *data) {
	struct our_output *our_output = wl_container_of(listener, our_output, frame_listener);
	kwl_output_t *output = data;

	clear_screen(output, 0x282a36);
}

void new_output(struct wl_listener *listener, void *data) {
	kwl_output_t *output;
	
	struct our_output *our_output = calloc(1, sizeof(*our_output));

	output = data;


	our_output->kwl_output = data;
	our_output->frame_listener.notify = output_frame;

	wl_signal_add(&output->events.frame, &our_output->frame_listener);
	kwl_debug("kwl ouput %s added\n", output->name);
}


int main(int argc, char **argv) {
	struct our_server *server = calloc(1, sizeof(*server));
	const char *socket_name;

	kwl_log_set_level(KWL_LOG_DEBUG);

	server->display = wl_display_create();

	socket_name = wl_display_add_socket_auto(server->display);
	kwl_debug("%s\n", socket_name);

	server->backend = kwl_backend_init_env(server->display);
	if(server->backend == NULL) {
		return 1;
	}

	server->loop = wl_display_get_event_loop(server->display);
	server->signal = wl_event_loop_add_signal(server->loop, SIGINT, server_sigint, server->display);

	server->output_listener.notify = new_output;
	wl_signal_add(&server->backend->events.new_output, &server->output_listener);
	server->backend->entry_points.start(server->backend);
	
	wl_display_run(server->display);

	server->backend->entry_points.deinit(server->backend);

	wl_event_source_remove(server->signal);
	wl_display_destroy(server->display);
	

	return 0;
}
