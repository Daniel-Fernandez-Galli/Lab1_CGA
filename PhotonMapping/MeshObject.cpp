#include "MeshObject.h"

using namespace math;

MeshObject::MeshObject(math::Matrix<4, 4> transform) : transform(transform) {}

void MeshObject::add_mesh(geometry::Mesh &mesh)
{
	meshes.push_back(std::make_shared<geometry::Mesh>(mesh));
}

void MeshObject::commit_object(Renderer& renderer)
{
	for (auto mesh : meshes) {
		geometry::Mesh& transformed_mesh = *mesh;
		for (auto& v : transformed_mesh.positions) {
			Matrix<4, 1> homogeneous_pos;
			homogeneous_pos[0][0] = v.x;
			homogeneous_pos[1][0] = v.y;
			homogeneous_pos[2][0] = v.z;
			homogeneous_pos[3][0] = 1.0f;

			homogeneous_pos = transform * homogeneous_pos;

			v.x = homogeneous_pos[0][0];
			v.y = homogeneous_pos[1][0];
			v.z = homogeneous_pos[2][0];
		}
		for (auto& n : transformed_mesh.normals) {
			Matrix<4, 1> homogeneous_norm;
			homogeneous_norm[0][0] = n.x;
			homogeneous_norm[1][0] = n.y;
			homogeneous_norm[2][0] = n.z;
			homogeneous_norm[3][0] = 1.0f;

			auto rotation = transform;
			rotation[0][3] = 0.0f;
			rotation[1][3] = 0.0f;
			rotation[2][3] = 0.0f;

			homogeneous_norm = rotation * homogeneous_norm;

			n.x = 0.0f;// homogeneous_norm[0][0];
			n.y = 1.0f;// homogeneous_norm[1][0];
			n.z = 0.0f;// homogeneous_norm[2][0];

			//n = normalize(n);
		}
		renderer.attach_mesh(transformed_mesh);
	}
}
