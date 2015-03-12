#include "Physics_Framework.h"

// plane ----------------

Plane::Plane(glm::vec3 normal, float distance)
{
	this->normal = glm::normalize(normal);
	this->distance = distance;
	shapeID = PLANE;
}

void Plane::createGizmo()
{
	float lineSegLength = 100;
	glm::vec3 center = normal * distance;
	glm::vec3 parallel = glm::vec3(normal.y, -normal.x, 0);
	glm::vec4 color(1);
	glm::vec3 start = center + (parallel * lineSegLength);
	glm::vec3 end = center - (parallel * lineSegLength);
	Gizmos::addLine(start, end, color);
}

// sphere ----------------

Sphere::Sphere(glm::vec3 position, glm::vec3 velocity, float rotation, float radius, float mass, glm::vec4 color)
		: RigidBody(position, velocity, rotation, mass)
{
	this->radius = radius;
	this->color = color;
	std::cout << "adding sphere " << this->position.x << ',' << this->position.y << ',' << this->position.z << std::endl;
	shapeID = SPHERE;
}

Sphere::Sphere(glm::vec3 position, float angle, float speed, float rotation, float radius, float mass, glm::vec4 color)
		: RigidBody(position, glm::vec3(speed * cos(angle), speed * sin(angle), 0), rotation, mass) 
{
	this->radius = radius;
	this->color = color;
	std::cout << "adding ball " << this->position.x << ',' << this->position.y << ',' << this->position.z << std::endl;
	shapeID = SPHERE;
}

void Sphere::createGizmo()
{
	Gizmos::addSphere(position, 15, 15, radius, color);
}

// rigidbody --------------

RigidBody::RigidBody(glm::vec3 position, glm::vec3 velocity, float rotation, float mass)
{
	std::cout << "adding rigid body " << position.x << ',' << position.y << ',' << position.z << std::endl;
	this->position = position;
	this->velocity = velocity;
	this->rotation = rotation;
	this->mass = mass;
	color = glm::vec4(1);
}

void RigidBody::applyForce(glm::vec3 force)
{
	velocity += force / mass;
}

void RigidBody::applyForceToActor(RigidBody* actorB, glm::vec3 force)
{
	actorB->applyForce(force);
	applyForce(-force);
}

void RigidBody::update(glm::vec3 gravity, float timeStep)
{
	position += velocity * timeStep;
	//applyForce(gravity * mass * timeStep);
	/*oldPosition = position; 
	position += velocity * timeStep;
	rotationMatrix = glm::rotate(rotation, 0.0f, 0.0f, 1.0f); glm::rotate()*/
}

// scene --------------------

void Scene::update()
{
	for (auto actor : actors)
	{
		actor->update(gravity, Utility::getDeltaTime());
		actor->createGizmo();
	}

}

void Scene::addActor(PhysObj* object)
{
	actors.push_back(object);
}

void Scene::removeActor(PhysObj* object)
{
	auto item = std::find(actors.begin(), actors.end(), object);
	if (item < actors.end())
	{
		actors.erase(item);
	}
}