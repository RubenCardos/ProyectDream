#include "../include/AI_Manager.h"
using namespace Ogre;

#define BOSS_ROOM 100.0
#define EPSILON 0.3 //Se va a 0.27
#define FLOOR_POSITION_Y -2.8

AI_Manager::AI_Manager (Ogre::SceneManager* sceneMgr , OgreBulletDynamics::DynamicsWorld * world , Hero* hero, std::vector<Boss*>* bossPieces,std::vector<Enemy*>* enemies){
	_hero = hero;
	_bossPieces = bossPieces;
	_enemies = enemies;
	_sceneMgr = sceneMgr;
	_world = world;
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
	_bossRoute.clear();

	//Cargamos la ruta por un fichero----------
	fstream fichero;//Fichero
	string frase;//Auxiliar
	fichero.open("data/BossRoute.txt",ios::in);
	if (fichero.is_open()) {
		while (getline (fichero,frase)) {
			int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
			//int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
			int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);

			//Vector3 routePoint = Vector3(x,y,z);

			Vector3 routePoint = Vector3(x,3.42,z);//Habria que calcular el Y pero no se me ocurre como.
			_bossRoute.push_back(routePoint);

		}
		fichero.close();
	}
}

void AI_Manager::updateBossMovement(){
	cout << "	posTren = " << _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition()<< endl;
	//cout << "	target = " << *_bossPieces->at(0)->getTargetPosition() << endl;
	cout << "	distancia =" <<  _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().distance(*_bossPieces->at(0)->getTargetPosition()) << endl;

	if(_bossPieces->size()>0){
		if(_bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().distance(*_bossPieces->at(0)->getTargetPosition()) < EPSILON){
			//lo llevo otra vez al punto inicial
			//_bossPieces->at(i)->getRigidBody()->setPosition(_bossRoute.at(i));
			cout << "He llegado a mi destino "<< _bossPieces->at(0)->getCurrentIndex() <<endl;

			//Compruebo nuevo indice--------------------------------
			int New_Index=_bossPieces->at(0)->getCurrentIndex()+2;
			if(New_Index>=_bossRoute.size()){
				New_Index=0;
			}
			//----------------------------------------------------

			//Aumento index-----------
			for(int i = 0;i<_bossPieces->size();i++){
				_bossPieces->at(0)->setCurrentIndex(New_Index);
				cout << "Actualizo Indice = "<< _bossPieces->at(0)->getCurrentIndex() <<endl;
			}
			//------------------------

			//Reposicionar----------------------------------------------------------------------------------------------------
			Vector3 pos= _bossRoute.at(_bossPieces->at(0)->getCurrentIndex());//Nuevo Origen

			Ogre::Vector3 speed(0,0,0);
			Ogre::Vector3* ptrSpeed = new Ogre::Vector3(0,0,0);

			speed = _bossRoute.at(_bossPieces->at(0)->getCurrentIndex()+1) - _bossRoute.at(_bossPieces->at(0)->getCurrentIndex());
			speed = speed.normalisedCopy();
			speed= speed*15;
			*ptrSpeed = speed;

			Ogre::Vector3 desp(0,0,0);
			int degrees = 0;

			if(speed.x >0){
				desp.x = -1;
				degrees = 90;
			}
			else if(speed.x <0){
				desp.x = 1;
				degrees = 270;
			}
			else if(speed.z >0){
				desp.z = -1;
				degrees = 0;

			}
			else if(speed.z <0){
				desp.z = 1;
				degrees = 180;

			}
			Ogre::Quaternion rot(Ogre::Degree(degrees),Ogre::Vector3::UNIT_Y);

			for(unsigned int i=0; i<_bossPieces->size(); i++){
				//pos.x -= 10*i;
				if(i==0){
					//desp = desp * 3;
				}
				if(i==1){
					//desp = desp /3;
					desp = desp * 11;
				}

				pos = pos + desp*i;
				btTransform transform = _bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> getCenterOfMassTransform();
				transform.setOrigin(OgreBulletCollisions::convert(pos));
				//Girar---------------------------------------
				transform.setRotation(OgreBulletCollisions::convert(rot));
				//--------------------------------------------
				_bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> setCenterOfMassTransform(transform);

			}
			//--------------------------------------------------------------------------------------------------------------------

			//Calcular nuevo desplazamiento y establecer nuevo destino---

			for(unsigned int i=0; i<_bossPieces->size(); i++){
				//_bossPieces->at(i)->getRigidBody()->setPosition(entryPosition);
				_bossPieces->at(i)->setV_Speed(ptrSpeed);
				Vector3* _aux = &_bossRoute.at(_bossPieces->at(0)->getCurrentIndex()+1);
				cout << "AUX = "<< *_aux <<endl;
				_bossPieces->at(i)->setTargetPosition(_aux);
			}
			cout << "===INFO=== " <<endl;
			cout << "Indice = "<< _bossPieces->at(0)->getCurrentIndex() <<endl;
			cout << "Estoy en = "<< _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition() <<endl;
			cout << "Voy a = "<< *_bossPieces->at(0)->getTargetPosition() <<endl;
			//-----------------------------------------------------------
			//--------------------------
		}
	}
}

