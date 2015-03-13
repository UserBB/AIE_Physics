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
	Gizmos::addSphere(position, radius, 10, 10, color);
}

// box --------------------

Box::Box(glm::vec3 position, glm::vec3 velocity, float rotation, float mass, float width, float height, glm::vec4 color)
: RigidBody(position, velocity, rotation, mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->color = color;
	shapeID = BOX;
}

Box::Box(glm::vec3 position, float angle, float speed, float rotation, float width, float height, float mass, glm::vec4 color)
: RigidBody(position, glm::vec3(speed * cos(angle), speed * sin(angle), 0), rotation, mass)  //call the base class constructor
{
	this->width = width;
	this->height = height;
	this->color = color;
	shapeID = BOX;
}

void Box::createGizmo()
{
	//Get a rotation matrix from our 2D rotation so we can draw our gizmo
	Gizmos::addAABBFilled(position, glm::vec3(width, height, 0), color, &rotationMatrix);
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
<<<<<<< HEAD
=======
		actor->createGizmo();
>>>>>>> parent of 04497f8... sphere collision/response
	}

	bool runPhysics = true;
	int maxIterations = 100; //emergency count to stop us repeating for ever in extreme situations
	while (runPhysics && maxIterations>0)
	{
		PhysObj * collidingObject = collisionCheck();
		if (collidingObject != nullptr)
		{
			//	cout<<"collision found"<<endl;
			//if a collision has a occured we need to fix our simulation
			solveIntersections(collidingObject);
		}
		else
		{
			runPhysics = false;
		}
		maxIterations--;
		if (maxIterations == 0)
		{
			std::cout << "solver maxed out";
		}
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
<<<<<<< HEAD
}

typedef bool(*fn)(PhysObj*, PhysObj*);
//function pointer array for doing our collisions

static fn collisionFunctionArray[] =
{
	Scene::plane2plane, Scene::plane2sphere, Scene::plane2Box,
	Scene::sphere2plane, Scene::sphere2sphere, Scene::sphere2Box,
	Scene::box2Plane, Scene::box2Sphere, Scene::box2Box
};

