#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBulletCollisionsRay.h"

#include "math.h"

#define FLOOR_POSITION_Y -2.8  // PONERLO BIEN (todos los define, meterlos en un archivo de configuracion)
#define FLOOR_POSITION_Z 4.0

#define WALLB_POSITION_Z 2.0
#define WALLB_POSITION_X -39.0

#define WALLL_POSITION_Y -2.8
#define WALLL_POSITION_Z -13.0

#define WALLR_POSITION_Y -2.8
#define WALLR_POSITION_Z 19.0

#define BOSS_ROOM 100.0
#define BOSS_ROOM_SCALE 3.0

#define WALL_LENGTH_X 100
#define WALL_HEIGHT_Y 20

using namespace std;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

Vector3 _desp ;
Vector3 *_despPtr;

void
PlayState::enter ()
{
	_root = Ogre::Root::getSingletonPtr();

	//GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("BGGame.ogg");
	//GameManager::getSingletonPtr()->_mainTrack->play();

	// Se recupera el gestor de escena y la cámara.----------------
	_sceneMgr = _root->getSceneManager("SceneManager");
	_camera = _sceneMgr->createCamera("PlayCamera");
	_viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
	_sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
	//_sceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

	//Camara--------------------
	//_camera->setPosition(Ogre::Vector3(-40,10,0));
	//_camera->lookAt(Ogre::Vector3(0,0,0));
	_camera->setPosition(Ogre::Vector3(-40,30,0));
	_camera->lookAt(Ogre::Vector3(15,0,0));
	_camera->setNearClipDistance(5);
	_camera->setFarClipDistance(10000);
	//-----------------------------

	_numEntities = 0;    // Numero de Shapes instanciadas
	_timeLastObject = 0; // Tiempo desde que se añadio el ultimo objeto

	// Creacion del modulo de debug visual de Bullet ------------------
	_debugDrawer = new OgreBulletCollisions::DebugDrawer();
	_debugDrawer->setDrawWireframe(true);
	SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("debugNode", Vector3::ZERO);
	node->attachObject(static_cast <SimpleRenderable *>(_debugDrawer));
	//-----------------------------------------------------------------

	// Creacion del mundo (definicion de los limites y la gravedad)----
	AxisAlignedBox worldBounds = AxisAlignedBox (Vector3 (-10000, -10000, -10000), Vector3 (10000,  10000,  10000));
	Vector3 gravity = Vector3(0, -9.8, 0);

	_world = new OgreBulletDynamics::DynamicsWorld(_sceneMgr,worldBounds, gravity);
	_world->setDebugDrawer (_debugDrawer);
	_world->setShowDebugShapes(true);  // Muestra los collision shapes
	//-----------------------------------------------------------------

	//Creacion de los elementos iniciales del mundo---
	CreateInitialWorld();
	//------------------------------------------------

	//Creo la interfaz---
	createGUI();
	//-------------------

	//Crear el MovementManager
	_movementManager = new MovementManager(_sceneMgr,_world,_hero,&_enemies,&_bossPieces,&_walls);
	//-------------------

	//Crear el PhysicsManager
	_physicsManager = new PhysicsManager(_sceneMgr,_world,_hero,&_gameEntities, &_enemies, &_walls);
	//-------------------

	//Crear el AnimationManager
	_animationManager = new AnimationManager(_sceneMgr, &_currentScenario, _hero);
	_animationManager->setupAnimations();

	//-------------------

	//Iniciacion Variables---
	_desp = Vector3(0,0,0);
	_despPtr = new Vector3(0,0,0);
	_despPtr = &_desp;
	_numModules = 0;
	_bossRoom = false;
	_wallsAreVisible = true;
	_bossCreated = false;
	_frameCounter = 0;
	//-----------------------

	//CameraPivote---
	_cameraPivot = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_CameraPivot");
	_cameraPivot->attachObject(_camera);
	//---------------

	//Animations---------------------------------------------------------------
	//_animEnemy = _sceneMgr->getEntity("E_Enemy")->getAnimationState("walkEnemy");
	
	//_animHero->setEnabled(true);
	//_animHero->setLoop(true);
	//_animHero->setTimePosition(0.0);
	//---------------------------------------------------------------------------
}

void
PlayState::exit ()
{
	//Salgo del estado------------------------------
	_sceneMgr->clearScene();
	_sceneMgr->destroyCamera("PlayCamera");
	_root->getAutoCreatedWindow()->removeAllViewports();
	//--------------------------------------------

	//Limpio la interfaz---------------------------
	CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
	sheet->destroyChild("background_wnd2");
	//---------------------------------------------

	//Limpiamos vectores-----------------------------
	_gameEntities.clear();
	_enemies.clear();
	_walls.clear();
	//----------------------------------------------

	GameManager::getSingletonPtr()->_mainTrack->unload();
}

void
PlayState::pause()
{
}

void
PlayState::resume()
{

}

