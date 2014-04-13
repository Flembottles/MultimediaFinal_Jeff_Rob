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
void Application::createRock(const btVector3 &Position, btScalar Mass)
{
	
	for (int i=0;i<12;i++)
	{
		Ogre::Vector3 size = Ogre::Vector3::ZERO;
		Ogre::Vector3 pos = Ogre::Vector3::ZERO;

		pos.x = Position.getX();
		pos.y = Position.getY();
		pos.z = Position.getZ();

		//Ogre::String number = Ogre::StringConverter::toString(i+1);
		Ogre::Entity *rockEntity = mSceneMgr->createEntity("Rock "+Ogre::StringConverter::toString(m_pNumRocks), "rock.mesh");
		rockEntity->setCastShadows(true);
		//rockNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("RockNode "+ number,Ogre::Vector3(0,1,170));
		Ogre::AxisAlignedBox boundingBox = rockEntity->getBoundingBox();
		size = boundingBox.getSize()*0.95f;
		
		//rockNode[i]->attachObject(rockEntity[i]);
		rockEntity->setMaterialName("RedRockMaterial");
		Ogre::SceneNode *rockNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		rockNode->scale(1.5,1.5,1.5);
		rockNode->pitch(Ogre::Degree(90));
		rockNode->attachObject(rockEntity);
		rockNode->setPosition(pos);

		btTransform Transform;
		Transform.setIdentity();
		Transform.setOrigin(Position);

		btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

		btVector3 HalfExtents(size.x*0.5f,size.y*0.5f,size.z*0.5f);
		btCollisionShape *Shape = new btBoxShape(HalfExtents);

		btVector3 localInertia;
		Shape->calculateLocalInertia(Mass, localInertia);

		btRigidBody *RigidBody = new btRigidBody(Mass, MotionState, Shape, localInertia);
		
		RigidBody->setUserPointer((void*)(rockNode));

		dynamicsWorld->addRigidBody(RigidBody);

		Rocks.push_back(RigidBody);

		m_pNumRocks++;
	}
	
}

void Application::createScene()
{
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
	Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(16);
	
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f,0.5f,0.5f));
	Ogre::Light* pointLight1 = mSceneMgr->createLight("light1");
	pointLight1->setType(Ogre::Light::LT_POINT);
	pointLight1->setPosition(Ogre::Vector3(0,30,0));

	pointLight1->setDiffuseColour(0.5,0.5,0.5);
	pointLight1->setSpecularColour(0.5,0.5,0.5);

	Ogre::Light* pointLight2 = mSceneMgr->createLight("light2");
	pointLight2->setType(Ogre::Light::LT_POINT);
	pointLight2->setPosition(Ogre::Vector3(0,30,-180));

	pointLight2->setDiffuseColour(0.5,0.5,0.5);
	pointLight2->setSpecularColour(0.5,0.5,0.5);

	Ogre::Light* pointLight3 = mSceneMgr->createLight("light3");
	pointLight3->setType(Ogre::Light::LT_POINT);
	pointLight3->setPosition(Ogre::Vector3(0,30,180));

	pointLight3->setDiffuseColour(0.5,0.5,0.5);
	pointLight3->setSpecularColour(0.5,0.5,0.5);

	//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE); <--- Really expensive, try it your GPU
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	// Create an Ogre plane, "surface" will be used in the parameter for the createPlane
	Ogre::Entity* entGround;
	Ogre::Plane surface(Ogre::Vector3::UNIT_Y,0);

	// Create the plane mesh and make it 100 by 1500
	Ogre::MeshManager::getSingleton().createPlane("icesheet",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		surface, 43, 445, 20, 20, true, 1, 1, 1,
		Ogre::Vector3::UNIT_Z);
	
	// Create an entity of the mesh so it can be placed with the scene
	entGround = mSceneMgr->createEntity("GroundEntity", "icesheet");
	mSceneMgr->getRootSceneNode()->createChildSceneNode();
	entGround->setMaterialName("Textures/CurlingIce"/*<----Ice texture goes here*/);
	entGround->setCastShadows(true);
	Ogre::SceneNode *groundNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	groundNode->attachObject(entGround);

	// The below code will get the plane to interact with the physics engine
	
	btTransform groundTransform;
	groundTransform.setIdentity();
	groundTransform.setOrigin(btVector3(0,0,0));

	btDefaultMotionState *motionState = new btDefaultMotionState(groundTransform);
	btCollisionShape *groundShape = new btStaticPlaneShape(btVector3(0,1,0),0);

	btVector3 localInertia;
	groundShape->calculateLocalInertia(0, localInertia);
	
	btRigidBody *groundRigidBody = new btRigidBody(0, motionState, groundShape, localInertia);
	groundRigidBody->setUserPointer((void*)(groundNode));

	dynamicsWorld->addRigidBody(groundRigidBody);

	//Ogre::Entity *rockEntity[24];//= mSceneMgr->createEntity("Rock", "rock.mesh");
	//Ogre::SceneNode *rockNode[24];//= mSceneMgr->getRootSceneNode()->createChildSceneNode("RockNode",Ogre::Vector3(0,1,170));
	
	createRock(btVector3(0,2,180),1);

	for (int i=12;i<24;i++)
	{
		//rockEntity[i] = mSceneMgr->createEntity("Rock"+i, "rock.mesh");
		//rockNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("RockNode"+i,Ogre::Vector3(0,1,170));
		//rockNode[i]->scale(1.5,1.5,1.5);
		//rockNode[i]->pitch(Ogre::Degree(90));
		//rockNode[i]->attachObject(rockEntity[i]);
		//rockEntity[i]->setMaterialName("YellowRockMaterial");
	}
}
void Application::updatePhysics(unsigned int deltaTime)
{
	dynamicsWorld->stepSimulation(deltaTime * 0.001f, 60);
	btRigidBody *tObject;

	for (std::vector<btRigidBody*>::iterator it = Rocks.begin(); it != Rocks.end(); ++it)
	{
		Ogre::SceneNode *node = static_cast<Ogre::SceneNode*>((*it)->getUserPointer());
		tObject = *it;

		btVector3 point = tObject->getCenterOfMassPosition();
	}
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