PhysObj* Scene::collisionCheck()
{
	int actorCount = actors.size();
	//need to check for collisions against all objects except this one.
	for (int outer = 0; outer < actorCount - 1; outer++)
	{
		for (int inner = outer + 1; inner < actorCount; inner++)
		{
			PhysObj* object1 = actors[outer];
			PhysObj* object2 = actors[inner];
			int shapeID1 = object1->shapeID;
			int shapeID2 = actors[inner]->shapeID;
			//using function pointers
			int functionIndex = (shapeID1 * NUMBERSHAPE) + shapeID2;
			fn collisionFunctionPtr = collisionFunctionArray[functionIndex];
			if (collisionFunctionPtr != NULL)
			{
				if (collisionFunctionPtr(object1, object2))
				{
					return object1;
				}
			}
		}
	}
	return false;
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

void getAxis(glm::vec3 *verts1, glm::vec3 *axis, int numberVerts)
{
	for (int vertIndex = 0; vertIndex<numberVerts; vertIndex++)
	{
		//find axis from vertices
		//there are quicker ways of doing this for boxes but our code works for other 2D shapes
		int nextIndex = (vertIndex + 1) % numberVerts;
		glm::vec3 perpendicular = glm::normalize(verts1[vertIndex] - verts1[nextIndex]);
		//for 3D we do a cross product between two edges here instead...
		axis[vertIndex] = glm::vec3(perpendicular.y, -perpendicular.x, 0);
	}
}

Projection project(glm::vec3 *verts, glm::vec3 axis, int numberVerts)
{
	float min = glm::dot(axis, verts[0]);
	float max = min;
	for (int vertIndex = 0; vertIndex<numberVerts; vertIndex++)
	{
		float p = glm::dot(axis, verts[vertIndex]);
		if (p< min)
		{
			min = p;
		}
		if (p> max)
		{
			max = p;
		}
	}
	return Projection(min, max);
}

bool overlap(Projection p1, Projection p2)
{
	if (p1.max < p2.min)
	{
		return false;
	}
	if (p1.min > p2.max)
	{
		return false;
	}
	return true;
}


//box collision routines
bool Scene::box2Box(PhysObj* obj1, PhysObj* obj2)
{
	//try to cast objects to ball and plane
	Box *box1 = dynamic_cast<Box*>(obj1);
	Box *box2 = dynamic_cast<Box*>(obj2);
	const int numberAxis = 4;
	if (box1 != NULL && box2 != NULL)
	{
		glm::vec3 axis1[numberAxis];
		glm::vec3 axis2[numberAxis];
		float width1 = box1->width;
		float height1 = box1->height;
		float width2 = box2->width;
		float height2 = box2->height;
		//create our vertices, they must be in correct order
		glm::vec3 verts1[4] = { glm::vec3(-width1, height1, 0), glm::vec3(width1, height1, 0), glm::vec3(width1, -height1, 0), glm::vec3(-width1, -height1, 0) };
		glm::vec3 verts2[4] = { glm::vec3(-width2, height2, 0), glm::vec3(width2, height2, 0), glm::vec3(width2, -height2, 0), glm::vec3(-width2, -height2, 0) };

		//transform to workd space
		for (int count = 0; count < numberAxis; count++)
		{
			verts1[count] = (box1->rotationMatrix*glm::vec4(verts1[count], 1)).xyz + box1->position;
			verts2[count] = (box2->rotationMatrix*glm::vec4(verts2[count], 1)).xyz + box2->position;
		}
		//get the axis
		getAxis(verts1, axis1, numberAxis);
		getAxis(verts2, axis2, numberAxis);
		for (int i = 0; i<numberAxis; i++)
		{
			Projection projection1 = project(verts1, axis1[i], numberAxis);
			Projection projection2 = project(verts2, axis1[i], numberAxis);
			if (!overlap(projection1, projection2))
			{
				return false;
			}
		}
		for (int i = 0; i<numberAxis; i++)
		{
			Projection projection1 = project(verts1, axis2[i], numberAxis);
			Projection projection2 = project(verts2, axis2[i], numberAxis);
			if (!overlap(projection1, projection2))
			{
				return false;
			}
		}
	}
	glm::vec3 closestPoint; //how to find this...
	box1->collisionResponse(closestPoint);
	box2->collisionResponse(closestPoint);
	return true;
}

int Scene::box2PlaneHelper(glm::vec3* verts, glm::vec3 normal, float distance)
{
	float min = FLT_MAX;
	float max = FLT_MIN;
	float minDistance = FLT_MAX;
	int closestPointIndex;
	//float projectedPoints[4];
	for (int count = 0; count < 4; count++)
	{
		//project the vertex onto the planes normal and work out the distance to the plane
		float projectedPoints = glm::dot(verts[count], normal) - distance;
		if (projectedPoints <min)
		{
			min = projectedPoints;
		}
		if (projectedPoints > max)
		{
			max = projectedPoints;
		}
		if (abs(projectedPoints)<minDistance)
		{
			minDistance = abs(projectedPoints);
			closestPointIndex = count;
		}
	}
	//if min and max have different signs then box is across plane
	if (min<0 && max > 0)
	{
		return closestPointIndex;
	}
	return -1;
}

bool Scene::box2Plane(PhysObj* obj1, PhysObj* obj2)
{
	//try to cast objects to ball and plane
	Box *box = dynamic_cast<Box*>(obj1);
	Plane *plane = dynamic_cast<Plane*>(obj2);
	if (box != NULL && plane != NULL)
	{
		float width = box->width;
		float height = box->height;
		glm::vec3 verts[4] = { glm::vec3(-width, height, 0), glm::vec3(width, height, 0), glm::vec3(width, -height, 0), glm::vec3(-width, -height, 0) };
		//find the coordinates for the four corners of our box
		for (int count = 0; count < 4; count++)
		{
			verts[count] = (box->rotationMatrix*glm::vec4(verts[count], 1)).xyz + box->position;
		}
		//do the collision check.
		int closetsPointIndex = Scene::box2PlaneHelper(verts, plane->normal, plane->distance);
		//if closetsPointIndex is not negative then a collision has occured 
		if (closetsPointIndex != -1)
		{
			//closetsPointIndex is the index of the closest point
			box->collisionResponse(verts[closetsPointIndex]);
			return true;
		}
	}
	return false;
}

bool Scene::box2Sphere(PhysObj* obj1, PhysObj* obj2)
{
	//try to cast objects to ball and plane
	Box *box = dynamic_cast<Box*>(obj1);
	Sphere *sphere = dynamic_cast<Sphere*>(obj2);
	if (box != NULL && sphere != NULL)
	{
		return false;
	}
	return false;
}

bool Scene::plane2Box(PhysObj* obj1, PhysObj* obj2)
{
	return box2Plane(obj2, obj1);
}
bool Scene::sphere2Box(PhysObj* obj1, PhysObj* obj2)
{
	return box2Sphere(obj2, obj1);
}



void Scene::solveIntersections(PhysObj* offender)
{
	offender->resetPosition();
=======
>>>>>>> parent of 04497f8... sphere collision/response
}