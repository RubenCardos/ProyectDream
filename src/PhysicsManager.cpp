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
	delete _sceneMgr;
	delete _world;
	delete _hero;
	delete _gameEntities;
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
		btPersistentManifold* contactManifold = bulletWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		//EXTRA--------------------------------------------------------
		btCollisionObject* _aux;
		//-------------------------------------------------------------
		OgreBulletCollisions::Object *obHero = _world->findObject(_hero->getSceneNode());
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);
    	OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
         
        if ((obOB_A == obHero) || (obOB_B == obHero)) {  //si uno de los objetos colisionados es el hero
			Ogre::SceneNode* node = NULL;
			if ((obOB_A != obHero) && (obOB_A)) {
				node = static_cast<Ogre::SceneNode*>(obA -> getUserPointer());
				_aux=obA;

				}
			else if ((obOB_B != obHero) && (obOB_B)) {
				node = static_cast<Ogre::SceneNode*>(obB -> getUserPointer());
				_aux=obB;
				}
			

     		if (node) {
				//cout << "Hero choca con: " << node->getName() << "\n" << endl;
				
				/*if(Ogre::StringUtil::startsWith(node->getName(),"SN_Floor")){
					if(_hero->getRigidBody()->getLinearVelocity().y < 0.0){
						if(_hero->getNumJumps() < N_JUMPS){
							_hero->setNumJumps(N_JUMPS);
						}
					}
				}*/

				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Ground")){
					if(_hero->getRigidBody()->getLinearVelocity().y < 0.0){
						if(_hero->getNumJumps() < N_JUMPS){
							_hero->setNumJumps(N_JUMPS);
						}
					}
				}	

				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Thread")){
					//Eliminar SceneNode, Entity y Cuerpo Fisico asi--------------
					Entity* _e = static_cast<Entity*>(node->getAttachedObject(0));
					_sceneMgr->destroyEntity(_e);
					_sceneMgr->getRootSceneNode()->removeChild(node);
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					//------------------------------------------------------------
					_hero->increaseScore(10);
					//Actualizar los puntos en la UI
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Reel")){
					//PlayState::getSingletonPtr()->changeScenarioQ();
					Scenario scenario = Menu;
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					PlayState::getSingletonPtr()->changeScenarioQ(scenario);
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Enemy")){
					_hero->loseLife();
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					//Actualizar las vidas en la UI
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorRoom")){
					Scenario scenario = LevelRoom;
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					PlayState::getSingletonPtr()->changeScenarioQ(scenario);

				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorGarden")){
					Scenario scenario = LevelGarden;
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					PlayState::getSingletonPtr()->changeScenarioQ(scenario);
				}
				//Para hacer que no se pegue a la pared
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Wall")){
					Ogre::Vector3 vec(0,0,0);
					vec = _hero->getRigidBody()->getLinearVelocity();
					if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallL") && vec.z < 0.0){
						vec.z = 0.0;
						_hero->getRigidBody()->setLinearVelocity(vec);
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallR") && vec.z > 0.0){
						vec.z = 0.0;
						_hero->getRigidBody()->setLinearVelocity(vec);
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallB") && vec.x < 0.0){
						vec.x = 0;
						_hero->getRigidBody()->setLinearVelocity(vec);
					}
				}
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
	if(index < _gameEntities->size() && index >= 0){
		std::string sAux = _gameEntities->at(index)->getSceneNode()->getName();
		Entity* entity = static_cast<Entity*>(_gameEntities->at(index)->getSceneNode()->getAttachedObject(0));
		//remove entity
		_sceneMgr->destroyEntity(entity);
		//remove sceneNode
		_sceneMgr->getSceneNode(sAux)->removeAndDestroyAllChildren();
		_sceneMgr->destroySceneNode(sAux);
		//remove rigidbody

		//remove GameEntity
		delete _gameEntities->at(index);
		_gameEntities->erase(_gameEntities->begin()+index);
		//delete gameEntity;
	}
	else{
		std::cout << "No existe una GameEntity con ese indice"<< std::endl;
	}
}

void PhysicsManager::removeGameEntity(std::string name){
	GameEntity* gameEntity = new GameEntity();
	int index = 0;
	bool found = false;

	for(std::vector<GameEntity*>::iterator it = _gameEntities->begin(); it != _gameEntities->end(); ++it) {
		if(gameEntity->getSceneNode()->getName().compare(name) != 0){
			index++;
		}
		else{
			found = true;
			break;
		}
	}

	if(found){
		std::string sAux = _gameEntities->at(index)->getSceneNode()->getName();
		Entity* entity = static_cast<Entity*>(_gameEntities->at(index)->getSceneNode()->getAttachedObject(0));
		//remove entity
		_sceneMgr->destroyEntity(entity);
		//remove sceneNode
		_sceneMgr->getSceneNode(sAux)->removeAndDestroyAllChildren();
		_sceneMgr->destroySceneNode(sAux);
		//remove rigidbody

		//remove GameEntity
		delete _gameEntities->at(index);
		_gameEntities->erase(_gameEntities->begin()+index);
		//delete gameEntity;
	}

}

