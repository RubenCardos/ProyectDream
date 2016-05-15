#include "MovementManager.h"
using namespace Ogre;


MovementManager::MovementManager(Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Enemy*>* enemies){
	_sceneMgr = sceneMgr;
	_hero = hero;
	_enemies = enemies;
}

MovementManager::~MovementManager(){
	/*delete _sceneMgr;
	delete _hero;
	delete _enemies;*/
}

template<> MovementManager* Ogre::Singleton<MovementManager>::msSingleton = 0;

MovementManager* MovementManager::getSingletonPtr(void){
    return msSingleton;
}

MovementManager& MovementManager::getSingleton(void){  
    assert( msSingleton );  return ( *msSingleton );  
}

void MovementManager::moveHero(Ogre::Vector3* movement, Ogre::Real deltaT){
	//de momento, que mueva el sceneNode del Hero, nada mas
	//_hero->getSceneNode()->translate(*movement * deltaT * 20);
	_hero->getRigidBody()->setAngularVelocity(Vector3(0,0,0));
	_hero->getRigidBody()->setLinearVelocity(*movement*25);
}

void MovementManager::jumpHero(){
	//de momento, que mueva el sceneNode del Hero, nada mas
	cout << "Salto" << endl;
	
}

void MovementManager::moveEnemies(Ogre::Real deltaT){
	//mas adelante, cuando esten los enemigos hechos
}

Ogre::SceneManager* MovementManager::getSceneManager(){
	return _sceneMgr;
}

Hero* MovementManager::getHero(){
	return _hero;
}

std::vector<Enemy*>* MovementManager::getEnemies(){
	return _enemies;
}

void MovementManager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}

void MovementManager::setHero(Hero* hero){
	_hero = hero;
}

void MovementManager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
}