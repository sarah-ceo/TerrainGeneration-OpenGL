#include "myEcosystem.h"
#include <iostream>

using namespace std;

myEcosystem::myEcosystem() {
}

myEcosystem::~myEcosystem() {
	clear();
}

void myEcosystem::clear() {
	for (auto p : objects)
	{
		delete p;
	}
	objects.clear();
}

void myEcosystem::generateEcosystem(myTerrain* t, string s) {
	terr = t;
	name = s;
	setProperties();
	populate();
}

//Different models for different environments, and different objects in each environment with a frequency
void myEcosystem::setProperties() {
	if (name == "grassland") {
		terrain_texture_path = "models/textures/grass.png";
		elements = {
			{ "tree2", 1.f },
			{ "rock1", 1.f },
		};
	}
	else if (name == "desert") {
		terrain_texture_path = "models/textures/sand.jpg";
		elements = {
		};
	}
	else if (name == "forest") {
		terrain_texture_path = "models/textures/forest.jpg";
		elements = {
			{ "tree1", 1.f },
			{ "tree3", 1.f }
		};
	}
	else if (name == "mountains") {
		terrain_texture_path = "models/textures/rock.png";
		elements = {
		};
	}
}

//This is the function that caused trouble : too many objects are created, I tried lowering but then it doesn't look as good
//I didn't manage to make instances work with VAO/VBO... so I just reduced the number of objects created
void myEcosystem::populate() {
	myObject* obj;
	string model_path;

	for (auto& it : elements) {
		model_path = it.first;

		for (int i = 0; i < (int)500 * it.second; i++) {
			obj = new myObject();
			obj->readObjects("models/" + model_path + ".obj", false, false);
			obj->createmyVAO();
			obj->scale(1.f, 1.f, 1.f);

			glm::vec3 pos = findAvailablePosition();
			
			obj->translate(pos[0], pos[1], pos[2]);

			objects.push_back(obj);
		}
	}
}

//This is supposed to choose a random vertex of terrain and retrieve the position (height especially) for the objects
glm::vec3 myEcosystem::findAvailablePosition() {

	int index = rand() % terr->indices.size();

	float x = terr->vertices[index][0];
	float y = terr->vertices[index][1];
	float z = terr->vertices[index][2];

	glm::vec3 position = glm::vec3(x, y, z);
	return position;
}