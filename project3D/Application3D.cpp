#include "Application3D.h"
#include "Input.h"
#include "MyTexture.h"
#include "ParticleEmitter.h"


#define PI 3.14159265359f

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;
using aie::Input;

Application3D::Application3D() {

	flyCamera = new FlyCamera(m_window, 10, 2.0f);
	
	//mesh = new Mesh("../Textures/Earth Textures/earth_cloud.jpg");
	rows = 100;
	columns = 100;
	
}

Application3D::~Application3D() {

}

bool Application3D::startup() {

	setBackgroundColour(0.25f, 0.25f, 0.25f);
	// initialise gizmo primitive counts
	Gizmos::create(10000,
		10000, 10000, 10000);
	
	float previousTime = 0;
	float currentTime = (float)glfwGetTime();
	float deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	
	objLoader = new ObjLoader();

	flyCamera->SetLookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	flyCamera->SetPerspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.f);
	
	/*mesh = new Mesh();
	mesh->GenerateFrameBufferObject();
	mesh->CreatePlane();*/
	

	//mesh->GetEmitter()->InitializeCone(1000, 5000, 0.1f, 5.0f, 1, 5, 0.1, 0.01f, 0.0f, PI/8.0f, vec3(0,1,0), vec4(1, 0, 0, 1), vec4(1, 1, 0, 1));
	

	//emitter->Initialize();	

	mesh = new Mesh("../Models/soulspear/soulspear/soulspear_diffuse.tga", "../Models/soulspear/soulspear/soulspear_normal.tga");
	mesh->GenerateFrameBufferObject();
	mesh->CreatePlane();

	mesh->LoadPostShaders("vsSourcePost.vs", "fsSourcePost.frag");
	mesh->LoadMorphShaders("vsMorph.vs", "fsMorph.frag");

	

	//mesh->GenerateGrid(rows, columns);

	return true;
}

void Application3D::shutdown() {

	delete flyCamera;
	delete mesh;
	delete objLoader;

	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	flyCamera->Update(m_window, deltaTime);
	//mesh->GetGPUEmitter()->Update(deltaTime, flyCamera->GetWorldTransform());
	//emitter->Update(deltaTime, flyCamera->GetWorldTransform());
	//emitter->Emit();
	
	// wipe the gizmos clean for this frame
	Gizmos::clear();
	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
						vec3(-10 + i, 0, -10),
						i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
						vec3(-10, 0, -10 + i),
						i == 10 ? white : black);

	}

	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));
	Gizmos::addSphere(vec3(0, 5, 0), 0.5f, 8, 8, vec4(1, 1, 0, 1));
	Gizmos::addSphere(vec3(10, 0, 0), 1.0f, 16, 16, vec4(1, 1, 0, 1));
	Gizmos::addCylinderFilled(vec3(-10, 0, 0), 1, 2, 8, vec4(0, 0, 1, 1));

	
	BoundingSphere sphere;
	sphere.centre = vec3(-10, cosf((float)glfwGetTime()) + 1, -10);
	sphere.radius = 0.5f;
	vec4 planes[6];
	flyCamera->GetFrustumPlanes(flyCamera->GetProjectionView(), planes);

	bool renderIt = true;
	for (int i = 0; i < 6; i++)
	{
		float d = glm::dot(glm::vec3(planes[i]), sphere.centre) + planes[i].w;

		if (d < -sphere.radius)
		{
			renderIt = false;
			//printf("Behind it, don't render it.\n");
			break;
		}

		else if (d < sphere.radius)
		{
			//printf("Touching, we still need to render it.\n");
		}

		else
		{
			//printf("Front, fully visible so render it.\n");
		}
	}

	if (renderIt == true)
	{
		std::cout << "Render it! " << std::endl;
		Gizmos::addSphere(sphere.centre, sphere.radius, 8, 8, vec4(1, 0, 1, 1));
	}

	else
	{
		std::cout << "Don't render it!" << std::endl;
	}




	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	float currentTime = getTime();
	//mesh->Draw(flyCamera->GetProjectionView(), currentTime);
	
	mesh->Draw((float)glfwGetTime(), flyCamera->GetProjectionView(), flyCamera->GetWorldTransform(), flyCamera);

	Gizmos::draw(flyCamera->GetProjectionView());

}