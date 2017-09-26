#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Mesh.h"

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

	///SETTERS///

	//mesh
	void setMesh(Mesh m) { m_mesh = m; }

	//dynamic variables
	void setAcc(const glm::vec3 &vect) { m_acc = vect; }
	void setVel(const glm::vec3 &vect) { m_vel = vect; }
	void setVel(int i, float p) { m_pos[i] = p; m_mesh.setPos(i, p); }
	void setPos(const glm::vec3 &vect) { m_pos = vect; m_mesh.setPos(vect); }
	void setPos(int i, float p) { m_pos[i] = p; m_mesh.setPos(i, p); }

	//physical properties
	void setCor(float cor) { m_cor = cor; }
	void setMass(float mass) { m_mass = mass; }

	///OTHER METHODS///
	void translate(const glm::vec3 &vect);
	void rotate(float angle, const glm::vec3 &vect);
	void scale(const glm::vec3 &vectt);

private:
	Mesh m_mesh;

	float m_mass;
	float m_cor;

	glm::vec3 m_acc;
	glm::vec3 m_vel;
	glm::vec3 m_pos;

};

