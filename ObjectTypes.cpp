#include"ObjectTypes.hpp"
#include <vector>
#include <string>

//Searches objlist for requested object info, if none exists, returns default
LightObj ObjectTypes::getObj(std::string objType) {
	for (size_t curObj = 0; curObj < objList.size(); curObj++) {
		if (objType.compare((objList[curObj]).name) == 0) return objList[curObj];
	}
	return objList[0];
}