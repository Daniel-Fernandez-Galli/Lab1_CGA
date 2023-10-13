#ifndef MODEL_H
#define MODEL_H

#include "utils.h"
#include "tiny_gltf.h"
#include "embree4/rtcore.h"

#include <memory>
#include <vector>
#include <optional>
#include <SDL.h>

struct FormatSize {
	RTCFormat format;
	size_t size;
};

class Model
{
private:

	tinygltf::Model m_model;

	RTCDevice m_device;

	std::vector<RTCScene> m_scenes;

	unsigned int m_current_scene_index = 0;

	static std::map<std::string, RTCBufferType> attribute_buffer_type_map;

	static std::map<std::pair<int, int>, std::pair<RTCFormat, size_t>> accesor_rtc_format_map;

	// Internal methods

	RTCBufferType get_attribute_rtc_buffer_type(std::string attribute_semantic);

	FormatSize get_accessor_formatsize(tinygltf::Accessor accessor);

	void attribute_to_geometry(tinygltf::Primitive prim, std::string attribute_semantic, RTCGeometry &geo);

	void indices_to_geometry(tinygltf::Primitive prim, RTCGeometry& geo);

public:

	Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void init_from_gltf(tinygltf::Model &data);

	void set_scene(unsigned int scene_index);

	Hit cast_ray(Ray ray);

	void test_draw_current_scene(SDL_Renderer* renderer, SDL_Texture* texture);

	RTCScene test_get_current_scene() { return m_scenes[m_current_scene_index]; }

};

#endif
