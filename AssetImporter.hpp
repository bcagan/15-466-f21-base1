#pragma once
#include <vector>
#include <string>
#include "AssetLoad.h"
#include <glm/glm.hpp>
#include "PPU466.hpp"

class AssetImporter {

public:
	//Does all the work in the constructor and destructor. Should do a full
	//replacement for the atlas, with a fresh atlas given by atlas.
	AssetImporter();
	~AssetImporter();

	void writePngToSave(glm::uvec2 size, std::vector< glm::u8vec4 > data);
	std::array<glm::u8vec4, PPU466::BackgroundWidth * PPU466::BackgroundHeight> GetBackgroundFromPNG();

	void LoadPNGS();

	void LoadTiles(AssetAtlas atlas);

	std::array<TileRef, PPU466::BackgroundWidth * PPU466::BackgroundHeight> GetPackedBackgroundFromPNG(std::string bgName);
	void LoadBackground(AssetAtlas atlas);
	void LoadLevel(AssetAtlas atlas, std::string levelName);
	void writeToPPU(PPU466 *ppu, size_t startIndex);

	std::vector<PPU466::Tile> tilesToSave;
	//AssetAtlas atlas;

private:
	#if defined(_WIN32)
	std::string subPath = "\\tiles\\";
	#else
	std::string subPath = "/tiles/";
	#endif
	std::string extension = ".png";

	std::string backgroundName = "backgrounds/bg";

	std::vector<std::string> files{ 
		"TestArrow"
	};

};