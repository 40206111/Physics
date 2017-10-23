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
#include "RigidBody.h"


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

	Shader rbShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");

	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));

	RigidBody rb = RigidBody::RigidBody();
	Mesh m = Mesh::Mesh(Mesh::MeshType::CUBE);
	rb.setMesh(m);
	rb.getMesh().setShader(rbShader);

	// rigid body motion values
	rb.translate(glm::vec3(0.0f, 5.0f, 0.0f));
	rb.setVel(glm::vec3(2.0f, 7.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.5f, 2.0f, 3.0f));

	rb.addForce(g);

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
			bool ok = true;
			for (int i = 0; i < rb.getMesh().getVertices().size(); i++)
			{
				glm::vec4 worldspace = rb.getMesh().getModel() * glm::vec4(glm::vec3(rb.getMesh().getVertices()[i].getCoord()), 1.0f);
				if (worldspace.y < plane.getPos().y)
				{
					std::cout << "(" << worldspace.x << ", " << worldspace.y << ", " << worldspace.z << ")" << std::endl;
					ok = false;
					break;
				}

			}
			if (ok)
			{
				{
					//total Force
					glm::vec3 F = rb.applyForces(rb.getPos(), rb.getVel(), t, dt);
					//intergration rotation
					glm::vec3 dRot = rb.getAngVel() * dt;
					if (glm::dot(dRot, dRot) > 0)
					{
						rb.rotate(sqrt(glm::dot(dRot, dRot)), dRot);
					}

					rb.setAcc(F);

					//semi implicit Eular
					rb.setVel(rb.getVel() + dt * rb.getAcc());
					rb.setPos(rb.getPos() + dt * rb.getVel());
				}
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
		// draw rigidbody
		app.draw(rb.getMesh());
		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

