#ifndef PhysicsManager_H
#define PhysicsManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"
#include "MovementManager.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include "GameEntity.h"
#include "Scenario.h"

using namespace Ogre;

class PhysicsManager : public Ogre::Singleton<PhysicsManager>
{
 public:
  PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<GameEntity*>* gameEntities, std::vector<Enemy*>* enemies, std::vector<Wall*>* walls);
  ~PhysicsManager();

  void detectHeroCollision();
  void detectEnemiesCollision();

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<GameEntity*>* getGameEntities();
  OgreBulletDynamics::DynamicsWorld * getWorld();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  void setWalls(std::vector<Wall*>* walls);
  void setGameEntities(std::vector<GameEntity*>* gameEntities);
  void setWorld(OgreBulletDynamics::DynamicsWorld * world);

  void removeGameEntity(std::string name);
  Vector3 calculateSpawnPoint();

  // Heredados de Ogre::Singleton.
  static PhysicsManager& getSingleton ();
  static PhysicsManager* getSingletonPtr ();
  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<GameEntity*>* _gameEntities;
  std::vector<Enemy*>* _enemies;
  std::vector<Wall*>* _walls;
  OgreBulletDynamics::DynamicsWorld * _world;
  int _numPS;
};

#endif
