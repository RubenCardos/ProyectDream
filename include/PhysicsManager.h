#ifndef PhysicsManager_H
#define PhysicsManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Character.h"
#include "Enemy.h"
#include "Hero.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>

using namespace Ogre;

class PhysicsManager : public Ogre::Singleton<PhysicsManager>
{
 public:
  PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<Enemy*>* enemies);
  ~PhysicsManager();

  void detectHeroCollision();

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<Enemy*>* getEnemies();
  OgreBulletDynamics::DynamicsWorld * getWorld();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  void setWorld(OgreBulletDynamics::DynamicsWorld * world);
  
  // Heredados de Ogre::Singleton.
  static PhysicsManager& getSingleton ();
  static PhysicsManager* getSingletonPtr ();
  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
  OgreBulletDynamics::DynamicsWorld * _world;
};

#endif
