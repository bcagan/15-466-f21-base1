#include "AssetImporter.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>

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

		if (std::filesystem::exists(path))
		{
			load_png(path, &size, &data, LowerLeftOrigin);
			std::cout << "File is " << size.x << ", " << size.y << std::endl;
		}
		else
		{
			std::cout << "File did not exist!" << std::endl;
		}

	}
}

AssetImporter::~AssetImporter()
{

}