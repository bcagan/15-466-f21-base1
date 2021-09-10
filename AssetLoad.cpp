#include "AssetLoad.h"
#include <cstdio>
#include <vector>
#include "PPU466.hpp"
#include <string>
#include "data_path.hpp"

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
BGRetType getBG(std::string name) {
	std::array< TileRef, BackgroundWidth* BackgroundHeight > thisBG = (getBGHelp(name)).data; //Get reference array
	BGRetType retBG = new BGRetType;
	for (size_t ind = 0; ind < backgroundHeight * backgroundWidth; ind++) {
		TileRef thisTile = thisBG[ind]; //Get tile from each reference and put into the return aray
		TileAssetData thisTileData = getTile(thisTile.name);
		retBG.tiles[ind].bit0 = thisTileData.bit0;
		retBG.tiles[ind].bit1 = thisTileData.bit1;
		retBG.pallets[ind] = thisTile.pallet;
	}
	return retBG;
}

//Loads the given tile data, name, and name size, into a new entry at the end of the tile vector
bool AssetAtlas::loadTile(size_t nameSize,char* name, uint64_t* packedTile) {
	if (tiles == NULL || tileNameList == NULL) return false; //Safety check
	if (tileNum == tiles.size()) { //If needed, extend vector
		tiles.resize(2 * tileNum);
		tileNameList.resize(2 * tileNum);
	}
	tileNum++;

	tileNameList[tileNum].name(name,nameSize); //Load information into arrray
	tileNameList[tileNum].nameSize = nameSize;
	tiles[tileNum].bit0 = *packedTile;
	tiles[tileNum].bit1 = packedTile[1];
	return true;
}

//Loads the reference data from the background data loaded in previously into the last entry in the background data vector
size_t AssetAtlas::loadBGRefs(size_t nameSize, char* name, char* packedBackground) {

	auto unPack = [this]() { //Lambda to interpret packed background data as an array of tileRefs
		//BackgroundHeight * BackgroundWidth in size
		std::array< TileRef, BackgroundWidth* BackgroundHeight > retBackground;
		size_t refSize = sizeof(TileRef);
		for (size_t whichTile = 0; whichTile < BackgroundWidth * BackgroundHeight; whichTile++) {
			retBackground[whichTile] = ((TileRef*)(packedBackground))[whichTile];
		}
		return retBackground;
	};

	if (bgs == NULL || bgNameList == NULL) return 0; //Safety check (NULL pointer is same as false in this case)
	if (bgNum == bgs.size()) { //If needed, resize
		bgs.resize(2 * bgNum);
		bgNameList.resize(2 * bgNum);
	}
	bgNum++;

	bgNameList[bgNum].name(name,nameSize); //Load name, nameSize, and newly created tileRef array into back of bg data vector
	bgNameList[bgNum].nameSize = nameSize;
	bgs[bgNum].background = unPack();
	return (size_t)(packedBackground+ (BackgroundWidth * BackgroundHeight * sizeof(TileRef))); //Return adress (as size_t) of next background
}

size_t loadTiles(size_t n, char* in) { //Given the adress of a size n tile array, interpret the bytes into an array and load into the atlas
	char* nextTile = in; //Begin marker that will be used to indicate the next tile to be loaded
	for (int whichTile = 0; whichTile < n; whichTile++) { 
		size_t* curSize = (size_t*)nextTile; //First 8 bytes of a tile are the name size
		if (curSize == NULL) return 0;
		char* name = (nextTile + 8); //Name follows the name size
		char* tileData = (name + *curSize); //The data of the tile itself is the rest current tile
		nextTile = (tileData + (8 * 2)); //The data takes up 2 uint64_ts, followed by the next tile
		if(!loadTile(*curSize, name, tileData)) return 0;
	}
	return (size_t) nextTile;
}

//Wrapper for loading a backgrounds tile and tile reference arrays
size_t loadBG(size_t nameSize, char* name, char* packedBackground) {
	size_t* nTiles = (size_t*)packedBackground; 
	if (nTiles == NULL) return 0;
	packedBackground = packedBackground + 8; //First load number of tiles, and the tileArray itself
	packedBackground = (char*) loadTiles(*nTiles, packedBackground);  //The return value is the start of the reference array
	if (!packedBackground) return 0; 
	return loadBGRefs(nameSize, name, packedBackground);  //Load the reference array
}

bool loadBGs(size_t n, char* in) {  //Loads an array of backgrounds*/
	char* nextBG = in;
	for (size_t index = 0; index < n; index++) {
		size_t* nameSize = (size_t*)nextBG;
		if (nameSize == NULL) return false;
		char* name = (nextBG + 8);
		char* bgArray = (name + *nameSize);
		nextBG = (char*)loadBG(nameSize, name, bgArray);
		if (nextBG == NULL) return false;
	}
	return true;
}

bool loadHelper(char* in) {
	size_t* numTiles = (size_t*)in;
	if (numTiles == NULL) return false;
	char* tileData = (in + 8);
	char* bgStart = loadTiles(numTiles, tileData);
	if (bgStart == NULL) return false;
	size_t* numBGs = (size_t*)bgStart;
	return loadBGs(*numBGs, (bgStart + 8));
}

//Temporary file loading code that I am very very unsure of
bool loadAssets(std::string fileName) {
	std::string path = data_path(fileName);
	std::ifstream assetFile(path, std::ios::binary);
	assert(assetFile.isOpen());
	//File size code from https://stackoverflow.com/questions/10712117/how-to-count-the-characters-in-a-text-file
	assetFile.seekg(0, std::ios_base::end);
	size_t fileSize = (size_t) std::ios_base::streampos end_pos = assetFile.tellg();
	char* dataString[fileSize];
	assetFile.get(dataString, fileSize);
	if (!assetFile) return false;
	assetFile.close();
	return loadHelper(dataString);
}

