#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "..\bootstrap\gl_core_4_4.h"
#include <iostream>

using glm::vec3;
using glm::vec4;
using glm::mat4;

struct Particle
{
	vec3 m_position;
	vec3 m_velocity;
	vec4 m_colour;

	float m_size;
	float m_lifetime;
	float m_lifespan;
};

struct ParticleVertex
{
	vec4 position;
	vec4 colour;
};

//particle emitter needs to emit particles, update particles and draw particles


class ParticleEmitter
{
public:
	ParticleEmitter();
	~ParticleEmitter();

	void Initialize(unsigned int maxParticles, float emitRate, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, const vec4 &startColour, const vec4 &endColour);

	void InitializeCone(unsigned int maxParticles, float emitRate, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, float coneAngle, const vec3 &coneDir, const vec4 &startColour, const vec4 &endColour);

	virtual void Emit();

    void Update(float deltaTime, mat4 cameraTransform);

	virtual void Test();

	void Draw();

protected:
	Particle* m_particles;
	unsigned int m_firstDeadIndex;
	unsigned int m_maxParticles;

	unsigned int m_VBO;
	unsigned int m_VAO;
	unsigned int m_IBO;

	ParticleVertex* m_vertexData;

	vec3 m_position;

	vec3 m_coneDir;
	float m_coneAngle;

	float m_gravity;

	float m_emitTimer;
	float m_emitRate;
	
	float m_lifespanMin;
	float m_lifespanMax;
	
	float m_velocityMin;
	float m_velocityMax;
	
	float m_startSize;
	float m_endSize;

	vec4 m_startColour;
	vec4 m_endColour;

};

class ParticleEmitterCone : public ParticleEmitter
{
public:

	void Test();

	void Emit();


private:
	vec3 m_coneDir;
};

