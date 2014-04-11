/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.h
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

#include "BaseApplication.h"


class Application : public BaseApplication
{
public:
	Application();
	virtual ~Application();
protected:
	btDefaultCollisionConfiguration* collisionConfiguraton;

	btCollisionDispatcher* dispatcher;

	btDbvtBroadphase* overLappingPairCache;

	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	btCollisionShape* collisionShape;

	btRigidBody* physicsAccessors;
	
	virtual bool setup();
	virtual void createScene();
	virtual void createCamera();
	//virtual void setupResources();
	//virtual void createViewports();
};
