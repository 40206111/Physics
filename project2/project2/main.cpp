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
	float stiffness = 100.0f;
	float dampening = 50.0f;
	float energy_loss = 0.2f;
	float rest_length = 1.0f;
	glm::vec3 wind(0.0f, 0.0f, 200.0f);
	std::default_random_engine generate;
	std::uniform_real_distribution<float> variable(-200.0f, 200.0f);

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
			p[i].addForce(new Drag());
			if (i % row != row - 1)
			{
				p[i].addForce(new Hook(&p[i], &p[i + 1], stiffness, dampening, rest_length));

			}
			if (i % row != 0)
			{
				p[i].addForce(new Hook(&p[i], &p[i - 1], stiffness, dampening, rest_length));
			}
			if (rowLevel != row - 1)
			{
				p[i].addForce(new Hook(&p[i], &p[i + row], stiffness, dampening, rest_length));
			}
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

				glm::vec3 v = p[i].getVel();
				glm::vec3 r = p[i].getPos();

				//total force
				glm::vec3 F = p[i].applyForces(p[i].getPos(), p[i].getVel(), t, dt);

				if (p[i].getPos()[1] < plane.getPos()[1] + 0.04)
				{
					r[1] = plane.getPos()[1] + 0.04f;
					v[1] *= -1;
					if (v.y > 0.1)
					{
						v *= energy_loss;
					}
					else
					{
						F.y = 0;
					}
				}

				if (i > 0 && i < row - 1)
				{
					if (i + 1 < amount - 1 && i + row < amount - 1)
					{
						glm::vec3 n = ((p[i + 1].getPos() - p[i].getPos()) * (p[i + row].getPos() - p[i].getPos()));
						if (n != glm::vec3(0.0f))
						{
							n = n / length(n);

							F += ((wind * n)/3) / p[i].getMass();
						}
						//F += wind / p[i].getMass();
					}
				}

				//calculate acceleration
				p[i].setAcc(F);
				//semi implicit Eular
				v += dt * p[i].getAcc();
				r += dt * v;
				p[i].setPos(r);
				//set vel
				p[i].setVel(v);

			}

			if (t - startt >= 10)
			{
				std::cout << wind.z << std::endl;
				wind.z = variable(generate);
				startt = t;
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

