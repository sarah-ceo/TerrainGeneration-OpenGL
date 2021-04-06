#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main


#include "helperFunctions.h"

#include "myShader.h"
#include "myCamera.h"
#include "mySubObject.h"

#include <glm/glm.hpp>
#include <iostream>
#include "myObject.h"
#include "myTerrain.h"
#include "myEcosystem.h"
#include "myGameManager.h"
#include "myLights.h"
#include "myFBO.h"
#include "default_constants.h"
#include "myScene.h"
#include "myShaders.h"
#include <stdlib.h>
#include <time.h>

using namespace std;

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;

int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;
bool windowsize_changed = true;
bool crystalballorfirstperson_view = false;
float movement_stepsize = DEFAULT_KEY_MOVEMENT_STEPSIZE;

// Camera parameters.
myCamera *cam1;

// All the meshes 
myScene scene;

// My personal classes
myTerrain *terr;
myEcosystem *eco;
myObject *portal;
myGameManager *gm;

// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
	case SDL_QUIT:
	{
		quit = true;
		break;
	}
	case SDL_KEYDOWN:
	{
		if (current_event.key.keysym.sym == SDLK_ESCAPE)
			quit = true;
		if (current_event.key.keysym.sym == SDLK_p) {
			if (gm->pause) {
				gm->pause = false;
			} else {
				gm->pause = true;
			}
		}
		if (current_event.key.keysym.sym == SDLK_n) {
			if (!gm->pause) {
				gm->generateEnvironment();
			}
		}
		if (current_event.key.keysym.sym == SDLK_r) {
			if (!gm->pause) {
				cam1->reset();
			}
		}
		if (current_event.key.keysym.sym == SDLK_UP || current_event.key.keysym.sym == SDLK_w) {
			if (gm->pause) {
				cam1->turnUp(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
			}
			else {
				cam1->moveForward(movement_stepsize);
			}
		}
		if (current_event.key.keysym.sym == SDLK_DOWN || current_event.key.keysym.sym == SDLK_s) {
			if (gm->pause) {
				cam1->turnDown(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
			}
			else {
				cam1->moveBack(movement_stepsize);
			}
		}
		if (current_event.key.keysym.sym == SDLK_LEFT || current_event.key.keysym.sym == SDLK_a) {
				cam1->turnLeft(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		}
		if (current_event.key.keysym.sym == SDLK_RIGHT || current_event.key.keysym.sym == SDLK_d) {
				cam1->turnRight(DEFAULT_LEFTRIGHTTURN_MOVEMENT_STEPSIZE);
		}
		
		break;
	}
	case SDL_MOUSEBUTTONDOWN:
	{
		mouse_position[0] = current_event.button.x;
		mouse_position[1] = current_event.button.y;
		mouse_button_pressed = true;
		break;
	}
	case SDL_MOUSEBUTTONUP:
	{
		mouse_button_pressed = false;
		break;
	}
	case SDL_MOUSEMOTION:
	{
		int x = current_event.motion.x;
		int y = current_event.motion.y;

		int dx = x - mouse_position[0];
		int dy = y - mouse_position[1];

		mouse_position[0] = x;
		mouse_position[1] = y;

		if ((dx == 0 && dy == 0) || !mouse_button_pressed) return;

		if ((SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalballorfirstperson_view) {
			if (!gm->pause) {
				cam1->firstperson_rotateView(dx, dy);
			}
		}

		break;
	}
	case SDL_WINDOWEVENT:
	{
		if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
			windowsize_changed = true;
		break;
	}
	default:
		break;
	}
}

int main(int argc, char *argv[])
{
	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

	// Using OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	// Create window
	window = nullptr;
	window = SDL_CreateWindow("MyGame", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	cam1 = new myCamera();
	SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);


	checkOpenGLInfo(true);


	/**************************INITIALIZING LIGHTS ***************************/
	scene.lights = new myLights();
	scene.lights->lights.push_back(new myLight(glm::vec3(0.f, 25.f, 0.f), glm::vec3(1.f, 1.f, 1.f), myLight::DIRECTIONALLIGHT));

	/**************************INITIALIZING FBO ***************************/
	//plane will draw the color_texture of the framebufferobject fbo.
	myFBO *fbo = new myFBO();
	fbo->initFBO(cam1->window_width, cam1->window_height);

	/**************************INITIALIZING OBJECTS THAT WILL BE DRAWN ***************************/

	//terrain
	terr = new myTerrain();
	terr->readObjects("models/flat_terrain.obj", false, false);

	//ecosystem
	eco = new myEcosystem();

	//portal
	portal = new myObject();
	portal->readObjects("models/sphere.obj", false, false);
	portal->computeTexturecoordinates_sphere();
	portal->createmyVAO();
	vector <string> cubemaps_portal = { "models/portal_lightblue/right.png", "models/portal_lightblue/left.png", "models/portal_lightblue/top.png", "models/portal_lightblue/bottom.png", "models/portal_lightblue/front.png", "models/portal_lightblue/back.png" };
	portal->setTexture(new myTexture("models/portal.jpg"), mySubObject::COLORMAP);
	portal->setTexture(new myTexture(cubemaps_portal), mySubObject::CUBEMAP);

	//GameManager
	myScene *ptr_scene = &scene;
	gm = new myGameManager(cam1, ptr_scene, terr, eco, portal);
	gm->generateEnvironment();

	scene.addObjects(terr, "terrain");
	scene.addObjects(portal, "portal");

	//skybox
	myObject *skybox;
	skybox = new myObject();
	skybox->readObjects("models/skybox.obj", false, false);
	skybox->computeTexturecoordinates_plane();
	skybox->createmyVAO();
	vector <string> cubemaps_skybox = { "models/skybox_clouds/east.bmp", "models/skybox_clouds/west.bmp", "models/skybox_clouds/down.bmp", "models/skybox_clouds/up.bmp", "models/skybox_clouds/north.bmp", "models/skybox_clouds/south.bmp" };
	skybox->setTexture(new myTexture(cubemaps_skybox), mySubObject::CUBEMAP);
	skybox->scale(terr->size, terr->size*2+100, terr->size);
	skybox->translate(0, -terr->size, 0);
	scene.addObjects(skybox, "skybox");

	/**************************SETTING UP OPENGL SHADERS ***************************/
	myShaders shaders;
	shaders.addShader(new myShader("shaders/texture+phong-vertexshader.glsl", "shaders/texture+phong-fragmentshader.glsl"), "shader_texturephong");
	shaders.addShader(new myShader("shaders/portal-vertexshader.glsl", "shaders/portal-fragmentshader.glsl"), "shader_portal");
	shaders.addShader(new myShader("shaders/environmentmap-vertexshader.glsl", "shaders/environmentmap-fragmentshader.glsl"), "shader_environmentmap");


	myShader *curr_shader;
	int angle = 0;
	int initial_time = time(NULL);
	int final_time;
	int frame_count = 0;
	int fps = 0;

	// display loop
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);
			windowsize_changed = false;

			if (fbo) delete fbo;
			fbo = new myFBO();
			fbo->initFBO(cam1->window_width, cam1->window_height);
		}

		glViewport(0, 0, cam1->window_width, cam1->window_height);
		glm::mat4 projection_matrix = cam1->projectionMatrix();
		glm::mat4 view_matrix = cam1->viewMatrix();
		glm::mat3 normal_matrix = glm::transpose(glm::inverse(glm::mat3(view_matrix)));

		//Computing a rotation matrix for the portal vertices
		angle = (angle+1) % 360;
		glm::mat3 portal_matrix_rotation = glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), glm::vec3(0, 0, 1));

		//Setting uniform variables for each shader + portal rotation matrix
		for (unsigned int i=0;i<shaders.size();i++)
		{
			curr_shader = shaders[i];
			curr_shader->start();
			curr_shader->setUniform("myprojection_matrix", projection_matrix);
			curr_shader->setUniform("myview_matrix", view_matrix);
			curr_shader->setUniform("portal_matrix_rotation", portal_matrix_rotation);
			scene.lights->setUniform(curr_shader, "Lights");
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		

		/************TERRAIN*************/
		curr_shader = shaders["shader_texturephong"];
		curr_shader->start();
		scene["terrain"]->displayObjects(curr_shader, view_matrix);
		
		fbo->clear();
		fbo->bind();
		{
			scene["terrain"]->displayObjects(curr_shader, view_matrix);
		}
		fbo->unbind();
		
		/*-----------SKYBOX------------*/
		//Environment map was modified so that there is no reflection -> it did not work with the inverted normals of the skybox
		curr_shader = shaders["shader_environmentmap"];
		curr_shader->start();
		scene["skybox"]->displayObjects(curr_shader, view_matrix);
		
		/*------------OBJECTS OF ECOSYSTEM-----------*/
		curr_shader = shaders["shader_texturephong"];
		curr_shader->start();
		for (int i = 0; i < eco->objects.size(); i++) {
			std::string text = "eco";
			text += std::to_string(i);
			scene[text]->displayObjects(curr_shader, view_matrix);

			fbo->clear();
			fbo->bind();
			{
				scene[text]->displayObjects(curr_shader, view_matrix);
			}
			fbo->unbind();
		}

		/*-----------PORTAL------------*/
		//Based on the original environment map, but with rotation of vertices added
		curr_shader = shaders["shader_portal"];
		curr_shader->start();
		scene["portal"]->displayObjects(curr_shader, view_matrix);

		fbo->clear();
		fbo->bind();
		{
			scene["portal"]->displayObjects(curr_shader, view_matrix);
		}
		fbo->unbind();

		/*-----------------------*/

		SDL_GL_SwapWindow(window);

		SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0)
			processEvents(current_event);
		
		frame_count++;
		final_time = time(NULL);

		//Little difficulties with rendering lots of objects in forest and grassland
		//I tried to use VAODrawElementsInstanced and VBOArrays with the shader but I did not managed to...
		//The fps changes a lot depending on terrain... this affected my gameplay as explained in GameManager.cpp
		if (final_time - initial_time > 0) {
			fps = frame_count / (final_time - initial_time);
			//cout << "FPS : " << fps << endl;
			frame_count = 0;
			initial_time = final_time;
		}

		gm->play();
	}

	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}