void 
PlayState::CreateInitialWorld() {
	//Suelo Infinito NO TOCAR---------------------------------
	Plane plane1(Vector3(0,1,0), -3);    // Normal y distancia  (antes estaba a 0)
	MeshManager::getSingleton().createPlane("p1",
	ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane1,
	1000, 1000, 1, 1, true, 1, 20, 20, Vector3::UNIT_Z);
	//--------------------------------------------------------

	//Suelo Grafico-----------------------------------------------
	SceneNode* _groundNode = _sceneMgr->createSceneNode("SN_Ground");
	Entity* _groundEnt = _sceneMgr->createEntity("E_Ground", "p1");
	_groundEnt->setMaterialName("GroundRoom");
	_groundNode->attachObject(_groundEnt);
	_groundEnt->setCastShadows(false); //PRUEBA DE SOMBRAS 
	_sceneMgr->getRootSceneNode()->addChild(_groundNode);
	//------------------------------------------------------------

	// Creamos forma de colision para el plano -----------------------
	OgreBulletCollisions::CollisionShape *Shape;
	Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
			(Ogre::Vector3(0,1,0), -3);   // Vector normal y distancia (antes estaba a 0)
	OgreBulletDynamics::RigidBody *rigidBodyPlane = new
			OgreBulletDynamics::RigidBody("RB_Ground", _world,PhysicsMask::COL_StaticWalls,PhysicsMask::staticwalls_collides_with);

	// Creamos la forma estatica (forma, Restitucion, Friccion) ------
	rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8);
	rigidBodyPlane->getBulletObject()->setUserPointer((void *) _groundNode);
	// Anadimos los objetos Shape y RigidBody ------------------------
	_shapes.push_back(Shape);
	_bodies.push_back(rigidBodyPlane);

	//------------------------------------------------------------

	_currentScenario = Scenario::Menu;
	createScenario(_currentScenario);
	//createAllWalls();
	//Paredes Laterales--------------------------

	//Pared Grafica---------------------------------
	/*Plane plane2(Vector3(0,0,-1), -20);    // Normal y distancia
  MeshManager::getSingleton().createPlane("p2",
  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane2,
  1000, 80, 1, 1, true, 1, 20, 20, Vector3::UNIT_Y);

  SceneNode* _groundNodeWallRight = _sceneMgr->createSceneNode("WallRight");
  Entity* _groundEntWallRight = _sceneMgr->createEntity("planeEnt2", "p2");
  _groundEntWallRight->setMaterialName("Ground");
  _groundNodeWallRight->attachObject(_groundEntWallRight);
  _sceneMgr->getRootSceneNode()->addChild(_groundNodeWallRight);
  _groundNodeWallRight->setVisible(false);

  //------------------------------------------------------------

  // Creamos forma de colision para el plano ----------------------- 
  OgreBulletCollisions::CollisionShape *ShapeWallRight;
  ShapeWallRight = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3(0,0,-1), -20);   // Vector normal y distancia
  OgreBulletDynamics::RigidBody *rigidBodyPlaneWallRight = new 
    OgreBulletDynamics::RigidBody("WallRight", _world,PhysicsMask::COL_StaticWalls,PhysicsMask::staticwalls_collides_with);

  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlaneWallRight->setStaticShape(ShapeWallRight, 0.1, 0.8); 


  // Anadimos los objetos Shape y RigidBody ------------------------
  _shapes.push_back(ShapeWallRight);  
  _bodies.push_back(rigidBodyPlaneWallRight);
  //-------------------------------------------

  Plane plane2Left(Vector3(0,0,1), -13);    // Normal y distancia
  MeshManager::getSingleton().createPlane("p2Left",
  ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane2Left,
  1000, 80, 1, 1, true, 1, 20, 20, Vector3::UNIT_Y);

  SceneNode* _groundNodeWallLeft = _sceneMgr->createSceneNode("WallLeft");
  Entity* _groundEntWallLeft = _sceneMgr->createEntity("planeEntLeft", "p2Left");
  _groundEntWallLeft->setMaterialName("Ground");
  _groundNodeWallLeft->attachObject(_groundEntWallLeft);
  _sceneMgr->getRootSceneNode()->addChild(_groundNodeWallLeft);
  _groundNodeWallLeft->setVisible(false);

  //------------------------------------------------------------

  // Creamos forma de colision para el plano ----------------------- 
  OgreBulletCollisions::CollisionShape *ShapeWallLeft;
  ShapeWallLeft = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3(0,0,1), -13);   // Vector normal y distancia
  OgreBulletDynamics::RigidBody *rigidBodyPlaneWallLeft = new 
    OgreBulletDynamics::RigidBody("WallLeft", _world,PhysicsMask::COL_StaticWalls,PhysicsMask::staticwalls_collides_with);

  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlaneWallLeft->setStaticShape(ShapeWallLeft, 0.1, 0.8); 



  // Anadimos los objetos Shape y RigidBody ------------------------
  _shapes.push_back(ShapeWallLeft);  
  _bodies.push_back(rigidBodyPlaneWallLeft);*/

	//---------------------------------------------------------------------

	//LUCES------------------------------------------------
	Ogre::Light* light = _sceneMgr->createLight();
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setSpecularColour(Ogre::ColourValue::White);
	light->setDirection(Ogre::Vector3(1,-1,0));

	//-----------------------------------------------------

	//PJ----------------------------------------
	Entity *entity = _sceneMgr->createEntity("E_Hero", "tedybear.mesh");
	SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_Hero");
	node->attachObject(entity);
	//node->attachObject(_camera);

	Vector3 size = Vector3::ZERO;
	Vector3 position = Vector3(0,1.5,0);

	OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter = NULL;
	OgreBulletCollisions::CollisionShape *bodyShape = NULL;
	OgreBulletDynamics::RigidBody *rigidBody = NULL;

	AxisAlignedBox boundingB = entity->getBoundingBox();
	size = boundingB.getSize();
	size *= node->getScale();
	size /= 2.0f;   // El tamano en Bullet se indica desde el centro

	trimeshConverter = new
			OgreBulletCollisions::StaticMeshToShapeConverter(entity);
	bodyShape = trimeshConverter->createConvex();

	//bodyShape = new OgreBulletCollisions::BoxCollisionShape(size);
	//CAMBIO!!!!
	//PARA METER LAS MASCARAS FISICAS HAY QUE AÑADIR DOS PARAMETROS MAS, SU MASCARA Y SU GRUPO DE COLISION
	rigidBody = new OgreBulletDynamics::RigidBody("RB_Hero", _world,PhysicsMask::COL_Player,PhysicsMask::player_collides_with);

	rigidBody->setShape(node, bodyShape,
			0.0 /* Restitucion */, 0.9 /* Friccion */,
			5.0 /* Masa */, position /* Posicion inicial */,
			Quaternion::IDENTITY /* Orientacion */);

	//Propiedades del cuerpo fisico--------------------------------------
	rigidBody->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
	rigidBody->disableDeactivation();
	rigidBody->getBulletObject()->setUserPointer((void *) node);
	//-------------------------------------------------------------------

	//creamos el Hero para que contenga lo anterior, el sceneNode y el RigidBody---
	_hero = new Hero();
	_hero->setSceneNode(node);
	_hero->setRigidBody(rigidBody);
	_hero->setMovementSpeed(150.0);
	_hero->setAttacking(false);
	//-----------------------------------------------------------------------------

	// Anadimos los objetos a las deques--
	_shapes.push_back(bodyShape);
	_bodies.push_back(rigidBody);
	//------------------------------------

	

}


bool
PlayState::frameStarted
(const Ogre::FrameEvent& evt)
{ 
	_deltaT = evt.timeSinceLastFrame;
	_world->stepSimulation(_deltaT); // Actualizar simulacion Bullet
	_timeLastObject -= _deltaT;

	//Actualizo camara---
	if(!_bossRoom){
		_cameraPivot->setPosition(_hero->getSceneNode()->getPosition());
	}
	else{
		_camera->setPosition(-175,150,100);
		_camera->lookAt(0,0,100);
		_camera->yaw(Ogre::Degree(180));
		//_camera->setFixedYawAxis(true,Vector3(0,0,100));
		//_camera->yaw(Degree(180));
	}

	//-------------------

	//Deteccion Colisiones---------
	_physicsManager->detectHeroCollision();
	_physicsManager->detectEnemiesCollision();
	//---------------------------

	if(_currentScenario != Scenario::Menu){
		//if((trunc(_hero->getRigidBody()->getCenterOfMassPosition().x) % WALL_LENGTH_X) == 0){ //si queremos que solo llame a poblar de vez en cuando
		populateEnemies();
		removeAllBehindBackWall();
		//}
	}

	//Actualizo GUI---
	updateGUI();
	//----------------

	cout << "POS  X: " << _hero->getRigidBody()->getCenterOfMassPosition().x << endl;
	cout << "POS  Z: " << _hero->getRigidBody()->getCenterOfMassPosition().z << endl;


	//Movimiento------------
	_movementManager->moveHero(_despPtr);
	_movementManager->moveEnemies();

	if(!_bossRoom ){
		_movementManager->moveWalls();
	}
	else{
		_movementManager->moveBoss(); //ACTIVAR
	}

	//Invulnerabilidad del hero---------------
	_hero->UpdateInvulnerability(_deltaT);
	if(_hero->isInvulnerable()){
		if(++_frameCounter>15){
			_hero->getSceneNode()->flipVisibility();
			_frameCounter = 0 ;
		}

	}
	else{
		_hero->getSceneNode()->setVisible(true);
	}
	//----------------------------------------

	//----------------------

	//Animations--------------------------------
	_animationManager->resetAnimations(_deltaT);
	//_animationManager->resetAnimations(AnimationManager::ANIM_IDLE_HERO, _deltaT);
	//--------------------------------------------

	return true;
}


