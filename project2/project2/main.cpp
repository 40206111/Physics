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


	// create particle
	Particle particle1 = Particle::Particle();
	//scale it down (x.1), translate it up by 2.5 and rotate it by 90 degrees around the x axis
	particle1.getMesh().setShader(Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag"));

	particle1.setVel(glm::vec3(4.0f, 6.0f, -10.0f));
	glm::vec3 g = glm::vec3(0.0f, -9.8f, 0.0f);
	glm::vec3 r = glm::vec3(0.0f, 4.5f, 0.0f);
	glm::vec3 o = glm::vec3(-2.5f, 0.0f, 2.5f);
	glm::vec3 d = glm::vec3(5.0f, 5.0f, 5.0f);
	float energy_loss = 0.99;
	glm::vec3 fg = particle1.getMass() * g;
	glm::vec3 drag;

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
		glm::vec3 v = particle1.getVel();
		drag = 0.5 * 1.225 * -v * glm::length(v) * 1.05 * 0.1;
		particle1.setAcc((drag + fg)/particle1.getMass());
		v += deltaTime * particle1.getAcc();
		particle1.setVel(v);
		r += deltaTime * particle1.getVel();
		particle1.setPos(r);
		//CHECK X left
		if ((particle1.getPos().x <= o.x))
		{
			r.x = o.x;
			v.x *= -1;
			v *= energy_loss;
		}
		//CHECK Y bottom
		if ((particle1.getPos().y <= o.y))
		{
			r.y = o.y;
			v.y *= -1;
			v *= energy_loss;
		}
		//CHECK Z forward
		if (particle1.getPos().z >= o.z)
		{
			r.z = o.z;
			v.z *= -1;
			v *= energy_loss;
		}
		//CHECK X right
		if (particle1.getPos().x >= (o.x + d.x))
		{
			r.x = o.x + d.x;
			v.x *= -1;
			v *= energy_loss;
		}
		//CHECK Y top
		if ((particle1.getPos().y >= (o.y + d.y)))
		{
			r.y = o.y + d.x;
			v.y *= -1;
			v *= energy_loss;
		}
		//CHECK Z back
		if (particle1.getPos().z <= (o.z - d.z))
		{
			r.z = o.z - d.x;
			v.z *= -1;
			v *= energy_loss;
		}
		particle1.setVel(v);


		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		// draw groud plane
		app.draw(plane);
		// draw particles
		app.draw(particle1.getMesh());				

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

