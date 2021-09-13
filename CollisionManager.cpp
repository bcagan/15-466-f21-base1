// George Whitfield 
// September 11, 2021

#include "CollisionManager.hpp"

CollisionManager::CollisionManager(std::array < PPU466::Sprite, 64 > *s)
{
	assert(s != NULL);
	sprites = s;
}

bool CollisionManager::Collides(int s1, int s2)
{
	PPU466::Sprite t1 = (*sprites)[s1];
	PPU466::Sprite t2 = (*sprites)[s2];
	int tSize = 8; //tile size

	//check whether t2 is inside collidable x range of t1
	if ((t1.x >= t2.x && t1.x - t2.x < tSize + 1) ||
		(t2.x >= t1.x && t2.x - t1.x < tSize))
	{
		//check y range
		if ((t1.y >= t2.y && t1.y - t2.y < tSize + 1) ||
			(t2.y >= t1.y && t2.y - t1.y < tSize))
		{
			return true;
		}
	}
	//std::cout << s1 << " does not collide with " <<  s2 << std::endl;
	return false;
}

bool CollisionManager::Touches(int s1, int s2)
{
	PPU466::Sprite t1 = (*sprites)[s1];
	PPU466::Sprite t2 = (*sprites)[s2];
	int tSize = 8; //tile size
	
	//early out: t1 is far from t2
	if (std::abs(t1.x - t2.x) > 8) return false;
	if (std::abs(t1.y - t2.y) > 8) return false;

	//check whether t2 is inside collidable x range of t1
	if ((t1.x > t2.x && t1.x - t2.x == tSize + 1) ||
		(t2.x > t1.x && t2.x - t1.x == tSize))
	{
		//check y range
		if ((t1.y > t2.y && t1.y - t2.y == tSize + 1) ||
			(t2.y > t1.y && t2.y - t1.y == tSize))
		{
			std::cout << s1 << " is touching " << s2 << std::endl;
			return true;
		}
	}
	std::cout << s1 << " does not touch " << s2 << std::endl;
	return false;
}