bool
PlayState::frameEnded
(const Ogre::FrameEvent& evt)
{

	_deltaT = evt.timeSinceLastFrame;
	_world->stepSimulation(_deltaT); // Actualizar simulacion Bullet

	//Salir del juego--
	if (_exitGame)
		return false;
	//-----------------
	return true;
}

void
PlayState::keyPressed
(const OIS::KeyEvent &e)
{
	//SceneNode* _pj = _sceneMgr->getSceneNode("SNCube");
	// Tecla p --> PauseState.-------
	if (e.key == OIS::KC_P) {
		pushState(PauseState::getSingletonPtr());
	}
	//-----------------

	// Tecla g --> GameOverState.-------
	if (e.key == OIS::KC_G) {
		changeState(GameOverState::getSingletonPtr());
	}
	//-----------------
	// Tecla N --> Boss Creation-------
	/*if (e.key == OIS::KC_N) {
		if(!_bossCreated){
			createBossRoom();
			createBoss();
			_bossCreated = true;
		}
		_movementManager->initializeBossMovement(&_deltaT);    //ACTIVAR
	}*/
	//-----------------

	// Tecla S --> Print current scenario and change backwall visibility-------
	if (e.key == OIS::KC_S) {
		if(_vScenario.size() > 0){
			std::cout << "ESCENARIO " << _currentScenario << _vScenario.at(0)<< std::endl;
		}
		for(unsigned int i=0; i<_walls.size(); i++){
			_walls.at(i)->getSceneNode()->setVisible(!_wallsAreVisible);
		}
		_wallsAreVisible = !_wallsAreVisible;
	}
	//-----------------

	//Movimiento PJ---------------
	if (e.key == OIS::KC_SPACE) {
		_movementManager->jumpHero();
		_animationManager->stopAnimations(AnimationManager::ANIM_RUN_HERO);
		_animationManager->playAnimations(AnimationManager::ANIM_JUMP_HERO);
	}
	if (e.key == OIS::KC_UP) {
		_desp+=Vector3(1,0,0);
	}
	if (e.key == OIS::KC_DOWN) {
		_desp+=Vector3(-1,0,0);
	}
	if (e.key == OIS::KC_LEFT) {
		_desp+=Vector3(0,0,-1);
	}
	if (e.key == OIS::KC_RIGHT) {
		_desp+=Vector3(0,0,1);
	}
	if (e.key == OIS::KC_V) {
		_animationManager->stopAnimations(AnimationManager::ANIM_RUN_HERO);
		_animationManager->playAnimations(AnimationManager::ANIM_ATTACK_HERO);
	}
	//--------------------------------


	//CEGUI--------------------------
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
	CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
	//-------------------------------
}

void
PlayState::keyReleased
(const OIS::KeyEvent &e)
{

	//Recargo flechas---------------
	/*if (e.key == OIS::KC_R) {

  }*/
	//------------------------------

	//Salgo del juego---------------
	if (e.key == OIS::KC_ESCAPE) {
		_exitGame = true;
	}
	//-------------------------------

	//Movimiento---------------------
	/*if (e.key == OIS::KC_SPACE) {

  }*/
	if (e.key == OIS::KC_UP) {
		_desp-=Vector3(1,0,0);
	}
	if (e.key == OIS::KC_DOWN) {
		_desp-=Vector3(-1,0,0);
	}
	if (e.key == OIS::KC_LEFT) {
		_desp-=Vector3(0,0,-1);
	}
	if (e.key == OIS::KC_RIGHT) {
		_desp-=Vector3(0,0,1);
	}
	/*if (e.key == OIS::KC_V) {
		_animationManager->stopAnimations(AnimationManager::ANIM_ATTACK_HERO);
	}*/
	/*if (e.key == OIS::KC_SPACE) {
		AnimationManager::getSingletonPtr()->stopAnimations(AnimationManager::ANIM_JUMP_HERO);
	}*/
	//-------------------------------
	
	//CEGUI--------------------------
	CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
	//-------------------------------
}

void
PlayState::mouseMoved
(const OIS::MouseEvent &e)
{

	//Movimiento camara-----------------------------------------------------------------------------
	float rotx = e.state.X.rel * _deltaT * -1;
	float roty = e.state.Y.rel * _deltaT * -1;
	_camera->yaw(Radian(rotx));
	_camera->pitch(Radian(roty));
	//----------------------------------------------------------------------------------------------

	//CEGUI--------------------------
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
	//-------------------------------
}

void
PlayState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	//CEGUI--------------------------
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
	//-------------------------------

}

void
PlayState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	//CEGUI--------------------------
	CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
	//-------------------------------


}

PlayState*
PlayState::getSingletonPtr ()
{
	return msSingleton;
}

PlayState&
PlayState::getSingleton ()
{ 
	assert(msSingleton);
	return *msSingleton;
}

CEGUI::MouseButton PlayState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
{
	//CEGUI--------------------------
	CEGUI::MouseButton ceguiId;
	switch(id)
	{
	case OIS::MB_Left:
		ceguiId = CEGUI::LeftButton;
		break;
	case OIS::MB_Right:
		ceguiId = CEGUI::RightButton;
		break;
	case OIS::MB_Middle:
		ceguiId = CEGUI::MiddleButton;
		break;
	default:
		ceguiId = CEGUI::LeftButton;
	}
	return ceguiId;
	//------------------------------
}


