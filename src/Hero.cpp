#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Hero.h"

#define INITIAL_LIVES 1
#define NUM_JUMPS 1
#define DEFAULT_LIVES 3

Hero::Hero(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody){
	_sNode = sNode;
	_rigBody = rigBody;
	_lives = INITIAL_LIVES;
	_score = 0;
	_movementspeed=50.0;
	_nJumps = NUM_JUMPS;
	_lives = DEFAULT_LIVES;
	_speed = Ogre::Vector3(0,0,0);
	spawn();
}

Hero::~Hero(){
	delete _sNode;
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
int Hero::getLives(){
	return _lives;
}
double Hero::getMovementSpeed(){
	return _movementspeed;
}
Ogre::Vector3 Hero::getSpeed(){
	return _speed;
}


void Hero::increaseScore(int amount){
	_score = _score + amount;
}
int Hero::getNumJumps(){
	return _nJumps;
}
void Hero::setNumJumps(int nJumps){
	_nJumps = nJumps;
}
void Hero::setMovementSpeed(double speed){
	_movementspeed=speed;
}
void Hero::loseLife(){
	if(_lives >1){
		_lives--;
	}
	else{
		die();
	}
}
void Hero::setSpeed(Ogre::Vector3 speed){
	_speed = speed;
}

