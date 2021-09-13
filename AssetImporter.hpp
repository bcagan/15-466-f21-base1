#pragma once
#include <vector>
#include <string>
#include "AssetLoad.h"
#include <glm/glm.hpp>

struct tileSaveData
{
	int64_t bit0;
	int64_t bit1;
	size_t nameStart;
	size_t nameEnd;
};

class AssetImporter {

public:
	//Does all the work in the constructor and destructor. Should do a full
	//replacement for the atlas, with a fresh atlas given by atlas.
	AssetImporter();
	~AssetImporter();

	void writePngToSave(glm::uvec2 size, std::vector< glm::u8vec4 > data, std::string name);

	std::vector<tileSaveData> LoadTiles();

	std::vector<tileSaveData> tilesToSave;
	std::vector<char> namesToSave;

	//AssetAtlas atlas;

private:
	std::string subPath = "\\tiles\\";
	std::string extension = ".png";

	std::vector<std::string> files{ 
		"TestArrow"
	};

	std::vector<std::string> names{
		"Arrow"
	};

	

};