void
PlayState::createGUI()
{
	// Interfaz Intro--------------------
	CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

	//Interfaz Cegui Cabecera--------------------------------------
	CEGUI::Window* sheetBG =  CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","background_wnd2");
	sheetBG->setPosition(CEGUI::UVector2(CEGUI::UDim(0.0f, 0.0f),CEGUI::UDim(0.0, 0)));
	sheetBG->setSize( CEGUI::USize(CEGUI::UDim(1.0, 0), CEGUI::UDim(0.10, 0)));
	sheetBG->setProperty("Image","BackgroundImagePlayState");
	sheetBG->setProperty("FrameEnabled","False");
	sheetBG->setProperty("BackgroundEnabled", "False");

	CEGUI::Window* pauseButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","PauseButton");
	pauseButton->setText("[font='SPIDER MONKEY'] Pause");
	pauseButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
	pauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0),CEGUI::UDim(0.3,0)));
	pauseButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::pauseB,this));

	CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","QuitButton");
	quitButton->setText("[font='SPIDER MONKEY'] Exit");
	quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
	quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8,0),CEGUI::UDim(0.3,0)));
	quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));

	CEGUI::Window* textPoints = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textPoints");
	textPoints->setText("[font='SPIDER MONKEY-18'] 000");
	textPoints->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
	textPoints->setXPosition(CEGUI::UDim(0.43f, 0.0f));
	textPoints->setYPosition(CEGUI::UDim(0.35f, 0.0f));
	textPoints->setProperty("FrameEnabled","False");
	textPoints->setProperty("BackgroundEnabled", "False");
	textPoints->setProperty("VertFormatting", "TopAligned");

	CEGUI::Window* textLives = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textLives");
	textLives->setText("[font='SPIDER MONKEY-18'] 0");
	textLives->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
	textLives->setXPosition(CEGUI::UDim(0.1f, 0.0f));
	textLives->setYPosition(CEGUI::UDim(0.35f, 0.0f));
	textLives->setProperty("FrameEnabled","False");
	textLives->setProperty("BackgroundEnabled", "False");
	textLives->setProperty("VertFormatting", "TopAligned");
	//-----------------------------------------------------------------------------------------------------

	//Add Cegui-----------------
	sheetBG->addChild(textPoints);
	sheetBG->addChild(textLives);
	sheetBG->addChild(pauseButton);
	sheetBG->addChild(quitButton);
	sheet->addChild(sheetBG);
	//--------------------------------
}

bool
PlayState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}

bool
PlayState::pauseB(const CEGUI::EventArgs &e)
{
  pushState(PauseState::getSingletonPtr());
  return true;
}

void
PlayState::updateGUI()
{

	CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

	//Actualizo la puntuacion------------
	sheet->getChild("background_wnd2")->getChild("textPoints")->setText("[font='SPIDER MONKEY-18']"+Ogre::StringConverter::toString(_hero->getScore()));
	sheet->getChild("background_wnd2")->getChild("textLives")->setText("[font='SPIDER MONKEY-18']"+Ogre::StringConverter::toString(_hero->getLives()));
	//----------------------------------

	if(_currentScenario==Scenario::Menu){
		sheet->getChild("background_wnd2")->setVisible(false);
	}else{
		sheet->getChild("background_wnd2")->setVisible(true);
	}
}



void PlayState::changeScenarioQ(Scenario::Scenario _nextScenario){
	cout << "Cambio de escenario" << endl;

	//Volvemos a poner al personaje en la posición (0,0,0)
	cout << "Ajustando posicion del PJ..." << endl;
	_movementManager->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
	//------------------------

	//Cambio de escenario---
	//Primero hay que borrar el escenario anterior y luego cargar el nuevo.
	cout << "Vengo de: " << _currentScenario << endl;
	deleteCurrentScenario();

	cout << "Voy a : " << _nextScenario << endl;
	createScenario(_nextScenario);

}

bool PlayState::deleteCurrentScenario(){

	std::cout << "Borrando escenario " << _currentScenario <<std::endl;

	switch(_currentScenario){
	case Scenario::Menu:
		cout << "\n\nVENGO DE MENU\n\n" << endl;
		break;
	case Scenario::LevelRoom:
		cout << "\n\nVENGO DE ROOM\n\n" << endl;
		break;
	case Scenario::LevelGarden:
		cout << "\n\nVENGO DE GARDEN\n\n" << endl;
		break;
	}

	/*Borrar vector*/
	for(unsigned int i=0; i<_vScenario.size(); i++){ //borrar los trozos de escenario
		delete _vScenario.at(i);
	}

	_vScenario.clear(); //limpiar vector

	switch(_currentScenario) {
	case Scenario::Menu:{
		deleteScenarioContent();
		break;
	}
	case Scenario::LevelGarden:{
		deleteScenarioContent();
		break;
	}
	case Scenario::LevelRoom:
		deleteScenarioContent();
		break;
	}
	printAll();
	return _vScenario.empty();
}

