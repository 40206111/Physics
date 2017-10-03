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
	return glm::vec3(0.0f);
}

///DRAG///
glm::vec3 Drag::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	return glm::vec3(0.0f);
}
