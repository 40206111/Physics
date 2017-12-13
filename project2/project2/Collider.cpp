#include "Collider.h"
#include "Body.h"
#include "Mesh.h"

#include "glm/ext.hpp"

///Virtual methods///
glm::vec3 Collider::testCollision(Body* b1, Body* b2)
{
	return glm::vec3(NULL);
}

glm::vec3 Collider::planeCollision(Body* b1, Body* b2)
{
	return glm::vec3(NULL);
}

///OBB///
//Constructor
OBB::OBB(Body* b)
{
	//set v to be vector of vertices
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

	//set axis
	this->localxyz[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	this->localxyz[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	this->localxyz[2] = glm::vec3(0.0f, 0.0f, 1.0f);

	//set vertices
	for (int i = -1; i < 2; i += 2)
	{
		for (int j = -1; j < 2; j += 2)
		{
			for (int k = -1; k < 2; k += 2)
			{
				glm::vec3 temp = this->center;
				temp += this->localxyz[0] * i * hx;
				temp += this->localxyz[1] * j * hy;
				temp += this->localxyz[2] * k * hz;

				this->vertices.push_back(temp);
			}
		}
	}
}

//Main collision method
glm::vec3 OBB::testCollision(Body* b1, Body* b2)
{
	//cast collision object as sphere
	Sphere* x = dynamic_cast<Sphere*>(b2->getCollider());
	//cast collision object as OBB
	OBB* y = dynamic_cast<OBB*>(b2->getCollider());

	//if collided with sphere
	if (x)
	{
		//call sphere collider method
		return b2->getCollider()->testCollision(b1, b2);
	}
	//if collided with obb
	else if (y)
	{
		//call obb collide with obb method
		return this->testCollision(b1, b2, y);
	}
	else
	{
		//no collision
		return glm::vec3(NULL);
	}
}

//to test obb against obb on axis
bool testAxis(glm::vec3 axis, OBB* obb, OBB* other, float distance, glm::vec3 xyz[], glm::vec3 xyz2[])
{
	//get first radius on axis
	float r1 =
		obb->gethel().x * abs(glm::dot(axis, xyz[0])) +
		obb->gethel().y * abs(glm::dot(axis, xyz[1])) +
		obb->gethel().z * abs(glm::dot(axis, xyz[2]));
	//get second radius on axis
	float r2 =
		other->gethel().x * abs(glm::dot(axis, xyz2[0])) +
		other->gethel().y * abs(glm::dot(axis, xyz2[1])) +
		other->gethel().z * abs(glm::dot(axis, xyz2[2]));

	// add radi together
	float radius = r1 + r2;

	//if radius greater than distance
	if (radius >= distance)
	{
		//return true
		return true;
	}
	else
	{
		//return false
		return false;
	}
}

glm::vec3 norm(glm::vec3 xyz[], glm::vec3 point, OBB* obb)
{
	float minPen = glm::length2(xyz[0] * obb->gethel()[0] - glm::dot(point, xyz[0]));
	int index = 0;
	for (int i = 1; i < 3; i++)
	{
		float pen = glm::length2(xyz[i] * obb->gethel()[i] - glm::dot(point, xyz[i]));
		if (pen < minPen)
		{
			minPen = pen;
			index = i;
		}
	}

	return xyz[index];
}

glm::vec3 deep(glm::vec3 normal, glm::vec3 v[])
{
	float minDist = 0;
	bool first = true;
	glm::vec3 deepPoint;
	int counter = 0;
	for (int i = 0; i < 8; i++)
	{
		float dist = glm::dot(normal, v[i]);
		if (first)
		{
			minDist = dist;
			first = false;
		}
		if (dist > minDist)
		{
			minDist = dist;
			deepPoint = v[i];
			counter = 1;
		}
		if (dist == minDist)
		{
			deepPoint += v[i];
			counter++;
		}
	}

	return deepPoint / counter;
}

//method for obb colliding with obb
glm::vec3 OBB::testCollision(Body* b1, Body* b2, OBB* other)
{
	//set center1 to world space
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	//put local axis into worldspace
	glm::vec3 worldxyz[3];
	worldxyz[0] = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[0], 1.0f));
	worldxyz[1] = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[1], 1.0f));
	worldxyz[2] = glm::vec3(b1->getMesh().getRotate() * glm::vec4(this->localxyz[2], 1.0f));

	//set center2 to world space
	glm::vec3 worldCOther = glm::vec3(b2->getMesh().getModel() * glm::vec4(other->center, 1.0f));
	//put local axis into worldspace
	glm::vec3 worldxyzOther[3];
	worldxyzOther[0] = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->localxyz[0], 1.0f));
	worldxyzOther[1] = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->localxyz[1], 1.0f));
	worldxyzOther[2] = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->localxyz[2], 1.0f));

	//calculate distance
	float distance = glm::length(worldCOther - worldC);
	//initiate index
	int index = 0;
	//initiate axis
	glm::vec3 axis[15];

	//set axis
	axis[0] = worldxyz[0];
	axis[1] = worldxyz[1];
	axis[2] = worldxyz[2];
	axis[3] = worldxyzOther[0];
	axis[4] = worldxyzOther[1];
	axis[5] = worldxyzOther[2];
	axis[6] = glm::normalize(glm::cross(worldxyz[0], worldxyzOther[0]));
	axis[7] = glm::normalize(glm::cross(worldxyz[0], worldxyzOther[1]));
	axis[8] = glm::normalize(glm::cross(worldxyz[0], worldxyzOther[2]));
	axis[9] = glm::normalize(glm::cross(worldxyz[1], worldxyzOther[0]));
	axis[10] = glm::normalize(glm::cross(worldxyz[1], worldxyzOther[1]));
	axis[11] = glm::normalize(glm::cross(worldxyz[1], worldxyzOther[2]));
	axis[12] = glm::normalize(glm::cross(worldxyz[2], worldxyzOther[0]));
	axis[13] = glm::normalize(glm::cross(worldxyz[2], worldxyzOther[1]));
	axis[14] = glm::normalize(glm::cross(worldxyz[2], worldxyzOther[2]));
	//initialise testbool
	bool test = true;

	//loop while test collision is true and there are more axis
	while (test && index < 6)
	{
		//if axis isn't 0 test if it collides on axis
		if (axis[index] != glm::vec3(0.0f))
			test = testAxis(axis[index], this, other, distance, worldxyz, worldxyzOther);
		//increase index
		index++;
	}
	
	//test is still true
	if (test)
	{
		//initialise arrays of world vertices
		glm::vec3 v1[8];
		glm::vec3 v2[8];

		//add world space vertices to list
		for (int i = 0; i < 8; i++)
		{
			v1[i] = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->vertices[i], 1.0f));
			v2[i] = glm::vec3(b2->getMesh().getModel() * glm::vec4(other->vertices[i], 1.0f));
		}

		//Declaire average point
		glm::vec3 ap;
		float minDist = 0;
		int counter = 0;
		bool first = true;

		for (int i = 0; i < 8; i++)
		{
			glm::vec3  d = v2[i] - worldC;

			glm::vec3 point = worldC;

			for (int j = 0; j < 3; j++)
			{
				float dist = dot(d, worldxyz[j]);
				if (dist > this->gethel()[j])
				{
					dist = this->gethel()[j];
				}
				if (dist < -this->gethel()[j])
				{
					dist = -this->gethel()[j];
				}
				point += dist * worldxyz[j];
			}
			float thisDist = glm::length2(point - v2[i]);

			if (thisDist == minDist)
			{
				ap += point;
				counter++;
			}
			else if (thisDist < minDist)
			{
				ap = point;
				minDist = thisDist;
				counter = 1;
			}
			if (first)
			{
				ap = point;
				minDist = thisDist;
				first = false;
				counter = 1;
			}
		}

		first = true;

		for (int i = 0; i < 8; i++)
		{
			glm::vec3  d = v1[i] - worldCOther;

			glm::vec3 point = worldCOther;

			for (int j = 0; j < 3; j++)
			{
				float dist = dot(d, worldxyzOther[j]);
				if (dist > other->gethel()[j])
				{
					dist = other->gethel()[j];
				}
				if (dist < -other->gethel()[j])
				{
					dist = -other->gethel()[j];
				}

				point += dist * worldxyzOther[j];
			}
			float thisDist = glm::length2(point - v1[i]);

			if (!first && thisDist == minDist)
			{
				ap += point;
				counter++;
			}
			else if (!first && thisDist < minDist)
			{
				ap = point;
				minDist = thisDist;
				counter = 1;
			}
			if (first && thisDist <= minDist)
			{
				ap = point;
				minDist = thisDist;
				first = false;
				counter = 1;
			}
		}
		ap = ap / counter;

		glm::vec3 deepPoint;
		if (first)
		{
			glm::vec3 normal = norm(worldxyz, ap, this);
			if (glm::dot(ap - worldC, normal) < 0)
			{
				normal *= -1;
			}
			this->setNormal(normal);
			other->setNormal(-normal);

			deepPoint = deep(normal, v2);
			glm::vec3 deepPoint2 = deep(normal, v1);

			glm::vec3 dpProj = normal * glm::dot(normal, deepPoint);
			glm::vec3 dpProj2 = normal * glm::dot(normal, deepPoint2);

			b2->setPos(b2->getPos() + (normal * glm::length(dpProj2 - dpProj)));
		}
		else
		{
			glm::vec3 normal = norm(worldxyzOther, ap, other);
			if (glm::dot(ap - worldCOther, normal) < 0)
			{
				normal *= -1;
			}
			this->setNormal(-normal);
			other->setNormal(normal);

			deepPoint = deep(normal, v1);
			glm::vec3 deepPoint2 = deep(normal, v2);

			glm::vec3 dpProj = normal * glm::dot(normal, deepPoint);
			glm::vec3 dpProj2 = normal * glm::dot(normal, deepPoint2);

			b1->setPos(b1->getPos() + (normal * glm::length(dpProj2 - dpProj)));
		}
		return deepPoint;
	}
	else
	{
		return glm::vec3(NULL);
	}
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
		return glm::vec3(worldC - distance * n);
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
	glm::vec3 worldC = glm::vec3(b1->getMesh().getModel() * glm::vec4(this->center, 1.0f));
	glm::vec3 worldCOther = glm::vec3(b2->getMesh().getModel() * glm::vec4(other->getCenter(), 1.0f));
	glm::vec3 worldX = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->getx(), 1.0f));
	glm::vec3 worldY = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->gety(), 1.0f));
	glm::vec3 worldZ = glm::vec3(b2->getMesh().getRotate() * glm::vec4(other->getz(), 1.0f));
	glm::vec3 centers = worldCOther - worldC;
	glm::vec3 n = glm::normalize(centers);

	glm::vec3 projC = n * glm::dot(n, worldC);
	glm::vec3 projCOther = n * glm::dot(n, worldCOther);

	float radius =
		other->gethel().x * abs(glm::dot(n, worldX)) +
		other->gethel().y * abs(glm::dot(n, worldY)) +
		other->gethel().z * abs(glm::dot(n, worldZ));

	radius += this->radius;
	float distance = glm::length(projCOther - projC);

	if (radius >= distance)
	{
		if (worldC.y > worldCOther.y)
		{
			b1->setPos(b1->getPos() + (n * (radius - distance)));
		}
		else
		{
			b2->setPos(b2->getPos() + (n * (radius - distance)));
		}
		this->setNormal(-n);
		other->setNormal(n);
		return glm::vec3(worldC + (n * this->radius));
	}
	else
	{
		return glm::vec3(NULL);
	}
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