void PlayState::createScenario(Scenario::Scenario nextScenario){

	//Musica Intro---------------------------------------
	GameManager::getSingletonPtr()->_mainTrack->unload();
	//---------------------------------------------------

	_nextScenario = nextScenario;
	//Creo el nuevo escenario---
	Entity* _ground = _sceneMgr->getEntity("E_Ground");

	switch(_nextScenario) {
	case Scenario::Menu:{
		std::cout << "Menu " << _nextScenario <<std::endl;
		//En el menu no aparecen hilos ni enemigos ni nada. Luego cuando se escoja nivel si
		GameEntity* gameEntity = new GameEntity();
		Ogre::Vector3 positionRoom(25,-2,15);
		Ogre::Vector3 scaleRoom = Ogre::Vector3(3,3,3);
		gameEntity = createGameEntity("DoorRoom", "doorRoom.mesh", positionRoom, scaleRoom);
		gameEntity->getRigidBody()->setOrientation(Ogre::Quaternion(Ogre::Degree(-95),Ogre::Vector3::UNIT_Y));

		Ogre::Vector3 positionGarden(25,-2,-15);
		Ogre::Vector3 scaleGarden = Ogre::Vector3(3,3,3);
		gameEntity = createGameEntity("DoorGarden", "doorGarden.mesh", positionGarden, scaleGarden);
		gameEntity->getRigidBody()->setOrientation(Ogre::Quaternion(Ogre::Degree(-95),Ogre::Vector3::UNIT_Y));

		_currentScenario=Scenario::Menu;
		_ground->setMaterialName("GroundRoom");

		//Musica -------------------------------------------
		GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("Menu.mp3");
		GameManager::getSingletonPtr()->_mainTrack->play();
		//--------------------------------------------------

		if(_hero!=NULL &&_hero->AllReelsPicked()){
			if(!_bossCreated){

				//Musica -------------------------------------------
				GameManager::getSingletonPtr()->_mainTrack->unload();
				//---------------------------------------------------

				createBossRoom();
				createBoss();
				_bossCreated = true;
			}
			_movementManager->initializeBossMovement(&_deltaT);    //ACTIVAR
		}



		break;
	}
	case Scenario::LevelGarden:
		createTestGameEntities();
		createAllWalls();
		for(unsigned int i=0;i<3;i++ ){
			String aux=Ogre::StringConverter::toString(i + _numModules);
			Entity* _entScn = _sceneMgr->createEntity("E_LevelGarden"+aux, "escenario2.mesh");
			SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_LevelGarden"+aux);
			_nodeScn->attachObject(_entScn);
			_nodeScn->yaw(Degree(270));
			_nodeScn->setPosition(0,-3,0);
			_nodeScn->setScale(Vector3(2.5,2.5,2.5));
			_nodeScn->translate(Vector3(230*i,0,0));

			_vScenario.push_back(_nodeScn);

			//Musica -------------------------------------------
			GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("Garden.ogg");
			GameManager::getSingletonPtr()->_mainTrack->play();
			//--------------------------------------------------
		}

		_ground->setMaterialName("Ground");
		_currentScenario = Scenario::LevelGarden;
		_numModules += 3;
		_sceneMgr->setSkyBox(true, "MatSkyboxlvl2");

		//Obstaculos--------------------------------
		populateObstacles("data/Levels/ObstaclesLvlGarden.txt");
		//------------------------------------------

		//Hilos--------------------------------------
		populateThreads("data/Levels/ThreadsGarden.txt");
		//-------------------------------------------

		//Leer fichero de enemigos--------------------------
		readEnemies("data/Levels/Enemies.txt");
		//--------------------------------------------------

		break;

	case Scenario::LevelRoom:
		createTestGameEntities();
		createAllWalls();
		for(unsigned int i=0;i<3;i++){
			String aux=Ogre::StringConverter::toString(i + _numModules);
			Entity* _entScn = _sceneMgr->createEntity("E_LevelRoom"+aux, "escenario.mesh");
			SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_LevelRoom"+aux);
			_nodeScn->attachObject(_entScn);
			_nodeScn->setPosition(0,-3,0);
			_nodeScn->setScale(Vector3(2.5,2.5,2.5));
			_nodeScn->yaw(Degree(270));
			_nodeScn->translate(Vector3(200*i,0,0));

			_vScenario.push_back(_nodeScn);
		}
		_ground->setMaterialName("GroundRoom");
		_currentScenario = Scenario::LevelRoom;
		_numModules += 3;
		_sceneMgr->setSkyBox(true, "MaterialSkybox");

		//Musica -------------------------------------------
		GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("Room.mp3");
		GameManager::getSingletonPtr()->_mainTrack->play();
		//--------------------------------------------------

		//PinchosLego--------------------------------------
		populateSpike("data/Levels/SpikeRoom.txt");
		//-------------------------------------------

		//Obstacles--------------------------------------
		populateObstacles("data/Levels/ObstaclesLvlRoom.txt");
		//-------------------------------------------

		break;
	}

	std::cout << "currentScenario: "<< _currentScenario << " nextScenario "<< _nextScenario <<std::endl;
	printAll();
}

void
PlayState::populateObstacles(String _path){
	//Leo el fichero-----------------------------------
	fstream fichero;//Fichero
	string frase;//Auxiliar
	fichero.open(_path.c_str(),ios::in);
	int index=0;
	if (fichero.is_open()) {
		while (getline (fichero,frase)) {
			int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
			int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
			int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);

			int g =Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[3]);

			int Sx = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[4]);
			int Sy = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[5]);
			int Sz = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[6]);

			Vector3 aux = Vector3(x,y,z);
			Quaternion q = Quaternion(Quaternion::IDENTITY);
			Vector3 scale = Vector3(Sx,Sy,Sz);

			GameEntity* ge = new GameEntity();
			ge=createGameEntity("Obstacle"+Ogre::StringConverter::toString(index),"cube.mesh",aux,scale);

			Entity* e = static_cast<Entity*>(ge->getSceneNode()->getAttachedObject(0));

			if(_currentScenario == Scenario::LevelRoom){
				if(x <120){
					e->setMaterialName("matRed");

				}else{
					e->setMaterialName("matGreen");

				}
				
			}else{
				e->setMaterialName("Ground");
			}
			
			index++;

		}
		fichero.close();
	}
	//-------------------------------------------------
}

void
PlayState::populateThreads(String _path){
	//Leo el fichero-----------------------------------
	fstream fichero;//Fichero
	string frase;//Auxiliar
	fichero.open(_path.c_str(),ios::in);
	int index=0;
	if (fichero.is_open()) {
		while (getline (fichero,frase)) {
			int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
			int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
			int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);

			int g =Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[3]);

			int Sx = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[4]);
			int Sy = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[5]);
			int Sz = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[6]);

			Vector3 aux = Vector3(x,y,z);
			Quaternion q = Quaternion(Quaternion::IDENTITY);
			Vector3 scale = Vector3(Sx,Sy,Sz);

			GameEntity* ge = new GameEntity();
			ge=createGameEntity("Thread"+Ogre::StringConverter::toString(index),"thread.mesh",aux,scale);
			ge->getRigidBody()->setOrientation(Ogre::Quaternion(Ogre::Degree(90),Ogre::Vector3::UNIT_X));
			index++;

		}
		fichero.close();
	}
	//-------------------------------------------------
}

void
PlayState::populateSpike(String _path){
	cout << "\nCreo pinchos\n"<<endl;
	//Leo el fichero-----------------------------------
	fstream fichero;//Fichero
	string frase;//Auxiliar
	fichero.open(_path.c_str(),ios::in);
	int index=0;
	if (fichero.is_open()) {
		while (getline (fichero,frase)) {
			int x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[0]);
			int y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[1]);
			int z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[2]);

			int g =Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[3]);

			int Sx = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[4]);
			int Sy = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[5]);
			int Sz = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase,",")[6]);

			Vector3 aux = Vector3(x,y,z);
			Quaternion q = Quaternion(Quaternion::IDENTITY);
			Vector3 scale = Vector3(Sx,Sy,Sz);

			GameEntity* ge = new GameEntity();
			ge=createGameEntity("Spike"+Ogre::StringConverter::toString(index),"spike.mesh",aux,scale);
			index++;

		}
		fichero.close();
	}
	//-------------------------------------------------
}

void PlayState::printAll(){
	SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();
	std::cout << "Printing all children of root" << std::endl;
	while (it.hasMoreElements()){
		String  _aux = it.getNext()->getName();
		std::cout << "  -"<< _aux << std::endl;
	}
}

