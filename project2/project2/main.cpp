#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include "glm/ext.hpp"


// Other Libs
#include "SOIL2/SOIL2.h"

// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"


// time
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;



// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 5.0f, 20.0f));
			
	// create ground plane
	Mesh plane = Mesh::Mesh();
	// scale it up x5
	plane.scale(glm::vec3(5.0f, 5.0f, 5.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));


	// create particle
	Mesh particle1 = Mesh::Mesh();
	//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	particle1.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	particle1.scale(glm::vec3(.1f, .1f, .1f));
	particle1.rotate((GLfloat) M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	particle1.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	glm::vec3 v = glm::vec3(2.0f, 3.0f, 0.0f);
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, 2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);

	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	
	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)  glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.5f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);


		/*
		**	SIMULATION
		*/
		v += deltaTime * g;
		r += deltaTime * v;
		particle1.setPos(r);
		if ((particle1.getPos().y <= o.y) || (particle1.getPos().y >= (o.y + d.y)) || (particle1.getPos().x <= o.x) || particle1.getPos().x >= (o.x + d.x) || particle1.getPos().z >= o.z || particle1.getPos().z <= (o.z - d.z))
		{
			std::cout <<"x: " << particle1.getPos().x << std::endl << "y: " << particle1.getPos().y << std::endl << "z: " << particle1.getPos().z << std::endl;
			v *= -1;
		}


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		app.draw(particle1);				

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

