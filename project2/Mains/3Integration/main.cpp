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
	Particle p2 = Particle::Particle();
	p2.getMesh().setShader(pShader);
	p2.setPos(glm::vec3(-1.0f, 2.0f, 0.0f));

	// create particle
	Particle p[2];
	glm::vec3 r[2];
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);

	for (int i = 0; i < 2; i++)
	{
		p[i] = Particle::Particle();
		p[i].getMesh().setShader(pShader);
		p[i].setPos(glm::vec3((p2.getPos()[0] + i + 1), 2.0f, 0.0f));
	}


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
			//SEMI-IMPLICIT EULER
			glm::vec3 v = p[0].getVel();
			glm::vec3 r = p[0].getPos();
			p[0].setAcc(g);
			v += dt * p[0].getAcc();
			r += dt * p[0].getVel();
			p[0].setPos(r);
			p[0].setVel(v);

			//FORWARD EULER
			v = p[1].getVel();
			r = p[1].getPos();
			p[1].setAcc(g);
			v += dt * p[1].getAcc();
			r += dt * p[1].getVel();
			p[1].setPos(r);
			p[1].setVel(v);

			for (int j = 0; j < 2; j++)
			{
				for (int i = 0; i < 3; i++)
				{
					if (p[j].getPos()[i] <= o[i])
					{
						p[j].setPos(i, o[i]);
						p[j].setVel(i, p[j].getVel()[i] * -1);
					}
					else if (p[j].getPos()[i] >= (o[i] + d[i]))
					{
						p[j].setPos(i, o[i] + d[i]);
						p[j].setVel(i, p[j].getVel()[i] * -1);
					}
				}
			}

			//accumulate
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
		app.draw(p[0].getMesh());
		app.draw(p[1].getMesh());
		app.draw(p2.getMesh());

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

