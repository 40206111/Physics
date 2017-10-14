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

///WIND///
glm::vec3 Wind::apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel)
{
	//set normal
	glm::vec3 n = ((this->m_b2->getPos() - pos) * (this->m_b3->getPos() - pos));
	//if the length is not zero
	if (length(n) != 0.0f)
	{
		//set velocity of surface to average velocity
		glm::vec3 vsur = (vel + this->m_b2->getVel() + this->m_b3->getVel()) / 3;

		//set velocity to surface velocity minus wind velocity
		glm::vec3 vtot = vsur - *this->m_windVel;
		//if length of total velocity not equal to zero
		if (length(vtot) != 0.0f)
		{
			//set initial area
			float a = 0.5f * glm::length(n);
			//normalise normal
			n = n / length(n);
			//calculate segment area
			a = a * ((glm::dot(vtot, n)/ length(vtot)));

			//work out force
			glm::vec3 Ftot = 0.5f * glm::pow2(glm::length(vtot)) * a * -n;

			//return force divided by 3
			return (Ftot / 3.0f);
		}
	}
	//return no force
	return glm::vec3(0.0f);
}
