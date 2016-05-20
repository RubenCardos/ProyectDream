#include "MovementManager.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
	_hero->setNumJumps(N_JUMPS);
	_jumps = _hero->getNumJumps();
	//_heroCoMPositionY = _hero->getRigidBody()->getCenterOfMassPosition().y;
	_heroCoMPositionY = _hero->getRigidBody()->getCenterOfMassPosition().y;
	std::cout << "CENTER OF MASS POSITION " << _heroCoMPositionY <<std::endl;
}

MovementManager::~MovementManager(){
	/*delete _sceneMgr;
	delete _hero;
	delete _enemies;*/
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
	//_hero->getSceneNode()->translate(*movement * deltaT * 20);
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	//cout << "Speed" <<_currentSpeed << endl;
	if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){
		_hero->getRigidBody()->applyImpulse(*movement, _hero->getRigidBody()->getCenterOfMassPosition());
	}
	
}

void MovementManager::jumpHero(){
	//salto del heroe
	//cuando el heroe salte, _jumps--. Cuando termine de saltar, _jumps++.
	cout << "SALTO" << endl;

	heroHasLanded();

	if(_jumps > 0){
		Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
		_currentSpeed.y = 12.0;
		_hero->getRigidBody()->setLinearVelocity(_currentSpeed);
		_jumps--;
	}
}

bool MovementManager::heroHasLanded(){
	cout << "CoMPosition " << _hero->getRigidBody()->getCenterOfMassPosition().y << " Speed "<< _hero->getRigidBody()->getLinearVelocity().y << endl;
	//Para ver si el heroe ha caido tras un salto
	//comprobar que su velocidad es negativa (o nula) en el eje Y y que la posicion de su centro de masa es igual o menor que en reposo
	std::cout << "CoMPosition: " << (_hero->getRigidBody()->getCenterOfMassPosition().y <= _heroCoMPositionY) << " Speed " << (_hero->getRigidBody()->getLinearVelocity().y <= 0.0) << std::endl;
	if((_hero->getRigidBody()->getCenterOfMassPosition().y <= _heroCoMPositionY) && (_hero->getRigidBody()->getLinearVelocity().y <= (JUMP_EPSILON))){
		std::cout << "Heroe ha caido de un salto" << std::endl;
		_jumps = _hero->getNumJumps();
	}
	return _jumps > 0;
}

void MovementManager::reposition(btVector3 position,btQuaternion orientation){
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
	Enemy *enemy = _enemies->at(0);
	std::cout << "Vector" <<enemy->getSpeed() << std::endl;
	Ogre::Vector3 _currentSpeed = enemy->getRigidBody()->getLinearVelocity();
	//enemy->getRigidBody()->setLinearVelocity(enemy->getSpeed());
	if(_currentSpeed.squaredLength() < enemy->getMovementSpeed()){
		enemy->getRigidBody()->applyImpulse(enemy->getSpeed(),enemy->getRigidBody()->getCenterOfMassPosition());
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

void MovementManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void MovementManager::setHero(Hero* hero){
	_hero = hero;
}

void MovementManager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
}
