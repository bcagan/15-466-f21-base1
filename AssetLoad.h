#pragma once
#include<vector>
#include "PPU466.hpp"
#include <glm/glm.hpp>

struct TileAsset { //nameSize is length of string, must be loaded in by our program
	size_t nameSize; 
	std::string name; //Name in dictionary to be reffered to by code later
	uint64_t bit0; //8x8 tile representing first 2 colors, bottom is top row, left to right
	uint64_t bit1; //Second 2 colors (3,4)
};

struct TileAssetData { //Tile without name info
	uint64_t bit0; //8x8 tile representing first 2 colors, bottom is top row, left to right
	uint64_t bit1; //Second 2 colors (3,4)
};

struct AssetName {
	size_t nameSize;
	std::string name; //Name in dictionary to be reffered to by code later
};

struct TileRef {
	size_t nameSize; //Name of tile being referenced, and its size
	std::string name;
	std::array< glm::u8vec4, 4> pallet; //Pallets being used for given tile
};

struct BGAsset {
	size_t nameSize; //Name and size of name of background
	std::string name;
	//Backgrounds are stored as tiles,  64x60
	std::array< TileRef, BackgroundWidth* BackgroundHeight > background
	//Stupid programming: All backgrounds will be inputted with unique (even if repeated with perviously loaded) set of tiles
	//followed by its set of refs
};

struct BGAssetData {
	//Backgrounds are stored as tiles,  64x60
	std::array< TileRef, BackgroundWidth* BackgroundHeight > background;
	//Stupid programming: All backgrounds will be inputted with unique (even if repeated with perviously loaded) set of tiles
	//followed by its set of refs
};

struct BGRetType {
	std::array< Tile, BackgroundWidth* BackgroundHeight > tiles;
	std::array< std::array< glm::u8vec4, 4>, BackgrondWidth* BackgroundHeight> pallets;
};

class AssetAtlas
{
public:
	AssetAtlas() = {
		defaultTile.bit0 = {
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010, };
		defaultTile.bit1 = {
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101, };
		defaultTile.name = "Default";
		defaultTile.nameSize = 7;

		defaultTileData.bit0 = defaultTile.bit0;
		defaultTileData.bit1 = defaultTile.bit1;

		defaultRef.nameSize = 7;
		defaultRef.name = "Default";
		default.pallet = {
			glm::u8vec4(0xFF, 0x00, 0x00, 0xFF),
			glm::u8vec4(0x00, 0xFF, 0x00, 0xFF),
			glm::u8vec4(0x00, 0x00, 0xFF, 0xff),
			glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		};


		defaultBG.name = "Default";
		defaultBG.nameSize = 7;
		for (int ind = 0; ind < BackgroundWidth * BackgroundHeight; ind++) {
			defaultBG.background[ind] = defaultRef;
		}
		defaultBGData.background = defaultBG.background;

	};
	~AssetAtlas();

	TileAssetData getTile(size_t nameSize, std::string name); //Gives tile of given name
	BGRetType getBG(size_t nameSize, std::string name); //Searches for an individual background

	bool loadAssets(/* needs to be file input*/); //Loads a file of assets

private:

	TileAsset defaultTile; //In case asset isn't loaded, the defaults will be loaded instead
	TileAssetData defaultTileData; 

	TileRef defaultRef;
	BGAsset defaultBG;
	BGAssetData defaultBGData;

	//Data strucutres are temporary for example, will be kept as vectors, resized as needed, linear searched.
	std::vector<TileAssetData> tiles(1024);
	std::vector<AssetName> tileNameList(1024);
	size_t tileNum = 0;
	std::vector<BGAssetData> bgs(16);
	std::vector<AssetNum> bgNameList(1024);
	size_t bgNum = 0;

	bool loadTile(size_t nameSize, char* name, uint64_t* packedTile); //Loads an individual tile
	bool loadBGRefs(size_t nameSize, char* name, char* packedBackground); //Loads an individual background
	bool loadBG(size_t nameSize, char* name, char* packedBackground); //Loads an individual background and unique tiles. (Seperates tile array from background)

	bool loadTiles(char* in); //Loads an array of tiles
	bool loadBGs(char* in);  //Loads an array of backgrounds

	BGAsset getBGHelp(size_t nameSize, char* name); //Searches for an individual background

	//Data structure of assets loaded in should then be
	//# of independent tiles, each tiles name size, name, data, in array (char))
	//# of backgrounds
	//# of bg tiles, each tiles name size, name, data, in array (char)), array of refs each packed as expected (also char array) (this is for each INDIVIDUAL BG)

};
