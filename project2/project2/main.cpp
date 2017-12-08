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
float timeMultiplier = 1.0; // controls the speed of the simulation

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

void checkColide(RigidBody &rb, Body &plane)
{
	glm::mat3 ininertia = glm::mat3(rb.getRotate()) * rb.getInvInertia() * glm::mat3(glm::transpose(rb.getRotate()));

	glm::vec3 collide = rb.getCollider()->testCollision(&rb, &plane, plane.getCollider());

	if (collide != glm::vec3(NULL))
	{
		Application::pauseSimulation = true;
	}
}

// main function
int main()
{
	// create application
	Application app = Application::Application();
	app.initRender();
	Application::camera.setCameraPosition(glm::vec3(0.0f, 3.0f, 20.0f));

	// create environment ( large plane at y=-3)
	Mesh p = Mesh::Mesh(Mesh::QUAD);
	RigidBody plane = RigidBody();
	plane.setMesh(p);
	//Mesh plane = Mesh::Mesh("resources/models/plane10.obj");
	plane.getMesh().setShader(Shader("resources/shaders/physics.vert", "resources/shaders/transp.frag"));
	plane.scale(glm::vec3(10.0f, 10.0f, 10.0f));
	plane.translate(glm::vec3(0.0f, -3.0f, 0.0f));
	plane.setCollider(new OBB(&plane));
	int rbAmount = 1;
	std::vector<RigidBody> rb(rbAmount);
	Application::pauseSimulation = true;
	
	// create sphere from obj
	Mesh m1 = Mesh::Mesh("resources/models/sphere1.obj");

	// create cube from obj
	//Mesh m1 = Mesh::Mesh("resources/models/cube1.obj");

	// load triangle
	//Mesh m1 = Mesh::Mesh(Mesh::TRIANGLE);
	
	// load quad
	//Mesh m1 = Mesh::Mesh(Mesh::QUAD);
	
	// create cube
	//Mesh m1 = Mesh::Mesh(Mesh::CUBE);
	Shader rbShader = Shader("resources/shaders/physics.vert", "resources/shaders/physics.frag");

	for (int i = 0; i < rbAmount; i++)
	{
		// rigid body set up
		rb[i] = RigidBody();
		rb[i].setMesh(m1);
		rb[i].getMesh().setShader(rbShader);
		rb[i].setMass(1.0f);
		rb[i].setPos(glm::vec3((-rbAmount/2) + i, 0.0f + sin((i/2)) * 2, 0.0f));
		rb[i].setVel(glm::vec3(5.0f, 10.0f, 0.0f));
		rb[i].setAngVel(glm::vec3(0.5f, 0.5f, 0.0f));
		// add forces to Rigid body
		rb[i].addForce(&g);
		rb[i].setCollider(new Sphere(&rb[i]));
	}

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

			/*
			**	SIMULATION
			*/

			if (!Application::pauseSimulation) {
				for (int i = 0; i < rbAmount; i++)
				{
					checkColide(rb[i], plane);
				}
				for (int i = 0; i < rbAmount; i++) {
					// integration (rotation)
					integrateRot(rb[i], dt);

					//Integration (position)
					integratePos(rb[i], t, dt);
				}

			}

			timeAccumulator -= dt;
			t += dt;
		}

		/*
		**	RENDER 
		*/		
		// clear buffer
		app.clear();
		
		// draw groud plane
		app.draw(plane.getMesh());		
		
		for (int i = 0; i < rbAmount; i++)
		{
			// draw rigid body
			app.draw(rb[i].getMesh());
		}

		app.display();
	}

	app.terminate();

	return EXIT_SUCCESS;
}

