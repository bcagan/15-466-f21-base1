#include "AssetImporter.hpp"
#include "data_path.hpp"
#include "load_save_png.hpp"
#include <glm/glm.hpp>
#include <filesystem>
#include <iostream>
#include "read_write_chunk.hpp"
#include <fstream>
#include "PPU466.hpp"

#if defined(__APPLE__)
#include <unistd.h>
#elif defined(__linux__)
#include <unistd.h>
#endif //WINDOWS



AssetImporter::AssetImporter()
{
	
}

void AssetImporter::LoadPNGS()
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

		std::string img_path = path + subPath + filename + extension;

		glm::uvec2 size;
		std::vector< glm::u8vec4 > data;

		std::cout << "Loading file: " << img_path << std::endl;

#if defined(_WIN32)
		if (std::filesystem::exists(img_path))
		{
			load_png(img_path, &size, &data, LowerLeftOrigin);
			writePngToSave(size, data);
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
		}
		else if (r == EACCES)
		{
			std::cout << "File is not readable!" << std::endl;
		}
		else if (r >= 0) //file read successful
		{
			load_png(img_path, &size, &data, LowerLeftOrigin);
			writePngToSave(size, data);
		}
		else {
			std::cout << "Error with file access" << std::endl;
		}
#endif
	}
}

void AssetImporter::loadLevel(std::string filename, std::vector<glm::vec2> *walls_at, std::vector<glm::vec2> *spikes_at)
{
	std::string path = data_path("");
	//Ends with wrong slash, so we remove and replace it.
	if (path.size() > 0)
	{
		path.pop_back();
	}


	std::string img_path = path + subPath + filename + extension;

	glm::uvec2 size;
	std::vector< glm::u8vec4 > data;

	std::cout << "Loading file: " << img_path << std::endl;

#if defined(_WIN32)
	if (std::filesystem::exists(img_path))
	{
		load_png(img_path, &size, &data, LowerLeftOrigin);
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
	}
	else if (r == EACCES)
	{
		std::cout << "File is not readable!" << std::endl;
	}
	else if (r >= 0) //file read successful
	{
		load_png(img_path, &size, &data, LowerLeftOrigin);
	}
	else {
		std::cout << "Error with file access" << std::endl;
	}
#endif

	spikes_at->clear();
	walls_at->clear();

	for (size_t i = 0; i < size.x; i++)
	{
		for (size_t j = 0; j < size.y; j++)
		{
			size_t bit_index = j * size.x + i;
			glm::u8vec4 pixel = data[bit_index];
			if (pixel.x > 0)
			{
				spikes_at->push_back(glm::vec2(uint8_t(8 * i), uint8_t(8  * j)));
			}
			else if (pixel.y > 0)
			{
				walls_at->push_back(glm::vec2(uint8_t(8 * i), uint8_t(8 * j)));
			}
		}
	}
}

//Cheap an easy way right now is to use the RBG data
void AssetImporter::writePngToSave(glm::uvec2 size, std::vector< glm::u8vec4 > data)
{

	if (size.x != 8 || size.y != 8)
	{
		std::cout << "Size of image is wrong! Aborting..." << std::endl;
		return;
	}

	int64_t bit0 = 0;
	int64_t bit1 = 0;
	(void)bit0;
	(void)bit1;
	
	PPU466::Tile save_data;

	for (int i = 0; i < 8; i++)
	{
		uint8_t bit0 = 0;
		uint8_t bit1 = 0;
		for (int j = 0; j < 8; j++)
		{
			int bit_index = j * 8 + i;
			glm::u8vec4 pixel = data[bit_index];
			std::cout << "Data at " << i << ", " << j << " is: " << (int) pixel.x << std::endl;
			if (pixel.x > 0)
			{
				bit0 = bit0 | (1 << j);
			}
			if (pixel.y > 0)
			{
				bit1 = bit1 | (1 << j);
			}
		}
		save_data.bit0[i] = bit0;
		save_data.bit1[i] = bit1;
		std::cout << (int)bit0 << std::endl;
	}

	tilesToSave.push_back(save_data);
}

void AssetImporter::writeToPPU(PPU466 *ppu, size_t startIndex)
{
	for (int i = 0; i < tilesToSave.size(); i++)
	{
		std::cout << "Writing to index " << (startIndex + i) << std::endl;
		for (int j = 0; j < 8; j++)
		{
			std::cout << (int)tilesToSave[i].bit0[j] << std::endl;
		}
		ppu->tile_table[startIndex + i].bit0 = tilesToSave[i].bit0;
		ppu->tile_table[startIndex + i].bit1 = tilesToSave[i].bit1;
	}
}



/*
void AssetImporter::LoadBackground(AssetAtlas atlas)
{
	std::array<TileRef, PPU466::BackgroundWidth * PPU466::BackgroundHeight> packedBackground = GetPackedBackgroundFromPNG(backgroundName);
	
	unsigned packBackSize = sizeof(TileRef) * packedBackground.size();
	unsigned bgSize = sizeof(size_t*) + packBackSize;
	char *c = new char[bgSize];
	// size of tile array = 0
	for (int i = 0; i < 8; i++)
		c[i] = 0;
	char* pb = &(c[8]);
	for (int i = 0; i < packBackSize; i++)
	{
		pb[i] = reinterpret_cast<char*>(&packedBackground)[i];
	}
	atlas.loadBG(pb);
}

void AssetImporter::LoadLevel(AssetAtlas atlas, std::string levelName)
{
	std::array<TileRef, PPU466::BackgroundWidth * PPU466::BackgroundHeight> packedBackground = GetPackedBackgroundFromPNG(levelName);
	atlas.loadBG(reinterpret_cast<char*>(&packedBackground));
}*/

AssetImporter::~AssetImporter()
{

}