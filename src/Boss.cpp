#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Boss.h"

#define DEFAULT_HEALTH 2
#define DEFAULT_POINTS 10

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
	_points = DEFAULT_POINTS;
	_health = DEFAULT_HEALTH;
	_vulnerable = false;
	_targetIndex = 0;
	_currentIndex = -1;
	_rageTimer = 0.0;
	_movementSpeed=50.0;
	_vSpeed = new Ogre::Vector3(0,0,0);
	_currentPosition = new Ogre::Vector3(0,0,0);
	_targetPosition = new Ogre::Vector3(0,0,0);
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
double Boss::getMovementspeed(){
	return _movementSpeed;
}
Ogre::Real Boss::getRageTimer(){
	return _rageTimer;
}
Ogre::Vector3* Boss::getVSpeed(){
	return _vSpeed;
}
int Boss::getTargetIndex(){
	return _targetIndex;
}
int Boss::getCurrentIndex(){
	return _currentIndex;
}
Ogre::Vector3* Boss::getTargetPosition(){
	return _targetPosition;
}
Ogre::Vector3* Boss::getCurrentPosition(){
	return _currentPosition;
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
void Boss::setMovementspeed(double movementspeed){
	_movementSpeed = movementspeed;
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
void Boss::setTargetIndex(int targetIndex){
	_targetIndex = targetIndex;
}
void Boss::setCurrentIndex(int currentIndex){
	_currentIndex = currentIndex;
}
void Boss::setTargetPosition(Ogre::Vector3* targetPosition){
	_targetPosition = targetPosition;
}
void Boss::setCurrentPosition(Ogre::Vector3* currentPosition){
	_currentPosition = currentPosition;
}
