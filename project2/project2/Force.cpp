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

///HOOK///
glm::vec3 Hook::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	float distance = glm::length(this->m_b2->getPos() - this->m_b1->getPos());

	if (distance == 0)
	{
		return(glm::vec3(0.0f));
	}
	else
	{
		glm::vec3 e = (this->m_b2->getPos() - this->m_b1->getPos()) / distance;

		float v1 = glm::dot(e, this->m_b1->getVel());
		float v2 = glm::dot(e, this->m_b2->getVel());

		float fsd = (-this->m_ks*(this->m_rest - distance)) - (this->m_kd * (v1 - v2));

		return glm::vec3(fsd * e);
	}
}
