#include "CelestialBody.h"



CelestialBody::CelestialBody(vec3 pos, vec4 colour, float radius, float orbitSpeed, CelestialBody* parent, float distanceFromParent)
{
	
	m_rad = 0;
	m_colour = colour;
	m_radius = radius;
	m_orbitSpeed = orbitSpeed;
	m_parent = parent;
	m_distanceFromParent = distanceFromParent;
	m_localTransform = mat4(1.0f); //identity
	m_globalTransform = mat4(1.0f); //identity
		
		
	if (parent == nullptr)
	{
		m_globalTransform[3][0] = pos.x;
		m_globalTransform[3][1] = pos.y;
		m_globalTransform[3][2] = pos.z;

		m_localTransform = m_globalTransform;
	}

	else
	{
		m_localTransform[3][0] = parent->m_globalTransform[3][0] + distanceFromParent * sin(m_rad);
		m_localTransform[3][2] = parent->m_globalTransform[3][2] + distanceFromParent * cos(m_rad);

		m_globalTransform = parent->m_globalTransform * m_localTransform;
	}
}


CelestialBody::~CelestialBody()
{
	if (m_parent != nullptr)
	{
		delete m_parent;
	}
}

void CelestialBody::Update(float deltaTime)
{
	if (m_parent != nullptr)
	{
		m_rad += m_orbitSpeed * deltaTime;
					
		m_localTransform[3][0] = m_parent->m_globalTransform[3][0] + m_distanceFromParent * sin(m_rad);
		m_localTransform[3][2] = m_parent->m_globalTransform[3][2] + m_distanceFromParent * cos(m_rad);
		m_globalTransform = m_parent->m_globalTransform * m_localTransform;
	}
}

float CelestialBody::GetRadius()
{
	return m_radius;
}

vec4 CelestialBody::GetColour()
{
	return m_colour;
}

vec3 CelestialBody::GetGlobalPosition()
{
	return vec3(m_globalTransform[3][0], m_globalTransform[3][1], m_globalTransform[3][2]);
}

mat4 CelestialBody::GetGlobalTransform()
{
	return m_globalTransform;
}
