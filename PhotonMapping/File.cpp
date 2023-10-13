// Allow unsafe functions used by std_image library
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS_GLOBALS

// Define these only in *one* .cc file.
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include "File.h"
#include "tiny_gltf.h"

using namespace File;
using namespace tinygltf;

/* Auxiliary method */
template <typename T>
std::vector<T> extract_from_accessor(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {

	auto buffer_view = model.bufferViews[accessor.bufferView];
	auto buffer = model.buffers[buffer_view.buffer];

	const T* buffer_data = reinterpret_cast<const T*>(&buffer.data[buffer_view.byteOffset + accessor.byteOffset]);

	return std::vector<T>(buffer_data, buffer_data + accessor.count);
}


struct File::GLTF_t {
	tinygltf::Model model;
};

GLTF_t* File::load_glb(std::string path)
{
	Model m_model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadBinaryFromFile(&m_model, &err, &warn, path); // for binary glTF(.glb)

	if (!warn.empty()) {
		printf("Warning: %s\n", warn.c_str());
	}

	if (!err.empty()) {
		printf("Error: %s\n", err.c_str());
	}

	if (!ret) {
		printf("Failed to parse glTF\n");
		abort();
	}

	return new GLTF_t{ m_model };
}

std::vector<geometry::Mesh> File::extract_meshes(GLTF_t* file, unsigned int scene_index)
{
	std::vector<geometry::Mesh> meshes;

	auto scene = file->model.scenes[scene_index];

	for (int node_index : scene.nodes) {
		auto node = file->model.nodes[node_index];

		if (node.mesh >= 0) {
			auto mesh = file->model.meshes[node.mesh];

			geometry::Mesh new_mesh;

			for (auto& primitive : mesh.primitives) {

				const int positions_index = primitive.attributes["POSITION"];
				if (positions_index >= 0) {
					const tinygltf::Accessor& positions_accessor = file->model.accessors[positions_index];
					new_mesh.positions = extract_from_accessor<math::Vector3>(file->model, positions_accessor);
				}

				const int normals_index = primitive.attributes["NORMAL"];
				if (normals_index >= 0) {
					const tinygltf::Accessor& normals_accessor = file->model.accessors[normals_index];
					new_mesh.normals = extract_from_accessor<math::Vector3>(file->model, normals_accessor);
				}

				const int texcoords_index = primitive.attributes["TEXCOORD"];
				if (texcoords_index >= 0) {
					const tinygltf::Accessor& texcoords_accessor = file->model.accessors[texcoords_index];
					new_mesh.texcoords = extract_from_accessor<math::Vector2>(file->model, texcoords_accessor);
				}

				const int indices_index = primitive.indices;
				if (indices_index >= 0) {
					const tinygltf::Accessor& indices_accessor = file->model.accessors[indices_index];
					if (indices_accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
						auto indices_ushort = extract_from_accessor<unsigned short>(file->model, indices_accessor);
						new_mesh.indices = std::vector<unsigned int>(indices_ushort.begin(), indices_ushort.end());
					}
					else { // UNSIGNED INT CASE
						new_mesh.indices = extract_from_accessor<unsigned int>(file->model, indices_accessor);
					}
				}

			}

			meshes.push_back(new_mesh);
		}
	}

	return meshes;
}