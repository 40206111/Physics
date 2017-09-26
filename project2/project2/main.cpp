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

	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, 2.5f);
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
		// Set frame time
		GLfloat currentFrame = (GLfloat)  glfwGetTime() - firstFrame;
		// the animation can be sped up or slowed down by multiplying currentFrame by a factor.
		currentFrame *= 1.0f;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;

		/*
		**	INTERACTION
		*/
		// Manage interaction
		app.doMovement(deltaTime);

		while (accumulator > dt)
		{

			/*
			**	SIMULATION
			*/
			glm::vec3 v = p1.getVel();
			drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.01;
			p1.setAcc((drag + fg) / p1.getMass());
			v += deltaTime * p1.getAcc();
			p1.setVel(v);
			r += deltaTime * p1.getVel();
			p1.setPos(r);
			//CHECK X left
			if ((p1.getPos().x <= o.x))
			{
				r.x = o.x;
				v.x *= -1;
				v *= energy_loss;
			}
			//CHECK Y bottom
			if ((p1.getPos().y <= o.y))
			{
				r.y = o.y;
				v.y *= -1;
				v *= energy_loss;
			}
			//CHECK Z forward
			if (p1.getPos().z >= o.z)
			{
				r.z = o.z;
				v.z *= -1;
				v *= energy_loss;
			}
			//CHECK X right
			if (p1.getPos().x >= (o.x + d.x))
			{
				r.x = o.x + d.x;
				v.x *= -1;
				v *= energy_loss;
			}
			//CHECK Y top
			if ((p1.getPos().y >= (o.y + d.y)))
			{
				r.y = o.y + d.x;
				v.y *= -1;
				v *= energy_loss;
			}
			//CHECK Z back
			if (p1.getPos().z <= (o.z - d.z))
			{
				r.z = o.z - d.x;
				v.z *= -1;
				v *= energy_loss;
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

