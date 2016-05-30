#ifndef AI_Manager_H
#define AI_Manager_H

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

class AI_Manager : public Ogre::Singleton<AI_Manager>
{
 public:
  AI_Manager(Hero* hero, std::vector<Enemy*>* enemies);
  ~AI_Manager();

  void updateEnemyMovement(Ogre::Real deltaT);

  Hero* getHero();
  std::vector<Enemy*>* getEnemies();

  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  
  // Heredados de Ogre::Singleton.
  static AI_Manager& getSingleton ();
  static AI_Manager* getSingletonPtr ();
  
 protected:
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
};

#endif
