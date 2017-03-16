#include "BoundingSphere.h"



BoundingSphere::BoundingSphere()
{
	centre = glm::vec3(0);
	radius = 0;
}


BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::Fit(std::vector<glm::vec3>& points)
{
	glm::vec3 min(1e37f);
	glm::vec3 max(-1e37f);

	for (auto& p : points)
	{
		if (p.x < min.x) min.x = p.x;
		if (p.y < min.y) min.y = p.y;
		if (p.z < min.z) min.z = p.z;
		if (p.x > max.x) max.x = p.x;
		if (p.y > max.y) max.y = p.y;
		if (p.z > max.z) max.z = p.z;
	}

	centre = (min + max) * 0.5f;
	radius = glm::distance(min, centre);
}

bool BoundingSphere::IsColliding(const BoundingSphere & other)
{
	if (glm::distance(centre, other.centre) < (radius + other.radius))
	{
		return true;
	}

	return false;
}

bool BoundingSphere::IsCollidingWithPlane(const glm::vec4 & plane)
{
	float d = glm::dot(glm::vec3(plane), centre) + plane.w;
	//simply gets the distance of the centre along the normal of the plane

	if (d > radius)
	{
		//if radius is greater than radius, then sphere can't be intersecting the plane
		printf("Front!");
		return false;
	}

	else if (d < -radius)
	{
		printf("Back!");
		return false;
	}

	else
	{
		printf("Intersecting with plane");
		return true;
	}

	return false;
}
