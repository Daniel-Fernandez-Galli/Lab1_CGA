#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include "utils.h"
#include "MeshObject.h"
#include "Camera.h"

namespace File
{
	struct GLTF_t;

	GLTF_t* load_glb(std::string path);

	std::vector<MeshObject> extract_meshes(GLTF_t* file, unsigned int scene_index = 0);

	std::vector<CamConstructorData> extract_cameras(GLTF_t* file, unsigned int scene_index = 0);

}

#endif

