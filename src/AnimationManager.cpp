#include "AnimationManager.h"

using namespace Ogre;

#define NUM_ANIMS 1


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

void AnimationManager::setupAnimations(){

	std::string animNames[]={"runHero"};//,"idleHero","dieHero","jumpHero"}; //SOLO ESTA LA PRIMERA DE MOMENTO

	for(int i=0; i < NUM_ANIMS;i++){

		_anims[i]=_sceneMgr->getEntity("E_Hero")->getAnimationState(animNames[i]);

	}

}


void AnimationManager::playAnimations(animID id){
	
	if(id==ANIM_RUN_HERO){
		_anims[id]->setEnabled(true);
		_anims[id]->setLoop(true);
		_anims[id]->setTimePosition(0.0);

	}

}

void AnimationManager::stopAnimations(animID id){
	
	if(id==ANIM_RUN_HERO){
		_anims[id]->setEnabled(false);
	}

}

void AnimationManager::resetAnimations(animID id, Real _deltaT){
	
	if (_anims[id] != NULL) {
		if (_anims[id]->hasEnded()) {
			_anims[id]->setTimePosition(0.0);
			_anims[id]->setEnabled(false);
		}
		else {
			_anims[id]->addTime(_deltaT);
		}
	}


}

template<> AnimationManager* Ogre::Singleton<AnimationManager>::msSingleton = 0;

AnimationManager* AnimationManager::getSingletonPtr(void){
    return msSingleton;
}

AnimationManager& AnimationManager::getSingleton(void){
    assert( msSingleton );  return ( *msSingleton );  
}

