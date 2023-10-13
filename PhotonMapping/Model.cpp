#include "Model.h"
#include "GLTF_loading_test.h"

using BYTE = unsigned char;

std::map<std::string, RTCBufferType> Model::attribute_buffer_type_map =
{
	{"POSITION", RTC_BUFFER_TYPE_VERTEX},
	{"NORMAL", RTC_BUFFER_TYPE_NORMAL},
	{"TANGENT", RTC_BUFFER_TYPE_TANGENT}
};

std::map<std::pair<int, int>, std::pair<RTCFormat, size_t>> Model::accesor_rtc_format_map =
{
	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_BYTE}, {RTC_FORMAT_CHAR, 1}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_BYTE}, {RTC_FORMAT_CHAR2, 2}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_BYTE}, {RTC_FORMAT_CHAR3, 3}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_BYTE}, {RTC_FORMAT_CHAR4, 4}},

	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE}, {RTC_FORMAT_UCHAR, 1}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE}, {RTC_FORMAT_UCHAR2, 2}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE}, {RTC_FORMAT_UCHAR3, 3}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE}, {RTC_FORMAT_UCHAR4, 4}},

	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_SHORT}, {RTC_FORMAT_SHORT, 2}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_SHORT}, {RTC_FORMAT_SHORT2, 4}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_SHORT}, {RTC_FORMAT_SHORT3, 6}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_SHORT}, {RTC_FORMAT_SHORT4, 8}},

	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT}, {RTC_FORMAT_USHORT, 2}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT}, {RTC_FORMAT_USHORT2, 4}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT}, {RTC_FORMAT_USHORT3, 6}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT}, {RTC_FORMAT_USHORT4, 8}},

	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT}, {RTC_FORMAT_UINT, 4}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT}, {RTC_FORMAT_UINT2, 8}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT}, {RTC_FORMAT_UINT3, 12}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT}, {RTC_FORMAT_UINT4, 16}},

	{{TINYGLTF_TYPE_SCALAR, TINYGLTF_COMPONENT_TYPE_FLOAT}, {RTC_FORMAT_FLOAT, 4}},
	{{TINYGLTF_TYPE_VEC2,	TINYGLTF_COMPONENT_TYPE_FLOAT}, {RTC_FORMAT_FLOAT2, 8}},
	{{TINYGLTF_TYPE_VEC3,	TINYGLTF_COMPONENT_TYPE_FLOAT}, {RTC_FORMAT_FLOAT3, 12}},
	{{TINYGLTF_TYPE_VEC4,	TINYGLTF_COMPONENT_TYPE_FLOAT}, {RTC_FORMAT_FLOAT4, 16}}
};

Model::Model() {
	m_device = rtcNewDevice(nullptr);
}

