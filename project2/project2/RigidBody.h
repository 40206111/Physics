#pragma once
#include "body.h"

class RigidBody : public Body
{
public:
	RigidBody();
	~RigidBody();

	void setAngVel(const glm::vec3 &omega) { m_angVel = omega; }
	void setAngAccl(const glm::vec3 &alpha) { m_angAcc = alpha; }
	void setInertia(const glm::mat3 &invInertia) { m_invInertia = invInertia; }
	void setMass(float mass) { Body::m_mass = mass; InInertia(); }

	glm::vec3 getAngVel() { return m_angVel; }
	glm::vec3 getAngAcc() { return m_angAcc; }
	glm::mat3 getInvInertia() { return m_invInertia; }
	void InInertia();

private:
	float m_density;
	glm::mat3 m_invInertia;
	glm::vec3 m_angVel;
	glm::vec3 m_angAcc;
};

