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
	unsigned tSize = 8; //tile size
	
	//check whether t2 is inside collidable x range of t1
	if ((t1.x > t2.x && t1.x - t2.x < tSize + 1) ||
		(t2.x > t1.x && t2.x - t1.x < tSize))
	{
		//check y range
		if ((t1.y > t2.y && t1.y - t2.y < tSize + 1) ||
			(t2.y > t1.y && t2.y - t1.y < tSize))
		{
			return true;
		}
	}
	return false;
}