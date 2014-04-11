/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
#include "Application.h"

//-------------------------------------------------------------------------------------
Application::Application(void)
{
}
//-------------------------------------------------------------------------------------
Application::~Application(void)
{
}

//-------------------------------------------------------------------------------------


void Application::createCamera()
{
	mCamera = mSceneMgr->createCamera("PlayerCam");
	mCamera->setPosition(Ogre::Vector3(0,15,300));
	mCamera->lookAt(Ogre::Vector3(0,0,0));
	mCamera->setNearClipDistance(5);

	//Create a camera controller (SdkCameraMan)
	mCameraMan = new OgreBites::SdkCameraMan(mCamera);
}

//-------------------------------------------------------------------------------------
/*
void Application::createViewports()
{

}
*/
//-------------------------------------------------------------------------------------
void Application::createScene()
{
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(16);
	
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));
	
	// Create an Ogre plane, "surface" will be used in the parameter for the createPlane
	Ogre::Plane surface(Ogre::Vector3::UNIT_Y,0);

	// Create the plane mesh and make it 100 by 1500
	Ogre::MeshManager::getSingleton().createPlane("icesheet",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		surface, 43, 445, 20, 20, true, 1, 1, 1,
		Ogre::Vector3::UNIT_Z);
	
	// Create an entity of the mesh so it can be placed with the scene
	Ogre::Entity* entGround = mSceneMgr->createEntity("GroundEntity", "icesheet");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
	entGround->setMaterialName("Textures/CurlingIce"/*<----Ice texture goes here*/);
	entGround->setCastShadows(false);

	// The below code will get the plane to interact with the physics engine
	
	btTransform iceTransform;
	iceTransform.setIdentity();
	iceTransform.setOrigin(btVector3(0,-50,0));

	// Set the mass of the ground. In this case it is 0 because the ground is static.
	btScalar iceMass(0);
	btVector3 localIceInertia(0,0,0);

	btCollisionShape *iceShape = new btBoxShape(btVector3(btScalar(50),btScalar(50),btScalar(50)));
	btDefaultMotionState *iceMotionState = new btDefaultMotionState(iceTransform);

	iceShape->calculateLocalInertia(iceMass, localIceInertia);

	btRigidBody::btRigidBodyConstructionInfo iceRBInfo(iceMass,iceMotionState,iceShape,localIceInertia);
	btRigidBody *iceBody = new btRigidBody(iceRBInfo);

	dynamicsWorld->addRigidBody(iceBody);
	
	//Ogre::Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");

	//Ogre::SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("HeadNode");
	//headNode->attachObject(ogreHead);

	//Ogre::Entity* surface = mSceneMgr->createEntity(

	//Ogre::Light* light = mSceneMgr->createLight("MainLight");
	//light->setPosition(20.0f, 80.0f, 50.0f);
}

bool Application::setup()
{
	mRoot = new Ogre::Root(mPluginsCfg);
 
	setupResources();
 
	bool carryOn = configure();
	if (!carryOn) return false;
	
	collisionConfiguraton = new btDefaultCollisionConfiguration();

	dispatcher = new btCollisionDispatcher(collisionConfiguraton);

	overLappingPairCache = new btDbvtBroadphase();

	solver = new btSequentialImpulseConstraintSolver();

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overLappingPairCache,solver,collisionConfiguraton);

	btCollisionShape* collisionShape;

	btRigidBody* physicsAccessors;

	chooseSceneManager();
	createCamera();
	createViewports();
 
	// Set default mipmap level (NB some APIs ignore this)
	Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
 
	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();
 
	// Create the scene
	createScene();
 
	createFrameListener();
 
	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        Application app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBoxA( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
