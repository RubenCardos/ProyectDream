#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Enemy.h"

#define DEFAULT_LIVES 1
#define DEFAULT_POINTS 10

Enemy::Enemy(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape, std::string type){
	_sNode = sNode;
	_rigBody = rigBody;
	_collShape = collShape;
	_type = type;
	_points = DEFAULT_POINTS;
	_movementspeed=50.0;
	_speed = Ogre::Vector3(0,0,0);
	spawn();
}

Enemy::~Enemy(){
	delete _sNode;
	delete _collShape;
	delete _rigBody;
}

std::string Enemy::getType(){
	return _type;
}	
int Enemy::getPoints(){
	return _points;
}
double Enemy::getMovementSpeed(){
	return _movementspeed;
}
Ogre::Vector3 Enemy::getSpeed(){
	return _speed;
}

void Enemy::setType(std::string type){
	_type = type;
}

void Enemy::setPoints(int points){
	_points = points;
}
void Enemy::setMovementSpeed(double speed){
	_movementspeed=speed;
}
void Enemy::setSpeed(Ogre::Vector3 speed){
	_speed = speed;
}

void Enemy::spawn(){
	//animacion de aparecer
	//las vidas y puntos se inicializan segun el tipo de enemigo
}

void Enemy::die(){
	_lives = 0;
	//animacion de morir
}
