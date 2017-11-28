#pragma once
// Math constants
#define _USE_MATH_DEFINES
#include <cmath>  
#include <random>

// Std. Includes
#include <string>
#include <time.h>
#include <vector>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <glm/gtx/orthonormalize.hpp>
#include "glm/ext.hpp"


// project includes
#include "Application.h"
#include "Shader.h"
#include "Mesh.h"
#include "Particle.h"
#include "RigidBody.h"


// time
float t = 0.0f;
const float dt = 0.01f;
float timeMultiplier = 2.0; // controls the speed of the simulation

// forces
Gravity g = Gravity(glm::vec3(0.0f, -9.8f, 0.0f));

// integration: returns the position difference from the acceleration and a timestep
void integratePos(RigidBody &rb, float t, float dt) {
	rb.setAcc(rb.applyForces(rb.getPos(), rb.getVel(), t, dt));
	rb.setVel(rb.getVel() + dt * rb.getAcc());
	glm::vec3 dPos = rb.getPos() + dt * rb.getVel();
	rb.setPos(dPos);
}

// integration: returns the rotation difference from the acceleration and a timestep
void integrateRot(RigidBody &rb, float dt) {
	rb.setAngVel(rb.getAngVel() + dt * rb.getAngAcc());
	glm::mat3 angVelSkew = glm::matrixCross3(rb.getAngVel());
	// create 3x3 rotation matrix from rb rotation matrix
	glm::mat3 R = glm::mat3(rb.getRotate());
	// update rotation matrix
	R += dt*angVelSkew*R;
	R = glm::orthonormalize(R);
	rb.setRotate(glm::mat4(R));
}


//apply impulse when impulse is a float
void applyImpulse(float impulse, glm::vec3 ipos, RigidBody &rb, glm::vec3 normal)
{
	//calculate ininertia with rotation
	glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//calculate change in velocity
	glm::vec3 deltav = (impulse / rb.getMass()) * normal;
	//set new velocity
	rb.setVel(rb.getVel() + deltav);
	//calculate vector from center of mass to impulse position
	glm::vec3 r = ipos - rb.getPos();
	//calculate change in angular velocity
	glm::vec3 deltaomega = impulse * ininertia * glm::cross(r, normal);
	//set new angular velocity
	rb.setAngVel(rb.getAngVel() + deltaomega);
}