void PlayState::createAllWalls(){
	Ogre::Vector3 position(0,1.5,0);
	Ogre::Vector3 scale(1,1,1);
	WallType name;
	GameEntity* gameEntity = new GameEntity();
	Wall* wall;

	//para paredes laterales //cambiar el 40 por la longitud de 2 o 3 modulos de nivel

	//Muro de la izquierda
	position.z = WALLL_POSITION_Z;
	position.y = WALLL_POSITION_Y;
	position.x = WALL_LENGTH_X/2 + 10;
	scale = Ogre::Vector3(WALL_LENGTH_X,WALL_HEIGHT_Y,1);
	name = LeftWall;
	gameEntity = createGameEntity("WallL", "cube.mesh", position, scale);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	wall->setSpawnPosition(gameEntity->getRigidBody()->getCenterOfMassPosition());
	cout << "POSICION PARA CREAR EL MURO = " << wall->getSpawnPosition() << endl;
	_walls.push_back(wall);

	//Muro de la derecha
	position.z = WALLR_POSITION_Z;
	position.y = WALLR_POSITION_Y;
	name = RightWall;
	gameEntity = createGameEntity("WallR", "cube.mesh", position, scale);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	wall->setSpawnPosition(gameEntity->getRigidBody()->getCenterOfMassPosition());
	cout << "POSICION PARA CREAR EL MURO = " << wall->getSpawnPosition() << endl;
	_walls.push_back(wall);

	//Muro trasero
	position.z = WALLB_POSITION_Z;
	position.x = WALLB_POSITION_X;
	scale = Ogre::Vector3(1,WALL_HEIGHT_Y,16);
	name = BackWall;
	gameEntity = createGameEntity("WallB", "cube.mesh", position, scale);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	wall->setSpawnPosition(gameEntity->getRigidBody()->getCenterOfMassPosition());
	cout << "POSICION PARA CREAR EL MURO = " << wall->getSpawnPosition() << endl;
	_walls.push_back(wall);

}

GameEntity* PlayState::createGameEntity(std::string name, std::string mesh, Ogre::Vector3 position, Ogre::Vector3 scale){
	std::cout << "Creando GameEntity " << name << std::endl;

	GameEntity* gameEntity;

	Ogre::SceneNode* node = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_" + name + Ogre::StringConverter::toString(_numEntities));

	Entity *entity = _sceneMgr->createEntity("E_" + name + Ogre::StringConverter::toString(_numEntities), mesh);
	node->attachObject(entity);

	OgreBulletDynamics::RigidBody* rigidBody;

	if(Ogre::StringUtil::startsWith(name,"Wall") || Ogre::StringUtil::startsWith(name,"Floor") || Ogre::StringUtil::startsWith(name,"Obstacle")|| Ogre::StringUtil::startsWith(name,"Thread")){
		node->scale(scale);
		entity->setCastShadows(false); //PRUEBA DE SOMBRAS 
		OgreBulletCollisions::BoxCollisionShape* bodyShape = new OgreBulletCollisions::BoxCollisionShape(scale);

		if(Ogre::StringUtil::startsWith(name,"Obstacle")){
			rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name+ Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Obs,PhysicsMask::obs_collides_with);
			rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.9f/*Friccion*/, 200.0f/*Masa*/, position);
		}
		else{
			if(Ogre::StringUtil::startsWith(name,"Thread")){
				rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Thread,PhysicsMask::thread_collides_with);
				rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.9f/*Friccion*/, 0.0f/*Masa*/, position);
			}
			else{
				if(Ogre::StringUtil::startsWith(name,"Wall")){
					if(Ogre::StringUtil::startsWith(name,"WallB")){
						rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_BackWall,PhysicsMask::backwall_collides_with);
						rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.9f/*Friccion*/, 100.0f/*Masa*/, position);
						entity->setCastShadows(false); //PRUEBA DE SOMBRAS 
					}
					else{
						rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Walls,PhysicsMask::walls_collides_with);
						rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.9f/*Friccion*/, 100.0f/*Masa*/, position);
					}
				}
				else{
					rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Walls,PhysicsMask::walls_collides_with);
					rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.9f/*Friccion*/, 100.0f/*Masa*/, position);
				}
				if(Ogre::StringUtil::startsWith(name,"Spike")){
					rigidBody = new OgreBulletDynamics::RigidBody("RB_"+ name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Spike,PhysicsMask::spikes_collides_with);
				}
				
			}
		}
	}
	else{
		OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter =
				new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
		OgreBulletCollisions::CollisionShape *bodyShape = trimeshConverter->createConvex();

		if(Ogre::StringUtil::startsWith(name,"Boss")){
			rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Boss,PhysicsMask::boss_collides_with);
		}
		else if(Ogre::StringUtil::startsWith(name,"Enemy")){
			rigidBody = new OgreBulletDynamics::RigidBody("RB_Enemy"+ Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Enemy,PhysicsMask::enemy_collides_with);
		}
		//else if(Ogre::StringUtil::startsWith(name,"Spike")){
			//rigidBody = new OgreBulletDynamics::RigidBody("RB_"+ name + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Spike,PhysicsMask::spikes_collides_with);
		//}
		else{
			rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name + Ogre::StringConverter::toString(_numEntities), _world);
		}

		if(Ogre::StringUtil::startsWith(name,"Door")){
			rigidBody->setShape(node, bodyShape,
					0.0 /* Restitucion */, 0.9 /* Friccion */,
					0.0 /* Masa */, position /* Posicion inicial */,
					Quaternion::IDENTITY /* Orientacion */);
		}
		else{
			rigidBody->setShape(node, bodyShape,
					0.0 /* Restitucion */, 0.9 /* Friccion */,
					15.0 /* Masa */, position /* Posicion inicial */,
					Quaternion::IDENTITY /* Orientacion */);
		}


	}

	rigidBody->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
	rigidBody->disableDeactivation();
	rigidBody->getBulletObject()->setUserPointer((void *) node);
	

	gameEntity = new GameEntity();
	gameEntity->setSceneNode(node);
	gameEntity->setRigidBody(rigidBody);

	std::cout << gameEntity->getSceneNode()->getName() << " " << gameEntity->getRigidBody()->getName() << std::endl;
	_gameEntities.push_back(gameEntity);

	_numEntities++;

	return gameEntity;
}

