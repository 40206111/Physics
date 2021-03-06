#pragma once
#include <glm/glm.hpp>
#include <iostream>

class Body;

class Force
{
public:
	Force() {}
	~Force() {}

	virtual glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);
};

/*
** GRAVITY CLASS
*/
class Gravity : public Force
{
public:
	Gravity() {}
	Gravity(const glm::vec3 &gravity) { m_gravity = gravity; }

	glm::vec3 getGravity() const { return m_gravity; }
	void setGravity(glm::vec3 gravity) { m_gravity = gravity; }

	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	glm::vec3 m_gravity = glm::vec3(0.0f, -9.8f, 0.0f);
};

/*
** DRAG CLASS
*/
class Drag : public Force
{
public:
	Drag() {}

	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);
private:

};

/*
** HOOK CLASS
*/
class Hook : public Force
{
public:
	Hook() {}
	Hook(Body* b1, Body* b2, float ks, float kd, float rest)
	{
		m_ks = ks;
		m_kd = kd;
		m_rest = rest;
		m_b1 = b1;
		m_b2 = b2;
	}

	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	//spring stiffness
	float m_ks;
	//damping coefficient
	float m_kd;
	//spring rest length
	float m_rest;

	Body* m_b1;
	Body* m_b2;
};

/*
** WIND CLASS
*/
class Wind : public Force
{
public:
	Wind() {}
	Wind(Body* b2, Body* b3, glm::vec3* windVel)
	{
		m_windVel = windVel;
		m_b2 = b2;
		m_b3 = b3;
	}

	glm::vec3 apply(float mass, const glm::vec3 &pos, const glm::vec3 &vel);

private:
	glm::vec3* m_windVel;

	Body* m_b2;
	Body* m_b3;
};

