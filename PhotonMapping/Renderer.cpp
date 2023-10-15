#include "Renderer.h"
#include "File.h"

#include <cmath>

Renderer::Renderer(SDL_Window* window) : camera(window), pixels(new uint32_t[600 * 600])
{
	device = rtcNewDevice(nullptr);
	scene = rtcNewScene(device);
}

void Renderer::attach_mesh(const geometry::Mesh& mesh)
{

	RTCGeometry geo = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);

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
	rtcAttachGeometry(scene, geo);
	rtcReleaseGeometry(geo);

}

void Renderer::commit_scene()
{
	rtcCommitScene(scene);
}

void Renderer::trace(SDL_Renderer* renderer, SDL_Texture* texture)
{
	//draw_lock(mtx);

	for (int x = 0; x < 600; x++) {
		for (int y = 0; y < 600; y++) {

			RTCRayHit rayhit = camera.ray_to_pixel(x, y);

			rtcIntersect1(scene, &rayhit);

			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				float norm2 = sqrt(rayhit.hit.Ng_x * rayhit.hit.Ng_x + rayhit.hit.Ng_y * rayhit.hit.Ng_y + rayhit.hit.Ng_z * rayhit.hit.Ng_z);
				uint32_t r = (uint32_t)(abs(rayhit.hit.Ng_x / norm2) * 255);
				uint32_t g = (uint32_t)(abs(rayhit.hit.Ng_y / norm2) * 255);
				uint32_t b = (uint32_t)(abs(rayhit.hit.Ng_z / norm2) * 255);
				pixels[600 * y + x] = (0xFF << 24) | (r << 16) | (g << 8) | b; // ARGB
			}
			else {
				pixels[600 * y + x] = 0xFF'FF'FF'FF; //white
			}

		}
	}

	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

}

void Renderer::move_camera(Direction dir)
{
	//camera_lock(mtx);
	camera.move(dir);
}

void Renderer::transform_camera(math::Matrix<4, 4> transform, bool local_origin, bool local_axis)
{
	//camera_lock(mtx);
	camera.transform(transform, local_origin, local_axis);
}

Renderer::~Renderer()
{
	delete[] pixels;
}
