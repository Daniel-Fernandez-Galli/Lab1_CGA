#ifndef MESHOBJECT_H
#define MESHOBJECT_H

#include "utils.h"
#include "Renderer.h"

class MeshObject
{
private:

	math::Matrix<4, 4> transform;

	std::vector<std::shared_ptr<geometry::Mesh>> meshes;

	//std::vector<Material> materials; NOT IMPLEMENTED

public:

	MeshObject(math::Matrix<4,4> transform = math::identity<4>());

	void add_mesh(geometry::Mesh &mesh);

	void commit_object(Renderer &renderer);

};

#endif

