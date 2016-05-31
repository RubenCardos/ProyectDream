#ifndef Enemy_H
#define Enemy_H

#include "GameEntity.h"

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class Enemy : public GameEntity
{
 public:
  Enemy() {}
  Enemy(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, std::string type);
  ~Enemy();

  std::string getType();
  int getPoints();

  void setType(std::string type);
  void setPoints(int points);
  int getLives();
  double getMovementSpeed();
  double getJumpSpeed();
  Ogre::Vector3 getSpeed();

  void loseLife();
  void setMovementSpeed(double speed);
  void setJumpSpeed(double speed);
  void setSpeed(Ogre::Vector3 speed);

  //El comportamiento de el enemigo al spawnear y al morir se pondria en estas funciones
  void spawn();
  void die();

 protected:
  std::string _type;
  int _points;
  int _lives;
  double _movementspeed;
  double _jumpSpeed;
  bool _jump;
  Ogre::Vector3 _speed;
};

#endif
