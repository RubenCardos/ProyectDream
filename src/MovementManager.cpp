#include "MovementManager.h"
#include "PlayState.h"
#include "AnimationManager.h"

using namespace Ogre;



MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies, std::vector<Boss*>* bossPieces, std::vector<Wall*>* walls){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
	_bossPieces = bossPieces;
	_walls= walls;
	_hero->setNumJumps(Constant::HERO_NUM_JUMPS);
	_inBossRoom = false;

	if(!AI_Manager::getSingletonPtr()){
		_aiManager = new AI_Manager(sceneMgr,_hero,_bossPieces,_enemies);
	}
	else{
		_aiManager = AI_Manager::getSingletonPtr();
		_aiManager->setSceneManager(_sceneMgr);
		_aiManager->setHero(_hero);
		_aiManager->setBossPieces(_bossPieces);
		_aiManager->setEnemies(_enemies);
	}

	_currentIndex = 0;
}

MovementManager::~MovementManager(){
	delete _sceneMgr;
	delete _hero;
	delete _enemies;
	delete _walls;
	delete _aiManager;
}

template<> MovementManager* Ogre::Singleton<MovementManager>::msSingleton = 0;

MovementManager* MovementManager::getSingletonPtr(void){
    return msSingleton;
}

MovementManager& MovementManager::getSingleton(void){  
    assert( msSingleton );  return ( *msSingleton );  
}

void MovementManager::moveHero(Ogre::Vector3* movement){
	Ogre::Vector3 currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	Ogre::Vector3 speed(1,1,1);
	_hero->setSpeed(currentSpeed);

	if(currentSpeed.squaredLength() < _hero->getMovementSpeed()){
		if(abs(_hero->getRigidBody()->getLinearVelocity().y) < 0.01){
			speed = *(movement) + _hero->getRigidBody()->getLinearVelocity();
			speed = speed*0.92;
			_hero->getRigidBody()->setLinearVelocity(speed);
		}
	}
	if(currentSpeed.squaredLength() >= 0.1 ){
		AnimationManager::getSingletonPtr()->stopAnimations(AnimationManager::ANIM_IDLE_HERO);
		AnimationManager::getSingletonPtr()->playAnimations(AnimationManager::ANIM_RUN_HERO);
	}
	else{
		AnimationManager::getSingletonPtr()->playAnimations(AnimationManager::ANIM_IDLE_HERO);
		AnimationManager::getSingletonPtr()->stopAnimations(AnimationManager::ANIM_RUN_HERO);

	}

	rotateHero();

	if(_hero->getRigidBody()->getLinearVelocity().y < 0.001 && _hero->getRigidBody()->getLinearVelocity().y > -0.0001 ){// a veces es 5.72205e-06
		Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
		_currentSpeed.y = 0 ;
		_hero->getRigidBody()->setLinearVelocity(_currentSpeed);
	}

	//Si la velocidad en y es 0 es que ya estoy tocando suelo---------
	if(_hero->getRigidBody()->getLinearVelocity().y == 0 ){// a veces es 5.72205e-06
		if(_hero->getNumJumps() < Constant::HERO_NUM_JUMPS){
			_hero->setNumJumps(Constant::HERO_NUM_JUMPS);
			AnimationManager::getSingletonPtr()->stopAnimations(AnimationManager::ANIM_JUMP_HERO);
		}
	}
}

void MovementManager::jumpHero(Ogre::Vector3* movement){
	Ogre::Vector3 speed(1,1,1);
	if(_hero->getNumJumps() >= 0){
		Ogre::Vector3 currentSpeed = _hero->getRigidBody()->getLinearVelocity();
		currentSpeed.y = 12.0;
		speed = currentSpeed + (*(movement) * 2);
		_hero->getRigidBody()->setLinearVelocity(speed);
		_hero->setNumJumps(_hero->getNumJumps()-1);
		GameManager::getSingletonPtr()->playSoundEffect("jump.ogg");
	}
}

