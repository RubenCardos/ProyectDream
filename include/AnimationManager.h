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


#define NUM_ANIMS 12


class AnimationManager : public Ogre::Singleton<AnimationManager>
{
  public: enum animID{

          ANIM_RUN_HERO,
          ANIM_IDLE_HERO,
          ANIM_DIE_HERO

           };

 public:
  AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario* currentScenario);
  ~AnimationManager();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setCurrentScenario(Scenario::Scenario* currentScenario);
  void playAnimations(animID id);
  void stopAnimations(animID id);
  void setupAnimations();
  void resetAnimations(animID id, Real _deltaT);
  // Heredados de Ogre::Singleton.
  static AnimationManager& getSingleton ();
  static AnimationManager* getSingletonPtr ();

 protected:
  Ogre::SceneManager* _sceneMgr;
  Scenario::Scenario* _currentScenario;
  Ogre::AnimationState *_anims[NUM_ANIMS];
};

#endif
