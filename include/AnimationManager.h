#ifndef AnimationManager_H
#define AnimationManager_H

#include <Ogre.h>
#include <OgreSingleton.h>
#include "Enemy.h"
#include "Hero.h"
#include "Boss.h"
#include "Scenario.h"
#include "Constant.h"

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>


using namespace Ogre;

class AnimationManager : public Ogre::Singleton<AnimationManager>
{
  public: enum animID{

            ANIM_RUN_HERO,
            ANIM_IDLE_HERO,
            ANIM_ATTACK_HERO,
            ANIM_JUMP_HERO,
            ANIM_DIE_HERO

           };

 public:
  AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario currentScenario, Hero* hero,std::vector<Enemy*>* enemies);
  ~AnimationManager();

  void setSceneManager(Ogre::SceneManager* sceneMgr);
  void setCurrentScenario(Scenario::Scenario currentScenario);
  void setHero(Hero* hero);
  void playAnimations(animID id);
  void stopAnimations(animID id);
  void setupAnimations();
  void setupEnemyAnimations(String name);
  void setupBossAnimations(GameEntity* ge);
  void resetAnimations(Real _deltaT);
  Ogre::AnimationState* getAnimation(animID id);
  void playEnemyAnimations(Ogre::Real deltaT);
  void playBossAnimations(Ogre::Real deltaT);
  void resetEnemyAnimations();
  void resetEnemyAnimation(String name);
  void resetBossAnimation(String name);


  // Heredados de Ogre::Singleton.
  static AnimationManager& getSingleton ();
  static AnimationManager* getSingletonPtr ();

 protected:
  Ogre::SceneManager* _sceneMgr;
  Scenario::Scenario _currentScenario;
  Ogre::AnimationState *_animsHero[Constant::ANIMATIONMANAGER_NUM_ANIMS];
  std::map<std::string,Ogre::AnimationState*> _animsEnemy;
  std::map<std::string,Ogre::AnimationState*> _animsBoss;
  //Ogre::AnimationState *_animsEnemy[];
  animID _currentAnimation;
  Hero* _hero;
  std::vector<Enemy*>* _enemies;
};

#endif
