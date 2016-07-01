#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Wall.h"

Wall::Wall(Ogre::SceneNode* sNode, OgreBulletDynamics::RigidBody* rigBody, WallType type){
	_sNode = sNode;
	_rigBody = rigBody;
	_type = type;
}

Wall::~Wall(){
	delete _sNode;
	delete _rigBody;
}

WallType Wall::getType(){
	return _type;
}
void Wall::setType(WallType type){
	_type = type;
}

Ogre::Vector3 Wall::getSpawnPosition(){
	return _spawnPosition;
}

void Wall::setSpawnPosition(Vector3 New_Position){
	_spawnPosition = New_Position;
}
