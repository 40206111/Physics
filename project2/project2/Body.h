#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Mesh.h"
#include "Force.h"
#include "Collider.h"

class Body
{
public:
	Body();
	~Body();

	///GETTERS///

	//mesh
	Mesh& getMesh() { return m_mesh; }

	//transform matrices
	glm::mat4 getTranslate() const { return m_mesh.getTranslate(); }
	glm::mat4 getRotate() const { return m_mesh.getRotate(); }
	glm::mat4 getScale() const { return m_mesh.getScale(); }

	//dynamic variables
	glm::vec3& getAcc() { return m_acc; }
	glm::vec3& getVel() { return m_vel; }
	glm::vec3& getPos() { return m_pos; }

	//physical properties
	float getMass() const { return m_mass; }
	float getCor() const { return m_cor; }

	//force
	std::vector<Force*> getForces() { return m_forces; }
	void addForce(Force *f) { m_forces.push_back(f); }
	
	//collider
	Collider* getCollider() { return m_collider; }
	void setCollider(Collider *c) { m_collider = c; }

	//sum all forces applied to a body and return acceleration
	glm::vec3 applyForces(glm::vec3 x, glm::vec3 v, double t, float dt)
	{
		glm::vec3 fAccumulator = glm::vec3(0.0f);

		for (auto &f : m_forces)
		{
			fAccumulator += f->apply(getMass(), x, v);
		}
		return fAccumulator / getMass();
	}

	///SETTERS///

	//mesh
	void setMesh(Mesh m) { m_mesh = m; }

	//dynamic variables
	void setAcc(const glm::vec3 &vect) { m_acc = vect; }
	void setVel(const glm::vec3 &vect) { m_vel = vect; }
	void setVel(int i, float p) { m_vel[i] = p; }
	void setPos(const glm::vec3 &vect) { m_pos = vect; m_mesh.setPos(vect); }
	void setPos(int i, float p) { m_pos[i] = p; m_mesh.setPos(i, p); }
	void setRotate(glm::mat4 R) { m_mesh.setRotate(R); }

	//physical properties
	void setCor(float cor) { m_cor = cor; }
	void setMass(float mass) { m_mass = mass; }

	///OTHER METHODS///
	void translate(const glm::vec3 &vect);
	void rotate(float angle, const glm::vec3 &vect);
	void scale(const glm::vec3 &vectt);

protected:
	float m_mass;

private:
	Mesh m_mesh;

	float m_cor;

	std::vector<Force*> m_forces;
	Collider* m_collider;
	glm::vec3 m_acc;
	glm::vec3 m_vel;
	glm::vec3 m_pos;

};