void MovementManager::repositionHero(btVector3 position,btQuaternion orientation){
	//Hago el hero invulnerable-------------------------------
	_hero->makeInvulnerable();
	//--------------------------------------------------------
	AnimationManager::getSingletonPtr()->resetEnemyAnimations();

	btTransform initialTransform;

	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);

	_hero->getRigidBody()->getBulletRigidBody()->setWorldTransform(initialTransform);
	_hero->getRigidBody()->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransform);
}

void MovementManager::repositionGameEntity(GameEntity* gameentity,btVector3 position,btQuaternion orientation){
	btVector3 vect(0,0,0);
	vect = position - gameentity->getRigidBody()->getBulletRigidBody()->getCenterOfMassPosition();

	gameentity->getRigidBody()->getBulletRigidBody()->activate(true);
	gameentity->getRigidBody()->getBulletRigidBody()->translate(vect);
}

void MovementManager::moveEnemies(){
	Ogre::Vector3 speed(1,1,1);
	//Mover a cada enemigo usando la speed calculada por el AI_Manager
	_aiManager->updateEnemyMovement();
	
	for(unsigned int i=0; i<_enemies->size();i++){
		Ogre::Vector3 _currentSpeed = _enemies->at(i)->getRigidBody()->getLinearVelocity();
		if(abs(_enemies->at(i)->getRigidBody()->getLinearVelocity().y) < 0.01){
			speed = _enemies->at(i)->getSpeed() * 3;
			_enemies->at(i)->getRigidBody()->setLinearVelocity(speed);
		}

		if(_currentSpeed.z >0){//Hacia la derecha
			_enemies->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(90),Vector3::UNIT_Y));
		}
		else{
			_enemies->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(-90),Vector3::UNIT_Y));
		}
	}
}

void MovementManager::moveWalls(){
	Ogre::Vector3 mov = _hero->getRigidBody()->getLinearVelocity();
	if(_hero->getSceneNode()->getPosition().x < Constant::REEL_POSITION-20){
		if(mov.x >= 0){
			mov.z = 0.0;
			mov.y = 0.0;
			for(unsigned int i=0; i<_walls->size();i++){
				if(i < _walls->size() -1){
					if(_hero->getRigidBody()->getCenterOfMassPosition().x -_walls->at(_walls->size() -1)->getRigidBody()->getCenterOfMassPosition().x <= Constant::WALLB_DISTANCE){
						mov.x = 0.0;
					}
				}
				_walls->at(i)->getRigidBody()->setLinearVelocity(mov);
			}
		}
	}
}

void MovementManager::moveBoss(){
	btVector3 vect(0,0,0);
	_aiManager->updateBossMovement();
	for(unsigned int i=0; i<_bossPieces->size(); i++){
		_bossPieces->at(i)->getRigidBody()->setLinearVelocity(*(_bossPieces->at(i)->getVSpeed()));
	}
}

Ogre::SceneManager* MovementManager::getSceneManager(){
	return _sceneMgr;
}

Hero* MovementManager::getHero(){
	return _hero;
}

std::vector<Enemy*>* MovementManager::getEnemies(){
	return _enemies;
}

std::vector<Wall*>* MovementManager::getWalls(){
	return _walls;
}

void MovementManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void MovementManager::setHero(Hero* hero){
	_hero = hero;
}

void MovementManager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
}

void MovementManager::setWalls(std::vector<Wall*>* walls){
	_walls = walls;
}

void MovementManager::setBossPieces(std::vector<Boss*>* bossPieces){
	_bossPieces = bossPieces;
}

void MovementManager::inBossRoom(){
	_inBossRoom=true;
}

void MovementManager::initializeBossMovement(Ogre::Real* deltaT){
	_aiManager->initializeBossMovement(deltaT);
	_currentIndex = 0;
}

void MovementManager::rotateHero(){
	Ogre::Degree degrees = Ogre::Degree(0);
	Ogre::Vector3 vel(0,0,0);
	vel = _hero->getSpeed();
	//calculo los grados entre el vector de velocidad del personaje y el unitario en Z
	degrees = vel.angleBetween(Ogre::Vector3::UNIT_X);
	if(vel.z <= 0){
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(degrees),Vector3::UNIT_Y));
	}
	else{
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(-degrees),Vector3::UNIT_Y));
	}
}
