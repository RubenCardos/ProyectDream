#include "AnimationManager.h"

using namespace Ogre;

AnimationManager::AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario* currentScenario){
	_sceneMgr = sceneMgr;
	_currentScenario = currentScenario;
}

AnimationManager::~AnimationManager(){
	delete _sceneMgr;
	delete _currentScenario;
}

void AnimationManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void AnimationManager::setCurrentScenario(Scenario::Scenario* currentScenario){
	_currentScenario = currentScenario;
}

void AnimationManager::playAnimations(){
	//COSITAS
}

template<> AnimationManager* Ogre::Singleton<AnimationManager>::msSingleton = 0;

AnimationManager* AnimationManager::getSingletonPtr(void){
    return msSingleton;
}

AnimationManager& AnimationManager::getSingleton(void){
    assert( msSingleton );  return ( *msSingleton );  
}

