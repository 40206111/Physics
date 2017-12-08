#include "Collider.h"
#include "Body.h"
#include "Mesh.h"

#include "glm/ext.hpp"

glm::vec3 Collider::testCollision(Body* b1, Body* b2, Collider* other)
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

glm::vec3 OBB::testCollision(Body* b, Body* other)
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


	//set center to middle of sphere
	this->center.x = minx + hx;
	this->center.y = miny + hy;
	this->center.z = minz + hz;

	this->radius = abs(glm::length(this->center - glm::vec3(maxx, maxy, maxz)));

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

glm::vec3 Sphere::testCollision(Body* b1, Body* b2, Collider* other)
{
	std::cout << glm::to_string(this->center) << std::endl;
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	std::cout << glm::to_string(worldC) << std::endl;
	glm::vec3 xAxis(0.0f, 1.0f, 0.0f);

	xAxis *= glm::dot(worldC, xAxis);

	std::cout << glm::length(xAxis - worldC) << std::endl;

	if (glm::length(xAxis - worldC) < this->radius)
	{
		return xAxis;
	}
	else
	{
		return glm::vec3(NULL);
	}
}
