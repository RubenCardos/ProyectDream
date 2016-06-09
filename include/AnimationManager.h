#ifndef AnimationManager_H
#define AnimationManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"
#include "Boss.h"
#include "Scenario.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>


using namespace Ogre;

class AnimationManager : public Ogre::Singleton<AnimationManager>
{
 public:
  AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario* currentScenario);
  ~AnimationManager();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setCurrentScenario(Scenario::Scenario* currentScenario);
  void playAnimations();

  // Heredados de Ogre::Singleton.
  static AnimationManager& getSingleton ();
  static AnimationManager* getSingletonPtr ();

 protected:
  Ogre::SceneManager* _sceneMgr;
  Scenario::Scenario* _currentScenario;
};

#endif
