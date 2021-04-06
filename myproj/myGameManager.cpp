#include "myGameManager.h"
#include <iostream>

using namespace std;

myGameManager::myGameManager(myCamera *c, myScene *s, myTerrain *t, myEcosystem *e, myObject *p) {
	camera = c;
	scene = s;
	terrain = t;
	ecosystem = e;
	portal = p;

	zlimitneg = -terrain->size;
	zlimitpos = terrain->size;
	xlimitneg = -terrain->size;
	xlimitpos = terrain->size;
	ylimitpos = terrain->size*2;

	level = 0;

	pause = false;
	finished = false;
}

myGameManager::~myGameManager() {

}

void myGameManager::clear() {

}

/*Frame rate depends heavily on terrain because some have a lot of objects and some have none
I wanted camera to move all the time and the user would have had to direct it to the portal
Also speed would have increased with level
But because of differences in frame per second depending on terrain I could not implement the "game"*/

void myGameManager::play() {
	//movement_stepsize = .05f + (level/100);

	if (!pause && !finished) {

		//camera->moveForward(movement_stepsize);

		if (reachedPortal()) {
			finished = true;
			level += 1;
			generateEnvironment();
		}

		if (crashed()) {
			finished = true;
			level = 0;
			generateEnvironment();
		}

	}
}

//Stepping out of skybox = crash, contact with ground = crash
bool myGameManager::crashed() {
	level = 0;
	if (camera->camera_eye[0] <= xlimitneg || camera->camera_eye[0] >= xlimitpos) {
		return true;
	}
	if (camera->camera_eye[1] <= terrainHeight + 5 || camera->camera_eye[1] >= ylimitpos) {
		return true;
	}
	if (camera->camera_eye[2] <= zlimitneg || camera->camera_eye[2] >= zlimitpos) {
		return true;
	}
	return false;
}

bool myGameManager::reachedPortal() {
	if (glm::distance(camera->camera_eye, portal_position) < 10) {
		return true;
	}
	return false;
}

//This is the function that generate the terrain and its ecosystem
//Ecosystem depends on terrain height 
//(low = grassland, low-middle = desert, high-middle = forest, high = mountains)
void myGameManager::generateEnvironment() {
	camera->reset();
	
	float frequency = 0.20f;
	terrainHeight = rand() % 30;

	if (terrainHeight < 7) {
		ecosystem_name = "grassland";
	}
	else if (terrainHeight > 23) {
		ecosystem_name = "mountains";
	}
	else if (terrainHeight < 14) {
		ecosystem_name = "desert";
	}
	else {
		ecosystem_name = "forest";
	}

	terrain->generateHeight(frequency, terrainHeight);
	terrain->createmyVAO();

	flushEcosystem();
	ecosystem->generateEcosystem(terrain, ecosystem_name);
	for (int i = 0; i < ecosystem->objects.size(); i++) {
		std::string text = "eco";
		text += std::to_string(i);
		scene->addObjects(ecosystem->objects[i], text);
	}

	terrain->setTexture(new myTexture(ecosystem->terrain_texture_path), mySubObject::COLORMAP);

	//Sets random portal position
	float x = rand() % (2*xlimitpos-20) - xlimitpos-10;
	float y = rand() % (ylimitpos - terrainHeight + 10) + terrainHeight + 10;
	float z = rand() % (2 * zlimitpos - 20) - zlimitpos - 10;
	float angle = rand() % 360;
	portal_position = glm::vec3(x , y, z);
	portal->model_matrix = glm::mat4(1.0f);
	portal->scale(10, 10, 1);
	portal->rotate(glm::vec3(1, 1, 1), angle);
	portal->translate(portal_position);

	finished = false;
}

//I have to remove all the objects I created with ecosystem from the scene
void myGameManager::flushEcosystem() {
	if (ecosystem->objects.size() > 0) {

		std::unordered_multimap<std::string, size_t> byname_bis;
		std::vector<myObject *> all_objects_bis;
		int index = 0;

		for (auto it = scene->byname.begin(); it != scene->byname.end(); it++) {
			if (it->first.find("eco") == std::string::npos) {
				byname_bis.emplace(it->first, index);
				all_objects_bis.push_back(scene->all_objects[it->second]);
				++index;
			}
		}

		scene->byname = byname_bis;
		scene->all_objects = all_objects_bis;
		ecosystem->clear();
	}
}