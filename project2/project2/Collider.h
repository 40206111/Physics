#pragma once
#include "Mesh.h"

class Body;

class Collider
{
public:
	Collider() {}
	~Collider() {}

	virtual bool testCollision(Body* other);
};

class OBB : public Collider
{
public:
	OBB() {}
	OBB(Body* b);

	bool testCollision(Body* other);

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

	bool testCollision(Body* other);

private:
	glm::vec3 center;
	float radius;
};
