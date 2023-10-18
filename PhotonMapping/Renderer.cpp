#include "Renderer.h"
#include "File.h"

#include <cmath>
#include <algorithm>

using namespace math;

void Renderer::normal_gradient_shading(const RTCRayHit &rayhit, uint32_t& r, uint32_t& g, uint32_t& b, bool smooth)
{
	Vector3 N(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);

	if (smooth) {
		N = normal_interpolation(
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
			rayhit.hit.u,
			rayhit.hit.v
		);
	}

	N = normalize(N);

	r = (uint32_t)(abs(N.x) * 255);
	g = (uint32_t)(abs(N.y) * 255);
	b = (uint32_t)(abs(N.z) * 255);
}

void Renderer::lambertian_reflectance_shading(const RTCRayHit& rayhit, uint32_t& r, uint32_t& g, uint32_t& b)
{
	Vector3 Ng = normal_interpolation(
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
		rayhit.hit.u,
		rayhit.hit.v
	);

	Ng = normalize(Ng);

	Vector3 light_dir = { 0.5f, 1.0f, 0.5f };
	light_dir = normalize(light_dir);

	float lambertian = dot_product(Ng, light_dir);
	constexpr float k_diffuse = 1.0f;

	float light_intensity = k_diffuse * lambertian;
	light_intensity = (light_intensity < 0.0f) ? 0.0f : light_intensity;

	auto mat = scene.get_material(rayhit.hit.geomID);
	Vector3 ambient_light = mat.basecolor;
	constexpr float k_ambient = 0.1f;
	mat.basecolor = k_ambient * ambient_light + mat.basecolor * light_intensity;
	mat.basecolor = linear_RGB_to_sRGB(mat.basecolor);
	r = mat.basecolor.x * 255;
	g = mat.basecolor.y * 255;
	b = mat.basecolor.z * 255;
}

void Renderer::photon_mapping_shading(const RTCRayHit& rayhit, uint32_t& r, uint32_t& g, uint32_t& b)
{
}

Renderer::Renderer(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture) :
	camera(window), renderer(renderer), texture(texture), pixels(new uint32_t[600 * 600])
{
}

Renderer::Renderer(SDL_Renderer* renderer, SDL_Window* window, SDL_Texture* texture, CamConstructorData cam_data) :
	camera(window, cam_data), renderer(renderer), texture(texture), pixels(new uint32_t[600 * 600])
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

void Renderer::trace()
{

	for (int x = 0; x < 600; x++) {
		for (int y = 0; y < 600; y++) {

			RTCRayHit rayhit = camera.ray_to_pixel(x, y);

			scene.ray_intersect(rayhit);

			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				uint32_t r, g, b;
				//normal_gradient_shading(rayhit, r, g, b);
				//normal_gradient_shading(rayhit, r, g, b, true);
				lambertian_reflectance_shading(rayhit, r,g,b);
				pixels[600 * y + x] = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB
			}
			else {
				pixels[600 * y + x] = 0x00000000;
			}

		}
	}

	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}

raytracing::Hit Renderer::cast_ray(const raytracing::Ray& ray)
{
	struct RTCRayHit rayhit {};
	rayhit.ray.org_x = ray.orig.x;
	rayhit.ray.org_y = ray.orig.y;
	rayhit.ray.org_z = ray.orig.z;
	rayhit.ray.dir_x = ray.dir.x;
	rayhit.ray.dir_y = ray.dir.y;
	rayhit.ray.dir_z = ray.dir.z;
	rayhit.ray.tnear = 0.01f;
	rayhit.ray.tfar = 1000.0f;
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

	scene.ray_intersect(rayhit);

	bool hasHit = rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID;

	Vector3 intersection = Vector3(0,0,0);
	Vector3 normal = Vector3(0, 0, 0);
	Material mat;

	if (hasHit) {
		intersection.x = ray.orig.x + rayhit.ray.tfar * ray.dir.x;
		intersection.y = ray.orig.y + rayhit.ray.tfar * ray.dir.y;
		intersection.z = ray.orig.z + rayhit.ray.tfar * ray.dir.z;

		normal = normal_interpolation(
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
			scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
			rayhit.hit.u,
			rayhit.hit.v
		);

		normal = normalize(normal);

		mat = scene.get_material(rayhit.hit.geomID);
	}
	return {intersection, normal, mat, hasHit};
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

#ifdef PHOTONMAP_DEBUG_API

void Renderer::draw_photon(int x, int y, Color color, float distance) {
	int size = 1 + (int)(5/(0.001*distance + 1));
	for (int i = -size; i <= size; i++) {
		for (int j = -size; j <= size; j++) {
			if ((0 <= y + j) && (y + j < 600) && (0 <= x + i) && (x + i < 600)) {
				uint32_t fillColor = color.get_argb();
				uint32_t borderColor = Color(0, 0, 0).get_argb();
				pixels[600 * (y + j) + (x + i)] = (std::abs(i) == size || std::abs(j) == size) ? borderColor : fillColor;
			}
		}
	}
}

void Renderer::debug_display_photons(const KDTree& tree)
{
	Vector3 eye = camera.get_cam_data().eye_point;
	auto res = tree.search(eye);

	for (auto r = res.end() - 1; r > res.begin(); r--) {

		Photon photon = tree.get_photon(r->index);
		Vector3 pos = photon.get_position();
		Vector2 screen_pos = camera.to_raster_space(pos);
		int px = !std::isnan(screen_pos.x) ? static_cast<int>(screen_pos.x) : -1;
		int py = !std::isnan(screen_pos.y) ? static_cast<int>(screen_pos.y) : -1;

		if ((0 <= px) && (px < 600) && (0 <= py) && (py < 600)) {
			draw_photon(px, py, photon.power, r->distance_squared);
		}

	}

	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void Renderer::draw_camera(Vector3 position, Color color, float distance) {
	Vector3 eye = camera.get_cam_data().eye_point;


	Vector2 screen_pos = camera.to_raster_space(position);
	int px = !std::isnan(screen_pos.x) ? static_cast<int>(screen_pos.x) : -1;
	int py = !std::isnan(screen_pos.y) ? static_cast<int>(screen_pos.y) : -1;

	if ((0 <= px) && (px < 600) && (0 <= py) && (py < 600)) {
		draw_photon(px, py, Color(255,255,0), 10);
	}


	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

#endif // PHOTONMAP_DEBUG_API


