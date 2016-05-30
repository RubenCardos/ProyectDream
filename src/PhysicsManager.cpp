#include "PhysicsManager.h"
#include "PlayState.h"
#include "MovementManager.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<GameEntity*>* gameEntities){
	_sceneMgr = sceneMgr;
	_world = world;
	_hero = hero;
	_gameEntities = gameEntities;
}

PhysicsManager::~PhysicsManager(){
	/*delete _sceneMgr;
	delete _world
	delete _hero;
	delete _gameEntities;*/
}

template<> PhysicsManager* Ogre::Singleton<PhysicsManager>::msSingleton = 0;

PhysicsManager* PhysicsManager::getSingletonPtr(void){
    return msSingleton;
}

PhysicsManager& PhysicsManager::getSingleton(void){
    assert( msSingleton );  return ( *msSingleton );  
}

void PhysicsManager::detectHeroCollision(){
	//Colisiones------------------------------
	btCollisionWorld *bulletWorld = _world->getBulletCollisionWorld();
	int numManifolds = bulletWorld->getDispatcher()->getNumManifolds();

	for (int i=0;i<numManifolds;i++) {
		btPersistentManifold* contactManifold =
				bulletWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA =
				(btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB =
				(btCollisionObject*)(contactManifold->getBody1());

		//EXTRA--------------------------------------------------------
		btCollisionObject* _aux;
		//-------------------------------------------------------------

		//Compruebo colisiones con el hero -----------------------------
		OgreBulletCollisions::Object *obHero = _world->findObject(_hero->getSceneNode());
		OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);

		if ((obOB_A == obHero) || (obOB_B == obHero)) {  //si uno de los objetos colisionados es el hero
			Ogre::SceneNode* node = NULL;
			if ((obOB_A != obHero) && (obOB_A)) {
				node = obOB_A->getRootNode();
				_aux=obA;
				
			}
			else if ((obOB_B != obHero) && (obOB_B)) {
				node = obOB_B->getRootNode();
				//delete obOB_B;
				_aux=obB;
			}
			if (node) {
				//cout << "Nodo que colisiona con el hero: " << node->getName() << "\n" << endl;

				//Colision con el suelo nuevo--
				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Floor")){
					cout << "Colision con el suelo" << "\n" << endl;
				}
				//-----------------------------

				//Colision con paredes nuevas--
				if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallR")){
					cout << "Colision derecha" << "\n" << endl;
				}
				//----------------------------

				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Thread")){

					//Eliminar SceneNode, Entity y Cuerpo Fisico asi--------------
					Entity* _e = static_cast<Entity*>(node->getAttachedObject(0));
					_sceneMgr->destroyEntity(_e);
					_sceneMgr->getRootSceneNode()->removeChild(node);
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					//Sumo puntuacion---
					_hero->increaseScore(10);
					cout << "Puntuacion : " << _hero->getScore() << endl;
					//------------------
					//-------------------------------------------------------------
				}

				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Reel")){
					PlayState::getSingletonPtr()->changeScenarioQ();
				}
				//Colision con enemigo------------------------------------------
				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Enemy")){
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					_hero->loseLife();
					cout << "Vidas : " << _hero->getLives() << endl;
				}
				//--------------------------------------------------------------
			}

		}
		//------------------------------------------------
	}
}
//----------------------------------------

Ogre::SceneManager* PhysicsManager::getSceneManager(){
	return _sceneMgr;
}

Hero* PhysicsManager::getHero(){
	return _hero;
}

std::vector<GameEntity*>* PhysicsManager::getGameEntities(){
	return _gameEntities;
}

void PhysicsManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void PhysicsManager::setHero(Hero* hero){
	_hero = hero;
}

void PhysicsManager::setGameEntities(std::vector<GameEntity*>* gameEntities){
	_gameEntities = gameEntities;
}
void PhysicsManager::removeGameEntity(unsigned int index){
	//GameEntity* gameEntity = new GameEntity();
	//gameEntity = _gameEntities->at(index);
	std::string sAux = _gameEntities->at(index)->getSceneNode()->getName();
	Entity* entity = static_cast<Entity*>(_gameEntities->at(index)->getSceneNode()->getAttachedObject(0));
	//remove entity
	_sceneMgr->destroyEntity(entity);
	//remove sceneNode
	_sceneMgr->getSceneNode(sAux)->removeAndDestroyAllChildren();
	_sceneMgr->destroySceneNode(sAux);
	//remove GameEntity
	delete _gameEntities->at(index);
	_gameEntities->erase(_gameEntities->begin()+index);
	//delete gameEntity;


}
