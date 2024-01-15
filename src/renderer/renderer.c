#include <kwl_private/allocator/allocator.h>
#include <kwl_private/interfaces/kwl_buffer.h>
#include <kwl/renderer/renderer.h>
#include <kwl/renderer/software.h>


kwl_renderer_t *kwl_renderer_init(kwl_backend_t *backend) {
	return kwl_sw_renderer_init(backend);
}
