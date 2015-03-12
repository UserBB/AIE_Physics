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

void RigidBody::collisionResponse(glm::vec3 collisionPoint)
{
	// for now we'll just set the velocity to zero
	velocity = glm::vec3(0, 0, 0);
	collisionPoint = collisionPoint;
}


void RigidBody::update(glm::vec3 gravity, float timeStep)
{
	applyForce(gravity * mass * timeStep);
	oldPosition = position; 
	position += velocity * timeStep;
	//rotationMatrix = glm::rotate(rotation, 0.0f, 0.0f, 1.0f);
}

// scene --------------------

void Scene::update()
{
	for (auto actor : actors)
	{
		actor->update(gravity, Utility::getDeltaTime());
		collisionCheck(actor);
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

typedef bool(*fn)(PhysObj*, PhysObj*);
//function pointer array for doing our collisions

static fn collisionFunctionArray[] =
{ Scene::plane2plane, Scene::plane2sphere,
Scene::sphere2plane, Scene::sphere2sphere };

void Scene::collisionCheck(PhysObj* A)
{
	// check for collisions against all but this one
	for (PhysObj* B : actors)
	{
		if (A != B)
		{
			int idA = A->shapeID;
			int idB = B->shapeID;
			// ignore collision if either is a spring
			if (idA < NUMBERSHAPE && idB < NUMBERSHAPE)
			{
				int functionIndex = (idA * NUMBERSHAPE) + idB;
				fn collisionFunction = collisionFunctionArray[functionIndex];
				if (collisionFunction != NULL)
				{
					collisionFunction(A, B);
				}
			}
		}
	}
}

bool Scene::sphere2sphere(PhysObj* objA, PhysObj* objB)
{
	//try to cast objects to ball and ball
	Sphere *A = dynamic_cast<Sphere*>(objA);
	Sphere *B = dynamic_cast<Sphere*>(objB);
	//if we are successful then test for collision
	if (A != NULL && B != NULL)
	{
		glm::vec3 delta = B->position - A->position;
		float distance = glm::length(delta);
		float intersection = A->radius + B->radius - distance;
		if (intersection > 0)
		{
			glm::vec3 collisionNormal = glm::normalize(delta);
			glm::vec3 relativeVelocity = A->velocity - B->velocity;
			glm::vec3 collisionVector = collisionNormal * (glm::dot(relativeVelocity, collisionNormal));
			glm::vec3 forceVector = collisionVector * 2.0f / (1 / A->mass + 1 / B->mass);
			A->applyForceToActor(B, forceVector);
			glm::vec3 seperationVector = collisionNormal * intersection * .505f;
			A->position -= seperationVector;
			B->position += seperationVector;
			return true;
		}
	}
	return false;
}

bool Scene::plane2sphere(PhysObj* A, PhysObj* B)
{
	//we just need to reverse the order of arguments and pass them into our sphere to plane collision detectio
	//don't want to write the same code twice :)
	return sphere2plane(B, A);
}

bool Scene::sphere2plane(PhysObj* A, PhysObj* B)
{
	//try to cast objects to ball and plane
	Sphere *sphere = dynamic_cast<Sphere*>(A);
	Plane *plane = dynamic_cast<Plane*>(B);
	//if we are successful then check for collision
	if (sphere != NULL && plane != NULL)
	{
		float sphereToPlane = glm::dot(sphere->position, plane->normal) - plane->distance;
		if (abs(sphereToPlane) < sphere->radius)//use abs because collision on both sides
		{
			//calculate collision response:
			glm::vec3 planeNormal = plane->normal;
			if (sphereToPlane <0)
			{
				planeNormal *= -1.0f; //flip the normal if we are behind the plane
			}
			glm::vec3 planeForceVector = sphere->mass * planeNormal * (glm::dot(planeNormal, sphere->velocity));
			sphere->applyForce(-2.0f * planeForceVector);
			return true;
		}
	}
	return false;
}

bool Scene::plane2plane(PhysObj* plane, PhysObj* plane2)
{
	//never used but we'll add it for completeness
	return false;
}