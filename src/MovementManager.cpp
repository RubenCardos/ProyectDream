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
	_jumps = _hero->getNumJumps();
	_heroCoMPositionY = _hero->getRigidBody()->getCenterOfMassPosition().y;
	//std::cout << "CENTER OF MASS POSITION " << _heroCoMPositionY <<std::endl;
	//luego cambiar lo del reseteo de los saltos de centro de masa a colision con el suelo
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
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	//cout << "Speed" <<_currentSpeed << endl;
	if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){
		_hero->getRigidBody()->applyImpulse(*movement, _hero->getRigidBody()->getCenterOfMassPosition());
		if(std::abs(movement->z - 0.0) <= 0.01){ //
			moveWalls(movement, deltaT);
		}
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
	/*//Muevo los muros a la par que el heroe-------------
		if(movement->x!=0){
			for(unsigned int i = 0; i < _walls->size();i++){
				Wall* _aux = _walls->at(i);
				cout << "Wall: " << _aux->getSceneNode()->getName() << endl;
				Vector3 _wallMovement = *movement;
				_wallMovement.y=0;
				_wallMovement.z=0;
				if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){
					_aux->getRigidBody()->applyImpulse(_wallMovement, _aux->getRigidBody()->getCenterOfMassPosition());
				}

			}
		}*/
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
