#include "../include/AI_Manager.h"
using namespace Ogre;

#define BOSS_ROOM 100.0
#define EPSILON 0.1
#define FLOOR_POSITION_Y -2.8

AI_Manager::AI_Manager (Ogre::SceneManager* sceneMgr, Hero* hero, std::vector<Boss*>* bossPieces,std::vector<Enemy*>* enemies){
	_hero = hero;
	_bossPieces = bossPieces;
	_enemies = enemies;
	_sceneMgr = sceneMgr;
}

AI_Manager::~AI_Manager(){
	delete _hero;
	delete _enemies;
	delete _bossPieces;
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
void AI_Manager::setSceneManager(Ogre::SceneManager* sceneMgr){
	_sceneMgr = sceneMgr;
}
std::vector<Ogre::Vector3>* AI_Manager::getBossRoute(){
	return &_bossRoute;
}
void AI_Manager::updateEnemyMovement(){
	//Aquí actualizar el Speed de cada Enemy según el tipo de enemy y el tipo de ia que tenga
	//Quizá hacer un metodo privado para cada tipo de IA de enemigo y llamarlos desde este
	//Luego mover cada enemigo con el metodo moveEnemies del movementmanager usando la nueva speed
	//de cada enemy


}

void AI_Manager::loadBossRoute(){
	//Habría que cargarlo desde un archivo. De momento, lo meto a mano
	double centerOfMass_y = 0.0;
	centerOfMass_y = _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().y;

	if(_bossRoute.size() == 0){
		//Creo la ruta--------------------------
		_bossRoute.clear();
		/*Ogre::Vector3 routePoint(1,centerOfMass_y,BOSS_ROOM);
		_bossRoute.push_back(routePoint);
		routePoint.z += 10;
		_bossRoute.push_back(routePoint);
		routePoint.x -= 10;
		_bossRoute.push_back(routePoint);
		routePoint.z -= 10;
		_bossRoute.push_back(routePoint);*/
		//--------------------------------------

		std::cout << "tamaño del vector de ruta: " << _bossRoute.size() << std::endl;
		std::cout << "tamaño del vector de piezas de boss: " << _bossPieces->size() << std::endl;
		//std::cout << "puntos de ruta metidos en el vector: " << std::endl;

		/*for(int i=0; i<_bossRoute.size(); i++){
			std::cout << "Punto " <<_bossRoute.at(i) << std::endl;
		}*/
	}
}

void AI_Manager::updateBossMovement(){
	/*Ogre::Vector3 posPiece(0,0,0);
	Ogre::Vector3 posRoutePoint(0,0,0);

	for(unsigned int i=0; i<_bossPieces->size(); i++){
		//Si has llegado a uno de los puntos de la ruta, te diriges al otro
		posPiece = _bossPieces->at(i)->getRigidBody()->getCenterOfMassPosition();
		posRoutePoint = _bossRoute.at(_bossPieces->at(i)->getCurrentIndex() + 1);
		if(posPiece.distance(posRoutePoint) <= EPSILON){
			if(_bossPieces->at(i)->getCurrentIndex() < (_bossRoute.size() - 1)){
				_bossPieces->at(i)->setCurrentIndex(_bossPieces->at(i)->getCurrentIndex() + 1);
				_bossPieces->at(i)->setTargetPosition(&(_bossRoute.at(_bossPieces->at(i)->getCurrentIndex())));
			}
			else{
				//Vuelvo a iniciar la ruta desde el primer punto
				_bossPieces->at(i)->setTargetPosition(&(_bossRoute.at(0)));
				_bossPieces->at(i)->setCurrentIndex(-1);
			}
		}
	}*/
	cout << "	posTren" << _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition()<< endl;
	cout << "	target" << _bossRoute.at(1) << endl;
	cout << "	distancia" << _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().distance(_bossRoute.at(1)) << endl;

	for(unsigned int i=0; i<_bossPieces->size(); i++){
		/*if(std::abs(_bossPieces->at(i)->getRigidBody()->getCenterOfMassPosition().x - _bossRoute.at(1).x) <= EPSILON){
			if(std::abs(_bossPieces->at(i)->getRigidBody()->getCenterOfMassPosition().z - _bossRoute.at(1).z) <= EPSILON){
				//lo llevo otra vez al punto inicial
				//_bossPieces->at(i)->getRigidBody()->setPosition(_bossRoute.at(i));
				_bossPieces->at(i)->getRigidBody()->setLinearVelocity(Ogre::Vector3(0,0,0));
			}
		}*/
		if(_bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().distance(_bossRoute.at(1)) <= EPSILON){
			//lo llevo otra vez al punto inicial
			//_bossPieces->at(i)->getRigidBody()->setPosition(_bossRoute.at(i));
			cout << "He llegado a mi destino" <<endl;
			//_bossPieces->at(i)->getRigidBody()->setLinearVelocity(Ogre::Vector3(0,0,0));
			_bossPieces->at(i)->setV_Speed(new Ogre::Vector3(0,0,0));
		}
	}
}

void AI_Manager::initializeBossMovement(Ogre::Real* deltaT){
		Ogre::Vector3 entryPosition(0,0, BOSS_ROOM -10);
		Ogre::Vector3 exitPosition(25,0, BOSS_ROOM -10);
		Ogre::Vector3 speed(0,0,0);
		Ogre::Vector3* ptrSpeed = new Ogre::Vector3(0,0,0);

		_bossRoute.clear();
		_bossRoute.push_back(entryPosition);
		_bossRoute.push_back(exitPosition);

		speed = exitPosition - entryPosition;
		speed = speed.normalisedCopy();
		speed= speed*3;
		*ptrSpeed = speed;

		std::cout << "VELOCIDAD DE LA LOCOMOTORA" << speed << std::endl;
		std::cout << "VELOCIDAD DE LA LOCOMOTORA PTR*" << *ptrSpeed << std::endl;

		std::cout << "ANTES DE REPOSICIONAR" << std::endl;
		for(unsigned int i=0; i<_bossPieces->size(); i++){
			_bossPieces->at(i)->getRigidBody()->setPosition(entryPosition);
			_bossPieces->at(i)->setV_Speed(ptrSpeed);
			entryPosition.z = entryPosition.z - WAGON_DISTANCE;
		}
		std::cout << "DESPUES DE REPOSICIONAR" << std::endl;

	/*if(_bossRoute.size() != 0){
		Ogre::Vector3* ptrPosition = new Ogre::Vector3();
		ptrPosition = &_bossRoute.at(0);
		Ogre::Vector3* ptrSpeed = new Ogre::Vector3();
		Ogre::Vector3 speed(0,0,0);

		//Inicializo los targets de cada parte del boss----------------
		for(unsigned int i=0; i<_bossPieces->size(); i++){
			_bossPieces->at(i)->setTargetPosition(ptrPosition);
			_bossPieces->at(i)->setCurrentIndex(-1);
		}
		//----------------------------------------------------------------------------

		//inicializo el vector de velocidad de la locomotora y los vagones-------------------------
		//calculo el vector unitario de velocidad de la locomotora (que va a ser el que use el resto, de momento)
		speed = _bossRoute.at(0) - _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition();
		speed = speed.normalisedCopy();
		ptrSpeed = &speed;


		cout << "IMPRIMIENDO DATOS DE LOCOMOTORA" << endl;
		cout << "	target = " << *(_bossPieces->at(0)->getTargetPosition()) << endl;
		cout << "	t_bossRoute(0) = " << _bossRoute.at(0) << endl;
		std::cout << "	speed " << speed << " ptrSpeed " << *ptrSpeed << endl;

		if(_bossRoute.size() > 0){
			for(unsigned int i=0; i<_bossPieces->size();i++){
				_bossPieces->at(i)->setV_Speed(ptrSpeed);
			}
		}
		//---------------------------------------------------------------------------
	}*/
}
