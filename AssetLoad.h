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
	uint8_t pallet; //Pallets being used for given tile
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
	std::array< uint8_t, PPU466::BackgroundWidth* PPU466::BackgroundHeight> pallets;
};

struct LevelAsset {
	size_t nameSize; //name and size of the level
	std::string name;
	//Levels are stored as tiles, 64x60
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > tiles;
};

struct LevelAssetData {
	//levels are stored as tiles, 64x60
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > level;
};

struct LevelRetType {
	std::array< TileAssetData, PPU466::BackgroundWidth* PPU466::BackgroundHeight > tiles;
	std::array< uint8_t, PPU466::BackgroundWidth* PPU466::BackgroundHeight> pallets;
};

class AssetAtlas
{
private:

	TileAsset defaultTile; //In case asset isn't loaded, the defaults will be loaded instead
	TileAssetData defaultTileData; 
	TileRef defaultRef;
	BGAsset defaultBG;
	BGAssetData defaultBGData;
	LevelAsset defaultLevel;
	LevelAssetData defaultLevelData;


	//Data strucutres are temporary for example, will be kept as vectors, resized as needed, linear searched.
	
	size_t tileNum = 0;
	std::vector<BGAssetData> bgs;
	std::vector<AssetName> bgNameList;
	size_t bgNum = 0;
	std::vector<LevelAssetData> levels;
	std::vector<AssetName> levelNameList;
	size_t levelNum = 0;

	bool loadTile(size_t nameSize, char* name, uint64_t* packedTile); //Loads an individual tile
	bool loadBGRefs(size_t nameSize, char* name, char* packedBackground, bool isBG); //Loads an individual background
	bool loadBGHelp(size_t nameSize, char* name, char* packedBackground, bool isBG); //Loads an individual background and unique tiles. (Seperates tile array from background)
	bool loadTilesHelp(size_t n, char* in); //Loads an array of tiles1
	char* loadFile(std::string fileName);

	BGAssetData getBGHelp(std::string name); //Searches for an individual background
	LevelAssetData getLevelHelp(std::string name); //Searches for an individual level

	//New updated file formats
	// 
	//Tile Array: # of independent tiles, each tiles name size, name, data, in array (char))
	//
	//Backrground: Array of tiles (see above), BGWidth*BGHeight tile ref array
	//Notes on backround: Array of tiles is tile data itself. Could be unique to background or already loaded. Could be all or none of the tiles referenced.
	//Tile refs are the name of the tile being used and the pallet for that specific tile index.
	//
	//Level Data: No tile array at all, each ref's name consist purley of the id (#) of the object type, refs are just the id name and pallet (Ill leave what to
	//do with the pallet up to you)

public:

	AssetAtlas() {
		tiles = std::vector<TileAssetData>(256); //We can store more than 256 tiles at a time, we might want to swap in and out tiles
		//(We wouldn't need to do this for this project, but if we decide to use PPU for the final it might be useful to be able to load in more at the start)
		tileNameList.resize(256);
		bgs.resize(16);
		bgNameList.resize(16);
		levels.resize(8);
		levelNameList.resize(8);
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
		defaultRef.pallet = 0;


		defaultBG.name = "Default";
		defaultBG.nameSize = 7;
		for (int ind = 0; ind < PPU466::BackgroundWidth * PPU466::BackgroundHeight; ind++) {
			defaultBG.background[ind] = defaultRef;
		}
		defaultBGData.background = defaultBG.background;

		defaultLevel.name = "Default";
		defaultLevel.nameSize = 7;
		for (int ind = 0; ind < PPU466::BackgroundWidth * PPU466::BackgroundHeight; ind++) {
			defaultLevel.tiles[ind] = defaultRef;
		}
		defaultLevelData.level = defaultLevel.tiles;
		tiles[0] = defaultTileData;
		tileNameList[0] = {
			defaultTile.nameSize,
			defaultTile.name
		};
	};
	~AssetAtlas() {};

	std::vector<TileAssetData> tiles;
	std::vector<AssetName> tileNameList;

	TileAssetData getTile(std::string name); //Gives tile of given name
	BGRetType getBG(std::string name); //Searches for an individual background
	LevelRetType getLevel(std::string name);//Searches for individual level

	size_t loadTiles(std::string fileName); //Loads an array of tiles
	bool loadBG(std::string fileName);  //Loads an array of backgrounds
	bool loadBG(char *packedBackground);  //Loads an array of backgrounds
	bool loadLevel(std::string fileName); //Loads a level, which is encoded as a background

};
