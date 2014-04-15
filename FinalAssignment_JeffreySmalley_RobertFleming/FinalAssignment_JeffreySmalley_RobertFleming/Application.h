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
	int m_pNumRocks;
	std::vector<btVector3> rockRackPos;
	int currentRock;
	int rockOp;
	int rocksThrown;
	bool redTeamStart;

	float angle;
	float power;

	float xPow;
	float zPow;
	bool hammer;

	int countToNextThrow;

	std::vector<Ogre::SceneNode*> rockNodes;

	std::vector<btRigidBody*> Rocks;
	
	btDefaultCollisionConfiguration* collisionConfiguraton;

	btCollisionDispatcher* dispatcher;

	btDbvtBroadphase* overLappingPairCache;

	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	btCollisionShape* collisionShape;

	btRigidBody* physicsAccessors;

	btIDebugDraw* mDebugDrawer;
	
	virtual bool setup();
	void createRock(const btVector3 &Position, btScalar Mass,Ogre::String material);
	void startRock(bool red);
	void updatePhysics(unsigned int deltaTime);
	virtual void createScene();
	virtual void createCamera();
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool keyPressed( const OIS::KeyEvent &arg );
	virtual bool keyReleased( const OIS::KeyEvent &arg );
	virtual bool mouseMoved( const OIS::MouseEvent &arg );
	void nextRock();
	void setRock();
	void rerack();
	void outOfBounds(int rockNumber);
	void boundsCheck();
	//virtual void setupResources();
	//virtual void createViewports();
};
