#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>
#include "utils.h"

namespace File
{
	struct GLTF_t;

	GLTF_t* load_glb(std::string path);

	std::vector<geometry::Mesh> extract_meshes(GLTF_t* file, unsigned int scene_index);

}

#endif

