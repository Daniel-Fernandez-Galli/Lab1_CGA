#ifndef FILELOADER_H
#define FILELOADER_H

#include "tiny_gltf.h"
#include <string>

namespace File
{

	tinygltf::Model load_glb(std::string path);

}

#endif

