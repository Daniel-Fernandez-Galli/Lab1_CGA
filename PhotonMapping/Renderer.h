#ifndef RENDERER_H
#define RENDERER_H

//#define PHOTONMAP_DEBUG_API //comment this line to remove debugging methods

#include "utils.h"
#include "Camera.h"
#include "Scene.h"
#include "MeshObject.h"
#include "KDTree.h"

#include <embree4/rtcore.h>
#include <map>
#include <unordered_map>

class Renderer
{
private:

	Camera camera;

	Scene scene;

	uint32_t* pixels;

	SDL_Renderer* renderer;

	SDL_Texture* texture;

	const KDTree* global_photonmap = nullptr;

	const KDTree* caustics_photonmap = nullptr;

	unsigned int photon_count = 0;

	std::unordered_map<Vector3, Vector3> discrete_radiances;

	void normal_gradient_shading(const RTCRayHit &rayhit, uint32_t &r, uint32_t& g, uint32_t& b, bool smooth = false);

	void lambertian_surfaces_shading(const RTCRayHit &rayhit, uint32_t& r, uint32_t& g, uint32_t& b);

	void photon_mapping_shading(const RTCRayHit &rayhit, uint32_t& r, uint32_t& g, uint32_t& b);

	Vector3 get_direct_light(const RTCRayHit& rayhit);

	Vector3 get_indirect_light(const RTCRayHit &rayhit);

	Vector3 compute_radiance(const Vector3 approx_hit_pos, unsigned int geom_id);

	Vector3 get_specular_reflection(const RTCRayHit &rayhit);

	Vector3 get_specular_refraction(const RTCRayHit& rayhit);

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

	void set_global_photonmap(const KDTree* map, unsigned int count);

	void set_caustics_photonmap(const KDTree* map, unsigned int count);

	void move_camera(Direction dir);

	void transform_camera(math::Matrix<4, 4> transform, bool local_origin = false, bool local_axis = false);

	~Renderer();

#ifdef PHOTONMAP_DEBUG_API
#define DEBUG_PHOTON_DISPLAY_COLOR_FILL 0xFFFFFFFF
#define DEBUG_PHOTON_DISPLAY_COLOR_EDGE 0xFFFF0000

	void draw_photon(int x, int y, Color color, float distance);

	void debug_display_photons(const KDTree &tree);

	void draw_camera(Vector3 position, Color color, float distance);

#endif // PHOTONMAP_DEBUG_API

};

#endif
