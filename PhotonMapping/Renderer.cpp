#include "Renderer.h"
#include "File.h"

#include <cmath>
#include <algorithm>

#define RESOLUTION 3

using namespace math;
using namespace raytracing;

Vector3 placeholder_light_position(0.0f, 9.67155f, 0.0f);

void Renderer::normal_gradient_shading(const RTCRayHit& rayhit, uint32_t& r, uint32_t& g, uint32_t& b, bool smooth)
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

void Renderer::lambertian_surfaces_shading(const RTCRayHit& rayhit, uint32_t& r, uint32_t& g, uint32_t& b)
{
	Vector3 N = normal_interpolation(
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
		rayhit.hit.u,
		rayhit.hit.v
	);

	N = normalize(N);

	Vector3 light_dir = { 0.5f, 1.0f, 0.5f };
	light_dir = normalize(light_dir);

	float lambertian = dot_product(N, light_dir);
	constexpr float k_diffuse = 1.0f;

	float light_intensity = k_diffuse * lambertian;
	light_intensity = std::max(0.0f, light_intensity);

	auto mat = scene.get_material(rayhit.hit.geomID);
	Vector3 ambient_light = mat.basecolor;
	constexpr float k_ambient = 0.1f;
	Vector3 aparent_color;
	aparent_color = k_ambient * ambient_light + mat.basecolor * light_intensity;
	aparent_color = linear_RGB_to_sRGB(aparent_color);
	r = (uint32_t)(aparent_color.x * 255);
	g = (uint32_t)(aparent_color.y * 255);
	b = (uint32_t)(aparent_color.z * 255);
}

void Renderer::photon_mapping_shading(const RTCRayHit& rayhit, uint32_t& r, uint32_t& g, uint32_t& b)
{
	auto mat = scene.get_material(rayhit.hit.geomID);
	if (mat.emission.x == 1) {
		r = (uint32_t)(mat.emission.x * 255);
		g = (uint32_t)(mat.emission.y * 255);
		b = (uint32_t)(mat.emission.z * 255);
		return;
	}


	Vector3 indirect_light = get_indirect_light(rayhit);
	Vector3 direct_light = get_direct_light(rayhit);
	Vector3 specular_reflection = mat.roughness == 0 ? get_specular_reflection(rayhit) : Vector3(0.0f, 0.0f, 0.0f);
	Vector3 specular_refraction = mat.transmission == 1 ? get_specular_refraction(rayhit) : Vector3(0.0f, 0.0f, 0.0f);

	Vector3 aparent_color = direct_light + indirect_light + specular_reflection + specular_refraction;

	aparent_color = linear_RGB_to_sRGB(aparent_color);

	r = (uint32_t)(aparent_color.x * 255);
	g = (uint32_t)(aparent_color.y * 255);
	b = (uint32_t)(aparent_color.z * 255);
}

