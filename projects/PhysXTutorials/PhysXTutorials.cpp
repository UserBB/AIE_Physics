#include "PhysXTutorials.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <vector>
#include <iostream>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

const int MAX_BULLET_LIFE = 100;
bool GLFWMouseButton1Down = false;

using namespace physx;

PxFoundation* g_PhysicsFoundation = nullptr;
PxPhysics* g_Physics = nullptr;
PxScene* g_PhysicsScene = nullptr;

PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
PxMaterial* g_PhysicsMaterial = nullptr;
PxCooking* g_PhysicsCooker = nullptr;
std::vector<PxRigidDynamic*> g_PhysXActors;

PhysXTutorials::PhysXTutorials()
{

}

PhysXTutorials::~PhysXTutorials()
{

}

bool PhysXTutorials::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	setUpPhysXTutorial();
	tutorial_1();

	return true;
}

void PhysXTutorials::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// add a plane
	//glm::mat4 planeRotateMatrix = glm::rotate(0.0f, 0.0f, 1.0f, 0.0f); // 9:37 in video

	static float count = 1;
	count += .01f;
	
	//ballgun();

	updatePhysx();
	
	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void PhysXTutorials::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(m_projectionMatrix, viewMatrix);
}

void PhysXTutorials::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();

	cleanUpPhysx();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new PhysXTutorials();
	
	if (app->create("AIE - PhysXTutorials",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void PhysXTutorials::setUpPhysXTutorial()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, *myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxCookingParams(PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	// create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f, 0.5f, 0.6f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -3.0f, 0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if (g_PhysicsScene)
	{ 
		std::cout << "start physx scene2";
	}

}

void PhysXTutorials::cleanUpPhysx()
{
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}

void PhysXTutorials::updatePhysx()
{
	g_PhysicsScene->simulate(1 / 60.f);
	while (g_PhysicsScene->fetchResults() == false)
	{
		// don't need anything here yet, but still need the fetch *?*
	}

	// Add widgets to represent all the physX actors which are in the scene
	for (auto actor : g_PhysXActors)
	{
		PxU32 nShapes = actor->getNbShapes();
		PxShape** shapes = new PxShape*[nShapes];
		actor->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--)
		{
			addWidget(shapes[nShapes], actor);
		}
		delete[] shapes;
	}

	// add widget for ground (static widget not in g_PhysXActors)
	//addPlane();

}

// ======================================================================================================

void PhysXTutorials::tutorial_1()
{
	// add a plane
	PxTransform pose = PxTransform(PxVec3(0.0f, 0, 0.0f), PxQuat(PxHalfPi*0.95f, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, PxPlaneGeometry(), *g_PhysicsMaterial);
	// add it to the physX scene
	g_PhysicsScene->addActor(*plane);
	// add it to our copy of the scene
	//g_PhysXActors.push_back(plane);
	{
		// add a sphere
		float density = 10;
		PxSphereGeometry sphere(5);
		PxTransform transform(PxVec3(0, 5, 0));
		PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
		// add it to the physX scene
		g_PhysicsScene->addActor(*dynamicActor);
		// add it to our copy of the scene
		g_PhysXActors.push_back(dynamicActor);
	}
	{
		// add a box
		float density = 10;
		PxBoxGeometry box(2, 2, 2);
		PxTransform transform(PxVec3(0, 20, 0));
		PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box, *g_PhysicsMaterial, density);
		// add it to the physX scene
		g_PhysicsScene->addActor(*dynamicActor);
		// add it to our copy of the scene
		g_PhysXActors.push_back(dynamicActor);
	}
}

//void PhysXTutorials::ballgun()
//{
//	float muzzleSpeed = -100;
//	GLFWwindow* window = glfwGetCurrentContext();
//	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS && !GLFWMouseButton1Down)
//	{
//		GLFWMouseButton1Down = true;
//		PxSphereGeometry sphere(1);
//		// get the camera position from the camera matrix
//		glm::vec3 position(m_cameraMatrix[3]);
//		// get the camera rotation from the camera matrix
//		glm::vec3 direction(m_cameraMatrix[2]);
//		physx::PxVec3 velocity = physx::PxVec3(direction.x, direction.y, direction.z);
//		float density = 500;
//		PxTransform transform(PxVec3(position.x, position.y, position.z), PxQuat::createIdentity());
//		PxRigidDynamic *dynamicActor = PxCreateDynamic(*g_Physics, transform, sphere, *g_PhysicsMaterial, density);
//		dynamicActor->setLinearVelocity(velocity, true);
//		g_PhysicsScene->addActor(*dynamicActor);
//		g_PhysXActors.push_back(dynamicActor);
//		bullets.push_back(new BulletStruct(dynamicActor));
//	}
//}

// ======================================================================================================

void PhysXTutorials::addWidget(PxShape* shape, PxRigidDynamic* actor)
{
	PxGeometryType::Enum type = shape->getGeometryType();
	switch (type)
	{
		case PxGeometryType::eSPHERE:
		addSphere(shape, actor);
		break;

	case PxGeometryType::eBOX:
		addBox(shape, actor);
		break;
	}
}

// not working quite right at the moment
void PhysXTutorials::addPlane()
{
	// get the geometry for this PhysX collision volume
	PxPlaneGeometry geometry;
	float size = 1;
	//bool status = shape->getPlaneGeometry(geometry);
	/*if (status)
	{
		size = geometry
	}*/
	// get the transform for this PhysX collision volume
	//PxMat44 m(PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(1);
	glm::vec3 position(0);
	int increments = 100;
	// create our box gizmo
	//Gizmos::addGrid(position, &M, increments, size);
} 

void PhysXTutorials::addSphere(PxShape* shape, PxRigidDynamic* actor)
{
	// get the geometry for this PhysX collision volume
	PxSphereGeometry geometry;
	float radius = 1;
	bool status = shape->getSphereGeometry(geometry);
	if (status)
	{
		radius = geometry.radius;
	}
	// get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
		m.column1.x, m.column1.y, m.column1.z, m.column1.w,
		m.column2.x, m.column2.y, m.column2.z, m.column2.w,
		m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	// get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	int resolution = 10;
	glm::vec4 color = glm::vec4(1, 0.5, 0, 1);
	// create our box gizmo
	Gizmos::addSphere(position, radius, resolution, resolution, color, &M);
}

void PhysXTutorials::addBox(PxShape* shape, PxRigidDynamic* actor)
{
	// get the geometry for this PhysX collision volume
	PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = shape->getBoxGeometry(geometry);
	if (status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	// get the transform for this PhysX collision volume
	PxMat44 m(PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
				m.column1.x, m.column1.y, m.column1.z, m.column1.w,
				m.column2.x, m.column2.y, m.column2.z, m.column2.w,
				m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position;
	// get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width, height, length);
	glm::vec4 color = glm::vec4(1, 0, 0, 1);
	// create our box gizmo
	Gizmos::addAABBFilled(position, extents, color, &M);
}