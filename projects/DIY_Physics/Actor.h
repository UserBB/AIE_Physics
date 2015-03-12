#pragma once

#include <glm/glm.hpp>

class Actor
{
public:

	Actor();
	virtual ~Actor();

	void Update(glm::vec3 gravity, float timeStep);

	int mass;
	glm::vec3 position;
	glm::vec3 velocity;
	float rotation;
	float sizeX, sizeY, sizeZ;
	float radius;
	glm::vec4 color;

};