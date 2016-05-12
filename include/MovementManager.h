#ifndef MovementManager_H
#define MovementManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Character.h"
#include "Enemy.h"
#include "Hero.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>

using namespace Ogre;

class MovementManager : public Ogre::Singleton<MovementManager>
{
 public:
  MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies);
  ~MovementManager();

  void moveHero(Ogre::Vector3* movement, Ogre::Real deltaT);
  void moveEnemies(Ogre::Real deltaT);

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<Enemy*>* getEnemies();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  
  // Heredados de Ogre::Singleton.
  static MovementManager& getSingleton ();
  static MovementManager* getSingletonPtr ();

  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
};

#endif
