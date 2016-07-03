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
  bool AllReelsPicked();
  bool isAttacking();
  bool isInvulnerable();

  void setNumJumps(int nJumps);
  void loseLife();
  void setMovementSpeed(double speed);
  void setJumpSpeed(double speed);
  void setSpeed(Ogre::Vector3 speed);
  void resetScore();
  void resetLives();
  void picksReel(string reelName);
  void setAttacking(bool attacking);
  void resetPickedReels();
  void makeInvulnerable();
  void UpdateInvulnerability(Ogre::Real deltaT);
  void godMode();

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
  bool _pickedGardenReel;
  bool _pickedRoomReel;
  bool _attacking;
  Ogre::Real _invulnerabilityLeft;
  bool _godModeOn;
};

#endif
