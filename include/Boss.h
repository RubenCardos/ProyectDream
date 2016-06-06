#ifndef Boss_H
#define Boss_H

#include "GameEntity.h"

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

enum BossType {
	Wagon,
	Locomotive
};

class Boss : public GameEntity
{
 public:
  Boss() {}
  Boss(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, BossType type);
  ~Boss();

  //El comportamiento de el enemigo al spawnear y al morir se pondria en estas funciones
  void spawn();
  void die();

  bool isEnraged();
  bool isVulnerable();

  BossType getType();
  int getPoints();
  int getHealth();
  double getMovementspeed();
  Ogre::Real getRageTimer();
  Ogre::Vector3* getVSpeed();
  int getTargetIndex();
  int getCurrentIndex();
  Ogre::Vector3* getTargetPosition();
  Ogre::Vector3* getCurrentPosition();

  void setType(BossType type);
  void loseHealth();
  void setMovementspeed(double movementspeed);
  void setRageTimer(Ogre::Real rageTimer);
  void setVulnerable(bool vulnerable);
  void setV_Speed(Ogre::Vector3* vSpeed);
  void setTargetIndex(int targetIndex);
  void setCurrentIndex(int currentIndex);
  void setTargetPosition(Ogre::Vector3* targetPosition);
  void setCurrentPosition(Ogre::Vector3* currentPosition);

 protected:
  BossType _type;
  int _points;
  int _health;
  double _movementSpeed;
  Ogre::Real _rageTimer;
  bool _vulnerable;
  Ogre::Vector3* _vSpeed;
  int _targetIndex;
  int _currentIndex;
  Ogre::Vector3* _targetPosition;
  Ogre::Vector3* _currentPosition;
};

#endif
