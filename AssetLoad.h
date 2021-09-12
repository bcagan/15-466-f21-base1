#pragma once
#include<vector>
#include "PPU466.hpp"
#include <glm/glm.hpp>
#include <string>


struct TileAsset { //nameSize is length of string, must be loaded in by our program
	size_t nameSize; 
	std::string name; //Name in dictionary to be reffered to by code later
	std::array< uint8_t, 8 > bit0; //8x8 tile representing first 2 colors, bottom is top row, left to right
	std::array< uint8_t, 8 > bit1; //Second 2 colors (3,4)
};

struct TileAssetData { //Tile without name info
	std::array< uint8_t, 8 > bit0; //8x8 tile representing first 2 colors, bottom is top row, left to right
	std::array< uint8_t, 8 > bit1; //Second 2 colors (3,4)
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
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > background;
	//Stupid programming: All backgrounds will be inputted with unique (even if repeated with perviously loaded) set of tiles
	//followed by its set of refs
};

struct BGAssetData {
	//Backgrounds are stored as tiles,  64x60
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > background;
	//Stupid programming: All backgrounds will be inputted with unique (even if repeated with perviously loaded) set of tiles
	//followed by its set of refs
};

struct BGRetType {
	std::array< TileAssetData, PPU466::BackgroundWidth* PPU466::BackgroundHeight > tiles;
	std::array< std::array< glm::u8vec4, 4>, PPU466::BackgroundWidth* PPU466::BackgroundHeight> pallets;
};

struct LevelAsset {
	size_t nameSize; //name and size of the level
	std::string name;
	//Levels are stored as tiles, 64x60
	std::array< TileAssetData, PPU466::BackgroundWidth* PPU466::BackgroundHeight > tiles;
};

struct LevelAssetData {
	//levels are stored as tiles, 64x60
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > level;
};

struct LevelRetType {
	std::array< TileAssetData, PPU466::BackgroundWidth* PPU466::BackgroundHeight > tiles;
	std::array< std::array< glm::u8vec4, 4>, PPU466::BackgroundWidth* PPU466::BackgroundHeight> pallets;
};

class AssetAtlas
{
private:

	TileAsset defaultTile; //In case asset isn't loaded, the defaults will be loaded instead
	TileAssetData defaultTileData; 
	TileRef defaultRef;
	BGAsset defaultBG;
	BGAssetData defaultBGData;

	//Data strucutres are temporary for example, will be kept as vectors, resized as needed, linear searched.
	std::vector<TileAssetData> tiles;
	std::vector<AssetName> tileNameList;
	size_t tileNum = 0;
	std::vector<BGAssetData> bgs;
	std::vector<AssetName> bgNameList;
	size_t bgNum = 0;

	bool loadTile(size_t nameSize, char* name, uint64_t* packedTile); //Loads an individual tile
	char* loadBGRefs(size_t nameSize, char* name, char* packedBackground); //Loads an individual background
	char* loadBG(size_t nameSize, char* name, char* packedBackground); //Loads an individual background and unique tiles. (Seperates tile array from background)

	char* loadTiles(size_t n, char* in); //Loads an array of tiles
	bool loadBGs(size_t n, char* in);  //Loads an array of backgrounds

	BGAssetData getBGHelp(std::string name); //Searches for an individual background
	bool loadHelper(char* in);

	//Data structure of assets loaded in should then be
	//# of independent tiles, each tiles name size, name, data, in array (char))
	//# of backgrounds
	//# of bg tiles, each tiles name size, name, data, in array (char)), array of refs each packed as expected (also char array) (this is for each INDIVIDUAL BG)

public:

	AssetAtlas() {
		tiles = std::vector<TileAssetData>(256); //PPU can only have max of 256 tiles
		tileNameList.resize(256);
		bgs.resize(16);
		bgNameList.resize(1024);
		defaultTile.bit0 =
		{	0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101, };
		defaultTile.bit1 =
		{	0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010,
			0b01010101,
			0b10101010, };
		defaultTile.name = "Default";
		defaultTile.nameSize = 7;

		defaultTileData.bit0 = defaultTile.bit0;
		defaultTileData.bit1 = defaultTile.bit1;

		defaultRef.nameSize = 7;
		defaultRef.name = "Default";
		defaultRef.pallet = {
			glm::u8vec4(0xFF, 0x00, 0x00, 0xFF),
			glm::u8vec4(0x00, 0xFF, 0x00, 0xFF),
			glm::u8vec4(0x00, 0x00, 0xFF, 0xff),
			glm::u8vec4(0x00, 0x00, 0x00, 0xff),
		};


		defaultBG.name = "Default";
		defaultBG.nameSize = 7;
		for (int ind = 0; ind < PPU466::BackgroundWidth * PPU466::BackgroundHeight; ind++) {
			defaultBG.background[ind] = defaultRef;
		}
		defaultBGData.background = defaultBG.background;

	};
	~AssetAtlas() {};

	TileAssetData getTile(std::string name); //Gives tile of given name
	BGRetType getBG(std::string name); //Searches for an individual background
	LevelRetType getLevel(std::string name);//Searched for individual level

	bool loadAssets(std::string fileName); //Loads a file of assets
	


};
