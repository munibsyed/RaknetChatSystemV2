#pragma once
#include "Camera.h"
#include <iostream>
#include "Input.h"


class FlyCamera : public Camera
{
public:
	FlyCamera(GLFWwindow *window, float speed, float rotateSpeed);
	~FlyCamera();

	void Update(GLFWwindow *window, float deltaTime);

	void SetSpeed(float speed);

private:
	float speed;
	float rotateSpeed;
	vec3 up;
	aie::Input *input;

	double oldCursorX;
	double oldCursorY;
	
};

