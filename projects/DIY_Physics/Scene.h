#pragma once

#include <glm/glm.hpp>
#include <vector>

class Actor;

class Scene
{
public:
	
	Scene();
	~Scene();

	void AddActor(Actor* a_actor);

	void Update();

	bool Sphere2Sphere(Actor* A, Actor* B);

	// global forces
	glm::vec3 gravity;

	std::vector<Actor*> ActorList;

	float timeStep;
	
};