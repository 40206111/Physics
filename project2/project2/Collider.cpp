#include "Collider.h"
#include "Body.h"
#include "Mesh.h"

#include "glm/ext.hpp"

glm::vec3 Collider::testCollision(Body* b1, Body* b2)
{
	return glm::vec3(NULL);
}

glm::vec3 Collider::planeCollision(Body* b1, Body* b2)
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

	glm::vec3 temp(minx, miny, minz);
	this->localxyz[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	this->localxyz[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	this->localxyz[2] = glm::vec3(0.0f, 0.0f, 1.0f);
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

glm::vec3 OBB::planeCollision(Body* b1, Body* plane)
{
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	glm::vec3 worldX = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[0], 1.0f));
	glm::vec3 worldY = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[1], 1.0f));
	glm::vec3 worldZ = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[2], 1.0f));

	glm::vec3 n(0.0f, 1.0f, 0.0f);

	float radius = 
		this->halfEdgeLength.x * abs(glm::dot(n, worldX)) +
		this->halfEdgeLength.y * abs(glm::dot(n, worldY)) +
		this->halfEdgeLength.z * abs(glm::dot(n, worldZ));

	float distance = glm::dot(n, worldC) - plane->getPos().y;

	if (abs(distance) <= radius)
	{
		b1->setPos(1, plane->getPos().y + radius);
		//dunno how to find collision point yet
		return glm::vec3(1.0f);
	}
	else
	{
		return glm::vec3(NULL);
	}
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
	glm::vec3 bothC = worldCOther - worldC;
	float totalR = this->radius + other->radius;
	float centerLen = glm::length(bothC);
	glm::vec3 normal = normalize(bothC);

	if (centerLen < totalR)
	{ 
		if (worldC.y > worldCOther.y)
		{
			b1->setPos(b1->getPos() + (normal * (totalR - centerLen)));
		}
		else
		{
			b2->setPos(b2->getPos() + (normal * (totalR - centerLen)));
		}
		this->setNormal(-normal);
		other->setNormal(normal);
		return glm::vec3(worldC + (normal * this->radius));
	}
	else
	{
		return glm::vec3(NULL);
	}
}

glm::vec3 Sphere::testCollision(Body* b1, Body* b2, OBB* other)
{
	return glm::vec3(NULL);
}

glm::vec3 Sphere::planeCollision(Body* b1, Body* plane)
{
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	glm::vec3 yAxis(0.0f, 1.0f, 0.0f);

	float distance = glm::dot(worldC - plane->getPos(), yAxis);

	if (distance< this->radius)
	{
		b1->setPos(1, plane->getPos().y + this->radius);
		return glm::vec3(worldC - distance * yAxis);
	}
	else
	{
		return glm::vec3(NULL);
	}
}
