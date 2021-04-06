#include "myTerrain.h"
#include <iostream>
#include <random>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

using namespace std;

myTerrain::myTerrain()
{
	model_matrix = glm::mat4(1.0f);
	vao = nullptr;
	name = "terrain";
	size = 100;
	offset = rand() % 256;
}

myTerrain::~myTerrain()
{
	clear();
}

void myTerrain::clear()
{
	if (vao) delete vao;

	for (std::unordered_multimap<std::string, mySubObject *>::iterator it = objects.begin(); it != objects.end(); ++it)
		delete it->second;
	objects.clear();

	vector<glm::vec3> empty; vertices.swap(empty);
	normals.swap(empty);
	vector<glm::ivec3> empty2; indices.swap(empty2);
}

//This is the function that generates the random height, it updates the y of each vertex with the result of perlin
void myTerrain::generateHeight(float frequency, int height) {
	int randArray[] = { 222, 64, 43, 174, 206, 217, 77, 12, 82, 93, 101, 205, 126, 196, 76, 60, 220, 42, 102, 28, 80, 108, 14,
		97, 106, 21, 50, 199, 230, 191, 83, 94, 40, 45, 155, 252, 208, 253, 90, 71, 197, 32, 22, 118, 170, 232, 218, 67,
		72, 4, 18, 105, 159, 11, 179, 26, 116, 151, 53, 10, 20, 8, 65, 169, 92, 130, 184, 16, 44, 226, 138, 119, 236, 123,
		15, 175, 38, 152, 165, 47, 36, 164, 120, 68, 140, 54, 122, 251, 66, 88, 237, 7, 3, 177, 185, 224, 244, 49, 153, 213,
		2, 186, 192, 30, 74, 35, 212, 233, 110, 131, 157, 104, 117, 69, 51, 210, 250, 100, 243, 39, 57, 234, 25, 180, 183, 1,
		249, 245, 79, 114, 124, 225, 187, 168, 189, 13, 239, 125, 56, 216, 158, 87, 34, 31, 156, 203, 115, 227, 167, 27, 235,
		247, 135, 223, 209, 85, 242, 1, 149, 107, 241, 121, 240, 254, 202, 214, 194, 59, 154, 238, 207, 6, 17, 129, 5, 143, 9,
		160, 142, 144, 70, 204, 141, 23, 188, 176, 75, 147, 95, 181, 171, 81, 255, 62, 198, 231, 173, 248, 195, 98, 136, 96, 55,
		29, 84, 133, 52, 162, 137, 172, 211, 182, 91, 24, 193, 111, 61, 145, 178, 128, 112, 228, 63, 150, 113, 33, 148, 229, 190,
		73, 127, 109, 48, 221, 139, 89, 78, 134, 19, 219, 99, 103, 41, 201, 37, 246, 166, 215, 163, 132, 200, 146, 58, 46, 161, 86 };

	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - 1; j++) {

			int index = i * size + j;

			vertices[index][1] = perlinInterpolation(i, j, 3, frequency, randArray)*height;
		}
	}

	computeNormals();
	computeTexturecoordinates_plane();
	computeTangents();
}

int myTerrain::randomFromArray(int x, int y, int randArray[256])
{
	int random_nb = randArray[(offset+y) % 256];
	return randArray[(random_nb + x) % 256];
}

float myTerrain::generateNoise(float x, float y, int randArray[256])
{
	int int_x = x;
	int int_y = y;
	float dec_x = x - int_x;
	float dec_y = y - int_y;

	int i = randomFromArray(int_x, int_y, randArray);
	int j = randomFromArray(int_x + 1, int_y, randArray);
	int k = randomFromArray(int_x, int_y + 1, randArray);
	int l = randomFromArray(int_x + 1, int_y + 1, randArray);

	float low = i + (dec_x * dec_x * (3 - 2 * dec_x)) * (j - i);
	float high = k + (dec_x * dec_x * (3 - 2 * dec_x)) * (l - k);
	return low + (dec_y * dec_y * (3 - 2 * dec_y)) * (high - low);
}

float myTerrain::perlinInterpolation(float x, float y, int l, float f, int randomFromArray[256])
{
	float xa = x * f;
	float ya = y * f;
	float a = 1.0;
	float signal = 0;
	float smooth = 0.0;

	int i;
	for (i = 0; i < l; i++)
	{
		smooth += 256 * a;
		signal += generateNoise(xa, ya, randomFromArray) * a;
		a /= 2;
		xa *= 2;
		ya *= 2;
	}

	return signal / smooth;
}