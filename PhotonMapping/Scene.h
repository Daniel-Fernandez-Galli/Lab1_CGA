#ifndef SCENE_H
#define SCENE_H

#include "MeshObject.h"
#include "utils.h"

#include <embree4/rtcore.h>
#include <map>
#include <memory>

class Scene
{
private:

	RTCDevice rtcdevice;

	RTCScene rtcscene;

	std::map<unsigned int, std::shared_ptr<geometry::Mesh>> meshes;

	std::map<unsigned int, Material> materials;

	unsigned int last_id = 0;

public:

	Scene();

	void attach_mesh(std::shared_ptr<geometry::Mesh> mesh_ptr);

	void attach_material(Material material);

	void commit_scene();

	void ray_intersect(RTCRayHit& rayhit);

	std::array<math::Vector3, 3> get_shading_normals(unsigned int geom_id, unsigned int prim_id);

	Material get_material(unsigned int geom_id);

};

#endif

