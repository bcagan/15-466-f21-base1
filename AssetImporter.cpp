#include "AssetImporter.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>

#if defined(__APPLE__)
#include <unistd.h>
#elif defined(__linux__)
#include <unistd.h>
#endif //WINDOWS



AssetImporter::AssetImporter()
{
	for (std::string filename : files)
	{
		std::string path = data_path("");

		//Ends with wrong slash, so we remove and replace it.
		if (path.size() > 0)
		{
			path.pop_back();
		}
		path = path + subPath + filename + extension;

		glm::uvec2 size;
		std::vector< glm::u8vec4 > data;

		std::cout << "Loading file: " << path << std::endl;
		
	#if defined(_WIN32)
		if (std::filesystem::exists(path))
		{
			load_png(path, &size, &data, LowerLeftOrigin);
			std::cout << "File is " << size.x << ", " << size.y << std::endl;
		}
		else
		{
			std::cout << "File does not exist!";
		}
	#else
		// code for access based on https://stackoverflow.com/questions/8580606/c-c-mac-os-x-check-if-file-exists
		int r = access(path.c_str(), R_OK);
		if (r == ENOENT)
		{
			std::cout << "File did not exist!" << std::endl;
		} else if (r == EACCES)
		{
			std::cout << "File is not readable!" << std::endl;
		} else if (r > 0) //file read successful
		{
			load_png(path, &size, &data, LowerLeftOrigin);
			std::cout << "File is " << size.x << ", " << size.y << std::endl;
		} else {
			std::cout << "Error with file access" << std::endl;
		}
	#endif

	}
}

AssetImporter::~AssetImporter()
{

}