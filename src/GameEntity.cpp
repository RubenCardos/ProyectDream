#include <GameEntity.h>
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

GameEntity::GameEntity(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody){
	_sNode = sNode;
	_rigBody = rigBody;
}

GameEntity::~GameEntity(){
	delete _sNode;
	delete _rigBody;
}

Ogre::SceneNode* GameEntity::getSceneNode(){
	return _sNode;
}

OgreBulletDynamics::RigidBody* GameEntity::getRigidBody(){
	return _rigBody;
}

void GameEntity::setSceneNode(Ogre::SceneNode* sceneNode){
	_sNode = sceneNode;
}

void GameEntity::setRigidBody(OgreBulletDynamics::RigidBody* rigidBody){
	_rigBody = rigidBody;
}
