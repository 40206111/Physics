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

//method for easier debugging
void outVec3(glm::vec3 v)
{
	std::cout << v.x << ",\t" << v.y << ",\t" << v.z << std::endl;
}

//apply impulse method
void applyImpulse(glm::vec3 impulse, glm::vec3 ipos, RigidBody &rb)
{
	//calculate ininertaia with rotation
	glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//calculate change in velocity
	glm::vec3 deltav = impulse / rb.getMass();
	//set new velocity
	rb.setVel(rb.getVel() + deltav);
	//calculate vector from center of mass to impulse position
	glm::vec3 r = ipos - rb.getPos();
	//calculate change in angular velocity
	glm::vec3 deltaomega = ininertia * glm::cross(r, impulse);
	//set new angular velocity
	rb.setAngVel(rb.getAngVel() + deltaomega);

}

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
	plane.scale(glm::vec3(100.0f, 5.0f, 100.0f));
	plane.setShader(Shader("resources/shaders/core.vert", "resources/shaders/core.frag"));

	Shader rbShader = Shader("resources/shaders/core.vert", "resources/shaders/core_blue.frag");

	Force* g = new Gravity(glm::vec3(0.0f, -9.8f, 0.0f));

	RigidBody rb = RigidBody::RigidBody();
	Mesh m = Mesh::Mesh(Mesh::MeshType::CUBE);
	rb.setMesh(m);
	rb.getMesh().setShader(rbShader);
	rb.scale(glm::vec3(2.0f, 6.0f, 2.0f));
	rb.setMass(2.0f);

	// rigid body motion values
	rb.translate(glm::vec3(0.0f, 6.0f, 0.0f));
	rb.setVel(glm::vec3(0.0f, 3.0f, 0.0f));
	rb.setAngVel(glm::vec3(0.0f, 0.0f, 0.0f));

	rb.addForce(g);

	glm::vec3 ipos(1.0f, 4.0f, 0.0f);
	glm::vec3 impulse(-4.0f, 0.0f, 0.0f);
	bool applied = false;
	std::vector<glm::vec3> collisions;

	double startt = t;
	bool ok = true;

	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		//// Set frame time
		double newTime = glfwGetTime();
		double frameTime = newTime - currentTime;
		frameTime *= 1.0f;
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
			//loop through collisions
			for (int i = 0; i < rb.getMesh().getVertices().size(); i++)
			{
				//put vertex into worldspace
				glm::vec4 worldspace = rb.getMesh().getModel() * glm::vec4(glm::vec3(rb.getMesh().getVertices()[i].getCoord()), 1.0f);
				//check if vertex is below plane
				if (worldspace.y <= plane.getPos().y && ok)
				{
					//add vertex to collisions
					collisions.push_back(glm::vec3(worldspace));
				}

			}
			//if collided
			if (collisions.size() > 0 && ok)
			{
				//declare average
				glm::vec3 average;
				//loop through collisions
				for (glm::vec3 c : collisions)
				{
					//add collision to average
					average += c;
					//output collision
					outVec3(c);
				}
				//calculate average
				average = average / collisions.size();
				//output average
				std::cout << "AVERAGE" << std::endl;
				outVec3(average);
				//set ok to false
				ok = false;
			}
			//if hasn't collided 
			if (ok)
			{
				///
				//calculate inverse inetia with rotation
				glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));

				//intergration rotation
				rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
				glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
				glm::mat3 R = glm::mat3(rb.getRotate());
				R += dt*angVelSkew * R;
				R = glm::orthonormalize(R);
				rb.setRotate(R);

				///

				//total Force/mass
				glm::vec3 F = rb.applyForces(rb.getPos(), rb.getVel(), t, dt);

				//sett acceleration
				rb.setAcc(F);

				//semi implicit Eular
				rb.setVel(rb.getVel() + dt * rb.getAcc());
				rb.setPos(rb.getPos() + dt * rb.getVel());

			}




			//accumulate
			accumulator -= dt;
			t += dt;
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
