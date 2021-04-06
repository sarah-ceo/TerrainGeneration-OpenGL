#pragma once
#include <string>
#include "myObject.h"
#include "myTerrain.h"
#include <unordered_map>

using namespace std;

class myEcosystem
{
public:
	myTerrain* terr;
	string name;
	std::unordered_multimap<std::string, float> elements;
	string terrain_texture_path;
	std::vector<myObject *> objects;

	myEcosystem();
	~myEcosystem();
	void clear();
	
	void generateEcosystem(myTerrain* t, string s);
	void setProperties();
	void populate();
	glm::vec3 myEcosystem::findAvailablePosition();
};