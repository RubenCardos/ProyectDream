#ifndef Enemy_H
#define Enemy_H

#include <Character.h>

using namespace Ogre;
using namespace OgreBulletCollisions;
using namespace OgreBulletDynamics;

class Enemy : public Character
{
 public:
  Enemy() {}
  Enemy(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape, std::string type);
  ~Enemy();

  std::string getType();
  int getPoints();

  void setType(std::string type);
  void setPoints(int points);

  //El comportamiento de el enemigo al spawnear y al morir se pondria en estas funciones
  void spawn();
  void die();

 protected:
  std::string _type;
  int _points;
};

#endif
