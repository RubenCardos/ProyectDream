#include "AnimationManager.h"

using namespace Ogre;

#define NUM_ANIMS 5


AnimationManager::AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario currentScenario, Hero* hero){
	_sceneMgr = sceneMgr;
	_currentScenario = currentScenario;
	_currentAnimation = ANIM_IDLE_HERO;
	_hero = hero;
}

AnimationManager::~AnimationManager(){
	delete _sceneMgr;
}

void AnimationManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void AnimationManager::setCurrentScenario(Scenario::Scenario currentScenario){
	_currentScenario = currentScenario;
}

void AnimationManager::setupAnimations(){

	std::string animNames[]={"runHero","idleHero","attackHero","jumHero","dieHero"}; //SOLO ESTA LA PRIMERA DE MOMENTO

	for(int i=0; i < NUM_ANIMS;i++){

		_animsHero[i]=_sceneMgr->getEntity("E_Hero")->getAnimationState(animNames[i]);

	}


}

void AnimationManager::playAnimationsEnemy(string name, string entity){
		
		//ANIMATIONS ENEMY-------------------------------------------------------
		_animsEnemy =_sceneMgr->getEntity(entity)->getAnimationState(name);
		_animsEnemy->setEnabled(true);
		_animsEnemy->setLoop(true);
		_animsEnemy->setTimePosition(0.0);
		//-----------------------------------------------------------------------

}


void AnimationManager::playAnimations(animID id){
	
	//ANIMATIONS HERO---------------------------------------
	if(id==ANIM_RUN_HERO){
		if(_currentAnimation==ANIM_IDLE_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(true);
			_animsHero[id]->setTimePosition(0.0);
			_currentAnimation=ANIM_RUN_HERO;
		}

	}

	if(id==ANIM_IDLE_HERO){
		if(_currentAnimation!=ANIM_IDLE_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(true);
			_animsHero[id]->setTimePosition(0.0);
		}

	}

	if(id==ANIM_ATTACK_HERO){
		if(_currentAnimation==ANIM_IDLE_HERO || _currentAnimation==ANIM_RUN_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(false);
			_animsHero[id]->setTimePosition(0.0);
			_currentAnimation=ANIM_ATTACK_HERO;
			_hero->setAttacking(true);
		}
	}

	if(id==ANIM_JUMP_HERO){
		if(_currentAnimation==ANIM_IDLE_HERO || _currentAnimation==ANIM_RUN_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(true);
			_animsHero[id]->setTimePosition(0.0);
			_currentAnimation=ANIM_JUMP_HERO;
		}

	}
	//----------------------------------------------------------------------

	
}

void AnimationManager::stopAnimations(animID id){
	
	if(id==ANIM_RUN_HERO){
		_animsHero[id]->setEnabled(false);
		_currentAnimation=ANIM_IDLE_HERO;
	}
	if(id==ANIM_JUMP_HERO){
		_animsHero[id]->setEnabled(false);
		_currentAnimation=ANIM_IDLE_HERO;
	}
	if(id==ANIM_ATTACK_HERO){
		_animsHero[id]->setEnabled(false);
		_currentAnimation=ANIM_IDLE_HERO;
	}
	if(id==ANIM_IDLE_HERO){
		_animsHero[id]->setEnabled(false);
	}

}

void AnimationManager::resetAnimations(Real _deltaT){
	
	for(int i=0; i < NUM_ANIMS;i++){

	//	if(_anims[i]!=_anims[ANIM_JUMP_HERO]){

			if (_animsHero[i] != NULL) {
				
				if(_animsHero[i] == _animsHero[ANIM_ATTACK_HERO] && _animsHero[i]->hasEnded()){
					_currentAnimation=ANIM_IDLE_HERO;
					_hero->setAttacking(false);
				}


				if (_animsHero[i]->hasEnded()) {
					_animsHero[i]->setTimePosition(0.0);
					_animsHero[i]->setEnabled(false);
				}
				else {
					_animsHero[i]->addTime(_deltaT);
				}
			//}

				if(_hero->getRigidBody()->getLinearVelocity().y == 0 && _currentAnimation==ANIM_JUMP_HERO){

					stopAnimations(AnimationManager::ANIM_JUMP_HERO);

				}

		}

		
	}
	if(_currentScenario != Scenario::Menu){


		if (_animsEnemy != NULL) {
			if (_animsEnemy->hasEnded()) {
				_animsEnemy->setTimePosition(0.0);
				_animsEnemy->setEnabled(false);
			}
			else {
				_animsEnemy->addTime(_deltaT);
			}
		}
	}

}
AnimationState* AnimationManager::getAnimation(animID id){
			return _animsHero[id];

}

template<> AnimationManager* Ogre::Singleton<AnimationManager>::msSingleton = 0;

AnimationManager* AnimationManager::getSingletonPtr(void){
    return msSingleton;
}

AnimationManager& AnimationManager::getSingleton(void){
    assert( msSingleton );  return ( *msSingleton );  
}

