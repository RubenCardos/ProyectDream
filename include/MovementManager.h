#ifndef MovementManager_H
#define MovementManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"
#include "Wall.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include "GameEntity.h"

using namespace Ogre;

class MovementManager : public Ogre::Singleton<MovementManager>
{
 public:
  MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies, std::vector<Wall*>* walls);
  ~MovementManager();

  void moveHero(Ogre::Vector3* movement, Ogre::Real deltaT);
  void jumpHero();
  void moveEnemies(Ogre::Real deltaT);
  void moveWalls(Ogre::Vector3* movement, Ogre::Real deltaT);

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<Enemy*>* getEnemies();
  std::vector<Wall*>* getWalls();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  void setWalls(std::vector<Wall*>* walls);
  
  // Heredados de Ogre::Singleton.
  static MovementManager& getSingleton ();
  static MovementManager* getSingletonPtr ();

  void repositionHero(btVector3 position,btQuaternion orientation);
  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
  std::vector<Wall*>* _walls;
};

#endif
