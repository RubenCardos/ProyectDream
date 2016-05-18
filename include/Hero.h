#ifndef Hero_H
#define Hero_H

#include <Character.h>

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class Hero : public Character
{
 public:
  Hero() {}
  Hero(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape);
  ~Hero();

  int getScore();
  void increaseScore(int amount);
  int getNumJumps();

  void setNumJumps(int nJumps);

  //El comportamiento de el heroe al spawnear y al morir se pondria en estas funciones
  void spawn();
  void die();


 protected:
  int _score;
  int _nJumps; //numero de saltos
};

#endif
