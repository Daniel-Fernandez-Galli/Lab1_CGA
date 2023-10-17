#ifndef RENDERER_H
#define RENDERER_H

#define PHOTONMAP_DEBUG_API //comment this line to remove debugging methods

#include "utils.h"
#include "Camera.h"
#include "Scene.h"
#include "MeshObject.h"
#include "KDTree.h"

#include <embree4/rtcore.h>
#include <map>

class Renderer
{
private:

	Camera camera;

	Scene scene;

	uint32_t* pixels;

	SDL_Renderer* renderer;

	SDL_Texture* texture;

	void normal_gradient_shading(const RTCRayHit &rayhit, uint32_t &r, uint32_t& g, uint32_t& b, bool smooth = false);

	void lambertian_reflectance_shading(const RTCRayHit &rayhit, uint32_t& r, uint32_t& g, uint32_t& b);

	void photon_mapping_shading(const RTCRayHit &rayhit, uint32_t& r, uint32_t& g, uint32_t& b);

public:

	Renderer(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture);

	Renderer(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, CamConstructorData cam_data);

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void attach_mesh(std::shared_ptr<geometry::Mesh> mesh_ptr);

	void attach_material(Material material);

	void commit_scene();

	void trace();

	raytracing::Hit cast_ray(const raytracing::Ray& ray);

	void move_camera(Direction dir);

	void transform_camera(math::Matrix<4, 4> transform, bool local_origin = false, bool local_axis = false);

	~Renderer();

#ifdef PHOTONMAP_DEBUG_API
#define DEBUG_PHOTON_DISPLAY_COLOR_FILL 0xFFFFFFFF
#define DEBUG_PHOTON_DISPLAY_COLOR_EDGE 0xFFFF0000

	void draw_photon(int x, int y, Color color, float distance);

	void debug_display_photons(const KDTree &tree);

#endif // PHOTONMAP_DEBUG_API

};

#endif
