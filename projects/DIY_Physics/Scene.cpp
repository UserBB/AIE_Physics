#include "Scene.h"

class Actor;

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::AddActor(Actor* a)
{
	ActorList.push_back(a);
}

void Scene::Update()
{
	// iterate through all the actors on our scene
	for (auto actor:ActorList)
	{
		actor->Update(gravity, timeStep);
	}
}



























































/*
// from http://www.gamasutra.com/view/feature/131424/pool_hall_lessons_fast_accurate_.php?page=2
bool Sphere2Sphere(Actor* A, Actor* B)
{
	// Early Escape test: if the length of the movevec is less
	// than distance between the centers of these circles minus 
	// their radii, there's no way they can hit
	double dist = B.center.distance(A.center);
	double sumRadii = (B.radius + A.radius);
	dist -= sumRadii;
	if (movevec.Magnitude() < dist)
	{
		return false;
	}

	// Normalize the movevec
	Vector N = movevec.copy();
	N.normalize();

	// Find C, the vector from the center of the moving
	// circle A to the center of B
	Vector C = B.center.minus(A.center);

	// D = N . C = ||C|| * cos(angle between N and C)
	double D = N.dot(C);

	// Another early escape: Make sure that A is moving 
	// towards B! If the dot product between the movevec and
	// B.center - A.center is less than or equal to 0, 
	// A isn't moving towards B
	if (D <= 0)
	{
		return false;
	}
	// Find the length of vector C
	double lengthC = C.Magnitude();

	double F = (lengthC * lengthC) - (D * D);
}
*/