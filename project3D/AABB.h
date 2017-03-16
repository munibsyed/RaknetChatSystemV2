#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>

class AABB
{
public:
	AABB();
	~AABB();

	void Reset();

	void Fit(const std::vector<glm::vec3> & points);

	void Fit(const std::vector<float>& points);

	bool IsColliding(const AABB &other);

	bool IsColliding(const glm::vec4 plane);

private:
	glm::vec3 min;
	glm::vec3 max;
};