Vector3 Renderer::get_direct_light(const RTCRayHit& rayhit)
{
	Vector3 N = normal_interpolation(
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
		rayhit.hit.u,
		rayhit.hit.v
	);

	N = normalize(N);

	Vector3 orig(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z);
	Vector3 dir(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
	float t = rayhit.ray.tfar;
	Vector3 hit_location = orig + t * dir;

	auto mat = scene.get_material(rayhit.hit.geomID);
	float k_diffuse = mat.roughness;

	Vector3 light_dir = placeholder_light_position - hit_location;

	light_dir = normalize(light_dir);
	float lambertian = dot_product(N, light_dir);
	float k_direct = k_diffuse * lambertian;
	k_direct = std::max(0.0f, k_direct);

	Vector3 direct_light = mat.basecolor * k_direct;

	Ray L(hit_location, light_dir);
	Hit L_hit = cast_ray(L);

	if (L_hit.material.emission.x == 0.0f) {
		direct_light = direct_light * L_hit.material.transmission;
	}

	return direct_light;
}

Vector3 Renderer::get_indirect_light(const RTCRayHit& rayhit)
{
	Vector3 orig(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z);
	Vector3 dir(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
	float t = rayhit.ray.tfar;
	Vector3 hit_location = orig + t * dir;
	Vector3 approx = truncate(hit_location, RESOLUTION);
	uint32_t encoded_hit = encode_b1024(approx);
	unsigned int geom_id = rayhit.hit.geomID;

	Vector3 indirect_light;
	if (global_photonmap == nullptr || caustics_photonmap == nullptr) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}
	auto radiance_itr = discrete_radiances.find(approx);
	if (radiance_itr == discrete_radiances.end()) {
		indirect_light = compute_radiance(approx, geom_id);
	}
	else {
		Vector3 decoded_hit = decode_b1024(encoded_hit);
		float dist = norm2(hit_location - decoded_hit);
		indirect_light = (Vector3)discrete_radiances[approx];
	}
	auto mat = scene.get_material(rayhit.hit.geomID);
	return element_wise_multiplication(mat.basecolor, indirect_light);

}

Vector3 Renderer::compute_radiance(const Vector3 approx_hit_pos, unsigned int geom_id)
{
	auto mat = scene.get_material(geom_id);
	Vector3 aparent_color(0.0f, 0.0f, 0.0f);

	float worst_dist = 0.0f;
	unsigned int N = (unsigned int)std::max(1.0f, 0.0001f * photon_global_count);
	auto results = global_photonmap->search_nearest(approx_hit_pos, N);
	for (auto& res : results) {
		Photon p = global_photonmap->get_photon(res.index);
		float dist = res.distance_squared;
		worst_dist = dist < worst_dist ? worst_dist : dist;

		float k_diffuse = mat.roughness;
		Vector3 power(p.get_power().fr(), p.get_power().fg(), p.get_power().fb());
		aparent_color = aparent_color + (power * k_diffuse) / (photon_global_count * dist);

	}
	aparent_color = aparent_color / (0.3f * pi * worst_dist);

	Vector3 caustics_color(0.0f, 0.0f, 0.0f);
	results = caustics_photonmap->search_nearest(approx_hit_pos, N);
	for (auto& res : results) {
		Photon p = caustics_photonmap->get_photon(res.index);
		float dist = res.distance_squared;
		worst_dist = dist < worst_dist ? worst_dist : dist;

		float k_diffuse = mat.roughness;
		Vector3 power(p.get_power().fr(), p.get_power().fg(), p.get_power().fb());
		caustics_color = caustics_color + (power * k_diffuse) / (photon_global_count * dist);

	}

	aparent_color = aparent_color + caustics_color / (0.3f * pi * worst_dist);

	discrete_radiances[approx_hit_pos] = aparent_color;

	return aparent_color;


}

Vector3 Renderer::get_specular_reflection(const RTCRayHit& rayhit)
{
	auto mat = scene.get_material(rayhit.hit.geomID);
	float ks = 1.0f - mat.roughness;

	if (ks != 1.0f) {

		if (mat.emission.x == 1.0f) {
			return Vector3(1.0f, 1.0f, 1.0f);
		}

		Vector3 indirect_light = get_indirect_light(rayhit);
		//Vector3 direct_light = get_direct_light(rayhit);

		Vector3 aparent_color = indirect_light;

		return aparent_color;
	}

	Vector3 orig(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z);
	Vector3 dir(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
	float t = rayhit.ray.tfar;
	Vector3 hit_location = orig + t * dir;

	Vector3 N = normal_interpolation(
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
		rayhit.hit.u,
		rayhit.hit.v
	);

	N = normalize(N);

	Vector3 reflected_dir = reflectRay(-normalize(dir), N);

	struct RTCRayHit rayhit_refl {};
	rayhit_refl.ray.org_x = hit_location.x;
	rayhit_refl.ray.org_y = hit_location.y;
	rayhit_refl.ray.org_z = hit_location.z;
	rayhit_refl.ray.dir_x = reflected_dir.x;
	rayhit_refl.ray.dir_y = reflected_dir.y;
	rayhit_refl.ray.dir_z = reflected_dir.z;
	rayhit_refl.ray.tnear = 0.01f;
	rayhit_refl.ray.tfar = 1000.0f;
	rayhit_refl.ray.mask = -1;
	rayhit_refl.ray.flags = 0;
	rayhit_refl.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit_refl.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

	scene.ray_intersect(rayhit_refl);

	if (rayhit_refl.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	Vector3 spec_refl = get_specular_reflection(rayhit_refl);
	if ((mat.metallic == 0.0f) && (spec_refl != Vector3(1.0f, 1.0f, 1.0f)))
	{
		//Fresnel-Schlick approximation
		float ior = 1.450f;
		float rho_zero = (1 - ior) / (1 - pow(ior, 2));
		float cos_theta = dot_product(N, normalize(dir));
		float rho_theta = rho_zero + (1 - rho_zero) * pow(cos_theta, 5);
		rho_theta = std::clamp(rho_theta, 0.0f, 1.0f);
		spec_refl = spec_refl * rho_theta;
		auto mat2 = scene.get_material(rayhit_refl.hit.geomID);
		float kt = mat2.transmission;
		if (kt == 1.0f) {
			Vector3 spec_refr = get_specular_refraction(rayhit);
			spec_refl = spec_refl + (1 - rho_theta) * spec_refr;
		}
	}
	return spec_refl;
}

Vector3 Renderer::get_specular_refraction(const RTCRayHit& rayhit)
{
	auto mat = scene.get_material(rayhit.hit.geomID);
	float kt = mat.transmission;

	if (kt != 1.0f) {
		if (mat.emission.x == 1.0f) {
			return Vector3(1.0f, 1.0f, 1.0f);
		}

		Vector3 indirect_light = get_indirect_light(rayhit);
		//Vector3 direct_light = get_direct_light(rayhit);

		Vector3 aparent_color = indirect_light;

		return aparent_color;
	}

	Vector3 orig(rayhit.ray.org_x, rayhit.ray.org_y, rayhit.ray.org_z);
	Vector3 dir(rayhit.ray.dir_x, rayhit.ray.dir_y, rayhit.ray.dir_z);
	float t = rayhit.ray.tfar;
	Vector3 hit_location = orig + t * dir;

	Vector3 N = normal_interpolation(
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[0],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[1],
		scene.get_shading_normals(rayhit.hit.geomID, rayhit.hit.primID)[2],
		rayhit.hit.u,
		rayhit.hit.v
	);

	N = normalize(N);

	float ior_inv = 1 / 1.45f;
	float cos_theta = dot_product(N, normalize(dir));
	Vector3 refracted_dir = ior_inv * (normalize(dir) + ((ior_inv * cos_theta - sqrt(1 - pow(ior_inv, 2) * (1 - pow(cos_theta, 2)))) * N));
	refracted_dir = normalize(refracted_dir);

	struct RTCRayHit rayhit_refr {};
	rayhit_refr.ray.org_x = hit_location.x;
	rayhit_refr.ray.org_y = hit_location.y;
	rayhit_refr.ray.org_z = hit_location.z;
	rayhit_refr.ray.dir_x = refracted_dir.x;
	rayhit_refr.ray.dir_y = refracted_dir.y;
	rayhit_refr.ray.dir_z = refracted_dir.z;
	rayhit_refr.ray.tnear = 0.01f;
	rayhit_refr.ray.tfar = 1000.0f;
	rayhit_refr.ray.mask = -1;
	rayhit_refr.ray.flags = 0;
	rayhit_refr.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit_refr.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

	scene.ray_intersect(rayhit_refr);

	if (rayhit_refr.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
		return Vector3(0.0f, 0.0f, 0.0f);
	}

	return get_specular_refraction(rayhit_refr);
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
				//lambertian_surfaces_shading(rayhit, r,g,b);
				photon_mapping_shading(rayhit, r, g, b);
				pixels[600 * y + x] = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB
			}
			else {
				pixels[600 * y + x] = 0x00000000;
			}

		}
	}

#ifdef PHOTONMAP_DEBUG_API // Uncomment the definition in Renderer.h to use
	if (global_photonmap != nullptr) {
		//debug_display_photons(*global_photonmap);
	}	
	if (caustics_photonmap != nullptr) {
		debug_display_photons(*caustics_photonmap);
	}
#endif

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

	Vector3 intersection = Vector3(0, 0, 0);
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
	return { intersection, normal, mat, hasHit };
}

void Renderer::set_global_photonmap(const KDTree* map, unsigned int count)
{
	photon_global_count += count;
	global_photonmap = map;
}

void Renderer::set_caustics_photonmap(const KDTree* map, unsigned int count)
{
	photon_caustic_count += count;
	caustics_photonmap = map;
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
	int size = 1 + (int)(5 / (0.001 * distance + 1));
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
	auto res = tree.search_radius(eye);

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
		draw_photon(px, py, Color(255, 255, 0), 10);
	}


	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

#endif // PHOTONMAP_DEBUG_API


