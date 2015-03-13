#include <vector>
#include <glm/glm.hpp>
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <iostream>

struct Projection
{
public:
	float min;
	float max;
	Projection(float min, float max)
	{
		this->min = min;
		this->max = max;
	}
};

enum ShapeType
{
	PLANE = 0,
	SPHERE = 1,
	BOX = 2,
	NUMBERSHAPE = 3,
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
	//virtual void collisionResponse(glm::vec3 collisionPoint);
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

class Box : public RigidBody
{
public:
	float width, height;
	Box(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, float width, float height, glm::vec4 color);
	Box(glm::vec3 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4 color);
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
	void solveIntersections(PhysObj * offender);
	void debugScene();
	void updateGizmos();
<<<<<<< HEAD
	PhysObj* collisionCheck();
	static bool sphere2sphere(PhysObj* A, PhysObj* B);
	static bool sphere2plane(PhysObj* ball, PhysObj* plane);
	static bool plane2sphere(PhysObj* plane, PhysObj* ball);
	static bool plane2plane(PhysObj* plane, PhysObj* plane2);
	static bool box2Box(PhysObj* ball1, PhysObj* ball2);
	static bool box2Plane(PhysObj* ball, PhysObj* plane);
	static bool box2Sphere(PhysObj* ball, PhysObj* plane);
	static bool plane2Box(PhysObj* ball, PhysObj* plane);
	static bool sphere2Box(PhysObj* plane, PhysObj* ball);
	static int box2PlaneHelper(glm::vec3* verts, glm::vec3 normal, float distance);
=======
	// bool collisionCheck();
	static bool sphere2sphere(PhysObj* A, PhysObj* B);
>>>>>>> parent of 04497f8... sphere collision/response
};