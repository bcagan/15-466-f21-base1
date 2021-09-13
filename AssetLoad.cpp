#include "AssetLoad.h"
#include <cstdio>
#include <vector>
#include "PPU466.hpp"
#include <string>
#include "data_path.hpp"
#include <iostream>
#include <fstream> 


//Trys to find (linear search) a tile with the name given by the user. If it doesn't exist, gives default tile
TileAssetData AssetAtlas::getTile(std::string name) {
	for (size_t index = 0; index < tileNum; index++) {
		AssetName curTile = tileNameList[index];
		if (curTile.name.compare(name) == 0) return tiles[index];
	}
	std::cout << "error: could not find tile ''" << name << "'' in atlas.\n Using default tile.\n";
	return defaultTileData;
}

//Trys to find (linear search) a background with the name given by the user. If it doesn't exist, gives default tile
//Gives background in refs, needs to still be converted to final background data type
BGAssetData AssetAtlas::getBGHelp(std::string name) {
	for (size_t index = 0; index < bgNum; index++) {
		AssetName curBG = bgNameList[index];
		if (curBG.name.compare(name) == 0) return bgs[index];
	}
	std::cout << "error: could not find background ''" << name << "'' in atlas.\n Using default background.\n";
	return defaultBGData;
}

//Takes a user's inputted name and tries to find a background using getBHHelp matching it. If it can't find it, uses default background
//Coverts refs into proper background tiles with the pallets from the refs.
BGRetType AssetAtlas::getBG(std::string name) {
	std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > thisBG = (getBGHelp(name)).background; //Get reference array
	BGRetType retBG;
	for (size_t ind = 0; ind < PPU466::BackgroundHeight * PPU466::BackgroundWidth; ind++) {
		TileRef thisTile = thisBG[ind]; //Get tile from each reference and put into the return aray
		TileAssetData thisTileData = getTile(thisTile.name);
		retBG.tiles[ind].bit0 = thisTileData.bit0;
		retBG.tiles[ind].bit1 = thisTileData.bit1;
		retBG.pallets[ind] = thisTile.pallet;
	}
	return retBG;
}

//Need a level search function

//Loads the given tile data, name, and name size, into a new entry at the end of the tile vector
bool AssetAtlas::loadTile(size_t nameSize,char* name, uint64_t* packedTile) {
	auto unPack = [this](uint64_t* thisTile) {
		(void)this;
		std::array< uint8_t, 8 > bit;
		for (size_t ind = 0; ind < 8; ind++) {
			bit[ind] = ((uint8_t*)thisTile)[ind];
		}
		return bit;
	};
	if (tiles.size() != tileNameList.size()) {
		if (tileNum == tiles.size()) tileNameList.resize(tileNum);
		else tiles.resize(tileNum);
	}
	if (tileNum == tiles.size()) { //If needed, extend vector
		tiles.resize(2 * tileNum);
		tileNameList.resize(2 * tileNum);
	}
	if (packedTile == NULL) return false;
	TileAssetData newTile;
	AssetName newTileName;
	newTileName.name = std::string(name,nameSize); //Load information into arrray
	newTileName.nameSize = nameSize;
	tileNameList[tileNum] = newTileName;
	newTile.bit0 = unPack(packedTile);
	newTile.bit1 = unPack(packedTile+1);
	tiles[tileNum] = newTile;
	assert(tiles.size() == tileNameList.size());
	tileNum++;

	return true;
}

