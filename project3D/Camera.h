#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <GLFW\glfw3.h>


using glm::vec3;
using glm::vec4;
using glm::mat4;

class Camera
{
public:
	Camera();

	~Camera();

	virtual void Update(GLFWwindow *window, float deltaTime) = 0;

	//set perspective matrix
	void SetPerspective(float fov, float aspectRatio, float near, float far);

	void SetLookAt(const vec3 &from, const vec3 &to, const vec3 &up);

	void SetPosition(const vec3 &pos);

	mat4 GetWorldTransform();

	mat4 GetView();

	mat4 GetProjection();

	mat4 GetProjectionView();

protected:

	void UpdateProjectionViewTransform();

	void UpdateWorldTransform();

	mat4 worldTransform;
	//inverse of world transform
	mat4 viewTransform;
	//used to turn world space into screen space
	mat4 projectionTransform;
	//viewTransform * projectionTransform;
	mat4 projectionViewTransform;
};

