#include <iostream>
#include <cmath>
#include "Force.h"
#include "Body.h"
#include "glm/ext.hpp"


glm::vec3 Force::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return glm::vec3(0.0f);
}

///GRAVITY///
glm::vec3 Gravity::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	glm::vec3 gravity = this->getGravity() * mass;
	return gravity;
}

///DRAG///
glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	glm::vec3 drag = 0.5 * 1.225 * -vel * glm::length(vel) * 1.05 * 0.01;
	return drag;
}
