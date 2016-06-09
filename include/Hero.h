#ifndef Hero_H
#define Hero_H

#include "GameEntity.h"

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class Hero : public GameEntity
{
 public:
  Hero();
  Hero(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody);
  ~Hero();

  int getScore();
  void increaseScore(int amount);
  int getNumJumps();
  int getLives();
  double getMovementSpeed();
  double getJumpSpeed();
  Ogre::Vector3 getSpeed();

  void setNumJumps(int nJumps);
  void loseLife();
  void setMovementSpeed(double speed);
  void setJumpSpeed(double speed);
  void setSpeed(Ogre::Vector3 speed);
  void resetScore();
  void resetLives();

  //El comportamiento de el heroe al spawnear y al morir se pondria en estas funciones
  void spawn();
  void die();

 protected:
  int _score;
  int _nJumps; //numero de saltos
  int _lives;
  double _movementspeed;
  double _jumpSpeed;
  Ogre::Vector3 _speed;
};

#endif
