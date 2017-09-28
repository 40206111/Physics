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
const double dt = 0.01;
double currentTime = glfwGetTime();
double accumulator = 0.0f;



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
	Particle p1 = Particle::Particle();
	p1.getMesh().setShader(pShader);

	//p1.setVel(glm::vec3(2.0f, 7.0f, 5.0f));
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);
	float energy_loss = 0.9f;
	glm::vec3 fg = p1.getMass() * g;
	glm::vec3 drag;

	// create particle
	Particle p2 = Particle::Particle();
	p2.getMesh().setShader(pShader);

	p2.setPos(glm::vec3(-1.0f, 2.0f, 0.0f));

	// create particle
	Particle p3 = Particle::Particle();
	p3.getMesh().setShader(pShader);

	p3.setPos(glm::vec3(1.0f, 2.0f, 0.0f));


	// time
	GLfloat firstFrame = (GLfloat) glfwGetTime();
	
	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//// Set frame time
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(dt);

		while (accumulator > dt)
		{

			/*
			**	SIMULATION
			*/
			glm::vec3 v = p1.getVel();
			drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
			p1.setAcc((drag + fg) / p1.getMass());
			v += dt * p1.getAcc();
			p1.setVel(v);
			r += dt * p1.getVel();
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

			p1.setVel(v);

			accumulator -= dt;
		}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		app.draw(p1.getMesh());
		app.draw(p2.getMesh());
		app.draw(p3.getMesh());

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

