#include "myLights.h"
#include <string>

myLights::myLights()
{
}


myLights::~myLights()
{
}

void myLights::addLight(myLight *l)
{
	lights.push_back(l);
}
 

void myLights::setUniform(myShader* shader, std::string lightvariable_in_shader)
{
	for (unsigned int i=0;i<lights.size(); ++i)
		lights[i]->setUniform(shader, lightvariable_in_shader + "[" + std::to_string(i) + "]");

	shader->setUniform("num_lights", static_cast<int>(lights.size()));
}