//apply impulse when impulse is a vector
void applyImpulse(glm::vec3 impulse, glm::vec3 ipos, RigidBody &rb)
{
	//calculate ininertia with roataion
	glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));
	//calculate change in velocity
	glm::vec3 deltav = (impulse / rb.getMass());
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
	Application::camera.setCameraPosition(glm::vec3(0.0f, 3.0f, 20.0f));

	// create environment ( large plane at y=-3)
	Mesh plane = Mesh::Mesh(Mesh::QUAD);
	//Mesh plane = Mesh::Mesh("resources/models/plane10.obj");
	plane.setShader(Shader("resources/shaders/physics.vert", "resources/shaders/transp.frag"));
	plane.scale(glm::vec3(20.0f, 20.0f, 20.0f));
	plane.translate(glm::vec3(0.0f, -3.0f, 0.0f));
		
	// rigid body set up
	RigidBody rb1 = RigidBody();
	
	// create sphere from obj
	//Mesh m1 = Mesh::Mesh("resources/models/sphere1.obj");

	// create cube from obj
	//Mesh m1 = Mesh::Mesh("resources/models/cube1.obj");

	// load triangle
	//Mesh m1 = Mesh::Mesh(Mesh::TRIANGLE);
	
	// load quad
	//Mesh m1 = Mesh::Mesh(Mesh::QUAD);
	
	// create cube
	Mesh m1 = Mesh::Mesh(Mesh::CUBE);

	rb1.setMesh(m1);
	Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");
	rb1.getMesh().setShader(rbShader);
	rb1.setMass(1.0f);
	rb1.translate(glm::vec3(5.0f, 0.1f, 0.0f));
	rb1.setVel(glm::vec3(-1.0f, 0.0f, 0.0f));
	rb1.setAngVel(glm::vec3(0.5f, .5f, 0.0f));
	// add forces to Rigid body
	rb1.addForce(&g);

	// time
	float currentTime = (float)glfwGetTime();
	float timeAccumulator = 0.0;


	// Game loop
	while (!glfwWindowShouldClose(app.getWindow()))
	{
		// physics time
		float newTime = (float)glfwGetTime();
		float frameTime = newTime - currentTime;
		currentTime = newTime;
		timeAccumulator += frameTime;
		timeAccumulator *= timeMultiplier;
	

		while (timeAccumulator >= dt) {
			// Manage interaction
			app.doMovement(dt);
			glm::mat3 ininertia = glm::mat3(rb1.getRotate()) * rb1.getInvInertia() * glm::mat3(glm::transpose(rb1.getRotate()));

			/*
			**	SIMULATION
			*/
			if (!Application::pauseSimulation) {
				
				
				//declare variaible for amount rb is below the plane
				float newY = 0;
				//declare variable to hold points of collision
				std::vector<glm::vec3> collisions;

				//loop throught vertices
				for (int i = 0; i < rb1.getMesh().getVertices().size(); i++)
				{
					//translate vertices into worldspace
					glm::vec4 worldspace = rb1.getMesh().getModel() * glm::vec4(glm::vec3(rb1.getMesh().getVertices()[i].getCoord()), 1.0f);
					//chack if vertices are below the plane
					if (worldspace.y <= plane.getPos().y)
					{
						//set newY to largest distance below plane
						if (plane.getPos().y - worldspace.y > newY)
							newY = plane.getPos().y - worldspace.y;
						//add vertex to collisions
						collisions.push_back(glm::vec3(worldspace));
					}

				}
				//check if rb has collided with plane
				if (collisions.size() > 0)
				{
					//translate object up to be on plane
					rb1.setPos(1, rb1.getPos().y + newY);
					//declare average
					glm::vec3 average;
					//loop through collisions
					for (glm::vec3 c : collisions)
					{
						//translate collisions y up to when object would be on plane
						c.y += newY;
						//add collision to average
						average += c;
					}
					//divide average by amount of collisions
					average = average / collisions.size();

					//set normal to be plane normal
					glm::vec3 normal(0.0f, 1.0f, 0.0f);
					//set r to be vector from center of mass to impulse
					glm::vec3 r = average - rb1.getPos();

					//set vr
					glm::vec3 vr = (rb1.getVel() + glm::cross(rb1.getAngVel(), r));
					//set vt
					glm::vec3 vt = vr - glm::dot(vr, normal) * normal;

					//calculate numerator of impulse equation
					float numerator = -(1 + rb1.getCor()) * glm::dot(vr, normal);
					//calculate denominator of impulse equation
					float denominator = pow(rb1.getMass(), -1) + glm::dot(normal, glm::cross(ininertia * glm::cross(r, normal), r));

					//divide numerator by denominator to get impulse
					float impulse = (numerator / denominator);

					//apply calculated impulse
					applyImpulse(impulse, average, rb1, normal);

					//declare frcition impulse
					glm::vec3 frictionimpulse;

					//check that velocity isn't zero
					if (vt != glm::vec3(0.0f))
					{
						//set friction impulse
						frictionimpulse = -0.1 * abs(impulse) * glm::normalize(vt);
						//angular energy loss to make object stop rotating
						rb1.setAngVel(rb1.getAngVel() *  0.9);
					}
					else
					{
						//if velocity is zero set friction impulse to 0
						frictionimpulse = glm::vec3(0.0f);
					}

					//apply friction impulse
					applyImpulse(frictionimpulse, average, rb1);

				}
				///	
			}
			// integration (rotation)
			integrateRot(rb1, dt);

			///

			//Integration (position)
			integratePos(rb1, t, dt);

			timeAccumulator -= dt;
			t += dt;
		}

		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		
		// draw groud plane
		app.draw(plane);		
		
		// draw rigid body
		app.draw(rb1.getMesh());

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

