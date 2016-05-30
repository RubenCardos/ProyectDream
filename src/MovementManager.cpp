#include "MovementManager.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies,std::vector<Wall*>* walls){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
	_walls= walls;
	_hero->setNumJumps(N_JUMPS);
	//std::cout << "CENTER OF MASS POSITION " << _heroCoMPositionY <<std::endl;
	//luego cambiar lo del reseteo de los saltos de centro de masa a colision con el suelo
}

MovementManager::~MovementManager(){
	delete _sceneMgr;
	delete _hero;
	delete _enemies;
	delete _walls;
}

template<> MovementManager* Ogre::Singleton<MovementManager>::msSingleton = 0;

MovementManager* MovementManager::getSingletonPtr(void){
    return msSingleton;
}

MovementManager& MovementManager::getSingleton(void){  
    assert( msSingleton );  return ( *msSingleton );  
}

void MovementManager::moveHero(Ogre::Vector3* movement, Ogre::Real deltaT){
	//movimiento del heroe
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){
		_hero->getRigidBody()->applyImpulse(*movement, _hero->getRigidBody()->getCenterOfMassPosition());
		if(std::abs(movement->z - 0.0) <= 0.01){ //
			moveWalls(movement, deltaT);
		}
	}
}

void MovementManager::jumpHero(){
	if(_hero->getNumJumps() > 0){
		Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
		_currentSpeed.y = 12.0;
		_hero->getRigidBody()->setLinearVelocity(_currentSpeed);
		_hero->setNumJumps(_hero->getNumJumps()-1);
	}
}

void MovementManager::repositionHero(btVector3 position,btQuaternion orientation){
	btTransform initialTransform;

	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);
	//initialTransform.setRotation(orientation);

	_hero->getRigidBody()->getBulletRigidBody()->setWorldTransform(initialTransform);
	_hero->getRigidBody()->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransform);
	//mMotionState->setWorldTransform(initialTransform);
}

void MovementManager::moveEnemies(Ogre::Real deltaT){
	//mas adelante, cuando esten los enemigos hechos
	for(unsigned int i=0; i<_enemies->size();i++){
		Enemy* enemy = static_cast<Enemy*>(_enemies->at(i));
		Ogre::Vector3 _currentSpeed = enemy->getRigidBody()->getLinearVelocity();
		//enemy->getRigidBody()->setLinearVelocity(enemy->getSpeed());
		if(_currentSpeed.squaredLength() < enemy->getMovementSpeed()){
			enemy->getRigidBody()->applyImpulse(enemy->getSpeed() ,enemy->getRigidBody()->getCenterOfMassPosition());
		}

	}
}

void MovementManager::moveWalls(Ogre::Vector3* movement, Ogre::Real deltaT){
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	for(unsigned int i=0; i<_walls->size();i++){
		_walls->at(i)->getRigidBody()->applyImpulse(*movement * 20, _walls->at(i)->getRigidBody()->getCenterOfMassPosition());
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
