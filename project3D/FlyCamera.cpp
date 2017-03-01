#include "FlyCamera.h"




FlyCamera::FlyCamera(GLFWwindow *window, float speed, float rotateSpeed)
{
	this->speed = speed;
	this->rotateSpeed = rotateSpeed;
	glfwGetCursorPos(window, &oldCursorX, &oldCursorY);

}


FlyCamera::~FlyCamera()
{
}



void FlyCamera::Update(GLFWwindow *window, float deltaTime)
{
	mat4 currentTransform = GetWorldTransform();
	currentTransform[3] = vec4(0, 0, 0, 1);

	vec3 currentPos(worldTransform[3][0], worldTransform[3][1], worldTransform[3][2]);
	vec3 forward(worldTransform[2][0], worldTransform[2][1], worldTransform[2][2]);
	vec3 right = glm::cross(vec3(0, 1, 0), forward);
	//vec3 right(worldTransform[0][0], worldTransform[0][1], worldTransform[0][2]);
		//get global right of camera 
	
	vec3 up(worldTransform[1][0], worldTransform[1][1], worldTransform[1][2]);
	aie::Input *input = aie::Input::getInstance();

	//movement with arrow keys
	if (input->isKeyDown(aie::INPUT_KEY_W))
	{
		currentPos -= forward * speed * deltaTime;
	}

	if (input->isKeyDown(aie::INPUT_KEY_S))
	{
		currentPos += forward * speed * deltaTime;
	}

	if (input->isKeyDown(aie::INPUT_KEY_Q))
	{
		currentPos += up * speed * deltaTime;
	}

	if (input->isKeyDown(aie::INPUT_KEY_E))
	{
		currentPos -= up * speed * deltaTime;
	}

	if (input->isKeyDown(aie::INPUT_KEY_A))
	{
		currentPos -= right * speed * deltaTime;
	}

	if (input->isKeyDown(aie::INPUT_KEY_D))
	{
		currentPos += right * speed * deltaTime;
	}

	double cursorX;
	double cursorY;

	glfwGetCursorPos(window, &cursorX, &cursorY);
	double deltaX = oldCursorX - cursorX;
	double deltaY = oldCursorY - cursorY;

	deltaX = glm::clamp((float)deltaX, -1.0f, 1.0f);
	deltaY = glm::clamp((float)deltaY, -1.0f, 1.0f);
	oldCursorX = cursorX;
	oldCursorY = cursorY;

	float angleX = (float)deltaX * rotateSpeed * deltaTime;
	float angleY = (float)deltaY * rotateSpeed * deltaTime;


	mat4 rotation = glm::rotate(angleX, vec3(0, 1, 0)); //rotate on y-axis
	rotation *= glm::rotate(angleY, vec3(worldTransform[0])); //rotate on x-axis

	worldTransform = rotation * currentTransform;

	SetPosition(currentPos);
		
}



void FlyCamera::SetSpeed(float speed)
{
	this->speed = speed;
}
