#include "AssetImporter.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>
#include "read_write_chunk.hpp"
#include <fstream>

#if defined(__APPLE__)
#include <unistd.h>
#elif defined(__linux__)
#include <unistd.h>
#endif //WINDOWS



AssetImporter::AssetImporter()
{
	std::string path = data_path("");
	//Ends with wrong slash, so we remove and replace it.
	if (path.size() > 0)
	{
		path.pop_back();
	}

	for (int i = 0; i < files.size(); i++)
	{
		std::string filename = files[i];
		std::string asset_name = names[i];

		std::string img_path = path + subPath + filename + extension;

		glm::uvec2 size;
		std::vector< glm::u8vec4 > data;

		std::cout << "Loading file: " << img_path << std::endl;
		
	#if defined(_WIN32)
		if (std::filesystem::exists(img_path))
		{
			load_png(img_path, &size, &data, LowerLeftOrigin);
			writePngToSave(size, data, asset_name);
		}
		else
		{
			std::cout << "File does not exist!";
		}
	#else
		// code for access based on https://stackoverflow.com/questions/8580606/c-c-mac-os-x-check-if-file-exists
		int r = access(img_path.c_str(), R_OK);
		if (r == ENOENT)
		{
			std::cout << "File did not exist!" << std::endl;
		} else if (r == EACCES)
		{
			std::cout << "File is not readable!" << std::endl;
		} else if (r > 0) //file read successful
		{
			load_png(img_path, &size, &data, LowerLeftOrigin);
			writePngToSave(size, data, asset_name);
		} else {
			std::cout << "Error with file access" << std::endl;
		}
	#endif

		//Tiles are now all pushed back, so we need to write them out!
		std::ofstream ofsTiles(data_path("tileSave.dat"), std::ofstream::out);
		write_chunk("tile", tilesToSave, &ofsTiles);

		std::ofstream ofsNames(data_path("nameSave.dat"), std::ofstream::out);
		write_chunk("name", namesToSave, &ofsNames);
	}
}

//Cheap an easy way right now is to use the RBG data
void AssetImporter::writePngToSave(glm::uvec2 size, std::vector< glm::u8vec4 > data, std::string name)
{
	if (size.x != 8 || size.y != 8)
	{
		std::cout << "Size of image is wrong! Aborting..." << std::endl;
		return;
	}

	int64_t bit0 = 0;
	int64_t bit1 = 0;
	long longOne = 1;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			int bit_index = j * 8 + i;
			glm::u8vec4 pixel = data[bit_index];
			if (pixel.x > 0)
			{
				bit0 = bit0 | longOne << bit_index;
			}
			if (pixel.y > 0)
			{
				bit1 = bit1 | longOne << bit_index;
			}
		}
	}

	tileSaveData save_data;

	save_data.bit0 = bit0;
	save_data.bit1 = bit1;
	save_data.nameStart = namesToSave.size();
	save_data.nameEnd = namesToSave.size() + name.size();

	tilesToSave.push_back(save_data);

	//Copy the name in!
	std::copy(name.begin(), name.end(), std::back_inserter(namesToSave));


}

std::vector<tileSaveData> AssetImporter::LoadTiles()
{

}

AssetImporter::~AssetImporter()
{

}