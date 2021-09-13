#include <glm/gtc/type_ptr.hpp>
#include "AssetLoad.h"
struct Object {
	std::array < std::array < std::array< glm::u8vec4, 4>, 3> pallets;
	glm::vec2 pos;
	TileRef tile;
};

struct Light {
	Object lightObject;
	float inner;
	float outer;
};

//Light, wall, goal

class ObjectTypes {

};

//Objects referenced by size_t
//"light0"
//"light1"
//"light2"
//"wall"
//"goal"