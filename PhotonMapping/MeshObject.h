#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include "utils.h"

enum class BufferType {
	POSITION_BUFFER, NORMAL_BUFFER, TEXCOORD_BUFFER, INDEX_BUFFER
};

struct Buffer {
	const void* data;
	unsigned int count;
};

class MeshObject
{
private:

	math::Matrix<4, 4> transform;

	std::vector<std::shared_ptr<geometry::Mesh>> meshes;

	//std::vector<Material> materials; NOT IMPLEMENTED

public:

	MeshObject(math::Matrix<4,4> transform = math::identity<4>());

	void add_mesh(geometry::Mesh &mesh);

	void commit_object();

	std::vector<std::shared_ptr<geometry::Mesh>> get_meshes();

};

#endif