void Model::init_from_gltf(tinygltf::Model& data)
{
	//m_scenes.push_back(rtcNewScene(m_device));
	//RTCScene embree_scene = m_scenes.back();
	//RTCGeometry geo = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
	//std::vector<float> cube = test_init_cube();
	//for (int i = 0; i < cube.size(); i++) {
	//	cube[i] *= 10.f;
	//}

	//float* vertices = cube.data();

	//rtcSetSharedGeometryBuffer(
	//	geo,
	//	RTC_BUFFER_TYPE_VERTEX,   // Buffer type (vertices)
	//	0,                        // Slot
	//	RTC_FORMAT_FLOAT3,        // Data format (3 floats for x, y, z)
	//	vertices,                 // Pointer to vertex data
	//	0,                        // Byte offset (0 since it's the first buffer)
	//	sizeof(float) * 3,        // Stride (size of a single vertex)
	//	36                         // Number of vertices
	//);
	//unsigned int* indices = new unsigned int[108];
	//for (int i = 0; i < 108; i++) {
	//	indices[i] = i;
	//}
	//rtcSetSharedGeometryBuffer(
	//	geo,
	//	RTC_BUFFER_TYPE_INDEX,    // Buffer type (indices)
	//	0,                        // Slot
	//	RTC_FORMAT_UINT3,         // Data format (3 unsigned ints for indices)
	//	indices,                  // Pointer to index data
	//	0,                        // Byte offset (0 since it's the first buffer)
	//	sizeof(unsigned int) * 3, // Stride (size of a single index)
	//	36                         // Number of indices
	//);

	//rtcCommitGeometry(geo);
	//rtcAttachGeometry(embree_scene, geo);
	//rtcReleaseGeometry(geo);
	//rtcCommitScene(embree_scene);

	//return;

	m_model = data;
	// SCENE
	for (auto& gltf_scene : m_model.scenes) {
		m_scenes.push_back(rtcNewScene(m_device));
		RTCScene& embree_scene = m_scenes.back();

		// NODE
		for (int node_index : gltf_scene.nodes) {
			auto gltf_node = m_model.nodes[node_index];

			// MESH - PRIMITIVE
			if (gltf_node.mesh >= 0) {
				auto gltf_mesh = m_model.meshes[gltf_node.mesh];
				for (auto& gltf_primitive : gltf_mesh.primitives) {
					RTCGeometry geo = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);

					//attribute_to_geometry(gltf_primitive, "POSITION", geo);
					//attribute_to_geometry(gltf_primitive, "NORMAL", geo);
					//attribute_to_geometry(gltf_primitive, "TANGENT", geo); //Not implemented

					//indices_to_geometry(gltf_primitive, geo);

					std::vector<float> cube = test_init_cube();
					float* vertices = cube.data();

					rtcSetSharedGeometryBuffer(
						geo,
						RTC_BUFFER_TYPE_VERTEX,   // Buffer type (vertices)
						0,                        // Slot
						RTC_FORMAT_FLOAT3,        // Data format (3 floats for x, y, z)
						vertices,                 // Pointer to vertex data
						0,                        // Byte offset (0 since it's the first buffer)
						sizeof(float) * 3,        // Stride (size of a single vertex)
						36                         // Number of vertices
					);
					unsigned int* indices = new unsigned int[108];
					for (int i = 0; i < 108; i++) {
						indices[i] = i;
					}
					rtcSetSharedGeometryBuffer(
						geo,
						RTC_BUFFER_TYPE_INDEX,    // Buffer type (indices)
						0,                        // Slot
						RTC_FORMAT_UINT3,         // Data format (3 unsigned ints for indices)
						indices,                  // Pointer to index data
						0,                        // Byte offset (0 since it's the first buffer)
						sizeof(unsigned int) * 3, // Stride (size of a single index)
						36                         // Number of indices
					);

					rtcCommitGeometry(geo);
					rtcAttachGeometry(embree_scene, geo);
					rtcReleaseGeometry(geo);
					
					
				}
			}
		}
		rtcCommitScene(embree_scene);
	}
}

void Model::set_scene(unsigned int scene_index)
{
	if (m_scenes.size() <= scene_index) {
		throw std::domain_error("Error: Scene does not exist.");
	}
}

Hit Model::cast_ray(Ray ray)
{
	RTCRayHit rayhit;
	rayhit.ray.org_x = ray.origin.x;
	rayhit.ray.org_y = ray.origin.y;
	rayhit.ray.org_z = ray.origin.z;
	rayhit.ray.dir_x = ray.direction.x;
	rayhit.ray.dir_y = ray.direction.y;
	rayhit.ray.dir_z = ray.direction.z;
	rayhit.ray.tnear = 0;
	rayhit.ray.tfar = std::numeric_limits<float>::infinity();
	rayhit.ray.mask = -1;
	rayhit.ray.flags = 0;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rtcIntersect1(m_scenes[m_current_scene_index], &rayhit);

	Hit ret;
	ret.normal = math::Vector3(rayhit.hit.Ng_x, rayhit.hit.Ng_y, rayhit.hit.Ng_z);
	ret.uv = math::Vector2(rayhit.hit.u, rayhit.hit.v);
	//ret.material = ???;
	return ret;
}

