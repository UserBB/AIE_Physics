#include <vector>
#include <glm/glm.hpp>
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	//BOX = 2,
	NUMBERSHAPE = 2,
};

class PhysObj
{
public:
	void virtual update(glm::vec3 gravity, float timeStep) = 0;
	//void virtual debug() = 0;
	void virtual createGizmo() = 0;
	void virtual resetPosition(){};
	ShapeType shapeID;
};

class Plane : public PhysObj
{
public:
	glm::vec3 normal;
	float distance;
	void virtual update(glm::vec3 gravity, float timeStep){};
	//void virtual debug(){};
	void virtual createGizmo();
	Plane(glm::vec3 normal, float distance);
	Plane();
};

class RigidBody : public PhysObj
{
public:
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	glm::vec3 oldPosition;
	glm::vec3 contactPoint;
	float mass;
	float rotation;
	glm::mat4 rotationMatrix;
	RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass);
	virtual void update(glm::vec3 gravity, float timeStep);
	//virtual void debug();
	virtual void collisionResponse(glm::vec3 collisionPoint);
	virtual void resetPosition(){ position = oldPosition; };
	void applyForce(glm::vec3 force);
	void RigidBody::applyForceToActor(RigidBody* actorB, glm::vec3 force);

};

class Sphere : public RigidBody
{
public: 
	float radius;
	Sphere(glm::vec3 position, glm::vec3 velocity, float rotation, float radius, float mass, glm::vec4 color);
	Sphere(glm::vec3 position, float angle, float speed, float rotation, float radius, float mass, glm::vec4 color);
	void createGizmo();
};

class Scene
{
public:
	glm::vec3 gravity;
	float timeStep;
	std::vector<PhysObj*> actors;
	void addActor(PhysObj*);
	void removeActor(PhysObj*);
	void update();
	// void solveIntersections();
	void debugScene();
	void updateGizmos();
	void collisionCheck(PhysObj* A);
	static bool sphere2sphere(PhysObj* A, PhysObj* B);
	static bool sphere2plane(PhysObj* ball, PhysObj* plane);
	static bool plane2sphere(PhysObj* plane, PhysObj* ball);
	static bool plane2plane(PhysObj* plane, PhysObj* plane2);
};