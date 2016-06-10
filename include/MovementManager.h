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
#include "AI_Manager.h"
#include "Scenario.h"

using namespace Ogre;

class MovementManager : public Ogre::Singleton<MovementManager>
{
 public:
  MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies, std::vector<Boss*>* bossPieces, std::vector<Wall*>* walls);
  ~MovementManager();

  void moveHero(Ogre::Vector3* movement);
  void jumpHero();
  void moveEnemies();
  void moveWalls();
  void moveBoss();
  void initializeBossMovement(Ogre::Real* deltaT);
  void rotateHero();
  void rotateBoss();

  Ogre::SceneManager* getSceneManager();
  Hero* getHero();
  std::vector<Enemy*>* getEnemies();
  std::vector<Wall*>* getWalls();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setHero(Hero* hero);
  void setEnemies(std::vector<Enemy*>* enemies);
  void setWalls(std::vector<Wall*>* walls);
  void setBossPieces(std::vector<Boss*>* bossPieces);
  void setAI_Manager(AI_Manager* aiManager);
  
  // Heredados de Ogre::Singleton.
  static MovementManager& getSingleton ();
  static MovementManager* getSingletonPtr ();

  void repositionHero(btVector3 position,btQuaternion orientation);
  void repositionGameEntity(GameEntity* gameentity,btVector3 position,btQuaternion orientation);
  
  void inBossRoom();

 protected:
  Ogre::SceneManager* _sceneMgr;
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
  std::vector<Wall*>* _walls;
  std::vector<Boss*>* _bossPieces;
  AI_Manager* _aiManager;
  bool _inBossRoom;
};

#endif
