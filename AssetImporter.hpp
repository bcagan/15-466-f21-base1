#pragma once
#include <vector>
#include <string>
#include "AssetLoad.h"
#include <glm/glm.hpp>

class AssetImporter {

public:
	//Does all the work in the constructor and destructor. Should do a full
	//replacement for the atlas, with a fresh atlas given by atlas.
	AssetImporter();
	~AssetImporter();

	void writePngToSave(glm::uvec2 size, std::vector< glm::u8vec4 > data);

	std::vector<int64_t> tilesToSave;

	//AssetAtlas atlas;

private:
	std::string subPath = "\\tiles\\";
	std::string extension = ".png";

	std::vector<std::string> files{ 
		"LeftBlock", 
		"Red",
		"TestArrow"
	};

	

};