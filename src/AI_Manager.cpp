#include "../include/AI_Manager.h"
using namespace Ogre;

#define BOSS_ROOM 100.0
#define EPSILON 0.1

AI_Manager::AI_Manager (Hero* hero, std::vector<Boss*>* bossPieces,std::vector<Enemy*>* enemies){
	_hero = hero;
	_bossPieces = bossPieces;
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
std::vector<Boss*>* AI_Manager::getBossPieces(){
	return _bossPieces;
}

void AI_Manager::setHero(Hero* hero){
	_hero = hero;
}
void AI_Manager::setEnemies(std::vector<Enemy*>* enemies){
	_enemies = enemies;
}
void AI_Manager::setBossPieces(std::vector<Boss*>* bossPieces){
	_bossPieces = bossPieces;
}

void AI_Manager::updateEnemyMovement(){
	//Aquí actualizar el Speed de cada Enemy según el tipo de enemy y el tipo de ia que tenga
	//Quizá hacer un metodo privado para cada tipo de IA de enemigo y llamarlos desde este
	//Luego mover cada enemigo con el metodo moveEnemies del movementmanager usando la nueva speed
	//de cada enemy
}

void AI_Manager::loadBossRoute(){
	//Habría que cargarlo desde un archivo. De momento, lo meto a mano
	if(_bossRoute.size() == 0){
		//Creo la ruta--------------------------
		_bossRoute.clear();
		Ogre::Vector3 routePoint(1,1,BOSS_ROOM);
		Ogre::Vector3* ptrRoutePoint = new Ogre::Vector3();
		_bossRoute.push_back(routePoint);
		routePoint.z += 10;
		_bossRoute.push_back(routePoint);
		routePoint.x -= 10;
		_bossRoute.push_back(routePoint);
		routePoint.z -= 10;
		_bossRoute.push_back(routePoint);
		//--------------------------------------

		//Inicializo los targets de cada parte del boss----------------
		for(int i=0; i<_bossPieces->size(); i++){
			*ptrRoutePoint = _bossRoute.at(i);
			_bossPieces->at(i)->setTargetPosition(ptrRoutePoint);
		}
		//----------------------------------------------------------------------------
	}

}

void AI_Manager::updateBossMovement(){
	Ogre::Vector3 pos(0,0,0);
	for(int i=0; i<_bossPieces->size(); i++){
		//Si has llegado a uno de los puntos de la ruta, te diriges al otro
		pos = _bossPieces->at(i)->getRigidBody()->getCenterOfMassPosition();
		if(std::abs(pos.x - _bossPieces->at(i)->getTargetPosition()->y) <= EPSILON){
			if(std::abs(pos.z - _bossPieces->at(i)->getTargetPosition()->z) <= EPSILON){
				if(_bossPieces->at(i)->getCurrentIndex() <= _bossRoute.size()){
					_bossPieces->at(i)->setTargetPosition(_bossPieces->at(i)->getTargetPosition()+1);
				}
				else{
					_bossPieces->at(i)->setTargetPosition(0);
				}
			}
		}
	}
}
