#ifndef MovementManager_H
#define MovementManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>
#include "GameEntity.h"

using namespace Ogre;

class MovementManager : public Ogre::Singleton<MovementManager>
{
 public:
  //MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies);
  MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<GameEntity*>* gameEntities);
  ~MovementManager();

  void moveHero(Ogre::Vector3* movement, Ogre::Real deltaT);
  void jumpHero();
  void moveEnemies(Ogre::Real deltaT);
  bool heroHasLanded();

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  //std::vector<Enemy*>* getEnemies();
  std::vector<GameEntity*>* getGameEntities();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  //void setEnemies(std::vector<Enemy*>* enemies);
  void setGameEntities(std::vector<GameEntity*>* gameEntities);
  
  // Heredados de Ogre::Singleton.
  static MovementManager& getSingleton ();
  static MovementManager* getSingletonPtr ();

  void repositionHero(btVector3 position,btQuaternion orientation);
  
 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  //std::vector<Enemy*>* _enemies;
  std::vector<GameEntity*>* _gameEntities;
  int _jumps;
  Ogre::Real _heroCoMPositionY; //Posicion del centro de masa del heroe (En reposo, con respecto del suelo)
};

#endif
