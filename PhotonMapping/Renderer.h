#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"
#include "Camera.h"

#include <embree4/rtcore.h>
#include <SDL.h>
#include <shared_mutex>

using mutex_type = std::shared_mutex;

class Renderer
{
private:

	RTCDevice device;

	RTCScene scene;

	Camera camera;

	//mutable mutex_type mtx;
	//using camera_lock = std::shared_lock<mutex_type>;
	//using draw_lock = std::unique_lock<mutex_type>;

	uint32_t* pixels;

public:

	Renderer(SDL_Window* window);

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void attach_mesh(const geometry::Mesh &mesh);

	void commit_scene();

	void trace(SDL_Renderer* renderer, SDL_Texture* texture);

	void move_camera(Direction dir);

	void transform_camera(math::Matrix<4, 4> transform, bool local_origin = false, bool local_axis = false);

	~Renderer();

};

#endif
