#include "../include/AI_Manager.h"
using namespace Ogre;

AI_Manager::AI_Manager (Hero* hero, std::vector<Enemy*>* enemies){
	_hero = hero;
	_enemies = enemies;
}

AI_Manager::~AI_Manager(){
	delete _hero;
	delete _enemies;
}

template<> AI_Manager* Ogre::Singleton<AI_Manager>::msSingleton = 0;

AI_Manager* AI_Manager::getSingletonPtr(void){
    return msSingleton;
}

AI_Manager& AI_Manager::getSingleton(void){
    assert( msSingleton );  return ( *msSingleton );  
}

Hero* AI_Manager::getHero(){
	return _hero;
}

std::vector<Enemy*>* AI_Manager::getEnemies(){
	return _enemies;
}

void AI_Manager::setHero(Hero* hero){
	_hero = hero;
}

void AI_Manager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
}

void AI_Manager::updateEnemyMovement(){
	//Aquí actualizar el Speed de cada Enemy según el tipo de enemy y el tipo de ia que tenga
	//Quizá hacer un metodo privado para cada tipo de IA de enemigo y llamarlos desde este
	//Luego mover cada enemigo con el metodo moveEnemies del movementmanager usando la nueva speed
	//de cada enemy
}
