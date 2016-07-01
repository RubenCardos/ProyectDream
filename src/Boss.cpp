#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Boss.h"

Boss::Boss(){
	spawn();
}

Boss::Boss(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, BossType type){
	_sNode = sNode;
	_rigBody = rigBody;
	_type = type;
	spawn();
}

Boss::~Boss(){
	delete _sNode;
	delete _rigBody;
}

void Boss::spawn(){
	//animacion de aparecer
	//las vidas y puntos se inicializan segun el tipo de enemigo
	//if(Ogre::StringUtil::startsWith(_sNode->getName(),"SN_BossLocomotive")){
		//_points = DEFAULT_POINTS * 5;
	//}
	//else{
		_points = Constant::BOSS_DEFAULT_POINTS;
	//}

	_health = Constant::BOSS_DEFAULT_HEALTH;
	_vulnerable = true;
	_currentIndex = -1;
	_rageTimer = 0.0;
	_movementSpeed=5.0;
	_vSpeed = new Ogre::Vector3(0,0,0);
	_targetPosition = new Ogre::Vector3(0,0,0);
	_distanceBetweenWagons = 10;
}

void Boss::die(){
	_health = 0;
	//animacion de morir
}

bool Boss::isEnraged(){
	bool enraged = false;
	if(_rageTimer > 0.0){
		enraged = true;
	}
	return enraged;
}
bool Boss::isVulnerable(){
	return _vulnerable;
}

BossType Boss::getType(){
	return _type;
}
int Boss::getPoints(){
	return _points;
}
int Boss::getHealth(){
	return _health;
}
double Boss::getMovementSpeed(){
	return _movementSpeed;
}
Ogre::Real Boss::getRageTimer(){
	return _rageTimer;
}
Ogre::Vector3* Boss::getVSpeed(){
	return _vSpeed;
}
int Boss::getCurrentIndex(){
	return _currentIndex;
}
Ogre::Vector3* Boss::getTargetPosition(){
	return _targetPosition;
}
int Boss::getDistanceBetweenWagons(){
	return _distanceBetweenWagons;
}

void Boss::setType(BossType type){
	_type = type;
}
void Boss::loseHealth(){
	if(_health > 0){
		_health--;
		if(_health <= 0){
			die();
		}
	}
}
void Boss::setMovementSpeed(double movementSpeed){
	_movementSpeed = movementSpeed;
}
void Boss::setRageTimer(Ogre::Real rageTimer){
	_rageTimer = rageTimer;
}
void Boss::setVulnerable(bool vulnerable){
	_vulnerable = vulnerable;
}
void Boss::setV_Speed(Ogre::Vector3* vSpeed){
	_vSpeed = vSpeed;
}
void Boss::setCurrentIndex(int currentIndex){
	_currentIndex = currentIndex;
}
void Boss::setTargetPosition(Ogre::Vector3* targetPosition){
	_targetPosition = targetPosition;
}

