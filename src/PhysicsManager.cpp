#include "PhysicsManager.h"
#include "PlayState.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<GameEntity*>* gameEntities, std::vector<Enemy*>* enemies){
	_sceneMgr = sceneMgr;
	_world = world;
	_hero = hero;
	_enemies = enemies;
	_gameEntities = gameEntities;
}

PhysicsManager::~PhysicsManager(){
	delete _sceneMgr;
	delete _world;
	delete _hero;
	delete _enemies;
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
				cout << "Hero choca con: " << node->getName() << "\n" << endl;
				
				/*if(Ogre::StringUtil::startsWith(node->getName(),"SN_Floor")){
					if(_hero->getRigidBody()->getLinearVelocity().y < 0.0){
						if(_hero->getNumJumps() < N_JUMPS){
							_hero->setNumJumps(N_JUMPS);
						}
					}
				}*/

				if(Ogre::StringUtil::startsWith(node->getName(),"SN_Ground")){
					/*if(_hero->getRigidBody()->getLinearVelocity().y < 0.0){
						if(_hero->getNumJumps() < N_JUMPS){
							_hero->setNumJumps(N_JUMPS);
						}
					}*/
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
					_hero->setNReel(1);
					cout << "\n\nNum Bobinas recogidas: " <<_hero->getNReel() << endl;
					if(_hero->getNReel()==2){
						cout << "\n\nAqui pasaria al boss\n\n" << endl;
					}
					Scenario::Scenario scenario = Scenario::Menu;
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					cout << "CAMBIANDO AL ESCENARIO DE MENU" <<endl;
					PlayState::getSingletonPtr()->changeScenarioQ(scenario);
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Enemy")){
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					_hero->loseLife();
					//Elimino el enemigo con el que te chocas-------------------
					for(unsigned int i=0; i<_gameEntities->size(); i++){
						if(Ogre::StringUtil::startsWith(_gameEntities->at(i)->getSceneNode()->getName(),"SN_Enemy")){
							Entity* _e = static_cast<Entity*>(_gameEntities->at(i)->getSceneNode()->getAttachedObject(0));//Recupero la entidad
							OgreBulletCollisions::Object* Baux =_world->findObject(_aux);
							_world->getBulletDynamicsWorld()->removeCollisionObject(Baux->getBulletObject());
							_sceneMgr->destroyEntity(_e);
							_sceneMgr->getRootSceneNode()->removeChild(_gameEntities->at(i)->getSceneNode());
							_gameEntities->erase(_gameEntities->begin() + i);
						}
					}
					//----------------------------------------------------------
					//Actualizar las vidas en la UI
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorRoom")){
					Scenario::Scenario scenario = Scenario::LevelRoom;
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					_world->getBulletDynamicsWorld()->removeCollisionObject(_aux);
					PlayState::getSingletonPtr()->changeScenarioQ(scenario);

				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorGarden")){
					Scenario::Scenario scenario = Scenario::LevelGarden;
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

void PhysicsManager::detectEnemiesCollision(){
	btCollisionWorld *bulletWorld = _world->getBulletCollisionWorld();
	int numManifolds = bulletWorld->getDispatcher()->getNumManifolds();

	Ogre::Vector3 vel(0,0,0);

	for (int i=0;i<numManifolds;i++) {
		btPersistentManifold* contactManifold = bulletWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		//EXTRA--------------------------------------------------------
		btCollisionObject* _aux;
		//-------------------------------------------------------------
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);
		OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);

		for(unsigned int j=0; j<_enemies->size(); j++){
			OgreBulletCollisions::Object *obEnemy = _world->findObject(_enemies->at(j)->getSceneNode());
			if ((obOB_A == obEnemy) || (obOB_B == obEnemy)) {  //si uno de los objetos colisionados es el hero
				Ogre::SceneNode* node = NULL;
				if ((obOB_A != obEnemy) && (obOB_A)) {
					node = static_cast<Ogre::SceneNode*>(obA -> getUserPointer());
					_aux=obA;

				}
				else if ((obOB_B != obEnemy) && (obOB_B)) {
					node = static_cast<Ogre::SceneNode*>(obB -> getUserPointer());
					_aux=obB;
				}

				if (node) {
					if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallL")){
						std::cout << "	EL ENEMIGO HA CHOCADO CON UN WALL L" << std::endl;
						vel = _enemies->at(j)->getSpeed();
						vel.z = std::abs(vel.z);
						_enemies->at(j)->setSpeed(vel);
						std::cout << "	la velocidad es " << _enemies->at(j)->getSpeed() << std::endl;
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallR")){
						std::cout << "	EL ENEMIGO HA CHOCADO CON UN WALL R" << std::endl;
						vel = _enemies->at(j)->getSpeed();
						vel.z = - std::abs(vel.z);
						_enemies->at(j)->setSpeed(vel);
						std::cout << "	la velocidad es " << _enemies->at(j)->getSpeed() << std::endl;

					}
				}
			}
		}
	}
}

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
void PhysicsManager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
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

