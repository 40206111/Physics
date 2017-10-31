#include "RigidBody.h"
#include "Mesh.h"



RigidBody::RigidBody()
{
	setMesh(Mesh::Mesh());
}


RigidBody::~RigidBody()
{
}

void RigidBody::InInertia()
{
	float twelthmass = (1.0f / 12.0f)*this->getMass();
	float w2 = pow(this->getScale()[0][0], 2);
	float h2 = pow(this->getScale()[1][1], 2);
	float d2 = pow(this->getScale()[2][2], 2);

	float first = twelthmass + (h2 + d2);
	float second = twelthmass + (w2 + d2);
	float third = twelthmass + (w2 + h2);
	
	glm::mat3 inertia = glm::mat3(0.0f);

	inertia[0][0] = first;
	inertia[1][1] = second;
	inertia[2][2] = third;

	this->m_invInertia = glm::inverse(inertia);
}
