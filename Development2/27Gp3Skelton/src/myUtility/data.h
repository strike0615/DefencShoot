#pragma once

#include "drawObject.hpp"
#include "movingObj.h"
#include "myCollider.h"
#include <iostream>

namespace myLib
{
	
	void lineFinder(std::ifstream& f, const std::string target);

	void loadDrawObjData(std::ifstream& f, DrawObject& obj);
	void loadVec3fData(std::ifstream&f, ci_ext::Vec3f& vec);
	void loadMovingObjData(std::ifstream& f, MovingObj& obj);
	void loadFloatFromLine(std::ifstream&f, float& data);
	void loadIntFromLine(std::ifstream&f, int& data);
	void loadRoundCollider(std::ifstream&f, roundCollider& col);

	int getIntFromFile(std::string tagName, std::string pathName);
	float getFloatFromFile(std::string tagName, std::string pathName);
	std::string getStringFromFile(std::string tagName, std::string pathName);
}