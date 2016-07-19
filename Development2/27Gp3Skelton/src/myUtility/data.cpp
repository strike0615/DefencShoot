#pragma once
#include "data.h"




namespace myLib
{

	void lineFinder(std::ifstream& f, const std::string target)
	{
		while (true)
		{
			std::string line;
			std::getline(f, line);
			if (line == target)
			{
				break;
			}
			if (f.eof())
			{
				assert(false);
			}
		}
	}
	void loadDrawObjData(std::ifstream& f, DrawObject& obj)
	{
		std::string line;
		std::getline(f, line);
		std::stringstream stream(line);
		std::string resName;
		
		std::string str;
		getline(stream, str,' ');
		resName = str;

		//pos, scale, src, srcSize;
		Vec3f temp[4];
		for (int i = 0; i < 4; i++)
		{
			getline(stream, str,' ');
			float x, y, z;
			sscanf_s(str.c_str(),
				"(%f,%f,%f)",
				&x,&y,&z
				);
			temp[i].set(x, y, z);
		}

		obj.initDrawObject(temp[0], temp[1], resName, temp[2], temp[3]);
	}
	void loadMovingObjData(std::ifstream& f, MovingObj& obj)
	{
		std::string line;
		std::getline(f, line);
		std::stringstream stream(line);
		std::string resName;
		std::string str;
		getline(stream, str, ' ');
		resName = str;

		//pos, scale, src, srcSize;
		Vec3f temp[4];
		for (int i = 0; i < 4; i++)
		{
			getline(stream, str, ' ');
			float x, y, z;
			sscanf_s(str.c_str(),
				"(%f,%f,%f)",
				&x, &y, &z
				);
			temp[i].set(x, y, z);
		}

		obj.initDrawObject(temp[0], temp[1], resName, temp[2], temp[3]);
	}


	void loadVec3fData(std::ifstream&f, ci_ext::Vec3f& vec)
	{
		std::string line;
		std::getline(f, line);
		float x, y, z;
		sscanf_s(line.c_str(),
			"(%f,%f,%f)",
			&x, &y, &z
			);
		vec.set(x, y, z);
	}
	void loadFloatFromLine(std::ifstream&f, float& data)
	{
		std::string line;
		std::getline(f, line);
		sscanf_s(line.c_str(),
			"%f",
			&data
			);
	}
	void loadIntFromLine(std::ifstream&f, int& data)
	{
		std::string line;
		std::getline(f, line);
		sscanf_s(line.c_str(),
			"%d",
			&data
			);
	}
	void loadRoundCollider(std::ifstream&f, roundCollider& col)
	{
		std::string line;
		std::getline(f, line);
		std::stringstream stream(line);
		std::string str;

		getline(stream, str, ' ');
		float x, y, z;
		sscanf_s(str.c_str(),
			"(%f,%f,%f)",
			&x, &y, &z
			);
		col.offset(x, y, z);
		getline(stream, str, ' ');
		float radius;
		sscanf_s(str.c_str(),
			"%f",
			&radius
			);
		col.setRadius(radius);
	}
	void loadRectCollider(std::ifstream&f, rectCollider& col)
	{
		std::string line;
		std::getline(f, line);
		std::stringstream stream(line);
		std::string str;

		getline(stream, str, ' ');
		float x, y, z;
		sscanf_s(str.c_str(),
			"(%f,%f,%f)",
			&x, &y, &z
			);


		col.offset(x, y, z);
		float h, w;
		getline(stream, str, ' ');
		sscanf_s(str.c_str(),
			"%f",
			&h
			);
		getline(stream, str, ' ');
		sscanf_s(str.c_str(),
			"%f",
			&w
			);
		col.setHeight(h);
		col.setWidth(w);
	}


	int getIntFromFile(std::string tagName, std::string pathName)
	{
		int temp;
		std::ifstream f(pathName);
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			lineFinder(f, tagName);
			loadIntFromLine(f,temp);
		}
		f.close();
		return temp;
	}
	float getFloatFromFile(std::string tagName, std::string pathName)
	{
		float temp;
		std::ifstream f(pathName);
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			lineFinder(f, tagName);
			loadFloatFromLine(f, temp);
		}
		f.close();
		return temp;
	}
	std::string getStringFromFile(std::string tagName, std::string pathName)
	{
		std::string temp;
		std::ifstream f(pathName);
		if (f.fail()) assert(false);
		if (f.is_open())
		{
			lineFinder(f, tagName);
			std::getline(f, temp);
		}
		f.close();
		return temp;
	}

}