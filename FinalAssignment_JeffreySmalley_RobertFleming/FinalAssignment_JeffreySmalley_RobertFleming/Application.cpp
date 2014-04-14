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

enum gameStateList{THROW,THROWN,HOUSE,REST};
gameStateList gameState;

Ogre::Vector3 throwCamera = Ogre::Vector3(0,8,213),
	followCamera,
	houseCamera;

Ogre::Vector3 throwCameraLook = Ogre::Vector3(0,8,213),
	followCameraLook,
	houseCameraLook;

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
	mCamera->setPosition(throwCamera);
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
void Application::createRock(const btVector3 &Position, btScalar Mass,Ogre::String material)
{
	Ogre::Vector3 size = Ogre::Vector3::ZERO;
	Ogre::Vector3 pos = Ogre::Vector3::ZERO;
	

	//Ogre::String number = Ogre::StringConverter::toString(i+1);
	Ogre::Entity *rockEntity = mSceneMgr->createEntity("Rock "+Ogre::StringConverter::toString(m_pNumRocks), "rock.mesh");
	rockEntity->setCastShadows(true);
	//rockNode[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("RockNode "+ number,Ogre::Vector3(0,1,170));
	Ogre::AxisAlignedBox boundingBox = rockEntity->getBoundingBox();
	size = boundingBox.getSize()*0.95f;
	btVector3 change;
	change = Position;
	change.setY(size.y*0.5f);
	pos.x = Position.getX();
	pos.y = Position.getY();
	pos.z = Position.getZ();
	//rockNode[i]->attachObject(rockEntity[i]);
	rockEntity->setMaterialName(material);
	Ogre::SceneNode *rockNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	rockNode->scale(1.5,1.5,1.5);
	//rockNode->pitch(Ogre::Degree(90));
	rockNode->attachObject(rockEntity);
	rockNode->setPosition(pos);

	btTransform Transform;
	Transform.setIdentity();
	Transform.setOrigin(change);

	btDefaultMotionState *MotionState = new btDefaultMotionState(Transform);

	btVector3 HalfExtents(size.x*0.5f,size.y*0.5f,size.z*0.5f);
	btCollisionShape *Shape = new btCylinderShape(HalfExtents);

	btVector3 localInertia;
	Shape->calculateLocalInertia(Mass, localInertia);

	btRigidBody *RigidBody = new btRigidBody(Mass, MotionState, Shape, localInertia);
		
	RigidBody->setUserPointer((void*)(rockNode));
	RigidBody->setRestitution(1);
	dynamicsWorld->addRigidBody(RigidBody);

	Rocks.push_back(RigidBody);

	m_pNumRocks++;
}

