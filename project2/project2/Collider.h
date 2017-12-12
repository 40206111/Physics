#pragma once
#include "Mesh.h"

class Body;

class Collider
{
public:
	Collider() {}
	~Collider() {}

	glm::vec3 getNormal() { return normal; }
	void setNormal(glm::vec3 n) { normal = n; }
	virtual glm::vec3 testCollision(Body* b1, Body* b2);
	virtual glm::vec3 planeCollision(Body* b1, Body* plane);

private:
	glm::vec3 normal;
};

class OBB : public Collider
{
public:
	OBB() {}
	OBB(Body* b);

	glm::vec3 testCollision(Body* b1, Body* b2);
	glm::vec3 testCollision(Body* b1, Body* b2, OBB* other);
	glm::vec3 planeCollision(Body* b1, Body* plane);
	glm::vec3 getCenter() { return center; }
	glm::vec3 getx() { return localxyz[0]; }
	glm::vec3 gety() { return localxyz[1]; }
	glm::vec3 getz() { return localxyz[2]; }
	glm::vec3 gethel() { return halfEdgeLength; }

private:
	glm::vec3 center;
	glm::vec3 localxyz[3];
	glm::vec3 halfEdgeLength;
};

class Sphere : public Collider
{
public:
	Sphere() {}
	Sphere(Body* b);

	glm::vec3 testCollision(Body* b1, Body* b2);
	glm::vec3 testCollision(Body* b1, Body* b2, Sphere* other);
	glm::vec3 testCollision(Body* b1, Body* b2, OBB* other);
	glm::vec3 planeCollision(Body* b1, Body* plane);

private:
	glm::vec3 center;
	float radius;
};