void PlayState::createBossRoom(){
	if(!_bossRoom){

		//Musica -------------------------------------------
		GameManager::getSingletonPtr()->_mainTrack = GameManager::getSingletonPtr()->_pTrackManager->load("BossFight.wav");
		GameManager::getSingletonPtr()->_mainTrack->play();
		//--------------------------------------------------

		Ogre::Vector3 position(0,1.5,0);
		Ogre::Vector3 scale(1,1,1);

		//WallType name = "";
		GameEntity* gameEntity = new GameEntity();
		Wall* wall = new Wall();
		Wall* aux = new Wall();
		//Aviso al MovementManager
		_movementManager->inBossRoom();

		//Elimino Muros Antiguos-------------------------------------------------------------------------------

		Ogre::SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();//Recupero el tablero del jugador en un iterador
		while ( it.hasMoreElements() ) {//Recorro el iterador
			SceneNode* _aux = static_cast<SceneNode*>(it.getNext());

			if(Ogre::StringUtil::startsWith(_aux->getName(),"SN_Wall") || Ogre::StringUtil::startsWith(_aux->getName(),"SN_Door")){
				Entity* _e = static_cast<Entity*>(_aux->getAttachedObject(0));//Recupero la entidad
				OgreBulletCollisions::Object* Baux =_world->findObject(_aux);
				_world->getBulletDynamicsWorld()->removeCollisionObject(Baux->getBulletObject());
				_sceneMgr->destroyEntity(_e);
				_sceneMgr->getRootSceneNode()->removeChild(_aux);
			}
		}
		//--------------------------------------------------------------------------------------------------------

	//Suelo-----------------------
	position.z = FLOOR_POSITION_Z + BOSS_ROOM;
	position.y = FLOOR_POSITION_Y;
	scale = Ogre::Vector3(100,50,100);

	//Muro de la izquierda--------
	position.z = BOSS_ROOM-BOSS_ROOM;
    position.y = WALLL_POSITION_Y;
    scale = Ogre::Vector3(BOSS_ROOM,10,1);
    gameEntity = createGameEntity("WallLBoss", "cube.mesh", position, scale);
    wall = new Wall();
    wall->setSceneNode(gameEntity->getSceneNode());
    wall->setRigidBody(gameEntity->getRigidBody());
    _walls.push_back(wall);
    Entity* eWallLBoss = static_cast<Entity*>(gameEntity->getSceneNode()->getAttachedObject(0));
	eWallLBoss->setMaterialName("matCarton");

    //Muro de la derecha----------
    position.z =  BOSS_ROOM*2;
    position.y = WALLR_POSITION_Y;
    gameEntity = createGameEntity("WallRBoss", "cube.mesh", position, scale);
    wall = new Wall();
    wall->setSceneNode(gameEntity->getSceneNode());
    wall->setRigidBody(gameEntity->getRigidBody());
    _walls.push_back(wall);
    Entity* eWallRBoss = static_cast<Entity*>(gameEntity->getSceneNode()->getAttachedObject(0));
	eWallRBoss->setMaterialName("matCarton");


    //Muro delantero--------
    position.z = BOSS_ROOM;
    position.x = BOSS_ROOM;
    position.y = WALLL_POSITION_Y;
    scale = Ogre::Vector3(1,10,100);
    gameEntity = createGameEntity("WallFBoss", "cube.mesh", position,scale);
    wall = new Wall();
    wall->setSceneNode(gameEntity->getSceneNode());
    wall->setRigidBody(gameEntity->getRigidBody());
    _walls.push_back(wall);
    Entity* eWallFBoss = static_cast<Entity*>(gameEntity->getSceneNode()->getAttachedObject(0));
	eWallFBoss->setMaterialName("matCarton");

    //Muro trasero----------
    position.z = BOSS_ROOM;
    position.y = WALLR_POSITION_Y;
    position.x = - BOSS_ROOM;
    gameEntity = createGameEntity("WallBBoss", "cube.mesh", position,scale);
    wall = new Wall();
    wall->setSceneNode(gameEntity->getSceneNode());
    wall->setRigidBody(gameEntity->getRigidBody());
    _walls.push_back(wall);
    Entity* eWallBBoss = static_cast<Entity*>(gameEntity->getSceneNode()->getAttachedObject(0));
	eWallBBoss->setMaterialName("matCarton");

	_movementManager->repositionHero(btVector3(0,0,BOSS_ROOM),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
	/*//Cambiar posicion de la camara--------------------
    int y = 10 + BOSS_ROOM;
    _camera->setPosition(Ogre::Vector3(-40,y,0));
    y = BOSS_ROOM;
    _camera->lookAt(Ogre::Vector3(0,y,0));
    _camera->setNearClipDistance(5);
    _camera->setFarClipDistance(10000);
    //-----------------------------*/
	//_movementManager->setWalls(&_walls);
	_bossRoom = true;

	}
}

void PlayState::createTestGameEntities(){
	string scenario = "";
	if(_nextScenario == Scenario::LevelGarden){
		scenario = "ReelGarden";
	}
	else{
		scenario = "ReelRoom";
	}

	//BOBINA-------------------------------------------------------------------------------
	Entity *entityReel = _sceneMgr->createEntity("E_"+ scenario + Ogre::StringConverter::toString(_numEntities), "Bobina.mesh");
	SceneNode *nodeReel = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_"+ scenario + Ogre::StringConverter::toString(_numEntities));
	nodeReel->attachObject(entityReel);

	Vector3 sizeReel = Vector3::ZERO;
	Vector3 positionReel = Vector3(250,0,0);

	OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverterReel = NULL;
	OgreBulletCollisions::CollisionShape *bodyShapeReel = NULL;
	OgreBulletDynamics::RigidBody *rigidBodyReel = NULL;

	AxisAlignedBox boundingBReel = entityReel->getBoundingBox();
	sizeReel = boundingBReel.getSize();
	sizeReel *= nodeReel->getScale();
	sizeReel /= 2.0f;   // El tamano en Bullet se indica desde el centro

	trimeshConverterReel = new
			OgreBulletCollisions::StaticMeshToShapeConverter(entityReel);
	bodyShapeReel = trimeshConverterReel->createConvex();

	//bodyShape = new OgreBulletCollisions::BoxCollisionShape(size);
	rigidBodyReel = new OgreBulletDynamics::RigidBody("RB_"+ scenario + Ogre::StringConverter::toString(_numEntities), _world,PhysicsMask::COL_Reel,PhysicsMask::reel_collides_with);

	rigidBodyReel->setShape(nodeReel, bodyShapeReel,
			0.0 /* Restitucion */, 0.9 /* Friccion */,
			0.0 /* Masa */, positionReel /* Posicion inicial */,
			Quaternion::IDENTITY /* Orientacion */);

	//Propiedades del cuerpo fisico--------------------------------------
	rigidBodyReel->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
	rigidBodyReel->disableDeactivation();
	rigidBodyReel->getBulletObject()->setUserPointer((void *) nodeReel);
	//-------------------------------------------------------------------
	_numEntities++;

	//------------------------------------------------------------------------------------
	//Enemigo----------------------------------------
	Vector3 position1 = Vector3(30,1.5,3.5);
	GameEntity* gameEntity = createGameEntity("Enemy","enemy.mesh",position1,Ogre::Vector3::UNIT_SCALE);

	//creamos el Enemy para que contenga lo anterior, el sceneNode y el RigidBody---
	Enemy *enemy = new Enemy();
	enemy->setSceneNode(gameEntity->getSceneNode());
	enemy->setRigidBody(gameEntity->getRigidBody());
	enemy->setSpeed(Vector3(0,0,-2));
	//-----------------------------------------------------------------------------

	//crear el vector de enemigos. De momento, con un enemigo---
	_enemies.push_back(enemy);
	_gameEntities.push_back(enemy);
	//-------------------------------------------------

}

void PlayState::createBoss(){
	//Crear Boss------------------------------------------------------
	_bossPieces.clear();
	GameEntity* gameEntity = new GameEntity();
	Boss* bossLocomotive = new Boss();
	Ogre::Vector3 position(-100,0,BOSS_ROOM -10);
	Ogre::Vector3 scale(1,1,1);

	scale *= 2;

	gameEntity = createGameEntity("BossLocomotive", "train.mesh", position, scale);
	bossLocomotive->setSceneNode(gameEntity->getSceneNode());
	bossLocomotive->setRigidBody(gameEntity->getRigidBody());
	bossLocomotive->setMovementSpeed(0.5);
	cout << "locomotive movementSpeed " << bossLocomotive->getMovementSpeed() <<endl;
	//bossLocomotive->getRigidBody()->setLinearVelocity(bossLocomotive->getMovementSpeed(),0,0);
	//bossLocomotive->getRigidBody()->setAngularVelocity(1,1,1);
	_bossPieces.push_back(bossLocomotive);
	//position.x -= 10.0;

	Boss* bossWagon = new Boss();
	gameEntity = createGameEntity("BossWagon1", "wagon.mesh", position, scale);
	bossWagon->setSceneNode(gameEntity->getSceneNode());
	bossWagon->setRigidBody(gameEntity->getRigidBody());
	bossWagon->setMovementSpeed(bossLocomotive->getMovementSpeed()); //la locomotora marca la velocidad
	_bossPieces.push_back(bossWagon);

	//position.x -= 10.0;

	bossWagon = new Boss();
	gameEntity = createGameEntity("BossWagon2", "wagon.mesh", position, scale);
	bossWagon->setSceneNode(gameEntity->getSceneNode());
	bossWagon->setRigidBody(gameEntity->getRigidBody());
	bossWagon->setMovementSpeed(bossLocomotive->getMovementSpeed()); //la locomotora marca la velocidad
	_bossPieces.push_back(bossWagon);

	//----------------------------------------------------------------
}



void PlayState::deleteScenarioContent(){
	//Borra puertas, hilos, carretes, bobinas, muros, suelo de boss, bosses, enemigos y obstaculos.

	Ogre::SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();//Recupero el tablero del jugador en un iterador
	while ( it.hasMoreElements() ) {//Recorro el iterador
		SceneNode* node = static_cast<SceneNode*>(it.getNext());
		if(Ogre::StringUtil::startsWith(node->getName(),"SN_Thread") || Ogre::StringUtil::startsWith(node->getName(),"SN_Door")
		|| Ogre::StringUtil::startsWith(node->getName(),"SN_Reel") || Ogre::StringUtil::startsWith(node->getName(),"SN_Enemy")
		|| Ogre::StringUtil::startsWith(node->getName(),"SN_Wall") || Ogre::StringUtil::startsWith(node->getName(),"SN_Floor")
		|| Ogre::StringUtil::startsWith(node->getName(),"SN_Boss") || Ogre::StringUtil::startsWith(node->getName(),"SN_Obstacle")
		|| Ogre::StringUtil::startsWith(node->getName(),"SN_Spike")){
			Entity* entity = static_cast<Entity*>(node->getAttachedObject(0));//Recupero la entidad
			OgreBulletCollisions::Object* Baux =_world->findObject(node);
			_world->getBulletDynamicsWorld()->removeCollisionObject(Baux->getBulletObject());
			_sceneMgr->destroyEntity(entity);
			_sceneMgr->getRootSceneNode()->removeChild(node);
		}
	}
	_enemies.clear();
	_bossRoom = false;
	_bossCreated = false;

	_gameEntities.clear();
}

void PlayState::populateEnemies(){
	int index = 100;
	GameEntity* gameEntity = new GameEntity();
	Enemy* enemy;

	double hero_x = _hero->getRigidBody()->getCenterOfMassPosition().x;
	double enemy_x = 0.0;
	for(unsigned int i=0; i<_posEnemies.size(); i++){
		enemy_x = _posEnemies.at(i).x;
		if(hero_x < enemy_x && enemy_x < (hero_x + WALL_LENGTH_X/2)){ //si el enemigo cae dentro de los muros (con un poco de margen) lo creo
			gameEntity = createGameEntity("Enemy"+Ogre::StringConverter::toString(index),"enemy.mesh",_posEnemies.at(i),Ogre::Vector3::UNIT_SCALE);
			enemy = new Enemy(gameEntity->getSceneNode(), gameEntity->getRigidBody(),"1");
			enemy->setMovementSpeed(50.0);
			enemy->setSpeed(Ogre::Vector3(0,0,-2));
			_enemies.push_back(enemy);
			index++;
			_posEnemies.at(i) = Ogre::Vector3(-50,-50,-50);
		}
	}

}

void PlayState::readEnemies(string path){
	_posEnemies.clear();
	//Leo el fichero-----------------------------------
	fstream file;//Fichero
	string line; //leer linea a linea
	Ogre::Vector3 vPos(0,0,0);

	file.open(path.c_str(),ios::in);

	if (file.is_open()) {
		while (getline (file,line)) {
			int enemyPos_x = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (line,",")[0]);
			int enemyPos_y = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (line,",")[1]);
			int enemyPos_z = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (line,",")[2]);

			int EnemyType = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (line,",")[3]);

			vPos = Ogre::Vector3(enemyPos_x,enemyPos_y,enemyPos_z);
			_posEnemies.push_back(vPos);
		}
		file.close();
	}
	//-------------------------------------------------
	std::cout <<"	COMPROBAR POSICIONES DE ENEMIGOS"<< std::endl;
	for(unsigned int i = 0; i < _posEnemies.size(); i++){
		std::cout << " " << _posEnemies.at(i) << std::endl;
	}
}

