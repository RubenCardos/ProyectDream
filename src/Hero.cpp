#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Hero.h"

#define INITIAL_LIVES 1

Hero::Hero(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape){
	_sNode = sNode;
	_rigBody = rigBody;
	_collShape = collShape;
	_lives = INITIAL_LIVES;
	_score = 0;
	_movementspeed=50.0;
	spawn();
}

Hero::~Hero(){
	delete _sNode;
	delete _collShape;
	delete _rigBody;
}

void Hero::spawn(){
	//animacion de aparecer
}

void Hero::die(){
	_lives = 0;
	//animacion de morir
}

int Hero::getScore(){
	return _score;
}

void Hero::increaseScore(int amount){
	_score = _score + amount;
}
