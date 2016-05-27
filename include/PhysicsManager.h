#ifndef PhysicsManager_H
#define PhysicsManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"


#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include "GameEntity.h"

using namespace Ogre;

class PhysicsManager : public Ogre::Singleton<PhysicsManager>
{
 public:
  PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<GameEntity*>* gameEntities);
  ~PhysicsManager();

  void detectHeroCollision();

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<GameEntity*>* getGameEntities();
  OgreBulletDynamics::DynamicsWorld * getWorld();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setGameEntities(std::vector<GameEntity*>* gameEntities);
  void setWorld(OgreBulletDynamics::DynamicsWorld * world);

  void removeGameEntity(unsigned int index);

  // Heredados de Ogre::Singleton.
  static PhysicsManager& getSingleton ();
  static PhysicsManager* getSingletonPtr ();
  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<GameEntity*>* _gameEntities;
  OgreBulletDynamics::DynamicsWorld * _world;
};

#endif