void PlayState::removeAllBehindBackWall(){
	//todo lo que vaya quedandose detrás del backwall (con un pequeño margen), ir eliminandolo
	int threshold = 5; //margen, borramos todo lo que se quede a 10 por detrás del backwall
	if(_walls.size() > 0){
		int backWall_x = _walls.at(_walls.size() - 1)->getRigidBody()->getCenterOfMassPosition().x;
		for(unsigned int i = 0; i < _gameEntities.size(); i++){
			if(_gameEntities.at(i)->getRigidBody()->getCenterOfMassPosition().x < (backWall_x - threshold)){
				//Borrar la gameentity (Si no es un obstaculo) porque está detrás del muro trasero
				if(Ogre::StringUtil::startsWith(_gameEntities.at(i)->getRigidBody()->getName(),"SN_Obstacle")){
					Entity* entity = static_cast<Entity*>(_gameEntities.at(i)->getSceneNode()->getAttachedObject(0));//Recupero la entidad
					OgreBulletCollisions::Object* Baux =_world->findObject(_gameEntities.at(i)->getSceneNode());
					_world->getBulletDynamicsWorld()->removeCollisionObject(Baux->getBulletObject());
					_sceneMgr->destroyEntity(entity);
					_sceneMgr->getRootSceneNode()->removeChild(_gameEntities.at(i)->getSceneNode());
				}
			}
		}
	}

}
