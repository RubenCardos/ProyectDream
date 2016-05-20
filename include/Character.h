#ifndef Character_H
#define Character_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameState.h"

#include <CEGUI.h>
#include <RendererModules/Ogre/Renderer.h>

#include <OgreOverlaySystem.h>
#include <OgreOverlayElement.h>
#include <OgreOverlayManager.h>

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>

#include <string>

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class Character
{
 public:
  Character() {}
  Character(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape);
  ~Character();

  Ogre::SceneNode* getSceneNode();
  OgreBulletCollisions::CollisionShape* getCollisionShape();
  OgreBulletDynamics::RigidBody* getRigidBody();
  int getLives();
  double getMovementSpeed();
  double getJumpSpeed();
  Ogre::Vector3 getSpeed();

  void setSceneNode(Ogre::SceneNode* sceneNode);
  void setCollisionShape(OgreBulletCollisions::CollisionShape* collisionShape);
  void setRigidBody(OgreBulletDynamics::RigidBody* rigidBody);
  void loseLife();
  void setMovementSpeed(double speed);
  void setJumpSpeed(double speed);
  void setSpeed(Ogre::Vector3 speed);

  //El comportamiento de que hace cada personaje al spawnear y al morir, que se defina en las clases hijas
  virtual void spawn() = 0;
  virtual void die() = 0;

 protected:
  Ogre::SceneNode* _sNode;
  OgreBulletCollisions::CollisionShape* _collShape;
  OgreBulletDynamics::RigidBody* _rigBody;
  int _lives;
  double _movementspeed;
  double _jumpSpeed;
  bool _jump;
  Ogre::Vector3 _speed;
};

#endif
