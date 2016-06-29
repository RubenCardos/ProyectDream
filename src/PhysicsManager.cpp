#include "PhysicsManager.h"
#include "PlayState.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<GameEntity*>* gameEntities, std::vector<Enemy*>* enemies, std::vector<Wall*>* walls){
	_sceneMgr = sceneMgr;
	_world = world;
	_hero = hero;
	_enemies = enemies;
	_walls = walls;
	_gameEntities = gameEntities;
}

PhysicsManager::~PhysicsManager(){
	delete _sceneMgr;
	delete _world;
	delete _hero;
	delete _enemies;
	delete _walls;
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
		btCollisionObject* aux;
		//-------------------------------------------------------------
		OgreBulletCollisions::Object *obHero = _world->findObject(_hero->getSceneNode());
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);
    	OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
         
        if((obOB_A == obHero) || (obOB_B == obHero)) {  //si uno de los objetos colisionados es el hero
			Ogre::SceneNode* node = NULL;
			if ((obOB_A != obHero) && (obOB_A)) {
				node = static_cast<Ogre::SceneNode*>(obA -> getUserPointer());
				aux=obA;
			}
			else if ((obOB_B != obHero) && (obOB_B)) {
				node = static_cast<Ogre::SceneNode*>(obB -> getUserPointer());
				aux=obB;
			}
     		if (node) {
				//cout << "Hero choca con: " << node->getName() << "\n" << endl;
				
				if(Ogre::StringUtil::startsWith(node->getName(),"SN_BossWagon")){
					//Si me choco contra un vagon atacando---
					if(_hero->isAttacking()){
						//Miro si es el ultimo vagon---------
						Boss* last= AI_Manager::getSingletonPtr()->getLastWagon();

						if(Ogre::StringUtil::match(last->getSceneNode()->getName(),node->getName()) && last->isVulnerable()){
							cout << "\nAtaco al ultimo vagon\n"<< endl;
							_hero->increaseScore(AI_Manager::getSingletonPtr()->getLastWagon()->getPoints());
							AI_Manager::getSingletonPtr()->deleteLastWagon();
							removeGameEntity(last->getSceneNode()->getName());
							//actualizar vulnerabilidad
							last = AI_Manager::getSingletonPtr()->getLastWagon();
							last->setVulnerable(false);
						}
						//-----------------------------------
					}
					//Si me choco sin atacar-----------------
					else{
						if(!_hero->isInvulnerable()){
							_hero->loseLife();
							MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,100),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
						}
					}
					//---------------------------------------
				}

				if(Ogre::StringUtil::startsWith(node->getName(),"SN_BossLocomotive")){
					//Si me choco contra un vagon atacando---
					if(_hero->isAttacking()){
						//Si puedo atacare a la locomotora---------
						Boss* _last= AI_Manager::getSingletonPtr()->getLastWagon();
						if(Ogre::StringUtil::startsWith(_last->getSceneNode()->getName(),"SN_BossLocomotive")){
							_hero->increaseScore(AI_Manager::getSingletonPtr()->getLastWagon()->getPoints());
							AI_Manager::getSingletonPtr()->deleteLastWagon();
							cout << "\nAtaco a la locomotora\n"<< endl;
							removeGameEntity(_last->getSceneNode()->getName());
						}
						//-----------------------------------------
					}
					//Si me choco sin atacar-----------------
					else{
						if(!_hero->isInvulnerable()){
							_hero->loseLife();

							MovementManager::getSingletonPtr()->repositionHero(OgreBulletCollisions::convert(calculateSpawnPoint()),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
						}
					}
					//---------------------------------------
				}

				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Thread")){
					//Eliminar SceneNode, Entity y Cuerpo Fisico asi--------------
					removeGameEntity(node->getName());
					//------------------------------------------------------------
					_hero->increaseScore(10);


					//Actualizar los puntos en la UI
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Reel")){
					_hero->getRigidBody()->setLinearVelocity(Ogre::Vector3(0,0,0));
					_hero->picksReel(node->getName());
					std::cout << "Recogida bobina " << node->getName() << std::endl;
					cout << "¿Todas las bobinas recogidas?: " << _hero->AllReelsPicked() << endl;

					Scenario::Scenario scenario = Scenario::Menu;
					_world->getBulletDynamicsWorld()->removeCollisionObject(aux);
					//cout << "CAMBIANDO AL ESCENARIO DE MENU" <<endl;
					PlayState::getSingletonPtr()->changeScenario(scenario);
				}

				else if (Ogre::StringUtil::startsWith(node->getName(),"SN_Spike") && !_hero->isInvulnerable()){
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					_hero->loseLife();
					for(int i=0 ; i< _walls->size();i++){
						//cout << "POSICION PARA Reposicionar EL MURO = " << _walls->at(i)->getSpawnPosition() << endl;
						btTransform transform = _walls->at(i)->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
						transform.setOrigin(OgreBulletCollisions::convert(_walls->at(i)->getSpawnPosition()));
						_walls->at(i)->getRigidBody()->getBulletRigidBody()->setCenterOfMassTransform(transform);
					}
					//Eliminar todos los enemigos--
					/*for(int i =0 ; i<_gameEntities->size();i++){
						if(Ogre::StringUtil::startsWith(_gameEntities->at(i)->getSceneNode()->getName(),"SN_Enemy")){
							removeGameEntity(_gameEntities->at(i)->getSceneNode()->getName());
						}
					}
					_enemies->clear();*/
					//-----------------------------
				}

				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Enemy")){
					if(!_hero->isAttacking() && !_hero->isInvulnerable()){
						_hero->loseLife();
						MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
						for(int i=0 ; i< _walls->size();i++){
							//cout << "POSICION PARA Reposicionar EL MURO = " << _walls->at(i)->getSpawnPosition() << endl;
							btTransform transform = _walls->at(i)->getRigidBody()->getBulletRigidBody()->getCenterOfMassTransform();
							transform.setOrigin(OgreBulletCollisions::convert(_walls->at(i)->getSpawnPosition()));
							_walls->at(i)->getRigidBody()->getBulletRigidBody()->setCenterOfMassTransform(transform);
						}
						//Eliminar todos los enemigos--
						for(int i =0 ; i<_gameEntities->size();i++){
							if(Ogre::StringUtil::startsWith(_gameEntities->at(i)->getSceneNode()->getName(),"SN_Enemy")){
								removeGameEntity(_gameEntities->at(i)->getSceneNode()->getName());
								i--;
							}
						}
						_enemies->clear();

						PlayState::getSingletonPtr()->printAll();

						PlayState::getSingletonPtr()->readEnemies("data/Levels/Enemies.txt");
						//-----------------------------

						_sceneMgr->getSceneNode("AureolaNode")->setVisible(false);
					}
					else if(_hero->isAttacking()){
						//Elimino el enemigo con el que te chocas-------------------
						AnimationManager::getSingletonPtr()->resetEnemyAnimation(node->getName());
						removeGameEntity(node->getName());
						//----------------------------------------------------------
						
						_sceneMgr->getSceneNode("AureolaNode")->setPosition(node->getPosition());
						_sceneMgr->getSceneNode("AureolaNode")->setVisible(true);
					}

					//Actualizar las vidas en la UI

					//Recoloco los muros---------------------------------------
					//btTransform transform = _bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> getCenterOfMassTransform();
					//transform.setOrigin(OgreBulletCollisions::convert(pos));
					//_bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> setCenterOfMassTransform(transform);

					
					//---------------------------------------------------------
				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorRoom")){
					Scenario::Scenario scenario = Scenario::LevelRoom;
					MovementManager::getSingletonPtr()->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
					_world->getBulletDynamicsWorld()->removeCollisionObject(aux);
					PlayState::getSingletonPtr()->changeScenario(scenario);

				}
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_DoorGarden")){
					Scenario::Scenario scenario = Scenario::LevelGarden;
					_world->getBulletDynamicsWorld()->removeCollisionObject(aux);
					PlayState::getSingletonPtr()->changeScenario(scenario);
				}
				//Para hacer que no se pegue a la pared
				else if(Ogre::StringUtil::startsWith(node->getName(),"SN_Wall")){
					Ogre::Vector3 vec(0,0,0);
					vec = _hero->getRigidBody()->getLinearVelocity();
					if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallL") && vec.z < 0.0){
						vec.z = 0.0;
						if(vec.y < 0.0 && vec.y > -16){
							vec.y = 1.5*vec.y;
						}
						_hero->getRigidBody()->setLinearVelocity(vec);
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallR") && vec.z > 0.0){
						vec.z = 0.0;
						if(vec.y < 0.0 && vec.y > -16){
							vec.y = 1.5*vec.y;
						}
						_hero->getRigidBody()->setLinearVelocity(vec);
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallB") && vec.x < 0.0){
						vec.x = 0;
						if(vec.y < 0.0 && vec.y > -16){
							vec.y = 1.5*vec.y;
						}
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
		btCollisionObject* aux;
		//-------------------------------------------------------------
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);
		OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);

		for(unsigned int j=0; j<_enemies->size(); j++){
			OgreBulletCollisions::Object *obEnemy = _world->findObject(_enemies->at(j)->getSceneNode());
			if ((obOB_A == obEnemy) || (obOB_B == obEnemy)) {  //si uno de los objetos colisionados es el hero
				Ogre::SceneNode* node = NULL;
				if ((obOB_A != obEnemy) && (obOB_A)) {
					node = static_cast<Ogre::SceneNode*>(obA -> getUserPointer());
					aux=obA;

				}
				else if ((obOB_B != obEnemy) && (obOB_B)) {
					node = static_cast<Ogre::SceneNode*>(obB -> getUserPointer());
					aux=obB;
				}

				if (node) {
					if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallL")){
						//std::cout << "	EL ENEMIGO HA CHOCADO CON UN WALL L" << std::endl;
						vel = _enemies->at(j)->getSpeed();
						vel.z = std::abs(vel.z);
						_enemies->at(j)->setSpeed(vel);
						//std::cout << "	la velocidad es " << _enemies->at(j)->getSpeed() << std::endl;
					}
					else if(Ogre::StringUtil::startsWith(node->getName(),"SN_WallR")){
						//std::cout << "	EL ENEMIGO HA CHOCADO CON UN WALL R" << std::endl;
						vel = _enemies->at(j)->getSpeed();
						vel.z = - std::abs(vel.z);
						_enemies->at(j)->setSpeed(vel);
						//std::cout << "	la velocidad es " << _enemies->at(j)->getSpeed() << std::endl;

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
void PhysicsManager::setWalls(std::vector<Wall*>* walls){
	_walls = walls;
}

void PhysicsManager::setGameEntities(std::vector<GameEntity*>* gameEntities){
	_gameEntities = gameEntities;
}

void PhysicsManager::setWorld(OgreBulletDynamics::DynamicsWorld * world){
	_world=world;
}

void PhysicsManager::removeGameEntity( std::string name){
	cout << "GAME ENTITY A ELIMINAR=" << name << endl;
	for(unsigned int i=0; i<_gameEntities->size(); i++){
		if(Ogre::StringUtil::match(_gameEntities->at(i)->getSceneNode()->getName(),name)){
			Entity* _e = static_cast<Entity*>(_gameEntities->at(i)->getSceneNode()->getAttachedObject(0));//Recupero la entidad
			OgreBulletCollisions::Object* Baux =_world->findObject(_gameEntities->at(i)->getSceneNode());
			_world->getBulletDynamicsWorld()->removeCollisionObject(Baux->getBulletObject());
			_sceneMgr->destroyEntity(_e);
			_sceneMgr->getRootSceneNode()->removeChild(_gameEntities->at(i)->getSceneNode());
			_gameEntities->erase(_gameEntities->begin() + i);
			cout << "GameEntity eliminada="<< name <<endl;
		}
	}

}

Vector3 PhysicsManager::calculateSpawnPoint(){
	Vector3 res = Vector3(0,0,0);
	Vector3 speed = *(AI_Manager::getSingletonPtr()->getLastWagon()->getVSpeed());
	Vector3 pos = AI_Manager::getSingletonPtr()->getLastWagon()->getRigidBody()->getCenterOfMassPosition();
	Vector3 per = Vector3(0,0,0);
	if(speed.z !=0){
		per = Vector3::UNIT_X*50;
		res=per+pos;
	}
	else if(speed.x !=0){
		per = Vector3::UNIT_Z*50;
		res=per+pos;
	}

	if(res.x > 100){
		res.x = res.x - 50;
	}
	if(res.x < -100){
		res.x = res.x + 50;
	}
	if(res.z > 200){
		res.z = res.z - 50;
	}
	if(res.x < 0){
		res.z = res.z + 50;
	}

	return res;
}
