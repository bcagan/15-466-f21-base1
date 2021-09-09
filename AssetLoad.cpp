#include "AssetLoad.h"
#include <cstdio>
#include <vector>
#include "PPU466.hpp"

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
	//name needs to be set to a string
	//tileNameList[tileNum].name = name;
	tileNameList[tileNum].nameSize = nameSize;
	tiles[tileNum].bit0 = *packedTile;
	tiles[tileNum].bit1 = packedTile[1];
	return true;
}

//@param -in char* packedBackground is the background data that will be stored in the backround, so everything not the tiles
//This is the char array of references, and can be indexed using the size info and the (to be made) indexing functions
bool AssetAtlas::loadBGRefs(size_t nameSize, char* name, char* packedBackground) {
	if (bgs == NULL || bgNameList == NULL) return false;
	if (bgNum == bgs.size()) {
		bgs.resize(2 * bgNum);
		bgNameList.resize(2 * bgNum);
	}
	bgNum++;
	//name needs to be set to a string
	//bgNameList[bgNum].name = name;
	bgNameList[bgNum].nameSize = nameSize;
	//bgs[bgNum].background = packedBackground; Need to turn into array of tilerefs
	return true;
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

/*

	bool loadAssets(/* needs to be file input); //Loads a file of assets

private:
	bool loadBG(size_t nameSize, char* name, char* packedBackground); //Loads an individual background and unique tiles. (Seperates tile array from background)

	bool loadTiles(char* in); //Loads an array of tiles
	bool loadBGs(char* in);  //Loads an array of backgrounds*/