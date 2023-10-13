#ifndef RENDERER_H
#define RENDERER_H

#include "utils.h"
#include "tiny_gltf.h"
#include "embree4/rtcore.h"

#include <SDL.h>

class Renderer
{
private:

	tinygltf::Model m_model;

	RTCDevice m_device;

	RTCScene m_scene;


public:

	Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void load_scene(const geometry::Mesh &mesh);

	Hit cast_ray(Ray ray);

	void trace(SDL_Renderer* renderer, SDL_Texture* texture);

};

#endif
