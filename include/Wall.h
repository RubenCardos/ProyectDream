#ifndef Wall_H
#define Wall_H

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

#include "GameEntity.h"

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

enum WallType {
  LeftWall,
  RightWall,
  Floor
};

class Wall : public GameEntity{
 public:
  Wall() {}
  Wall(WallType type);
  Wall(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape, WallType type);
  ~Wall();

  WallType getType();
  void setType(WallType type);

 protected:
  WallType _type;

};

#endif
