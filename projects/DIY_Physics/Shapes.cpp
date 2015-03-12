#include "Shapes.h"

Sphere::Sphere() {}

Sphere::Sphere(glm::vec3 a_position, glm::vec3 a_velocity,
	float a_rotation,
	float a_radius,
	int a_mass,
	glm::vec4 a_color)
{
	position = a_position;
	velocity = a_velocity;
	rotation = a_rotation;
	radius = a_radius;
	mass = a_mass;
	color = a_color;
}

Sphere::~Sphere() {}

void Sphere::Update(glm::vec3 gravity, float timeStep)
{

}