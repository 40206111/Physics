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
const GLfloat dt = 0.005f;
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
	plane.scale(glm::vec3(100.0f, 5.0f,100.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	Shader pShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");

	// create particles
	int row = 10;
	int amount = glm::pow2(row);
	std::vector<Particle> p(amount);
	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));
	float stiffness = 200.0f;
	float dampening = 10.0f;
	float energy_loss = 0.2f;
	float rest_length = 1.0f;
	glm::vec3 wind(0.0f, 0.0f, 10.0f);
	std::default_random_engine generate;
	std::uniform_real_distribution<float> variable(-10.0f, 10.0f);

	//first stationary particle
	p[0] = Particle::Particle();
	p[0].getMesh().setShader(pShader);
	p[0].setPos(glm::vec3(-(row / 2), 8.0f, 0.0f));

	//set row level
	int rowLevel = 0;

	//set particle forces
	for (int i = 1; i < amount; i++)
	{
		p[i] = Particle::Particle();
		p[i].setMass(0.7f);
		p[i].getMesh().setShader(pShader);
		p[i].setPos(glm::vec3(p[0].getPos().x + (i % row), p[0].getPos().y, p[0].getPos().z - rowLevel));

		if (i != row - 1)
		{
			p[i].addForce(g);
			//p[i].addForce(new Drag());
			// if not in last colum
			if (i % row != row - 1)
			{
				p[i].addForce(new Hook(&p[i], &p[i + 1], stiffness, dampening, rest_length));
				//if row not first row
				if (rowLevel != 0)
				{
					p[i].addForce(new Wind(&p[i + 1], &p[i - row], &wind));
					p[i].addForce(new Wind(&p[i + 1], &p[(i - row) + 1], &wind));
				}


			}
			//if not in fist column
			if (i % row != 0)
			{
				p[i].addForce(new Hook(&p[i], &p[i - 1], stiffness, dampening, rest_length));
				//if not bottom row
				if (rowLevel != 0)
				{
					p[i].addForce(new Wind(&p[i - 1], &p[i - row], &wind));
				}
				//if not top row
				if (rowLevel != row - 1)
				{
					p[i].addForce(new Wind(&p[i - 1], &p[(i + row) - 1], &wind));
				}
			}
			//if not top row
			if (rowLevel != row - 1)
			{
				p[i].addForce(new Hook(&p[i], &p[i + row], stiffness, dampening, rest_length));

				//if not last column
				if (i % row != row - 1)
				{
					p[i].addForce(new Wind(&p[i + 1], &p[i + row], &wind));
				}
				//if not first column
				if (i % row != 0)
				{
					p[i].addForce(new Wind(&p[i + row], &p[(i + row) - 1], &wind));
				}

			}
			//if not on bottom row
			if (rowLevel != 0)
			{
				p[i].addForce(new Hook(&p[i], &p[i - row], stiffness, dampening, rest_length));
			}

		}
		if (i % row == row - 1)
		{
			rowLevel += 1;
		}
	}

	double startt = t;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//// Set frame time
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		frameTime *= 1.2f;
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

				//total force
				glm::vec3 F = p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt);

				if (p[i].getPos()[1] < plane.getPos()[1] + 0.04)
				{
					p[i].getPos()[1] = plane.getPos()[1] + 0.04f;
					p[i].getVel()[1] *= -1;
					if (p[i].getVel().y > 0.1)
					{
						p[i].setVel(p[i].getVel() * energy_loss);
					}
					else
					{
						F.y = 0;
					}
				}

				//calculate acceleration
				p[i].setAcc(F);

			}

			for (int i = 0; i < amount; i++)
			{
				glm::vec3 v = p[i].getVel();
				glm::vec3 r = p[i].getPos();
				//semi implicit Eular
				v += dt * p[i].getAcc();
				r += dt * v;
				p[i].setPos(r);
				//set vel
				p[i].setVel(v);
			}

			//change wind every 10 secs
			if (t - startt >= 10)
			{
				wind.z = variable(generate);
				startt = t;
				std::cout << wind.z << std::endl;
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
		// draw ground plane
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

