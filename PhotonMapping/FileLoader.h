#ifndef FILELOADER_H
#define FILELOADER_H

#include <string>

namespace FileLoader
{
	/* Contains the data of a binray gltf 2.0 (.glb) file */
	struct GLB_t;

	GLB_t load_glb(std::string path);

}

#endif

