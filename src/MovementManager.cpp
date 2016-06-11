#include "MovementManager.h"
#include "PlayState.h"
#include "AnimationManager.h"

using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01
#define DISTANCE_BETWEEN_WAGONS 10

MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies, std::vector<Boss*>* bossPieces, std::vector<Wall*>* walls){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
	_bossPieces = bossPieces;
	_walls= walls;
	_hero->setNumJumps(N_JUMPS);
	_inBossRoom = false;
	_aiManager = new AI_Manager(sceneMgr, _hero,_bossPieces,_enemies);
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
	//cout <<"Velocidad Y = " << _hero->getRigidBody()->getLinearVelocity().y <<endl;
	cout <<"DESP = " << *movement <<endl;
	Ogre::Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
	_hero->setSpeed(_currentSpeed);
	if(_currentSpeed.squaredLength() < _hero->getMovementSpeed()){

		_hero->getRigidBody()->applyImpulse(*movement, _hero->getRigidBody()->getCenterOfMassPosition());
		if(!_inBossRoom){//Si estoy en la zona del boss la zona es fija, no se mueven las paredes

		}
		
		
	}
	
	if(_hero->getRigidBody()->getLinearVelocity() == Vector3(0,0,0)){
		AnimationManager::getSingletonPtr()->playAnimations(AnimationManager::ANIM_RUN_HERO);

	}else{
		//IRA LA ANIMACION DE IDLE
	}



	rotateHero();

	if(_hero->getRigidBody()->getLinearVelocity().y < 0.001 && _hero->getRigidBody()->getLinearVelocity().y > -0.0001 ){// a veces es 5.72205e-06
		Vector3 _currentSpeed = _hero->getRigidBody()->getLinearVelocity();
		_currentSpeed.y = 0 ;
		_hero->getRigidBody()->setLinearVelocity(_currentSpeed);
	}

	//Si la velocidad en y es 0 es que ya estoy tocando suelo---------
	if(_hero->getRigidBody()->getLinearVelocity().y == 0 ){// a veces es 5.72205e-06
		if(_hero->getNumJumps() < N_JUMPS){
			_hero->setNumJumps(N_JUMPS);
		}
	}
	//----------------------------------------------------------------

	//Si no no mueve que el personaje mire hacia adelante(Arreglar)------
	if(*movement == Vector3(0,0,0) && _hero->getRigidBody()->getLinearVelocity().x == 0 && _hero->getRigidBody()->getLinearVelocity().z == 0 ){
		_hero->getRigidBody()->setOrientation(Quaternion(Degree(0),Vector3::UNIT_Y));
	}
	//-------------------------------------------------------------------

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
		_currentSpeed.y = 16.0;
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
			//cout << "		Velocidad del enemigo = " << _enemies->at(i)->getSpeed() <<endl;
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
	_aiManager->updateBossMovement();
	for(unsigned int i=0; i<_bossPieces->size(); i++){
		_bossPieces->at(i)->getRigidBody()->setLinearVelocity(*(_bossPieces->at(i)->getVSpeed()));
	}
	rotateBoss();

	//std::cout << "	velocidad obtenida de la locomotora"<< _bossPieces->at(0)->getVSpeed() << "o" << *(_bossPieces->at(0)->getVSpeed()) << std::endl;

	//cout << "IMPRIMIENDO DATOS DE LAS PIEZAS DEL BOSS" << endl;
	/*for(unsigned int i=0; i<_bossPieces->size(); i++){
		//_bossPieces->at(i)->getRigidBody()->setAngularVelocity(velocity);
		//_bossPieces->at(i)->getRigidBody()->setLinearVelocity(*(_bossPieces->at(i)->getVSpeed()));
		_bossPieces->at(i)->getRigidBody()->setLinearVelocity(2,0,0);
		cout << "	VMov = " << _bossPieces->at(i)->getMovementSpeed() << "	VSpeed = " << *(_bossPieces->at(i)->getVSpeed()) << endl;
		cout << "	target = " << *(_bossPieces->at(i)->getTargetPosition()) << " current = " << _bossPieces->at(i)->getCurrentIndex() << endl;
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

void MovementManager::setBossPieces(std::vector<Boss*>* bossPieces){
	_bossPieces = bossPieces;
	//_aiManager->setBossPieces(bossPieces);
}

void MovementManager::inBossRoom(){
	_inBossRoom=true;
}

void MovementManager::initializeBossMovement(Ogre::Real* deltaT){
	//_aiManager->loadBossRoute();
	_aiManager->initializeBossMovement(deltaT);
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

void MovementManager::rotateBoss(){
	Ogre::Degree degrees = Ogre::Degree(0);
	Ogre::Vector3* vel = new Ogre::Vector3(0,0,0);
	for(unsigned i=0; i<_bossPieces->size(); i++){
		vel = _bossPieces->at(i)->getVSpeed();
		//calculo los grados entre el vector de velocidad del personaje y el unitario en Z
		degrees = vel->angleBetween(Ogre::Vector3::UNIT_X);
		if(vel->z < 0){
			_bossPieces->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(degrees),Vector3::UNIT_Y));
		}
		else{
			_bossPieces->at(i)->getRigidBody()->setOrientation(Quaternion(Degree(-degrees),Vector3::UNIT_Y));
		}
	}
}
