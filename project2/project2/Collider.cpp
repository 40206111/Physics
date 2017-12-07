#include "Collider.h"
#include "Body.h"
#include "Mesh.h"

#include "glm/ext.hpp"

bool Collider::testCollision(Body* other)
{
	return false;
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
}

bool OBB::testCollision(Body* other)
{
	return true;
}

///Sphere///
Sphere::Sphere(Body* b)
{

}

bool Sphere::testCollision(Body* other)
{
	return true;
}
