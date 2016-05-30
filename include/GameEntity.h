#ifndef GameEntity_H
#define GameEntity_H

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

class GameEntity
{
 public:
  GameEntity() {}
  GameEntity(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody);
  ~GameEntity();

  Ogre::SceneNode* getSceneNode();
  OgreBulletDynamics::RigidBody* getRigidBody();

  void setSceneNode(Ogre::SceneNode* sceneNode);
  void setRigidBody(OgreBulletDynamics::RigidBody* rigidBody);

 protected:
  Ogre::SceneNode* _sNode;
  OgreBulletDynamics::RigidBody* _rigBody;
};

#endif