void AI_Manager::initializeBossMovement(Ogre::Real* deltaT){
		Ogre::Vector3 speed(0,0,0);
		Ogre::Vector3* ptrSpeed = new Ogre::Vector3(0,0,0);

		//Cargo la ruta---
		loadBossRoute();
		//----------------

		//speed = exitPosition - entryPosition;
		speed =_bossRoute.at(1)-_bossRoute.at(0);
		speed = speed.normalisedCopy();
		speed= speed*15;
		*ptrSpeed = speed;

		std::cout << "VELOCIDAD DE LA LOCOMOTORA" << speed << std::endl;
		std::cout << "VELOCIDAD DE LA LOCOMOTORA PTR*" << *ptrSpeed << std::endl;

		std::cout << "ANTES DE REPOSICIONAR" << std::endl;

		Vector3 pos = _bossRoute.at(0);
		int degrees = 0;

		//Cambio la rotacion donde aparece el siguiente modulo---
		if(speed.x >0){
			degrees=90;
		}
		else if(speed.x <0){
			degrees=270;
		}
		else if(speed.z >0){
			degrees=0;
		}
		else if(speed.z <0){
			degrees=180;
		}
		//--------------------------------------------------------

		for(unsigned int i=0; i<_bossPieces->size(); i++){
			_bossPieces->at(i)->setV_Speed(ptrSpeed);
			_bossPieces->at(i)->setCurrentIndex(0);
			Vector3* aux = new Vector3(0,0,0);
			aux=&_bossRoute.at(_bossPieces->at(i)->getCurrentIndex()+1);
			cout << "EXIT = " << *aux << endl;
 			_bossPieces->at(i)->setTargetPosition(aux);

			btTransform transform = _bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> getCenterOfMassTransform();
			transform.setOrigin(OgreBulletCollisions::convert(pos));
			//Girar---------------------------------------
			transform.setRotation(OgreBulletCollisions::convert(Quaternion(Degree(degrees),Vector3::UNIT_Y)));
			//--------------------------------------------
			_bossPieces->at(i)->getRigidBody()->getBulletRigidBody() -> setCenterOfMassTransform(transform);

			//Cambio la posicion donde aparece el siguiente modulo---
			if(speed.x >0){
				pos.x -= 25;
			}
			else if(speed.x <0){
				pos.x += 25;
			}
			else if(speed.z >0){
				pos.z -= 25;
			}
			else if(speed.z <0){
				pos.z += 25;
			}
			//--------------------------------------------------------
		}
		std::cout << "DESPUES DE REPOSICIONAR" << std::endl;

		Real y = _bossPieces->at(0)->getRigidBody()->getCenterOfMassPosition().y;
		cout << "Y : " <<y << endl;
		//---------------------------------------------------------------------------
}

void AI_Manager::deleteLastWagon(){
	if(_bossPieces->size()>0){
		_bossPieces->pop_back();
	}
}

Boss* AI_Manager::getLastWagon(){
	return _bossPieces->back();
}
