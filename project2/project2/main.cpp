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
const GLfloat dt = 0.01f;
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
	int amount = 2;
	std::vector<Particle> p(amount);
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	float energy_loss = 0.9f;
	std::vector<glm::vec3> fg(amount);
	std::default_random_engine generate;
	std::uniform_real_distribution<float> place(-2.5f, 2.5f);
	std::uniform_real_distribution<float> placey(0.5f, 5.0f);


	for (int i = 0; i < amount; i++)
	{
		p[i] = Particle::Particle();
		//p[i].setVel(glm::vec3(1.0f, 0.0f, 1.0f));
		p[i].getMesh().setShader(pShader);
		p[i].setPos(glm::vec3(0.0f, 4.9f, 0.0f));
		fg[i] = p[i].getMass() * g;
	}
	p[0].setPos(0, 0.0f);
	p[1].setPos(0, 2.0f);

	//Define Box
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, -2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);

	///DEFINE CONE///
	//Top and bottom
	glm::vec3 Ct = glm::vec3(0.0f, 2.5f, 0.0f);
	glm::vec3 Cb = glm::vec3(0.0f);
	//top and bottom radius
	float Ctr = 1.25f;
	float Cbr = 0.675f;
	//Cone total hieght if it went to a point
	float Ch = (Ctr*Ct.y) / (Cbr*((Ctr / Cbr) - 1));
	//Where the cone would be a point if it got that far
	glm::vec3 point = glm::vec3(Ct);
	point.y -= Ch;
	//force
	glm::vec3 Fc = glm::vec3(0.0f);
	//force coefficient
	float coneForceCo = 5.0f;


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

				//Check if in cone height 
				if (r.y <= Ct.y && r.y >= Cb.y)
				{
					//cone at the hieght of the particle
					glm::vec3 currentH = glm::vec3(Ct);
					currentH.y = r.y;
					//radius of cone at height
					float coneR = Cbr + (currentH.y / Ct.y)*(Ctr - Cbr);
					//distance from center to particle
					float newR = glm::length(currentH - r);

					//check if in cone radius
					if (newR < coneR)
					{
						//direction of force
						glm::vec3 fdir = r - point;
						//radius to point at top of cone
						float topR = (newR / (Ct.y - currentH.y))*Ct.y;
						//point at top at new top radius
						glm::vec3 topPoint = Ct;
						topPoint.x = Ct.x + topR;
						//vector from point to top of cone through particle
						glm::vec3 dirTop = glm::vec3(topPoint - point);
						//Fc = ((dirTop - fdir) * (coneR - newR))*coneForceCo;
						Fc = glm::vec3(0.0f, 20.0f, 0.0f);
					}
					else
					{
						//set Cone force to zero
						Fc = glm::vec3(0.0f);
					}

				}
				else
				{
					Fc = glm::vec3(0.0f);
				}

				//calculate drag
				glm::vec3 drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
				//total force
				glm::vec3 F = drag + fg[i] + Fc;
				//calculate acceleration
				p[i].setAcc((F) / p[i].getMass());
				//semi implicit Eular
				v += dt * p[0].getAcc();
				r += dt * v;
				//set postition and velocity
				p[i].setPos(r);
				p[i].setVel(v);

				//check if in box
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

