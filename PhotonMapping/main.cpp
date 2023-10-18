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

#include <thread>

#define ROTATION_SPEED 0.002f

int main(int argc, char* argv[]) {

	//INICIALIZACION
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return 1;
	}

	/* Embree window init */
	SDL_Window* sdlwindow = SDL_CreateWindow("Photon Mapping", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 600, 0);
	SDL_Renderer* sdlrenderer = SDL_CreateRenderer(sdlwindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture* sdltexture = SDL_CreateTexture(sdlrenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 600, 600);

	/* photon kd tree test */

	std::vector<Photon> photons;
	photons.push_back(Photon({ 1.0f, 2.0f, 3.0f }, { 1.0f, 0.0f, 0.0f }, { 255, 0, 0, 255 }));
	photons.push_back(Photon({ 4.0f, 5.0f, 6.0f }, { 0.0f, 1.0f, 0.0f }, { 0, 255, 0, 255 }));
	photons.push_back(Photon({ 7.0f, 8.0f, 9.0f }, { 0.0f, 0.0f, 1.0f }, { 0, 0, 255, 255 }));
	KDTree tree;
	tree.init(photons);
	math::Vector3 query_point(7.0, 6.0, 9.0);
	std::vector<SearchResult> res = tree.search_radius(query_point, 5.0f);
	res = tree.search_nearest(query_point, 5);
	// Print the nearest neighbor and its distance
	std::cout << "Nearest neighbor index: " << res[0].index << std::endl;
	std::cout << "Distance to nearest neighbor: " << res[0].distance_squared << std::endl;

	/**/

	bool running = true; // set running to true
	SDL_Event sdlEvent;  // variable to detect SDL events
	bool left_click_pressed = false;
	int last_mouse_x, last_mouse_y;
	float rotation_x = 0.0f;
	float rotation_y = 0.0f;

	auto glbfile = File::load_glb("../cornell_box.glb");
	auto objects = File::extract_meshes(glbfile, 0);
	auto cameras = File::extract_cameras(glbfile, 0);

	std::unique_ptr<Renderer> renderer_ptr;
	if (cameras.size() == 0) {
		renderer_ptr = std::make_unique<Renderer>(sdlrenderer, sdlwindow, sdltexture);
	} else {
		renderer_ptr = std::make_unique<Renderer>(sdlrenderer, sdlwindow, sdltexture, cameras[0]);
	}
	
	Renderer& renderer = *renderer_ptr;
	for (auto& object : objects) {
		auto meshes = object.get_meshes();
		auto materials = object.get_materials();
		for (auto it = meshes.begin(); it < meshes.end(); it++) {
			renderer.attach_mesh(meshes[std::distance(meshes.begin(), it)]);
			renderer.attach_material(materials[std::distance(meshes.begin(), it)]);
		}
	}

	renderer.commit_scene();

	std::vector<Photon> photons2;
	constexpr float step = 1.0f;
	for (float i = -9.5f; i <= 9.5f; i++) {
		for (float j = -9.5f; j <= 9.5f; j++) {
			for (float k = -9.5; k <= 9.5; k++) {
				if (std::abs(i) == 9.5f || std::abs(j) == 9.5f || k == -9.5f) {
					photons2.push_back(Photon({ i * step, j * step, k * step }, { 1.0f, 0.0f, 0.0f }, { 255, 0, 0, 255 }));
				}
			}
		}
	}
	std::unique_ptr<KDTree> tree_ptr = std::make_unique<KDTree>();
	KDTree& tree2 = *tree_ptr;
	tree2.init(photons2);
	renderer.set_global_photonmap(tree2);

	while (running)		// the event loop
	{
		renderer.trace();

		while (SDL_PollEvent(&sdlEvent))
		{
			if (sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE)
				running = false;
			else if (sdlEvent.type == SDL_KEYDOWN) {
				// Handle key press
				SDL_KeyboardEvent* keyEvent = &sdlEvent.key;

				switch (keyEvent->keysym.sym) {
				case SDLK_w:
					renderer.move_camera(Direction::FORWARD);
					break;
				case SDLK_a:
					renderer.move_camera(Direction::LEFT);
					break;
				case SDLK_s:
					renderer.move_camera(Direction::BACKWARD);
					break;
				case SDLK_d:
					renderer.move_camera(Direction::RIGHT);
					break;
				case SDLK_q:
					renderer.move_camera(Direction::DOWN);
					break;
				case SDLK_e:
					renderer.move_camera(Direction::UP);
					break;
				case SDLK_ESCAPE:
					running = false;
					break;
				}

			}
			else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					left_click_pressed = true;
					SDL_GetMouseState(&last_mouse_x, &last_mouse_y);
				}
			}
			else if (sdlEvent.type == SDL_MOUSEBUTTONUP) {
				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					left_click_pressed = false;
				}
			}
			else if (sdlEvent.type == SDL_MOUSEMOTION && left_click_pressed) {
				int mouse_x, mouse_y;
				SDL_GetMouseState(&mouse_x, &mouse_y);

				int delta_x = mouse_x - last_mouse_x;
				int delta_y = mouse_y - last_mouse_y;

				rotation_x = delta_y * ROTATION_SPEED;
				rotation_y = delta_x * ROTATION_SPEED;

				Matrix<4, 4> rot_x = identity<4>();
				Matrix<4, 4> rot_y = identity<4>();

				rot_x[1][1] = cos(rotation_x);
				rot_x[1][2] = -sin(rotation_x);
				rot_x[2][1] = sin(rotation_x);
				rot_x[2][2] = cos(rotation_x);

				// Rotate around Y-axis
				rot_y[0][0] = cos(rotation_y);
				rot_y[0][2] = sin(rotation_y);
				rot_y[2][0] = -sin(rotation_y);
				rot_y[2][2] = cos(rotation_y);

				renderer.transform_camera(rot_x, true, true);

				renderer.transform_camera(rot_y, true, false);

				last_mouse_x = mouse_x;
				last_mouse_y = mouse_y;
			}
		}
	}
	//trace_tread.join();
	SDL_Quit();
	return 0;
}
