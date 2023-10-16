#include "Renderer.h"
#include "File.h"

#include <cmath>

using namespace math;

Renderer::Renderer(SDL_Window* window) : camera(window), pixels(new uint32_t[600 * 600])
{
}

void Renderer::attach_mesh(std::shared_ptr<geometry::Mesh> mesh_ptr)
{
	scene.attach_mesh(mesh_ptr);
}

void Renderer::attach_material(Material material)
{
	scene.attach_material(material);
}

void Renderer::commit_scene()
{
	scene.commit_scene();
}

void Renderer::trace(SDL_Renderer* renderer, SDL_Texture* texture)
{
	//draw_lock(mtx);

	for (int x = 0; x < 600; x++) {
		for (int y = 0; y < 600; y++) {

			RTCRayHit rayhit = camera.ray_to_pixel(x, y);

			scene.ray_intersect(rayhit);

			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				//Old normal to color
				//uint32_t r = (uint32_t)(abs(Ng.x) * 255);
				//uint32_t g = (uint32_t)(abs(Ng.y) * 255);
				//uint32_t b = (uint32_t)(abs(Ng.z) * 255);

				Vector3 Ng = normal_interpolation(
					scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
					scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
					scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
					rayhit.hit.u,
					rayhit.hit.v
					);

				Ng = normalize(Ng);

				Vector3 ambient_light = { 0.1f, 0.1f, 0.1f };
				Vector3 light_dir = { 1.0f, 1.0f, 3.0f};
				light_dir = normalize(light_dir);

				float lambertian = dot_product(Ng,light_dir);
				constexpr float k_diffuse = 1.0f;

				float light_intensity = k_diffuse * lambertian;
				
				if (light_intensity < 0.0f){
					light_intensity = 0.0f;
				}

				auto mat = scene.get_material(rayhit.hit.geomID);
				mat.basecolor = ambient_light + mat.basecolor * light_intensity;
				mat.basecolor = linear_RGB_to_sRGB(mat.basecolor);
				uint32_t r = mat.basecolor.x * 255;
				uint32_t g = mat.basecolor.y * 255;
				uint32_t b = mat.basecolor.z * 255;


				pixels[600 * y + x] = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB
			}
			else {
				pixels[600 * y + x] = 0xFF404040;
			}

		}
	}

	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}

void Renderer::move_camera(Direction dir)
{
	camera.move(dir);
}

void Renderer::transform_camera(Matrix<4, 4> transform, bool local_origin, bool local_axis)
{
	camera.transform(transform, local_origin, local_axis);
}

Renderer::~Renderer()
{
	delete[] pixels;
}
