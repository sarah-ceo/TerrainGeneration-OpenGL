#pragma once
#include "myObject.h"
#include "myTerrain.h"
#include "myEcosystem.h"
#include "myScene.h"
#include "myCamera.h"

class myGameManager
{
public:
	myGameManager(myCamera *c, myScene *s, myTerrain *t, myEcosystem *e, myObject *p);
	~myGameManager();
	void clear();

	void play();
	bool crashed();
	bool reachedPortal();
	void generateEnvironment();
	void flushEcosystem();

	myCamera *camera;
	myScene *scene;
	myTerrain *terrain;
	myEcosystem *ecosystem;
	myObject *portal;

	bool pause;
	bool finished;

	int terrainHeight;
	int zlimitneg;
	int zlimitpos;
	int xlimitneg;
	int xlimitpos;
	int ylimitpos;
	glm::vec3 portal_position;
	string ecosystem_name;

	float movement_stepsize;
	int level;
};
