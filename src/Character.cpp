#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Character.h"

#define DEFAULT_LIVES 1

Character::Character(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape){
	_sNode = sNode;
	_rigBody = rigBody;
	_collShape = collShape;
	_lives = DEFAULT_LIVES;
}

Character::~Character(){
	delete _sNode;
	delete _collShape;
	delete _rigBody;
}

Ogre::SceneNode* Character::getSceneNode(){
	return _sNode;
}
OgreBulletCollisions::CollisionShape* Character::getCollisionShape(){
	return _collShape;
}
OgreBulletDynamics::RigidBody* Character::getRigidBody(){
	return _rigBody;
}
int Character::getLives(){
	return _lives;
}

void Character::setSceneNode(Ogre::SceneNode* sceneNode){
	_sNode = sceneNode;
}
void Character::setCollisionShape(OgreBulletCollisions::CollisionShape* collisionShape){
	_collShape = collisionShape;
}
void Character::setRigidBody(OgreBulletDynamics::RigidBody* rigidBody){
	_rigBody = rigidBody;
}
void Character::loseLife(){
	if(_lives >1){
		_lives--;
	}
	else{
		die();
	}
}