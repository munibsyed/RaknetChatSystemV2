#pragma once
#include  "..\bootstrap\gl_core_4_4.h"
#include "tiny_obj_loader.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>


struct OpenGLInfo { 
	unsigned int m_VAO; 
	unsigned int m_VBO;
	unsigned int m_VBO2;
	unsigned int m_faceCount; 
};

struct OBJVertex
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float tx, ty, tz, tw;
};

class ObjLoader
{
public:
	ObjLoader();
	~ObjLoader();

	std::vector<OpenGLInfo> CreateOpenGLBuffers(tinyobj::attrib_t &attribs, std::vector<tinyobj::shape_t>& shapes);

	std::vector<OpenGLInfo> CreateOpenGLBuffers(tinyobj::attrib_t & attribs, tinyobj::shape_t shape);

	void CalculateTangents(std::vector<OBJVertex>& vertices);

	void Render();

private:
	std::vector<OpenGLInfo> m_glInfo;
};