void Application::startRock(int startNum)
{
	currentRock = startNum;
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
	m_pNumRocks = 0;
	createRock(btVector3(-11,5,215),1,"RedRockMaterial");
	createRock(btVector3(-11,5,218),1,"RedRockMaterial");
	createRock(btVector3(-11,5,221),1,"RedRockMaterial");

	createRock(btVector3(-14,5,215),1,"RedRockMaterial");
	createRock(btVector3(-14,5,218),1,"RedRockMaterial");
	createRock(btVector3(-14,5,221),1,"RedRockMaterial");

	createRock(btVector3(-17,5,215),1,"RedRockMaterial");
	createRock(btVector3(-17,5,218),1,"RedRockMaterial");
	createRock(btVector3(-17,5,221),1,"RedRockMaterial");

	createRock(btVector3(-20,5,215),1,"RedRockMaterial");
	createRock(btVector3(-20,5,218),1,"RedRockMaterial");
	createRock(btVector3(-20,5,221),1,"RedRockMaterial");
	


	createRock(btVector3(11,5,221),1,"YellowRockMaterial");
	createRock(btVector3(11,5,215),1,"YellowRockMaterial");
	createRock(btVector3(11,5,218),1,"YellowRockMaterial");

	createRock(btVector3(14,5,215),1,"YellowRockMaterial");
	createRock(btVector3(14,5,218),1,"YellowRockMaterial");
	createRock(btVector3(14,5,221),1,"YellowRockMaterial");

	createRock(btVector3(17,5,215),1,"YellowRockMaterial");
	createRock(btVector3(17,5,218),1,"YellowRockMaterial");
	createRock(btVector3(17,5,221),1,"YellowRockMaterial");

	createRock(btVector3(20,5,215),1,"YellowRockMaterial");
	createRock(btVector3(20,5,218),1,"YellowRockMaterial");
	createRock(btVector3(20,5,221),1,"YellowRockMaterial");
	
	rockOp = 12;

	mDebugDrawer = new OgreDebugDrawer( mSceneMgr );
    mDebugDrawer->setDebugMode( btIDebugDraw::DBG_DrawWireframe );
    dynamicsWorld->setDebugDrawer( mDebugDrawer );

	// code to set rock position 0-11 red, 12-23 yellow
	btTransform transform = Rocks[1]-> getCenterOfMassTransform();
	transform.setOrigin(btVector3(0,1,195));
	Rocks[1] -> setCenterOfMassTransform(transform);
	// how to throw a rock, 10 with no spin is a button shot
	Rocks[1]->setLinearVelocity(btVector3(0,0,-10));
	//does not curve the rock, but does make it go futher
	//Rocks[1]->setAngularVelocity(btVector3(0,1,0));

	//activates the rock for motion
	Rocks[1]->activate(true);
	//perfect ice friction
	groundRigidBody->setFriction(0.02647);

	transform = Rocks[12]-> getCenterOfMassTransform();
	transform.setOrigin(btVector3(0,1,150));
	Rocks[12] -> setCenterOfMassTransform(transform);
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
		node->setPosition(Ogre::Vector3((float)point[0],(float)point[1], (float)point[2]));

		btQuaternion btq = tObject->getOrientation();
		Ogre::Quaternion quart = Ogre::Quaternion(btq.w(),btq.x(),btq.y(),btq.z());

		node->setOrientation(quart);
	}
}
bool Application::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;
 
	if(mShutDown)
		return false;
 
	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();
 
	mTrayMgr->frameRenderingQueued(evt);
 
	if (!mTrayMgr->isDialogVisible())
	{
		mCameraMan->frameRenderingQueued(evt);   // if dialog isn't up, then update the camera
		if (mDetailsPanel->isVisible())   // if details panel is visible, then update its contents
		{
			mDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(mCamera->getDerivedPosition().x));
			mDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(mCamera->getDerivedPosition().y));
			mDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(mCamera->getDerivedPosition().z));
			mDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().w));
			mDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().x));
			mDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().y));
			mDetailsPanel->setParamValue(7, Ogre::StringConverter::toString(mCamera->getDerivedOrientation().z));
		}
	}

	updatePhysics(16);

	return true;
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

bool Application::mouseMoved( const OIS::MouseEvent &arg )
{
	if (mTrayMgr->injectMouseMove(arg)) return true;
	mCameraMan->injectMouseMove(arg);
	return true;
}
bool Application::keyPressed( const OIS::KeyEvent &arg )
{
	if (mTrayMgr->isDialogVisible()) return true;   // don't process any more keys if dialog is up
 
	if (arg.key == OIS::KC_F)   // toggle visibility of advanced frame stats
	{
		mTrayMgr->toggleAdvancedFrameStats();
	}
	else if (arg.key == OIS::KC_G)   // toggle visibility of even rarer debugging details
	{
		if (mDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			mTrayMgr->moveWidgetToTray(mDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
			mDetailsPanel->show();
		}
		else
		{
			mTrayMgr->removeWidgetFromTray(mDetailsPanel);
			mDetailsPanel->hide();
		}
	}
	/*
	else if (arg.key == OIS::KC_T)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;
 
		switch (mDetailsPanel->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso = 1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
		}
 
		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mDetailsPanel->setParamValue(9, newVal);
	}
	*/
	else if (arg.key == OIS::KC_R)   // cycle polygon rendering mode
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;
 
		switch (mCamera->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal = "Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points";
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}
 
		mCamera->setPolygonMode(pm);
		mDetailsPanel->setParamValue(10, newVal);
	}
	else if(arg.key == OIS::KC_F5)   // refresh all textures
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if (arg.key == OIS::KC_SYSRQ)   // take a screenshot
	{
		mWindow->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	else if (arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
	}
	else if (arg.key == OIS::KC_1)
	{

	}
 
	mCameraMan->injectKeyDown(arg);
	return true;
}
 
bool Application::keyReleased( const OIS::KeyEvent &arg )
{
	mCameraMan->injectKeyUp(arg);
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
