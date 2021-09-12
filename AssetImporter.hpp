#pragma once
#include <vector>
#include <string>
#include "AssetLoad.hpp"

class AssetImporter {

public:
	//Does all the work in the constructor and destructor. Should do a full
	//replacement for the atlas, with a fresh atlas given by atlas.
	AssetImporter();
	~AssetImporter();

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