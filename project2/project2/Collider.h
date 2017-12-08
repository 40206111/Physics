#pragma once
#include "Mesh.h"

class Body;

class Collider
{
public:
	Collider() {}
	~Collider() {}

	virtual glm::vec3 testCollision(Body* b1, Body* b2, Collider* other);
};

class OBB : public Collider
{
public:
	OBB() {}
	OBB(Body* b);

	glm::vec3 testCollision(Body* b, Body* other);

private:
	glm::vec3 center;
	glm::vec3 localxyz;
	glm::vec3 halfEdgeLength;
};

class Sphere : public Collider
{
public:
	Sphere() {}
	Sphere(Body* b);

	glm::vec3 testCollision(Body* b1, Body* b2, Sphere* other);
	glm::vec3 testCollision(Body* b1, Body* b2, Collider* other);

private:
	glm::vec3 center;
	float radius;
};
