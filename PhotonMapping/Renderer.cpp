#include "Renderer.h"
#include "File.h"

#include <cmath>

Renderer::Renderer()
{
	m_device = rtcNewDevice(nullptr);
	m_scene = rtcNewScene(m_device);
}

void Renderer::load_scene(const geometry::Mesh& mesh)
{

	RTCGeometry geo = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);

	const float* positions = reinterpret_cast<const float*>(mesh.positions.data());

	rtcSetSharedGeometryBuffer(
		geo,
		RTC_BUFFER_TYPE_VERTEX,   // Buffer type (vertices)
		0,                        // Slot
		RTC_FORMAT_FLOAT3,        // Data format (3 floats for x, y, z)
		positions,                 // Pointer to vertex data
		0,                        // Byte offset (0 since it's the first buffer)
		sizeof(float) * 3,        // Stride (size of a single vertex)
		mesh.positions.size()                         // Number of vertices
	);

	const float* normals = reinterpret_cast<const float*>(mesh.normals.data());
	rtcSetSharedGeometryBuffer(
		geo,
		RTC_BUFFER_TYPE_NORMAL,   // Buffer type (vertices)
		0,                        // Slot
		RTC_FORMAT_FLOAT3,        // Data format (3 floats for x, y, z)
		normals,                 // Pointer to vertex data
		0,                        // Byte offset (0 since it's the first buffer)
		sizeof(float) * 3,        // Stride (size of a single vertex)
		mesh.normals.size()                         // Number of normals
	);

	const unsigned int* indices = reinterpret_cast<const unsigned int*>(mesh.indices.data());
	rtcSetSharedGeometryBuffer(
		geo,
		RTC_BUFFER_TYPE_INDEX,    // Buffer type (indices)
		0,                        // Slot
		RTC_FORMAT_UINT3,         // Data format (3 unsigned ints for indices)
		indices,                  // Pointer to index data
		0,                        // Byte offset (0 since it's the first buffer)
		sizeof(unsigned int) * 3, // Stride (size of a single index)
		mesh.indices.size() / 3                         // Number of indices
	);

	rtcCommitGeometry(geo);
	rtcAttachGeometry(m_scene, geo);
	rtcReleaseGeometry(geo);
	rtcCommitScene(m_scene);
}

Hit Renderer::cast_ray(Ray ray)
{
	return Hit();
}

void Renderer::trace(SDL_Renderer* renderer, SDL_Texture* texture)
{
	uint32_t* pixels = new uint32_t[600 * 600];

	for (int x = 0; x < 600; x++) {
		for (int y = 0; y < 600; y++) {
			struct RTCRayHit rayhit;
			rayhit.ray.org_x = 0;
			rayhit.ray.org_y = 0;
			rayhit.ray.org_z = 0;
			rayhit.ray.dir_x = x - 300;
			rayhit.ray.dir_y = y - 300;
			rayhit.ray.dir_z = -1000;
			rayhit.ray.tnear = 0;
			rayhit.ray.tfar = std::numeric_limits<float>::infinity();
			rayhit.ray.mask = -1;
			rayhit.ray.flags = 0;
			rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

			rtcIntersect1(m_scene, &rayhit);

			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				float norm2 = sqrt(rayhit.hit.Ng_x * rayhit.hit.Ng_x + rayhit.hit.Ng_y * rayhit.hit.Ng_y + rayhit.hit.Ng_z * rayhit.hit.Ng_z);
				uint32_t r = abs(rayhit.hit.Ng_x/norm2) * 255;
				uint32_t g = abs(rayhit.hit.Ng_y/norm2) * 255;
				uint32_t b = abs(rayhit.hit.Ng_z/norm2) * 255;
				pixels[600 * (600 - y - 1) + x] = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB
			}
			else {
				pixels[600 * (600 - y - 1) + x] = 0xFFFFFFFF; //white
			}
		}
	}
	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	delete[] pixels;
}
