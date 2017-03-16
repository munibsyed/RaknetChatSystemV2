#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Gizmos.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "Texture.h"
#include <vector>
#include "..\project3D\ObjLoader.h"
#include "Input.h"
#include "..\project3D\ParticleEmitter.h"
#include "..\project3D\GPUParticleEmitter.h"
#include "imgui_glfw3.h"
#include "Application.h"
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

	void GenerateFrameBufferObject();

	void CreatePlane();

	void GenerateGrid(int rows, int columns);

	void LoadPostShaders(const char * postShaderVs, const char * postShaderFs);

	void LoadMorphShaders(const char * morphShaderVs, const char* morphShaderFs);

	void LoadAndAttachShaders(const char* vsFilename, const char* fsFilename);

	void Draw(mat4 projectionView, float time);

	void DrawForwardRendering(float time, mat4 & projectionView, mat4 & cameraWorld);

	void DrawPost(float time, mat4 &projectionView);

	void Draw(float time, mat4 &projectionView, mat4 &cameraWorld, FlyCamera *cam);

	void Draw(mat4 projectionView, float t, float height);

	ParticleEmitter* GetEmitter();

	GPUParticleEmitter* GetGPUEmitter();

private:
	std::string LoadShader(const char* filename);

	int m_rows;
	int m_columns;

	vec3 lightPosition;

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

	//resulting ID of compiled shader
	unsigned int m_programID;
	unsigned int m_postProgramID;
	unsigned int m_morphShader;

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

