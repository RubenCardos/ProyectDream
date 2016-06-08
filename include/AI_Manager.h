#ifndef AI_Manager_H
#define AI_Manager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"
#include "Wall.h"
#include "Boss.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include "GameEntity.h"

using namespace Ogre;

class AI_Manager : public Ogre::Singleton<AI_Manager>
{
 public:
  AI_Manager(Hero* hero, std::vector<Boss*>* bossPieces, std::vector<Enemy*>* enemies);
  ~AI_Manager();

  Hero* getHero();
  std::vector<Enemy*>* getEnemies();
  std::vector<Boss*>* getBossPieces();

  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  void setBossPieces(std::vector<Boss*>* bossPieces);
  
  void updateEnemyMovement();
  void loadBossRoute();
  void initializeBossMovement(Ogre::Real* deltaT);
  void updateBossMovement();

  // Heredados de Ogre::Singleton.
  static AI_Manager& getSingleton ();
  static AI_Manager* getSingletonPtr ();
  
 protected:
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
  std::vector<Boss*>* _bossPieces;
  std::vector<Ogre::Vector3> _bossRoute;
};

#endif
