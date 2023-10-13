//// Allow unsafe functions used by std_image library
//#define _CRT_SECURE_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS_GLOBALS
//
//// Define these only in *one* .cc file.
//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.

#include "GLTF_loading_test.h"
#include "tiny_gltf.h"

using namespace tinygltf;

std::vector<float> test_init_cube() {

	Model model;
	TinyGLTF loader;
	std::string err;
	std::string warn;

	bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, "../cube.gltf");
	//bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)

	if (!warn.empty()) {
		printf("Warn: %s\n", warn.c_str());
	}

	if (!err.empty()) {
		printf("Err: %s\n", err.c_str());
	}

	if (!ret) {
		printf("Failed to parse glTF\n");
		abort();
	}

	// Extract vertex positions and triangle indices
	std::vector<float> vertexData; // To store the vertex coordinates
	std::vector<int> triangleIndices; // To store triangle indices

	if (!model.meshes.empty()) {
		const int meshIndex = 0; // Assuming there's only one mesh in the GLTF file

		if (!model.meshes[meshIndex].primitives.empty()) {
			const int accessorPositionIndex = model.meshes[meshIndex].primitives[0].attributes["POSITION"];
			const int accessorIndicesIndex = model.meshes[meshIndex].primitives[0].indices;

			if (accessorPositionIndex >= 0 && accessorIndicesIndex >= 0) {
				const tinygltf::Accessor& accessorPosition = model.accessors[accessorPositionIndex];
				const tinygltf::Accessor& accessorIndices = model.accessors[accessorIndicesIndex];
				const tinygltf::BufferView& bufferViewPosition = model.bufferViews[accessorPosition.bufferView];
				const tinygltf::BufferView& bufferViewIndices = model.bufferViews[accessorIndices.bufferView];
				const tinygltf::Buffer& bufferPosition = model.buffers[bufferViewPosition.buffer];
				const tinygltf::Buffer& bufferIndices = model.buffers[bufferViewIndices.buffer];

				// Extract vertex positions (x, y, z)
				const float* bufferDataPosition = reinterpret_cast<const float*>(&bufferPosition.data[bufferViewPosition.byteOffset + accessorPosition.byteOffset]);
				const int numVertices = static_cast<int>(accessorPosition.count);

				for (int i = 0; i < numVertices; ++i) {
					vertexData.push_back(bufferDataPosition[i * 3]);     // x-coordinate
					vertexData.push_back(bufferDataPosition[i * 3 + 1]); // y-coordinate
					vertexData.push_back(bufferDataPosition[i * 3 + 2]); // z-coordinate
				}

				// Extract triangle indices
				const uint16_t* bufferDataIndices = reinterpret_cast<const uint16_t*>(&bufferIndices.data[bufferViewIndices.byteOffset]);
				const int numIndices = static_cast<int>(accessorIndices.count);

				for (int i = 0; i < numIndices; ++i) {
					triangleIndices.push_back(static_cast<int>(bufferDataIndices[i]));
				}
			}
		}
	}

	SortVerticesByTriangle(vertexData, triangleIndices, 3);

	return vertexData;

}

// Define a structure to hold a vertex's position (x, y, z) and its original index.
struct VertexWithIndex {
	float x, y, z;
	int originalIndex;
};

// Function to sort vertexData based on triangleIndices.
void SortVerticesByTriangle(std::vector<float>& vertexData, const std::vector<int>& triangleIndices, int verticesPerTriangle) {
	int numTriangles = triangleIndices.size() / verticesPerTriangle;
	int numVertices = numTriangles * verticesPerTriangle;

	// Create a vector of VertexWithIndex structures to hold vertices with their original indices.
	std::vector<VertexWithIndex> verticesWithIndices(numVertices);

	// Copy vertex data into the verticesWithIndices vector while preserving original indices.
	for (int i = 0; i < numVertices; ++i) {
		int vertexIndex = triangleIndices[i];
		verticesWithIndices[i].x = vertexData[vertexIndex * 3];
		verticesWithIndices[i].y = vertexData[vertexIndex * 3 + 1];
		verticesWithIndices[i].z = vertexData[vertexIndex * 3 + 2];
		verticesWithIndices[i].originalIndex = vertexIndex;
	}

	vertexData.resize(numVertices * 3);
	// Copy the sorted vertices back into vertexData.
	for (int i = 0; i < numVertices; ++i) {
		vertexData[i * 3] = verticesWithIndices[i].x;
		vertexData[i * 3 + 1] = verticesWithIndices[i].y;
		vertexData[i * 3 + 2] = verticesWithIndices[i].z;
	}
}