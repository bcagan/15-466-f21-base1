#include <glm/gtc/type_ptr.hpp>
#include "AssetLoad.h"
class Object {
public:
	std::string name;
	std::array <TileRef, 3> pallets;
};

class LightObj : public Object{
public:
	float inner;
	float outer;
};

//Light, wall, goal


//TO DO
//Set up pallets
//Set up function that references object type and gets updated pallet from that (not lights)
//Test test test!
//Seatch (get object)

class ObjectTypes {
	ObjectTypes() {
		objList.resize(5);

		Object wall;
		Object goal;
		LightObj light0;
		LightObj light1;
		LightObj light2;

		wall.name = "wall";
		TileRef pallet0;
		TileRef pallet1;
		TileRef pallet2;
		pallet0.name = "wall";
		pallet1.name = "wall";
		pallet2.name = "wall";
		wall.pallets[0] pallet0;
		wall.pallets[1] pallet1;
		wall.pallets[2] pallet2;
		goal.name = "goal";
		pallet0.name = "goal";
		pallet1.name = "goal";
		pallet2.name = "goal";
		goal.pallets[0] pallet0;
		goal.pallets[1] pallet1;
		goal.pallets[2] pallet2;
		light0.name = "light0";
		pallet0.name = "light";
		pallet1.name = "light";
		pallet2.name = "light";
		light0.pallets[0] pallet0;
		light0.pallets[1] pallet1;
		light0.pallets[2] pallet2;
		light0.inner = 45.f / 2.f;
		light0.outer = 45.5;
		light1.name = "light1";
		pallet0.name = "light";
		pallet1.name = "light";
		pallet2.name = "light";
		light1.pallets[0] pallet0;
		light1.pallets[1] pallet1;
		light1.pallets[2] pallet2;
		light1.inner = 45.f / 2.f;
		light1.outer = 45.5;
		light2.name = "light2";
		pallet0.name = "light";
		pallet1.name = "light";
		pallet2.name = "light";
		light2.pallets[0] pallet0;
		light2.pallets[1] pallet1;
		light2.pallets[2] pallet2;
		light2.inner = 45.f / 2.f;
		light2.outer = 45.5;
	}
	~ObjectTypes();
	Object getObj(std::string objType);
	std::vector<Object> objList;
};

//Objects referenced by size_t
//"light0"
//"light1"
//"light2"
//"wall"
//"goal"