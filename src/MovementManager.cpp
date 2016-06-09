#include "MovementManager.h"

using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies, std::vector<Boss*>* bossPieces, std::vector<Wall*>* walls){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
	_bossPieces = bossPieces;
	_walls= walls;
	_hero->setNumJumps(N_JUMPS);
	_inBossRoom = false;
	_aiManager = new AI_Manager(_hero,_bossPieces,_enemies);
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
	//movimiento del heroe
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	_hero->setSpeed(_currentSpeed);
	if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){
		_hero->getRigidBody()->applyImpulse(*movement, _hero->getRigidBody()->getCenterOfMassPosition());
		if(_inBossRoom==false){//Si estoy en la zona del boss la zona es fija, no se mueven las paredes
		}
	}
	rotateHero();
	/*if(_currentSpeed.z >0){//Hacia la derecha
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(-90),Vector3::UNIT_Y));
	}
	if(_currentSpeed.z <0){//Hacia la derecha
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(90),Vector3::UNIT_Y));
	}
	if(_currentSpeed.x >0){//Hacia la derecha
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(0),Vector3::UNIT_Y));
	}
	if(_currentSpeed.x <0){//Hacia la derecha
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(180),Vector3::UNIT_Y));
	}*/

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

	//Creo que hay que reposicionar las paredes y el suelo tambien
	/*for(int i=0; i<_walls->size(); i++){
		_walls->at(i)->getRigidBody()->getBulletRigidBody()->setWorldTransform(initialTransform);
		_walls->at(i)->getRigidBody()->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransform);
	}*/
}

void MovementManager::repositionGameEntity(GameEntity* gameentity,btVector3 position,btQuaternion orientation){
	btTransform initialTransform;

	initialTransform.setOrigin(position);
	initialTransform.setRotation(orientation);
	//initialTransform.setRotation(orientation);

	gameentity->getRigidBody()->getBulletRigidBody()->setWorldTransform(initialTransform);
	gameentity->getRigidBody()->getBulletRigidBody()->getMotionState()->setWorldTransform(initialTransform);
	//mMotionState->setWorldTransform(initialTransform);

	//Creo que hay que reposicionar las paredes y el suelo tambien
}

void MovementManager::moveEnemies(){
	//Cuando esté hecho el AI_Manager, mover a cada enemigo usando la speed calculada por el AI_Manager
	_aiManager->updateEnemyMovement();
	
	for(unsigned int i=0; i<_enemies->size();i++){
		Ogre::Vector3 _currentSpeed = _enemies->at(i)->getRigidBody()->getLinearVelocity();
		//enemy->getRigidBody()->setLinearVelocity(enemy->getSpeed());
		if(_currentSpeed.squaredLength() < _enemies->at(i)->getMovementSpeed()){
			cout << "		Velocidad del enemigo = " << _enemies->at(i)->getSpeed() <<endl;
			_enemies->at(i)->getRigidBody()->applyImpulse(_enemies->at(i)->getSpeed() ,_enemies->at(i)->getRigidBody()->getCenterOfMassPosition());

		}
		if(_currentSpeed.z >0){//Hacia la derecha
			_enemies->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(90),Vector3::UNIT_Y));
		}else{
			_enemies->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(-90),Vector3::UNIT_Y));
		}
	}
}

void MovementManager::moveWalls(){
	Ogre::Vector3 mov = _hero->getRigidBody()->getLinearVelocity();
	if(mov.x >= 0){
		mov.z = 0.0;
		mov.y = 0.0;
		for(unsigned int i=0; i<_walls->size();i++){
			//_walls->at(i)->getRigidBody()->applyImpulse(mov * 20, _walls->at(i)->getRigidBody()->getCenterOfMassPosition());
			_walls->at(i)->getRigidBody()->setLinearVelocity(mov);
		}
	}
}

void MovementManager::moveBoss(){
	//_aiManager->updateBossMovement();

	//cout << "IMPRIMIENDO DATOS DE LAS PIEZAS DEL BOSS" << endl;
	for(unsigned int i=0; i<_bossPieces->size(); i++){
		//_bossPieces->at(i)->getRigidBody()->setAngularVelocity(velocity);
		//_bossPieces->at(i)->getRigidBody()->setLinearVelocity(*(_bossPieces->at(i)->getVSpeed()));
		_bossPieces->at(i)->getRigidBody()->setLinearVelocity(2,0,0);
		cout << "	VMov = " << _bossPieces->at(i)->getMovementSpeed() << "	VSpeed = " << *(_bossPieces->at(i)->getVSpeed()) << endl;
		cout << "	target = " << *(_bossPieces->at(i)->getTargetPosition()) << " current = " << _bossPieces->at(i)->getCurrentIndex() << endl;
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
	//_aiManager->setBossPieces(bossPieces);
}

void MovementManager::inBossRoom(){
	_inBossRoom=true;
}

void MovementManager::initializeBossMovement(Ogre::Real* deltaT){
	_aiManager->loadBossRoute();
	_aiManager->initializeBossMovement(deltaT);
}

void MovementManager::rotateHero(){
	Ogre::Degree degrees = Ogre::Degree(0);
	Ogre::Vector3 vel(0,0,0);
	vel = _hero->getSpeed();
	//calculo los grados entre el vector de velocidad del personaje y el unitario en Z
	degrees = vel.angleBetween(Ogre::Vector3::UNIT_X);
	_hero->getRigidBody()->setOrientation(Quaternion(Degree(degrees),Vector3::UNIT_Y));
}
