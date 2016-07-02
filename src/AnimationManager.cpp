#include "AnimationManager.h"

using namespace Ogre;

AnimationManager::AnimationManager(Ogre::SceneManager* sceneMgr, Scenario::Scenario currentScenario, Hero* hero,std::vector<Enemy*>* enemies){
	_sceneMgr = sceneMgr;
	_currentScenario = currentScenario;
	_currentAnimation = ANIM_IDLE_HERO;
	_hero = hero;
	_enemies = enemies;
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

void AnimationManager::setHero(Hero* hero){
	_hero=hero;
}

void AnimationManager::setupAnimations(){
	std::string animNames[]={"runHero","idleHero","attackHero","jumHero","dieHero"};
	for(int i=0; i < Constant::ANIMATIONMANAGER_NUM_ANIMS_HERO;i++){
		_animsHero[i]=_sceneMgr->getEntity("E_Hero")->getAnimationState(animNames[i]);
	}
}

void AnimationManager::setupEnemyAnimations(String name){
	std::string animName = "";

	if(Ogre::StringUtil::match(_enemies->back()->getType(),"rabbit")){
		animName = "walkEnemy";
	}
	else{
		animName = "walkRex";
	}
	Ogre::AnimationStateIterator it = static_cast<Entity*>(_enemies->back()->getSceneNode()->getAttachedObject(0))->getAllAnimationStates()->getAnimationStateIterator();
	it.begin()->second->setEnabled(true);
	it.begin()->second->setLoop(true);
	it.begin()->second->setTimePosition(0.0);
	_animsEnemy[name]=it.begin()->second;
}

void AnimationManager::playEnemyAnimations(Ogre::Real deltaT){

	for (std::map<std::string,Ogre::AnimationState*>::iterator it=_animsEnemy.begin(); it!=_animsEnemy.end(); ++it){
	    //std::cout << it->first << " => " << it->second << '\n';
		AnimationState* aux = it->second;
		if(aux != NULL){
			if(aux->hasEnded()){
				aux->setTimePosition(0.0);
				aux->setEnabled(false);
			}
			else{
				aux->addTime(deltaT);
			}
		}
	}
}

void AnimationManager::resetEnemyAnimations(){
	_animsEnemy.clear();
}

void AnimationManager::resetEnemyAnimation(String name){
	for (std::map<std::string,Ogre::AnimationState*>::iterator it=_animsEnemy.begin(); it!=_animsEnemy.end(); ++it){
		    //std::cout << it->first << " => " << it->second << '\n';
		if(Ogre::StringUtil::match(name,it->first)){
			_animsEnemy.erase(it);
		}
	}
	for(unsigned int i =0;i<_enemies->size();i++){
		if(Ogre::StringUtil::match(name,_enemies->at(i)->getSceneNode()->getName())){
			_enemies->erase(_enemies->begin()+i);
		}
	}

	//_animsEnemy.clear();
}

void AnimationManager::setupBossAnimations(GameEntity* ge){
	Ogre::AnimationStateIterator it = static_cast<Entity*>(ge->getSceneNode()->getAttachedObject(0))->getAllAnimationStates()->getAnimationStateIterator();
	it.begin()->second->setEnabled(true);
	it.begin()->second->setLoop(true);
	it.begin()->second->setTimePosition(0.0);
	_animsBoss[ge->getSceneNode()->getName()]=it.begin()->second;
}

void AnimationManager::playBossAnimations(Ogre::Real deltaT){
	for (std::map<std::string,Ogre::AnimationState*>::iterator it=_animsBoss.begin(); it!=_animsBoss.end(); ++it){
		//std::cout << it->first << " => " << it->second << '\n';
		AnimationState* aux = it->second;
		if(aux != NULL){
			if(aux->hasEnded()){
				aux->setTimePosition(0.0);
				aux->setEnabled(false);
			}
			else{
				aux->addTime(deltaT);
			}
		}
	}
}


void AnimationManager::resetBossAnimation(String name){
	for (std::map<std::string,Ogre::AnimationState*>::iterator it=_animsBoss.begin(); it!=_animsBoss.end(); ++it){
		if(Ogre::StringUtil::match(name,it->first)){
			_animsBoss.erase(it);
		}
	}
}

void AnimationManager::playAnimations(animID id){
	//HERO ANIMATIONS---------------------------------------
	if(id==ANIM_RUN_HERO){
		if(_currentAnimation==ANIM_IDLE_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(true);
			_animsHero[id]->setTimePosition(0.0);
			_currentAnimation=ANIM_RUN_HERO;
		}
	}
	else if(id==ANIM_IDLE_HERO){
		if(_currentAnimation!=ANIM_IDLE_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(true);
			_animsHero[id]->setTimePosition(0.0);
		}
	}
	else if(id==ANIM_ATTACK_HERO){
		if(_currentAnimation==ANIM_IDLE_HERO || _currentAnimation==ANIM_RUN_HERO){
			_animsHero[id]->setEnabled(true);
			_animsHero[id]->setLoop(false);
			_animsHero[id]->setTimePosition(0.0);
			_currentAnimation=ANIM_ATTACK_HERO;
			_hero->setAttacking(true);
		}
	}
	else if(id==ANIM_JUMP_HERO){
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
	else if(id==ANIM_JUMP_HERO){
		_animsHero[id]->setEnabled(false);
		_currentAnimation=ANIM_IDLE_HERO;
	}
	else if(id==ANIM_ATTACK_HERO){
		_animsHero[id]->setEnabled(false);
		_currentAnimation=ANIM_IDLE_HERO;
	}
	else if(id==ANIM_IDLE_HERO){
		_animsHero[id]->setEnabled(false);
	}
}

void AnimationManager::resetAnimations(Real _deltaT){
	for(int i=0; i < Constant::ANIMATIONMANAGER_NUM_ANIMS_HERO;i++){
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

