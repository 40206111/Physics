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
#include "Body.h"
#include "Particle.h"


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

	Shader pShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");
	// create particle
	Mesh p1 = Mesh::Mesh();
	p1.translate(glm::vec3(0.0f, 2.5f, 0.0f));
	p1.scale(glm::vec3(.1f, .1f, .1f));
	p1.rotate((GLfloat)M_PI_2, glm::vec3(1.0f, 0.0f, 0.0f));
	p1.setShader(pShader);

	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 2.0f, 0.0f);
	p1.setPos(r);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);
	float energy_loss = 0.9f;
	glm::vec3 fg = g;
	glm::vec3 drag;
	glm::vec3 v = glm::vec3(2.0f, 7.0f, 5.0f);
	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	
	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// Set frame time
		GLfloat currentFrame = (GLfloat)glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.5f;
		currentFrame *= 1.0f;
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
			
			drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
			glm::vec3 a = (drag + fg);
			v += deltaTime * a;
			r += deltaTime * v;
			p1.setPos(r);

			for (int i = 0; i < 3; i++)
			{
				if (p1.getPos()[i] <= o[i])
				{
					r[i] = o[i];
					v[i] *= -1;
					v *= energy_loss;
				}
				else if (p1.getPos()[i] >= (o[i] + d[i]))
				{
					r[i] = o[i] + d[i];
					v[i] *= -1;
					v *= energy_loss;
				}
			}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		app.draw(p1);

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

