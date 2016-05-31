#include "Shapes/OgreBulletCollisionsTrimeshShape.h"  
#include "Shapes/OgreBulletCollisionsSphereShape.h" 
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "OgreBulletCollisionsRay.h"

#include "Wall.h"

#define INITIAL_LIVES 1
#define NUM_JUMPS 1

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
