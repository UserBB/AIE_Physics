#include "Actor.h"

class Sphere : public Actor
{
public:

	Sphere();
	Sphere(glm::vec3 a_position, glm::vec3 a_velocity,
		float a_rotation,
		float a_radius,
		int a_mass,
		glm::vec4 a_color);
	~Sphere();
};