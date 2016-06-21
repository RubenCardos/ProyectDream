#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Hero.h"

#define INITIAL_LIVES 3
#define NUM_JUMPS 1
#define INVULNERABILITY_TIME 3.0

Hero::Hero(){
	_lives = INITIAL_LIVES;
	_score = 0;
	_movementspeed=50.0;
	_nJumps = NUM_JUMPS;
	_speed = Ogre::Vector3(0,0,0);
	_pickedReels[0] = false;
	_pickedReels[1] = false;
}

Hero::Hero(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody){
	_sNode = sNode;
	_rigBody = rigBody;
	_lives = INITIAL_LIVES;
	_score = 0;
	_movementspeed=50.0;
	_nJumps = NUM_JUMPS;
	_speed = Ogre::Vector3(0,0,0);
	_attacking = false;
	_pickedReels[0] = false;
	_pickedReels[1] = false;
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
bool Hero::isAttacking(){
	return _attacking;
}

void Hero::increaseScore(int amount){
	_score = _score + amount;
}
int Hero::getNumJumps(){
	return _nJumps;
}
bool Hero::isInvulnerable(){
	bool invulnerable = false;
	if(_invulnerabilityLeft > 0.0){
		invulnerable = true;
	}
	return invulnerable;
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
		cout << "Hero pierde una vida " << " vida actual: " << _lives << endl;
	}
	else{
		cout << "Hero muere " << " vida actual: " << _lives << endl;
		die();
	}
}
void Hero::setSpeed(Ogre::Vector3 speed){
	_speed = speed;
}

void Hero::resetScore(){
	_score = 0;
}

void Hero::resetLives(){
	_lives = INITIAL_LIVES;
}

bool Hero:: AllReelsPicked(){
	bool picked = false;
	if(_pickedReels[0] && _pickedReels[1]){
		picked = true;
	}
	return picked;
}
void Hero::picksReel(string reelName){
	if(Ogre::StringUtil::startsWith(reelName, "SN_ReelGarden")){
		_pickedReels[0] = true;
	}
	else if(Ogre::StringUtil::startsWith(reelName, "SN_ReelRoom")){
		_pickedReels[1] = true;
	}
}
void Hero::resetPickedReels(){
	_pickedReels[0] = false;
	_pickedReels[1] = false;
}

void Hero::makeInvulnerable(){
	_invulnerabilityLeft = INVULNERABILITY_TIME;
}
void Hero::UpdateInvulnerability(Ogre::Real deltaT){
	if((_invulnerabilityLeft - deltaT) > 0){
		_invulnerabilityLeft = _invulnerabilityLeft - deltaT;
	}
	else{
		_invulnerabilityLeft = 0.0;
	}
}

void Hero::setAttacking(bool attacking){
	_attacking = attacking;
}
