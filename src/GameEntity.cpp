#include <GameEntity.h>
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

GameEntity::GameEntity(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, OgreBulletCollisions::CollisionShape* collShape){
	_sNode = sNode;
	_rigBody = rigBody;
	_collShape = collShape;
}

GameEntity::~GameEntity(){
	delete _sNode;
	delete _collShape;
	delete _rigBody;
}

Ogre::SceneNode* GameEntity::getSceneNode(){
	return _sNode;
}
OgreBulletCollisions::CollisionShape* GameEntity::getCollisionShape(){
	return _collShape;
}
OgreBulletDynamics::RigidBody* GameEntity::getRigidBody(){
	return _rigBody;
}

void GameEntity::setSceneNode(Ogre::SceneNode* sceneNode){
	_sNode = sceneNode;
}
void GameEntity::setCollisionShape(OgreBulletCollisions::CollisionShape* collisionShape){
	_collShape = collisionShape;
}
void GameEntity::setRigidBody(OgreBulletDynamics::RigidBody* rigidBody){
	_rigBody = rigidBody;
}
