#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"
#include "Camera.h"
#include "Scene.h"
#include "MeshObject.h"

#include <embree4/rtcore.h>
#include <map>

class Renderer
{
private:

	Camera camera;

	Scene scene;

	uint32_t* pixels;

public:

	Renderer(SDL_Window* window);

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void attach_mesh(std::shared_ptr<geometry::Mesh> mesh_ptr);

	void attach_material(Material material);

	void commit_scene();

	void trace(SDL_Renderer* renderer, SDL_Texture* texture);

	raytracing::Hit cast_ray(const raytracing::Ray& ray);

	void move_camera(Direction dir);

	void transform_camera(math::Matrix<4, 4> transform, bool local_origin = false, bool local_axis = false);

	~Renderer();

};

#endif