//Loads the reference data from the background data loaded in previously into the last entry in the background data vector
bool AssetAtlas::loadBGRefs(size_t nameSize, char* name, char* packedBackground, bool isBG) {

	auto unPack = [this](char* backgroundRef) { //Lambda to interpret packed background data as an array of tileRefs
		//BackgroundHeight * BackgroundWidth in size
		std::array< TileRef, PPU466::BackgroundWidth* PPU466::BackgroundHeight > retBackground;
		size_t refSize = sizeof(TileRef);
		(void)refSize;
		(void)this;
		for (size_t whichTile = 0; whichTile < PPU466::BackgroundWidth * PPU466::BackgroundHeight; whichTile++) {
			retBackground[whichTile] = ((TileRef*)(backgroundRef))[whichTile];
		}
		return retBackground;
	};

	if (packedBackground == NULL) return (char*)false; //Safety check (NULL pointer is same as false in this case)
	if (bgNum == bgs.size()) { //If needed, resize
		bgs.resize(2 * bgNum);
		bgNameList.resize(2 * bgNum);
	}
	if (levelNum == levels.size()) {
		levels.resize(2 * levelNum);
		levelNameList.resize(2 * levelNum);
	}
	if (isBG)
		bgNum++;
	else
		levelNum++;

	if(isBG){
		bgNameList[bgNum].name = std::string(name,nameSize); //Load name, nameSize, and newly created tileRef array into back of bg data vector
		bgNameList[bgNum].nameSize = nameSize;
		bgs[bgNum].background = unPack(packedBackground);
	}
	else {
		levelNameList[levelNum].name = std::string(name, nameSize); //Load name, nameSize, and newly created tileRef array into back of bg data vector
		levelNameList[levelNum].nameSize = nameSize;
		//levels[bgNum].level = //Put your unpacking function here;
	}
	return true; //Return adress (as size_t) of next background
}
/*
*/


bool AssetAtlas::loadTilesHelp(size_t n, char* in) { //Given the adress of a size n tile array, interpret the bytes into an array and load into the atlas
	char* nextTile = in; //Begin marker that will be used to indicate the next tile to be loaded
	for (int whichTile = 0; whichTile < n; whichTile++) {
		size_t* curSize = (size_t*)nextTile; //First 8 bytes of a tile are the name size
		if (curSize == NULL) return false;
		char* name = (nextTile + 8); //Name follows the name size
		char* tileData = (name + *curSize); //The data of the tile itself is the rest current tile
		nextTile = (tileData + (8 * 2)); //The data takes up 2 uint64_ts, followed by the next tile
		if(!loadTile(*curSize, name, (uint64_t*)tileData)) return false;
	}
	return (size_t)nextTile;
}


//Takes a file name in the path of the game, and loads the data into a char*
char* AssetAtlas::loadFile(std::string fileName) {
	std::string path = data_path(fileName);
	std::ifstream assetFile = std::ifstream(path.c_str(), std::ios::binary);
	assert(assetFile.is_open());

	auto getSize = [this](std::ifstream assetFile) {
		size_t fChars = 0;
		while (assetFile.get() != EOF) {
			fChars += 1;
		}
		return fChars;
	};
	size_t fChars = getSize(assetFile);
	char* dataString = new char[fChars + 1]; //I'm sorry for doing Malloc, we couldn't find a different solution
	dataString[fChars] = '\0';
	assetFile.get(dataString, fChars); //Load file contents into string
	assetFile.close();
	return dataString;
}

//Wrapper function that takes a file name and sets up the data to load a tile array
size_t AssetAtlas::loadTiles(std::string fileName) {
	char* fileData = loadFile(fileName); //Get data from file
	size_t n = *((size_t*)fileData);
	size_t resVal = loadTilesHelp(n, (fileData + 8));
	free(fileData);
	return resVal;
}

//Wrapper for loading a backgrounds tile and tile reference arrays
bool AssetAtlas::loadBGHelp(size_t nameSize, char* name, char* packedBackground, bool isBG) { //isBG == true -> background, isBG == false -> level data
	if (isBG) {
		size_t* nTiles = (size_t*)packedBackground;
		if (nTiles == NULL) return 0;
		packedBackground = packedBackground + 8; //First load number of tiles, and the tileArray itself
		packedBackground = (char*)(loadTiles(*nTiles));  //The return value is the start of the reference array
	}
	if (!packedBackground) return 0; 
	return loadBGRefs(nameSize, name, packedBackground, isBG);  //Load the reference array
}

bool AssetAtlas::loadBG(std::string fileName) {  //Loads a file for a  background
	char* in = loadFile(fileName); //Get data from file
	size_t* nameSize = (size_t*)in;
	if (nameSize == NULL) return false;
	char* name = (in + 8);
	char* bgArray = (name + *nameSize);
	bool resVal =  (loadBGHelp(*nameSize, name, bgArray,true)); //Load background given extracted variables
	free(in);
	return resVal;
}


