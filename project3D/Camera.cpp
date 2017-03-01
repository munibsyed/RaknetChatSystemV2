#include "Camera.h"



Camera::Camera()
{
}

//forward - third row of world position

Camera::~Camera()
{
}

void Camera::SetPerspective(float fov, float aspectRatio, float near, float far)
{
	projectionTransform = glm::perspective(fov, aspectRatio, near, far);
	UpdateProjectionViewTransform();
}

void Camera::SetLookAt(const vec3 &from, const vec3 &to, const vec3 &up)
{
	viewTransform = glm::lookAt(from, to, up);
	//worldTransform[0][3] = from.x;
	//worldTransform[1][3] = from.y;
	//worldTransform[2][3] = from.z;
	UpdateProjectionViewTransform();
	UpdateWorldTransform();
}

void Camera::SetPosition(const vec3 &pos)
{
	worldTransform[3][0] = pos.x;
	worldTransform[3][1] = pos.y;
	worldTransform[3][2] = pos.z;
	viewTransform = glm::inverse(worldTransform);
	UpdateProjectionViewTransform();
	//UpdateWorldTransform();

}

mat4 Camera::GetWorldTransform()
{
	return worldTransform;
}

mat4 Camera::GetView()
{
	return viewTransform;
}

mat4 Camera::GetProjection()
{
	return projectionTransform;
}

mat4 Camera::GetProjectionView()
{
	return projectionViewTransform;
}

void Camera::UpdateProjectionViewTransform()
{
	projectionViewTransform = projectionTransform * viewTransform;
}

void Camera::UpdateWorldTransform()
{
	worldTransform = glm::inverse(viewTransform);
}
