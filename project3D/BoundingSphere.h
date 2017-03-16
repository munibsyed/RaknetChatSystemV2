#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>

class BoundingSphere
{
public:
	BoundingSphere();
	~BoundingSphere();

	void Fit(std::vector<glm::vec3> & points);
	
	bool IsColliding(const BoundingSphere &other);

	bool IsCollidingWithPlane(const glm::vec4 &plane);


	glm::vec3 centre;
	float radius;
};

