#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "..\bootstrap\gl_core_4_4.h"
#include <iostream>

using glm::vec3;
using glm::vec4;
using glm::mat4;


struct GPUParticle
{
	GPUParticle() : lifetime(1), lifespan(0)
	{

	}

	vec3 position;
	vec3 velocity;

	//Start and end values for size and colour can be set within the shader using uniforms. This should be faster.
	//These attributes are being enabled in CreateBuffers()

	float lifetime;
	float lifespan;
};

class GPUParticleEmitter
{
public:
	GPUParticleEmitter();
	~GPUParticleEmitter();

	void Initialize(unsigned int maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, float gravity, const vec4 &startColour, const vec4 &endColour, vec3 position);

	void Draw(float time, const mat4& cameraTransform, const mat4& projectionView);

	void CreateBuffers();

	void CreateUpdateShader();

	void CreateDrawShader();

private:
	unsigned int m_updateShader; //update particle buffers
	unsigned int m_drawShader; //render particle points with a Geometry Shader

	GPUParticle* m_particles;

	unsigned int m_maxParticles;
	float m_gravity;
	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;
	vec4 m_startColour;
	vec4 m_endColour;
	vec3 m_position;

	unsigned int m_activeBuffer;
	unsigned int m_VBO[2];
	unsigned int m_VAO[2];

	float m_lastDrawTime;
};

