#include <windows.h>

#include <glad/glad.h>
#include "SDL.h"
#include "SDL_opengl.h"
#include "FreeImage.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include "embree4/rtcore.h"

#include "utils.h"
#include "KDTree.h"
#include "Renderer.h"
#include "File.h"

int main(int argc, char* argv[]) {

	//INICIALIZACION
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	/* Embree window init */
	SDL_Window* embree_window = SDL_CreateWindow("Photon Mapping", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, 0);
	SDL_Renderer* embree_renderer = SDL_CreateRenderer(embree_window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* embree_texture = SDL_CreateTexture(embree_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 600, 600);

	/* photon kd tree test */

	std::vector<Photon> photons;
	photons.push_back(Photon({1.0f, 2.0f, 3.0f}, {1.0f, 0.0f, 0.0f}, {255, 0, 0, 255}));
	photons.push_back(Photon({4.0f, 5.0f, 6.0f}, {0.0f, 1.0f, 0.0f}, {0, 255, 0, 255}));
	photons.push_back(Photon({7.0f, 8.0f, 9.0f}, {0.0f, 0.0f, 1.0f}, {0, 0, 255, 255}));
	KDTree tree;
	tree.init(photons);
	math::Vector3 query_point(7.0, 6.0, 9.0);
	std::vector<SearchResult> res = tree.search(query_point, 5.0f);
	// Print the nearest neighbor and its distance
	std::cout << "Nearest neighbor index: " << res[0].index << std::endl;
	std::cout << "Distance to nearest neighbor: " << res[0].distance_squared << std::endl;

	/**/

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events

	auto glbfile = File::load_glb("../suzanne.glb");
	auto meshes = File::extract_meshes(glbfile,0);

	Renderer renderer;
	renderer.load_scene(meshes[0]);

	while (running)		// the event loop
	{
		while (SDL_PollEvent(&sdlEvent))
		{
			if (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
				running = false;
		}
		
		renderer.trace(embree_renderer, embree_texture);
	}
	SDL_Quit();
	return 0;
}
