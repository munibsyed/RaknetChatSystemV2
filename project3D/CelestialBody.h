#pragma once
#include <Gizmos.h>
#include <glm\glm.hpp>
#include <glm/ext.hpp>

using glm::mat4;
using glm::vec3;
using glm::vec4;

class CelestialBody
{
public:
	CelestialBody(vec3 pos, vec4 colour, float radius, float orbitSpeed, CelestialBody* parent, float distanceFromParent);
	~CelestialBody();

	void Update(float deltaTime);

	float GetRadius();

	vec4 GetColour();

	vec3 GetGlobalPosition();

	mat4 GetGlobalTransform();

private:
	CelestialBody* m_parent;
	mat4 m_localTransform;
	mat4 m_globalTransform;
	vec4 m_colour;
	float m_rad;
	float m_distanceFromParent;
	float m_orbitSpeed;
	float m_radius;
};