RTCBufferType Model::get_attribute_rtc_buffer_type(std::string attribute_semantic)
{
	RTCBufferType buffer_type = RTC_BUFFER_TYPE_INDEX; //TEMP VALUE, DELETE THIS LINE
	auto it = attribute_buffer_type_map.find(attribute_semantic);
	if (it != attribute_buffer_type_map.end()) {
		buffer_type = it->second;
	}
	return buffer_type;
}

FormatSize Model::get_accessor_formatsize(tinygltf::Accessor accessor)
{
	RTCFormat format = RTC_FORMAT_UNDEFINED;
	size_t size = -1;
	auto it = accesor_rtc_format_map.find({ accessor.type, accessor.componentType });
	if (it != accesor_rtc_format_map.end()) {
		format = it->second.first;
		size = it->second.second;
	}
	return { format, size };
}

void Model::attribute_to_geometry(tinygltf::Primitive prim, std::string attribute_semantic, RTCGeometry& geo)
{
	auto btype_iter = attribute_buffer_type_map.find(attribute_semantic);
	if (btype_iter == attribute_buffer_type_map.end()) {
		return;
	}

	RTCBufferType buffer_type = btype_iter->second;

	auto attrib_iter = prim.attributes.find(attribute_semantic);
	if (attrib_iter != prim.attributes.end()) {
		int index = attrib_iter->second;
		auto accessor = m_model.accessors[index];
		auto buffer_view = m_model.bufferViews[accessor.bufferView];
		auto buffer = m_model.buffers[buffer_view.buffer];

		unsigned int offset = buffer_view.byteOffset + accessor.byteOffset;
		BYTE* data = &buffer.data[offset];

		auto formatsize = get_accessor_formatsize(accessor);

		rtcSetSharedGeometryBuffer(
			geo,
			buffer_type,
			0,
			formatsize.format,
			data,
			offset,
			formatsize.size,
			36
		);

	}
}

void Model::indices_to_geometry(tinygltf::Primitive prim, RTCGeometry& geo)
{
	int index = prim.indices;
	auto accessor = m_model.accessors[index];
	auto buffer_view = m_model.bufferViews[accessor.bufferView];
	auto buffer = m_model.buffers[buffer_view.buffer];

	unsigned int offset = buffer_view.byteOffset + accessor.byteOffset;
	BYTE* data = &buffer.data[offset];

	FormatSize formatsize = get_accessor_formatsize(accessor);

	rtcSetSharedGeometryBuffer(
		geo,
		RTC_BUFFER_TYPE_INDEX,
		0,
		formatsize.format,
		data,
		offset,
		formatsize.size,
		accessor.count
	);
}

void Model::test_draw_current_scene(SDL_Renderer* renderer, SDL_Texture* texture)
{
	auto&scene = m_scenes[m_current_scene_index];
	uint32_t* pixels = new uint32_t[600 * 600];

	for (int x = 0; x < 600; x++) {
		for (int y = 0; y < 600; y++) {
			struct RTCRayHit rayhit;
			rayhit.ray.org_x = 0;
			rayhit.ray.org_y = 0;
			rayhit.ray.org_z = 0;
			rayhit.ray.dir_x = x - 300;
			rayhit.ray.dir_y = y - 300;
			rayhit.ray.dir_z = 200;
			rayhit.ray.tnear = 0;
			rayhit.ray.tfar = std::numeric_limits<float>::infinity();
			rayhit.ray.mask = -1;
			rayhit.ray.flags = 0;
			rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			rayhit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;

			rtcIntersect1(scene, &rayhit);

			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID)
			{
				uint32_t r = rayhit.hit.Ng_x != 0 ? 0xFF : 0;
				uint32_t g = rayhit.hit.Ng_y != 0 ? 0xFF : 0;
				uint32_t b = rayhit.hit.Ng_z != 0 ? 0xFF : 0;
				pixels[600 * y + x] = (0xFF << 24) | (r << 16) | (g << 8) | b;
			}
			else {
				pixels[600 * y + x] = 0xFFFFFFFF; //white
			}
		}
	}
	SDL_UpdateTexture(texture, NULL, pixels, 600 * sizeof(uint32_t));
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
	delete[] pixels;
}
