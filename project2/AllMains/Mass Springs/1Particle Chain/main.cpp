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
#include "Force.h"


// time
const GLfloat dt = 0.001f;
double currentTime = glfwGetTime();
double accumulator = 0.0f;
double t = 0.0f;



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

	// create particles
	int amount = 5;
	std::vector<Particle> p(amount);
	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
	float stiffness = 10.0f;
	float dampening = 01.0f;

	p[0] = Particle::Particle();
	p[0].getMesh().setShader(pShader);
	p[0].setPos(glm::vec3(0.0f, 10.0f, 0.0f));

	for (int i = 1; i < amount; i++)
	{
		p[i] = Particle::Particle();
		p[i].setMass(0.1f);
		p[i].getMesh().setShader(pShader);
		p[i].setPos(glm::vec3(p[0].getPos().x + i , p[0].getPos().y, p[0].getPos().z));
		p[i].addForce(g);
		p[i].addForce(new Drag());
		if (i != amount - 1) 
		{
			p[i].addForce(new Hook(&p[i], &p[i + 1], stiffness, dampening, 1.0f));
		}
		p[i].addForce(new Hook(&p[i], &p[i - 1], stiffness, dampening, 1.0f));
	}

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//// Set frame time
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		frameTime *= 2.0f;
		currentTime = newTime;
		accumulator += frameTime;

		/*
		**	INTERACTION
		*/


		while (accumulator > dt)
		{
			// Manage interaction
			app.doMovement(dt);

			/*
			**	SIMULATION
			*/

			for (int i = 0; i < amount; i++)
			{

				glm::vec3 v = p[i].getVel();
				glm::vec3 r = p[i].getPos();

				//total force
				glm::vec3 F = p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt);
				//calculate acceleration
				p[i].setAcc(F);
				//semi implicit Eular
				v += dt * p[i].getAcc();
				r = dt * v;
				//set postition and velocity
				p[i].translate(r);
				p[i].setVel(v);

			}
			
			//accumulate
			accumulator -= dt;
			t += accumulator;
		}

		/*
		**	RENDER
		*/
		// clear buffer
		app.clear();
		// draw groud plane
		//app.draw(plane);
		// draw particles
		for (int i = 0; i < amount; i++)
		{
			app.draw(p[i].getMesh());
		}
		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

