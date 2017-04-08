#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "Application.h"
#include "Gizmos.h"
#include "Input.h"
#include "imgui_glfw3.h"
#include "Texture.h"
#include "..\project3D\ObjLoader.h"
#include "..\project3D\ParticleEmitter.h"
#include "..\project3D\GPUParticleEmitter.h"
#include "..\project3D\AABB.h"
#include "..\project3D\BoundingSphere.h"
#include "..\project3D\\FlyCamera.h"

using glm::mat4;
using glm::vec4;
using glm::vec3;
using glm::vec2;
using aie::Gizmos;
using aie::MyTexture;

struct Vertex
{
	vec4 position;
	vec2 texCoord;
};

class Mesh
{
public:
	Mesh();
	Mesh(ParticleEmitter* emitter);
	Mesh(GPUParticleEmitter* gpuEmitter);
	Mesh(ParticleEmitter * emitter, const char * filename);
	Mesh(const char* filename, const char* normalMapFilename);
	~Mesh();

	void GeneratePerlinValues();

	void GenerateFrameBufferObject();

	void GenerateShadowFrameBufferObject();

	void CreatePlane();

	void GenerateGrid(int rows, int columns);
	
	void LoadShadowShaders(const char* shadowShaderVs, const char* shadowShaderFs, const char * shadowMapShaderVs, const char * shadowMapShaderFs);

	void LoadAndAttachShaders(const char* vsFilename, const char* fsFilename, const char * shaderKey);

	void Draw(mat4 projectionView, float time);

	void DrawForwardRendering(float time, mat4 & projectionView, mat4 & cameraWorld, const char * shaderKey, const char * morphShader);

	void DrawPost(float time, mat4 &projectionView, const char* postShaderKey);

	void Draw(float time, mat4 &projectionView, mat4 &cameraWorld, const char* forwardShaderKey, const char* postShaderKey, const char* morphShaderKey);

	void Draw(mat4 projectionView, float t, float height);
	
	void DrawPlane(mat4 projectionView, const char *shaderKey);

	ParticleEmitter* GetEmitter();

	GPUParticleEmitter* GetGPUEmitter();

private:
	std::string LoadShader(const char* filename);

	int m_rows;
	int m_columns;

	vec3 lightPosition;
	vec3 m_lightDirection;
	
	mat4 m_lightMatrix;
	
	mat4 yRotate;
	float angleRotate;

	MyTexture*	m_texture;
	aie::Texture* m_tex;
	ParticleEmitter *m_emitter;
	GPUParticleEmitter *m_gpuEmitter;

	unsigned int	m_VAO;
	unsigned int	m_VBO;
	unsigned int	m_IBO;

	//frame buffer object
	unsigned int	m_FBO;
	unsigned int	m_FBOTexture;
	unsigned int	m_FBODepth; //render buffer

	unsigned int	m_FBOShadow;
	unsigned int	m_FBOTextureShadow;
	unsigned int	m_FBODepthShadow;

	//resulting ID of compiled shader
	unsigned int m_programID;
	unsigned int m_postProgramID;
	unsigned int m_morphShader;
	unsigned int m_useShadowProgram;
	unsigned int m_shadowMapShaderID;
	unsigned int m_shadowGenProgram;

	unsigned int m_perlinTexture;

	std::map<std::string, unsigned int> m_shaderMap;

	std::vector<OpenGLInfo> m_glInfo;

	std::vector<std::string> m_errorMessages;
	std::vector<std::string> m_warningMessages;

	AABB* m_soulSpearAABB;
	vec4 planes[6];


	bool specLightOn;
	bool spotLightOn;
	bool ambientLightOn;
	bool normalsOn;

	bool noPostProcess;
	bool scanLines;
	bool grayScale;
	bool distort;
};

