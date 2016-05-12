#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Enemy.h"

Enemy::Enemy(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape, std::string type){
	_sNode = sNode;
	_rigBody = rigBody;
	_collShape = collShape;
	_type = type;
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

void Enemy::setType(std::string type){
	_type = type;
}

void Enemy::setPoints(int points){
	_points = points;
}

void Enemy::spawn(){
	//animacion de aparecer
	//las vidas y puntos se inicializan segun el tipo de enemigo
}

void Enemy::die(){
	_lives = 0;
	//animacion de morir
}
