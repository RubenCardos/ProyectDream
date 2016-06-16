#include "AnimationManager.h"

using namespace Ogre;

#define NUM_ANIMS 4


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

	std::string animNames[]={"runHero","idleHero","attackHero","jumpHero"}; //SOLO ESTA LA PRIMERA DE MOMENTO

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

	if(id==ANIM_IDLE_HERO){
		_anims[id]->setEnabled(true);
		_anims[id]->setLoop(true);
		_anims[id]->setTimePosition(0.0);

	}

	if(id==ANIM_ATTACK_HERO){
		_anims[id]->setEnabled(true);
		_anims[id]->setLoop(true);
		_anims[id]->setTimePosition(0.0);

	}

	if(id==ANIM_JUMP_HERO){
		_anims[id]->setEnabled(true);
		_anims[id]->setLoop(true);
		_anims[id]->setTimePosition(1.0);

	}

}

void AnimationManager::stopAnimations(animID id){
	
	if(id==ANIM_RUN_HERO){
		_anims[id]->setEnabled(false);
	}
	if(id==ANIM_JUMP_HERO){
		_anims[id]->setEnabled(false);
	}
	if(id==ANIM_ATTACK_HERO){
		_anims[id]->setEnabled(false);
	}
	if(id==ANIM_IDLE_HERO){
		_anims[id]->setEnabled(false);
	}

}

void AnimationManager::resetAnimations(Real _deltaT){
	
	for(int i=0; i < NUM_ANIMS;i++){

		if(_anims[i]!=_anims[ANIM_JUMP_HERO]){

			if (_anims[i] != NULL) {
				if (_anims[i]->hasEnded()) {
					_anims[i]->setTimePosition(0.0);
					_anims[i]->setEnabled(false);
				}
				else {
					_anims[i]->addTime(_deltaT);
				}
			}



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

