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
const double dt = 0.005f;
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

	// create particles
	int amount = 400;
	std::vector<Particle> p(amount);
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);
	float energy_loss = 0.9f;
	std::vector<glm::vec3> fg(amount);
	std::default_random_engine generate;
	std::uniform_real_distribution<float> place(-2.5f, 2.5f);
	std::uniform_real_distribution<float> placey(0.5f, 5.0f);


	for (int i = 0; i < amount; i++)
	{
		p[i] = Particle::Particle();
		p[i].getMesh().setShader(pShader);
		p[i].setPos(glm::vec3(place(generate), placey(generate), place(generate)));
		fg[i] = p[i].getMass() * g;
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
				glm::vec3 drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
				p[i].setAcc((drag + fg[i]) / p[i].getMass());
				v += dt * p[0].getAcc();
				r += dt * v;
				p[i].setPos(r);
				p[i].setVel(v);
				for (int j = 0; j < 3; j++)
				{
					if (p[i].getPos()[j] <= o[j])
					{
						p[i].setPos(j, o[j]);
						p[i].setVel(j, p[i].getVel()[j] * -1);
						p[i].setVel(p[i].getVel() * energy_loss);
					}
					else if (p[i].getPos()[j] >= (o[j] + d[j]))
					{
						p[i].setPos(j, o[j] + d[j]);
						p[i].setVel(j, p[i].getVel()[j] * -1);
						p[i].setVel(p[i].getVel() * energy_loss);
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
		for (int i = 0; i < amount; i++)
		{
			app.draw(p[i].getMesh());
		}
		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

