#pragma once

#include "Application.h"
#include "CelestialBody.h"
#include "Mesh.h"
#include <glm/mat4x4.hpp>
#include "FlyCamera.h"
#include <vector>
#include "ObjLoader.h"
#include "MyTexture.h"


class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	FlyCamera	*flyCamera;

	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;	

	Mesh* mesh;
	ObjLoader* objLoader;

	int rows;
	int columns;

	unsigned int	m_VAO;
	unsigned int	m_VBO;
	unsigned int	m_IBO;
	//resulting ID of compiled shader
	unsigned int m_programID;



	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err;
};