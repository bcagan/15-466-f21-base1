#pragma once

// George Whitfield
// September 11, 2021

#include "PPU466.hpp"
#include <vector>
#include <iostream>

// CollisionManager will determine whether 2 tiles in the game overlap. This 
// is to be used for player-ground interaction.
class CollisionManager
{
	public:

		CollisionManager() {};
		CollisionManager(std::array< PPU466::Sprite, 64 > *s);
		~CollisionManager() {};

		// returns true if two sprites overlap.
		// s1: index of tile 1 in the spite array, s2: index of tile 2.
		// This function implicitly assumes that each is of size 8x8
		bool Collides(int s1, int s2);

		// returns true if the sprite at index s in the PPU sprites array 
		// is a collidable tile, e.g. the floor or wall. 
		bool isTileCollidable(int s)
		{
			for (auto i = COLLIDABLE_TILES.begin(); i < COLLIDABLE_TILES.end(); i++)
			{
				if (s == *i)
				{
					return true;
				}
			}
			return false;
		}

		// returns true if ppu.sprites[s1] is diretly next to ppu.sprites[s2]
		// s1: index of tile 1 in the spite array, s2: index of tile 2.
		// This function implicitly assumes that each is of size 8x8
		bool Touches(int s1, int s2);
	private:

		// CollisionManager.sprites should be a pointer to PPU466.sprites 
		std::array< PPU466::Sprite, 64 > *sprites;

		// ndicies into sprite array for collidable tiles
		std::vector<int> COLLIDABLE_TILES = {35};

};