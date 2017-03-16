#include "AABB.h"



AABB::AABB()
{
}


AABB::~AABB()
{
}

void AABB::Reset()
{
	min.x = min.y = min.z = 1e37f;
	max.x = max.y = max.z = -1e37f;
}

void AABB::Fit(const std::vector<glm::vec3>& points)
{
	for (auto& p : points)
	{
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}
}

void AABB::Fit(const std::vector<float>& points)
{
	for (int i = 0; i < points.size(); i += 3)
	{
		glm::vec3 p(points[i], points[i + 1], points[i+2]);
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}
}

bool AABB::IsColliding(const AABB & other)
{
	if (max.x > other.min.x && max.y > other.min.y && max.z > other.min.z)
		return true;
	if (min.x < other.max.x && min.y < other.max.y && min.z < other.max.z)
		return true;

	return false;
}

bool AABB::IsColliding(const glm::vec4 plane)
{
	//convert AABB to center-extents representation
	glm::vec3 center = (max + min) * 0.5f; //get center of AABB
	glm::vec3 positiveExtent = max - center; //get positive extent

	return false;



	//Compute the projection interval radius of AABB onto L(t) = AABB.center + t * positiveExtent.n

	
}
