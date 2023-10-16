#include "Scene.h"

using namespace math;

Scene::Scene() {
	rtcdevice = rtcNewDevice(nullptr);
	rtcscene = rtcNewScene(rtcdevice);
}

void Scene::attach_mesh(std::shared_ptr<geometry::Mesh> mesh_ptr)
{
	auto& mesh = *mesh_ptr;

	RTCGeometry geo = rtcNewGeometry(rtcdevice, RTC_GEOMETRY_TYPE_TRIANGLE);

	auto positions = reinterpret_cast<const float*>(mesh.positions.data());

	rtcSetSharedGeometryBuffer(
		geo,
		RTC_BUFFER_TYPE_VERTEX,   // Buffer type (vertices)
		0,                        // Slot
		RTC_FORMAT_FLOAT3,        // Data format (3 floats for x, y, z)
		positions,                 // Pointer to vertex data
		0,                        // Byte offset (0 since it's the first buffer)
		sizeof(float) * 3,        // Stride (size of a single vertex)
		mesh.positions.size()                        // Number of vertices
	);

	auto indices = reinterpret_cast<const unsigned int*>(mesh.indices.data());
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
	unsigned int id = rtcAttachGeometry(rtcscene, geo);
	rtcReleaseGeometry(geo);

	meshes[id] = mesh_ptr;

	last_id = id;

}

void Scene::attach_material(Material material)
{
	materials[last_id] = material;
}

void Scene::commit_scene()
{
	rtcCommitScene(rtcscene);
}

void Scene::ray_intersect(RTCRayHit& rayhit)
{
	rtcIntersect1(rtcscene, &rayhit);
}

std::array<math::Vector3, 3> Scene::get_shading_normals(unsigned int geom_id, unsigned int prim_id) {
	Vector3 Ns1, Ns2, Ns3;
	auto mesh_iter = meshes.find(geom_id);
	auto& mesh = *mesh_iter->second;

	unsigned int index_1 = mesh.indices[3 * prim_id];
	unsigned int index_2 = mesh.indices[3 * prim_id + 1];
	unsigned int index_3 = mesh.indices[3 * prim_id + 2];

	Ns1 = mesh.normals[index_1];
	Ns2 = mesh.normals[index_2];
	Ns3 = mesh.normals[index_3];

	return { Ns1, Ns2, Ns3 };
}

Material Scene::get_material(unsigned int geom_id)
{
	return materials[geom_id];
}
