#pragma once
#include "myObject.h"

class myTerrain: public myObject 
{
public:
	myTerrain();
	~myTerrain();
	void clear();
	void generateHeight(float frequency, int height);
	int size;
	int offset;
	int randomFromArray(int x, int y, int randArray[256]);
	float generateNoise(float x, float y, int randArray[256]);
	float perlinInterpolation(float x, float y, int d, float frequency, int randArray[256]);
};