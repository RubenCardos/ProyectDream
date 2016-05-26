#include "PhysicsManager.h"
using namespace Ogre;

#define N_JUMPS 1
#define JUMP_EPSILON 0.01

PhysicsManager::PhysicsManager(Ogre::SceneManager* sceneMgr, OgreBulletDynamics::DynamicsWorld * world, Hero* hero, std::vector<Enemy*>* enemies){
	_sceneMgr = sceneMgr;
	_world = world;
	_hero = hero;
	_enemies = enemies;
}

PhysicsManager::~PhysicsManager(){
	/*delete _sceneMgr;
	delete _world
	delete _hero;
	delete _enemies;*/
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

		//Compruebo colisiones con el hero -----------------------------
		OgreBulletCollisions::Object *obHero = _world->findObject(_hero->getSceneNode());
		OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
		OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);

		if ((obOB_A == obHero) || (obOB_B == obHero)) {  //si uno de los objetos colisionados es el hero
			Ogre::SceneNode* node = NULL;
			if ((obOB_A != obHero) && (obOB_A)) {
				node = obOB_A->getRootNode();
				//delete obOB_A;
				if (Ogre::StringUtil::startsWith(node->getName(),"sceneThread")) { //Prueba
					node->setVisible(false); //Destruir aqui el scenenode
					// _sceneMgr->destroySceneNode(node->getName()); //Cuando destruyes scenenode peta
	    			
				}
			}
			else if ((obOB_B != obHero) && (obOB_B)) {
				node = obOB_B->getRootNode();
				//delete obOB_B;
			}
			if (node) {
				cout << "Nodo que colisiona con el hero: " << node->getName() << "\n" << endl;
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

std::vector<Enemy*>* PhysicsManager::getEnemies(){
	return _enemies;
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
