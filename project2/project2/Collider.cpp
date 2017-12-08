#include "Collider.h"
#include "Body.h"
#include "Mesh.h"

#include "glm/ext.hpp"

glm::vec3 Collider::testCollision(Body* b1, Body* b2)
{
	return glm::vec3(NULL);
}


///OBB///
OBB::OBB(Body* b)
{
	std::vector<Vertex> v = b->getMesh().getVertices();
	//declair minimums
	float minx = v[0].getCoord().x;
	float miny = v[0].getCoord().y;
	float minz = v[0].getCoord().z;
	float maxx = v[0].getCoord().x;
	float maxy = v[0].getCoord().y;
	float maxz = v[0].getCoord().z;

	// find minimum x, y and z
	for (int i = 1; i < v.size(); i++)
	{
		if (v[i].getCoord().x < minx)
		{
			minx = v[i].getCoord().x;
		}
		if (v[i].getCoord().y < miny)
		{
			miny = v[i].getCoord().y;
		}
		if (v[i].getCoord().z < minz)
		{
			minz = v[i].getCoord().z;
		}
		if (v[i].getCoord().x < maxx)
		{
			maxx = v[i].getCoord().x;
		}
		if (v[i].getCoord().y < maxy)
		{
			maxy = v[i].getCoord().y;
		}
		if (v[i].getCoord().z < maxz)
		{
			maxz = v[i].getCoord().z;
		}
	}

	//calculate half lengths
	float hx = (glm::abs(maxx) + abs(minx)) / 2;
	float hy = (glm::abs(maxy) + abs(miny)) / 2;
	float hz = (glm::abs(maxz) + abs(minz)) / 2;

	//set half length
	this->halfEdgeLength = glm::vec3(hx, hy, hz);

	//set center to middle of obb
	this->center.x = minx + hx;
	this->center.y = miny + hy;
	this->center.z = minz + hz;
}

glm::vec3 OBB::testCollision(Body* b1, Body* b2)
{
	Sphere* x = dynamic_cast<Sphere*>(b2->getCollider());
	OBB* y = dynamic_cast<OBB*>(b2->getCollider());

	if (x)
	{
		return b2->getCollider()->testCollision(b1, b2);
	}
	else if (y)
	{
		return this->testCollision(b1, b2, y);
	}
	else
	{
		return glm::vec3(NULL);
	}
}

glm::vec3 OBB::testCollision(Body* b1, Body* b2, OBB* Collider)
{
	return glm::vec3(NULL);
}

///Sphere///
Sphere::Sphere(Body* b)
{
	std::vector<Vertex> v = b->getMesh().getVertices();
	//declair minimums
	float minx = v[0].getCoord().x;
	float miny = v[0].getCoord().y;
	float minz = v[0].getCoord().z;
	float maxx = v[0].getCoord().x;
	float maxy = v[0].getCoord().y;
	float maxz = v[0].getCoord().z;
	glm::vec3 maxCoord = v[0].getCoord();

	// find minimum x, y and z
	for (int i = 1; i < v.size(); i++)
	{
		if (v[i].getCoord().x < minx)
		{
			minx = v[i].getCoord().x;
		}
		if (v[i].getCoord().y < miny)
		{
			miny = v[i].getCoord().y;
		}
		if (v[i].getCoord().z < minz)
		{
			minz = v[i].getCoord().z;
		}
		if (v[i].getCoord().x > maxx)
		{
			maxx = v[i].getCoord().x;
		}
		if (v[i].getCoord().y > maxy)
		{
			maxy = v[i].getCoord().y;
		}
		if (v[i].getCoord().z > maxz)
		{
			maxz = v[i].getCoord().z;
		}
		if (glm::length(v[i].getCoord()) > glm::length(maxCoord))
		{
			maxCoord = v[i].getCoord();
		}
	}

	//calculate half lengths
	float hx = (glm::abs(maxx) + abs(minx)) / 2;
	float hy = (glm::abs(maxy) + abs(miny)) / 2;
	float hz = (glm::abs(maxz) + abs(minz)) / 2;


	//set center to middle of sphere
	this->center.x = minx + hx;
	this->center.y = miny + hy;
	this->center.z = minz + hz;

	//set radius
	this->radius = length(maxCoord);
}


glm::vec3 Sphere::testCollision(Body* b1, Body* b2)
{
	Sphere* x = dynamic_cast<Sphere*>(b2->getCollider());
	OBB* y = dynamic_cast<OBB*>(b2->getCollider());

	if (x)
	{
		return this->testCollision(b1, b2, x);
	}
	else if (y)
	{
		return this->testCollision(b1, b2, y);
	}
	else
	{
		return glm::vec3(NULL);
	}
}

glm::vec3 Sphere::testCollision(Body* b1, Body* b2, Sphere* other)
{

	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	glm::vec3 worldCOther = glm::vec3(b2->getMesh().getModel() * glm::vec4(other->center, 1.0f));

	if (glm::length(worldCOther - worldC) < this->radius + other->radius)
	{ 
		return glm::vec3(worldCOther - worldC);
	}
	else
	{
		return glm::vec3(NULL);
	}
}

glm::vec3 Sphere::testCollision(Body* b1, Body* b2, OBB* other)
{
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	glm::vec3 yAxis(0.0f, 1.0f, 0.0f);

	float distance= glm::dot(worldC, yAxis);

	if (distance - b2->getPos().y< this->radius)
	{
		return glm::vec3(b1->getPos().x, b2->getPos().y, b1->getPos().z);
	}
	else
	{
		return glm::vec3(NULL);
	}
}
