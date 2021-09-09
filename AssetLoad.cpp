#include "AssetLoad.h"
#include <cstdio>
#include <vector>
#include "PPU466.hpp"
#include <string>

TileAssetData AssetAtlas::getTile(std::string name) {
	for (size_t index = 0; index < tileNum; index++) {
		AssetName curTile = tileNameList[index];
		if (curTile.name.compare(name) == 0) return tiles[index];
	}
	std::cout << "error: could not find tile ''" << name << "'' in atlas.\n Using default tile.\n";
	return defaultTileData;
}

BGAssetData AssetAtlas::getBGHelp(std::string name) {
	for (size_t index = 0; index < bgNum; index++) {
		AssetName curBG = bgNameList[index];
		if (curBG.name.compare(name) == 0) return bgs[index];
	}
	std::cout << "error: could not find background ''" << name << "'' in atlas.\n Using default background.\n";
	return defaultBGData;
}

bool AssetAtlas::loadTile(size_t nameSize,char* name, uint64_t* packedTile) {
	if (tiles == NULL || tileNameList == NULL) return false;
	if (tileNum == tiles.size()) {
		tiles.resize(2 * tileNum);
		tileNameList.resize(2 * tileNum);
	}
	tileNum++;

	tileNameList[tileNum].name(name,nameSize);
	tileNameList[tileNum].nameSize = nameSize;
	tiles[tileNum].bit0 = *packedTile;
	tiles[tileNum].bit1 = packedTile[1];
	return true;
}

//@param -in char* packedBackground is the background data that will be stored in the backround, so everything not the tiles
//This is the char array of references, and can be indexed using the size info and the (to be made) indexing functions
size_t AssetAtlas::loadBGRefs(size_t nameSize, char* name, char* packedBackground) {

	auto unPack = [this]() {
		std::array< TileRef, BackgroundWidth* BackgroundHeight > retBackground;
		size_t refSize = sizeof(TileRef);
		for (size_t whichTile = 0; whichTile < BackgroundWidth * BackgroundHeight; whichTile++) {
			TileRef* thisTile = (TileRef*)(packedBackground + whichTile * refSize);
			retBackground[whichTile] = *thisTile;
		}
		return retBackground;
	};

	if (bgs == NULL || bgNameList == NULL) return 0;
	if (bgNum == bgs.size()) {
		bgs.resize(2 * bgNum);
		bgNameList.resize(2 * bgNum);
	}
	bgNum++;

	bgNameList[bgNum].name(name,nameSize);
	bgNameList[bgNum].nameSize = nameSize;
	bgs[bgNum].background = unPack();
	return (size_t)(packedBackground+ (BackgroundWidth * BackgroundHeight * sizeof(TileRef)));
}

BGRetType getBG(size_t nameSize, std::string name) {a
	std::array< TileRef, BackgroundWidth* BackgroundHeight > thisBG =  (getBGHelp(name)).data;
	BGRetType retBG = new BGRetType;
	for (size_t ind = 0; ind < backgroundHeight * backgroundWidth; ind++) {
		TileRef thisTile = thisBG[ind];
		TileAssetData thisTileData = getTile(thisTile.name);
		retBG.tiles[ind].bit0 = thisTileData.bit0;
		retBG.tiles[ind].bit1 = thisTileData.bit1;
		retBG.pallets[ind] = thisTile.pallet;
	}
	return retBG;
}
size_t loadTiles(size_t n, char* in) {
	char* nextTile = in;
	for (int whichTile = 0; whichTile < n; whichTile++) {
		size_t* curSize = (size_t*)nextTile;
		if (curSize == NULL) return 0;
		char* name = (nextTile + 8);
		char* tileData = (name + *curSize);
		nextTile = (tileData + (8 * 2));
		if(!loadTile(*curSize, name, tileData)) return 0;
	}
	return (size_t) nextTile;
}

size_t loadBG(size_t nameSize, char* name, char* packedBackground) {
	size_t* nTiles = (size_t*)packedBackground;
	if (nTiles == NULL) return 0;
	packedBackground = packedBackground + 8;
	packedBackground = (char*) loadTiles(*nTiles, packedBackground);
	if (!packedBackground) return 0;
	return loadBGRefs(nameSize, name, packedBackground);
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


bool loadAssets(/* needs to be file input); //Loads a file of assets
//Open said file in binary mode