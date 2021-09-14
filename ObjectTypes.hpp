#pragma once
#include <glm/gtc/type_ptr.hpp>
#include "AssetLoad.h"

class LightObj {
public:
	std::string name;
	float inner;
	float outer;
};

//TO DO
//Set up light types
//Set up background
//Test test test!

class ObjectTypes {
public:
	ObjectTypes() {
		objList.resize(3);
		LightObj light0;
		LightObj light1;
		LightObj light2;

		light0.name = "light0";
		light0.inner = 45.f / 2.f;
		light0.outer = 45.5;
		light1.name = "light1";
		light1.inner = 45.f / 2.f;
		light1.outer = 45.5;
		light2.name = "light2";
		light2.inner = 45.f / 5.f;
		light2.outer = 45.f /2.5f;
		objList[0] = light0;
		objList[1] = light1;
		objList[2] = light2;
	}
	~ObjectTypes();
	LightObj getObj(std::string objType);
private:
	std::vector<LightObj> objList;
};

//Objects referenced by string
//"light0"
//"light1"
//"light2"