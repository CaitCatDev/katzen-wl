#pragma once

/*Std sized ints*/
#include <stdint.h>

/*Wayland Headers*/
#include <wayland-server.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>

#define KWL_BACKEND_FAILED (void*)0
#define KWL_XCB_BACKEND_NAME "xcb"
#define KWL_DRM_BACKEND_NAME "drm"

/*Foward declear type*/
typedef struct _kwl_backend_s kwl_backend_t;

/*Callback function types*/
typedef int (*pfn_kwl_backend_start)(kwl_backend_t *backend);
typedef void (*pfn_kwl_backend_deinit)(kwl_backend_t *backend);


/*Actually declear the struct*/
struct _kwl_backend_s {
	/**Events that call user speficied code when triggered*/
	struct {
		struct wl_signal new_output; /**A new monitor became avaliable*/
	} events;
	
	/**Functions for the user to call into backend speficied code*/
	struct {
		pfn_kwl_backend_start start;
		pfn_kwl_backend_deinit deinit;
	} entry_points; 
};

/** \breif Create a kwl_backend_t from enviroment variables
 *	\param display the wayland display to create a backend
 *	\retval A pointer to kwl_backend_t
 */
kwl_backend_t *kwl_backend_init_env(struct wl_display *display);

/** \breif Create a kwl_backend_t from Name
 *	\param display the wayland display to create a backend
 *	\param name of backend to create
 *	\retval A pointer to kwl_backend_t
 */
kwl_backend_t *kwl_backend_init_name(struct wl_display *display, const char *name);
