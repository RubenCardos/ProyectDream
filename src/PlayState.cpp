#include "PlayState.h"
#include "PauseState.h"
#include "GameOverState.h"

#include "Shapes/OgreBulletCollisionsConvexHullShape.h"
#include "Shapes/OgreBulletCollisionsTrimeshShape.h"    
#include "Utils/OgreBulletCollisionsMeshToShapeConverter.h"
#include "Shapes/OgreBulletCollisionsSphereShape.h"
#include "Shapes/OgreBulletCollisionsBoxShape.h"
#include "OgreBulletCollisionsRay.h"

#define FLOOR_POSITION_Y -12.0;  // PONERLO BIEN
#define FLOOR_POSITION_Z 4.0;
//#define WALLL_POSITION_Z -13.0;
//#define WALLR_POSITION_Z -20.0;
#define WALLL_POSITION_Y 5.0;
#define WALLL_POSITION_Z -13.0;
#define WALLR_POSITION_Y 5.0;
#define WALLR_POSITION_Z 19.0;


using namespace std;

template<> PlayState* Ogre::Singleton<PlayState>::msSingleton = 0;

Vector3 _desp ;
Vector3 *_despPtr;

struct RigidNode{
	Ogre::SceneNode* node;
	OgreBulletDynamics::RigidBody* rigidBody;
};

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
  
   
  //Camara--------------------
  _camera->setPosition(Ogre::Vector3(-40,10,0));
  _camera->lookAt(Ogre::Vector3(0,0,0));
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
  //_movementManager = new MovementManager(_sceneMgr,_hero,_enemies);
  _movementManager = new MovementManager(_sceneMgr,_hero,_gameEntities,_walls);
  //-------------------

  //Crear el PhysicsManager
    //_physicsManager = new PhysicsManager(_sceneMgr,_world,_hero,_enemies);
  _physicsManager = new PhysicsManager(_sceneMgr,_world,_hero,_gameEntities);
  //-------------------

  //Iniciacion Variables---
  _desp = Vector3(0,0,0);
  _despPtr = new Vector3(0,0,0);
  _despPtr = &_desp;
  _currentScenario=LevelGarden;
  _numModules = 0;
  //-----------------------
  _exitGame = false;

  //Animations---------------------------------------------------------------
  _animEnemy = _sceneMgr->getEntity("E_Enemy")->getAnimationState("walkEnemy");
  _animEnemy->setEnabled(true);
  _animEnemy->setLoop(true);
  _animEnemy->setTimePosition(0.0);
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
  
  //---------------------------------------------

  //GameManager::getSingletonPtr()->_mainTrack->unload();
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
  SceneNode* _groundNode = _sceneMgr->createSceneNode("ground");
  Entity* _groundEnt = _sceneMgr->createEntity("planeEnt", "p1");
  _groundEnt->setMaterialName("Ground");
  _groundNode->attachObject(_groundEnt);
  _sceneMgr->getRootSceneNode()->addChild(_groundNode);
  //------------------------------------------------------------

  // Creamos forma de colision para el plano ----------------------- 
  OgreBulletCollisions::CollisionShape *Shape;
  Shape = new OgreBulletCollisions::StaticPlaneCollisionShape
    (Ogre::Vector3(0,1,0), -3);   // Vector normal y distancia (antes estaba a 0)
  OgreBulletDynamics::RigidBody *rigidBodyPlane = new 
    OgreBulletDynamics::RigidBody("ground", _world);

  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlane->setStaticShape(Shape, 0.1, 0.8);

  // Anadimos los objetos Shape y RigidBody ------------------------
  _shapes.push_back(Shape);
  _bodies.push_back(rigidBodyPlane);

  //Prueba LVL1-------------------------------------------------
  /*for(int i=0;i<3;i++ ){
    String aux=Ogre::StringConverter::toString(i);
    Entity* _entScn = _sceneMgr->createEntity("EntGarden"+aux, "escenario2.mesh");
	  SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("SNGarden"+aux);
	  _nodeScn->attachObject(_entScn);
	  _nodeScn->yaw(Degree(270));
	  _nodeScn->setScale(Vector3(2.5,2.5,2.5));
	  _nodeScn->translate(Vector3(230*i,0,0));

	  std::cout << "meter trozo de escenario en vector" << std::endl;
	  _vScenario.push_back(_nodeScn);
	  std::cout << "trozo de escenario en vector metido" << std::endl;
  }*/

  _currentScenario = Menu;
  _nextScenario = LevelGarden;
  createScenario();
  
  //------------------------------------------------------------


  
  
  //Paredes Laterales--------------------------
  
  //Pared Grafica---------------------------------
  Plane plane2(Vector3(0,0,-1), -20);    // Normal y distancia
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
    OgreBulletDynamics::RigidBody("WallRight", _world);

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
    OgreBulletDynamics::RigidBody("WallLeft", _world);

  // Creamos la forma estatica (forma, Restitucion, Friccion) ------
  rigidBodyPlaneWallLeft->setStaticShape(ShapeWallLeft, 0.1, 0.8); 

  // Anadimos los objetos Shape y RigidBody ------------------------
  _shapes.push_back(ShapeWallLeft);  
  _bodies.push_back(rigidBodyPlaneWallLeft);

  //---------------------------------------------------------------------


  //HILO-------------------------------------------------------------------------------
  Entity *entityThread = _sceneMgr->createEntity("EntThread", "thread.mesh");
  SceneNode *nodeThread = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_Thread");
  nodeThread->attachObject(entityThread);
  
  Vector3 sizeThread = Vector3::ZERO; 
  Vector3 positionThread = Vector3(8,3,-10);
 
  OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverterThread = NULL; 
  OgreBulletCollisions::CollisionShape *bodyShapeThread = NULL;
  OgreBulletDynamics::RigidBody *rigidBodyThread = NULL;

  AxisAlignedBox boundingBThread = entityThread->getBoundingBox();
  sizeThread = boundingBThread.getSize();
  sizeThread *= nodeThread->getScale(); 
  sizeThread /= 2.0f;   // El tamano en Bullet se indica desde el centro

  trimeshConverterThread = new
      OgreBulletCollisions::StaticMeshToShapeConverter(entityThread);
  bodyShapeThread = trimeshConverterThread->createConvex();

  //bodyShape = new OgreBulletCollisions::BoxCollisionShape(size);
  rigidBodyThread = new OgreBulletDynamics::RigidBody("SN_Thread", _world);

  rigidBodyThread->setShape(nodeThread, bodyShapeThread,
         0.0 /* Restitucion */, 0.9 /* Friccion */,
         0.0 /* Masa */, positionThread /* Posicion inicial */,
         Quaternion::IDENTITY /* Orientacion */);

  //Propiedades del cuerpo fisico--------------------------------------
  rigidBodyThread->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
  rigidBodyThread->disableDeactivation();
  //-------------------------------------------------------------------
  //------------------------------------------------------------------------------------

  //HILO-------------------------------------------------------------------------------
  Entity *entityReel = _sceneMgr->createEntity("EntReel", "Bobina.mesh");
  SceneNode *nodeReel = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_Reel");
  nodeReel->attachObject(entityReel);
  
  Vector3 sizeReel = Vector3::ZERO; 
  Vector3 positionReel = Vector3(20,6,-10);
 
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
  rigidBodyReel = new OgreBulletDynamics::RigidBody("SN_Reel", _world);

  rigidBodyReel->setShape(nodeReel, bodyShapeReel,
         0.0 /* Restitucion */, 0.9 /* Friccion */,
         0.0 /* Masa */, positionReel /* Posicion inicial */,
         Quaternion::IDENTITY /* Orientacion */);

  //Propiedades del cuerpo fisico--------------------------------------
  rigidBodyReel->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
  rigidBodyReel->disableDeactivation();
  //-------------------------------------------------------------------
  //------------------------------------------------------------------------------------


  //CREAR LAS PAREDES
  createAllWalls();
  //------------------------------------------------------------


  //SkyBox-------------------------------------
  _sceneMgr->setSkyBox(true, "MatSkyboxlvl2");
  //-------------------------------------------
  
  //LUCES------------------------------------------------
  Light* _directionalLight = _sceneMgr->createLight("DirectionalLight");
  _directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
  //_directionalLight->setDiffuseColour(.60, .60, 0);
  _directionalLight->setDirection(Ogre::Vector3(0, -1, 0));

  //-----------------------------------------------------

  //CUBO PRUEBA PJ----------------------------------------
  Entity *entity = _sceneMgr->createEntity("E_Hero", "TeddyBear/tedybear.mesh");
  SceneNode *node = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_Hero");
  node->attachObject(entity);
  node->attachObject(_camera);
  
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
  rigidBody = new OgreBulletDynamics::RigidBody("RB_Hero", _world);

  rigidBody->setShape(node, bodyShape,
         0.0 /* Restitucion */, 0.9 /* Friccion */,
         5.0 /* Masa */, position /* Posicion inicial */,
         Quaternion::IDENTITY /* Orientacion */);

  //Propiedades del cuerpo fisico--------------------------------------
  rigidBody->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
  rigidBody->disableDeactivation();
  //-------------------------------------------------------------------

  //creamos el Hero para que contenga lo anterior, el sceneNode y el RigidBody---
  _hero = new Hero();
  _hero->setSceneNode(node);
  _hero->setRigidBody(rigidBody);
  _hero->setMovementSpeed(150.0);
  //-----------------------------------------------------------------------------

  //Enemigo----------------------------------------
   Entity *entity1 = _sceneMgr->createEntity("E_Enemy", "Enemies/Level 1/enemy.mesh");
   SceneNode *node1 = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_Enemy");
   node1->attachObject(entity1);

   Vector3 size1 = Vector3::ZERO;
   Vector3 position1 = Vector3(30,1.5,3.5);

   OgreBulletCollisions::CollisionShape *bodyShape1 = NULL;
   OgreBulletDynamics::RigidBody *rigidBody1 = NULL;

   AxisAlignedBox boundingB1 = entity1->getBoundingBox();
   size1 = boundingB1.getSize();
   size1 *= node1->getScale();
   size1 /= 2.0f;   // El tamano en Bullet se indica desde el centro

   trimeshConverter = new
       OgreBulletCollisions::StaticMeshToShapeConverter(entity1);
   bodyShape1 = trimeshConverter->createConvex();

   //bodyShape = new OgreBulletCollisions::BoxCollisionShape(size);
   rigidBody1 = new OgreBulletDynamics::RigidBody("RB_Enemy", _world);

   rigidBody1->setShape(node1, bodyShape1,
          0.0 /* Restitucion */, 0.9 /* Friccion */,
          5.0 /* Masa */, position1 /* Posicion inicial */,
          Quaternion::IDENTITY /* Orientacion */);

   //Propiedades del cuerpo fisico--------------------------------------
   rigidBody1->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
   rigidBody1->disableDeactivation();
   //-------------------------------------------------------------------

   //creamos el Enemy para que contenga lo anterior, el sceneNode y el RigidBody---
   Enemy *enemy = new Enemy();
   enemy->setSceneNode(node1);
   enemy->setRigidBody(rigidBody1);
   enemy->setMovementSpeed(50.0);
   enemy->setSpeed(Ogre::Vector3(-1,0,0));
   //-----------------------------------------------------------------------------

  //crear el vector de enemigos. De momento, con un enemigo---
  _enemies = new std::vector<Enemy*>();
  _enemies->push_back(enemy);
  _gameEntities = new std::vector<GameEntity*>();
  _gameEntities->push_back(enemy);
  //-------------------------------------------------

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

  //Deteccion Colisones---------
  _physicsManager->detectHeroCollision();
  //---------------------------

  //Actualizo GUI---
  updateGUI();
  //----------------

  //Movimiento------------
  _movementManager->moveHero(_despPtr,_deltaT);
  _movementManager->moveEnemies(_deltaT);
  //----------------------
  

  //Animations--------------------------------
  if (_animEnemy != NULL) {
    if (_animEnemy->hasEnded()) {
      _animEnemy->setTimePosition(0.0);
      _animEnemy->setEnabled(false);
    }
    else {
      _animEnemy->addTime(_deltaT);
    }
  }
  //--------------------------------------------

  return true;
}

void PlayState::DetectCollisionAim() {
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
    
    //Compruebo con todas dianas-----------------------------
    /*for(unsigned int i=0;i<_targets.size();i++){
      Ogre::SceneNode* drain = _targets[i];

      OgreBulletCollisions::Object *obDrain = _world->findObject(drain);
      OgreBulletCollisions::Object *obOB_A = _world->findObject(obA);
      OgreBulletCollisions::Object *obOB_B = _world->findObject(obB);

      if ((obOB_A == obDrain) || (obOB_B == obDrain)) {
        Ogre::SceneNode* node = NULL;
        if ((obOB_A != obDrain) && (obOB_A)) {
          node = obOB_A->getRootNode(); delete obOB_A;
        }
        else if ((obOB_B != obDrain) && (obOB_B)) {
          node = obOB_B->getRootNode(); delete obOB_B;
        }
        if (node) {
          cout << "Nodo que colisiona: " << node->getName() << "\n" << endl;
          
          //Creo una flecha en la misma posicion y rotacion---
          Entity *ent = _sceneMgr->createEntity("Arrow" + StringConverter::toString(_numEntities), "arrow.mesh");
          SceneNode *nod = _sceneMgr->getRootSceneNode()->createChildSceneNode("Arrow" + StringConverter::toString(_numEntities)+"SN");
          nod->attachObject(ent);
          nod->setPosition(node->getPosition());
          nod->setOrientation(node->getOrientation());
          _numEntities++;
          //--------------------------------------------------
          
          //Aumento la puntuacion-----------------------------
          _punt+=100;
          //-------------------------------------------------
          
          //Muestro lbl de impacto---------------------------
          _impact=true;
          //-------------------------------------------------

          //Cargo y reproduzco el sonido---
          GameManager::getSingletonPtr()->_simpleEffect = GameManager::getSingletonPtr()->_pSoundFXManager->load("ArrowImpact.wav");
          GameManager::getSingletonPtr()->_simpleEffect->play();
          //-------------------------------
          _sceneMgr->getRootSceneNode()->removeAndDestroyChild (node->getName());
        }
      }
    }
    //------------------------------------------------

     */
  }
  //----------------------------------------  
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

  // Tecla p --> GameOverState.-------
  if (e.key == OIS::KC_G) {
    changeState(GameOverState::getSingletonPtr());
  }
  //-----------------

  // Tecla T --> Test Cambiar Escenario-------
  if (e.key == OIS::KC_T) {
    changeScenarioQ();
  }
  //-----------------

  //Movimiento CUBO---------------
  if (e.key == OIS::KC_SPACE) {
    _movementManager->jumpHero();
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
  if (e.key == OIS::KC_R) {
    
  }
  //------------------------------

  //Salgo del juego---------------
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
  //-------------------------------
  
  //Movimiento---------------------
  if (e.key == OIS::KC_SPACE) {
 
  }
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
  //sheetBG->setProperty("Image","Background");
  sheetBG->setProperty("FrameEnabled","False");
  sheetBG->setProperty("BackgroundEnabled", "False");

  CEGUI::Window* pauseButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","PauseButton");
  pauseButton->setText("Pause");
  pauseButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  pauseButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.6,0),CEGUI::UDim(0.3,0)));
  //pauseButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::pauseB,this));

  CEGUI::Window* quitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","QuitButton");
  quitButton->setText("Exit");
  quitButton->setSize(CEGUI::USize(CEGUI::UDim(0.15,0),CEGUI::UDim(0.5,0)));
  quitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.8,0),CEGUI::UDim(0.3,0)));
 // quitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&PlayState::quit,this));

  CEGUI::Window* textPoints = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textPoints");
  textPoints->setText("SCORE 000");
  textPoints->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.70,0)));
  textPoints->setXPosition(CEGUI::UDim(0.43f, 0.0f));
  textPoints->setYPosition(CEGUI::UDim(0.35f, 0.0f));
  textPoints->setProperty("FrameEnabled","False");
  textPoints->setProperty("BackgroundEnabled", "False");
  textPoints->setProperty("VertFormatting", "TopAligned");

  CEGUI::Window* textLives = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textLives");
  textLives->setText("LIVES 0");
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

void
PlayState::updateGUI()
{

  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  //Actualizo la puntuacion------------
  sheet->getChild("background_wnd2")->getChild("textPoints")->setText(Ogre::StringConverter::toString(_hero->getScore()));
  //----------------------------------
  
}


void 
PlayState::changeScenario(Scenario _nextScenario){
  cout << "Cambio de escenario" << endl;
  //Ogre::SceneNode* _snIter = new SceneNode("snIter");

  //Reajustando personaje---
  //Volvemos a poner al personaje en la posición (0,0,0)
  cout << "Ajustando posicion del PJ..." << endl;
  _movementManager->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
  //------------------------

  //Cambio de escenario---
  //Primero hay que borrar el escenario anterior y luego cargar el nuevo.

  SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();
  Ogre::SceneManager::MovableObjectIterator iterator = _sceneMgr->getMovableObjectIterator("Entity");

  switch(_currentScenario) {
    case Menu:
      /* circle stuff */ 
      break;
    case LevelGarden:
    	while (it.hasMoreElements()){
    		String  _aux = it.getNext()->getName();
    		if(Ogre::StringUtil::startsWith(_aux,"SNGarden")){
    			_sceneMgr->getRootSceneNode()->removeChild(_aux); //detach node from parent
    		}
    	}
    	while(iterator.hasMoreElements()){
    	        Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
    	        if(Ogre::StringUtil::startsWith(e->getName(),"EntGarden")){
    	        	_sceneMgr->destroyEntity(e); //detach node from parent
    	        }
    	}
    	break;
    case LevelRoom:
        	while (it.hasMoreElements()){
        		String  _aux = it.getNext()->getName();
        		if(Ogre::StringUtil::startsWith(_aux,"SNRoom")){
        			_sceneMgr->getRootSceneNode()->removeChild(_aux); //detach node from parent
        		}
        	}
        	while(iterator.hasMoreElements()){
        		Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
        			if(Ogre::StringUtil::startsWith(e->getName(),"EntGarden")){
        				_sceneMgr->destroyEntity(e); //detach node from parent
        	    	}
        	    }
        	break;
  }

  //----------------------

  cout << "Vengo de: " << _currentScenario << endl;

  //Cambio de valor de escenario---
  switch(_currentScenario) {
    case Menu:
      /* circle stuff */ 
      break;
    case LevelGarden:
    	_currentScenario=LevelRoom;
    	break;
    case LevelRoom:
    	_currentScenario=LevelGarden;
      break;
    case LevelTest:
    	_currentScenario=LevelRoom;
      break;
  }
  //-------------------------------

  cout << "Voy a : " << _currentScenario << endl;

  //Creo el nuevo escenario---
  Entity* _ground = _sceneMgr->getEntity("planeEnt");
  switch(_currentScenario) {
      case Menu:
        /* circle stuff */
        break;
      case LevelGarden:
    	  for(int i=0;i<3;i++ ){
    	      String aux=Ogre::StringConverter::toString(i);
    	      Entity* _entScn = _sceneMgr->createEntity("EntGarden"+aux, "escenario2.mesh");
    	  	  SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("SNGarden"+aux);
    	  	  _nodeScn->attachObject(_entScn);
    	  	  _nodeScn->yaw(Degree(270));
    	  	  _nodeScn->setScale(Vector3(2.5,2.5,2.5));
    	  	  _nodeScn->translate(Vector3(205*i,0,0));
    	    }
    	  _ground->setMaterialName("Ground");
      	break;
      case LevelRoom:
    	  for(int i=0;i<3;i++ ){
    	      String aux=Ogre::StringConverter::toString(i);
    	      Entity* _entScn = _sceneMgr->createEntity("EntRoom"+aux, "escenario1.mesh");
    	  	  SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("SNRoom"+aux);
    	  	  _nodeScn->attachObject(_entScn);
    	  	  _nodeScn->yaw(Degree(270));
    	  	  _nodeScn->setScale(Vector3(2.5,2.5,2.5));
    	  	  _nodeScn->translate(Vector3(230*i,0,0));
    	  	  /*std::cout << "meter trozo de escenario en vector" << std::endl;
    	  	  //_vScenario.push_back(_nodeScn);
    	  	  std::cout << "trozo de escenario en vector metido" << std::endl;*/
    	    }
    	  _ground->setMaterialName("GroundRoom");
        break;
      case LevelTest:
      	_currentScenario=LevelRoom;
        break;
    }
    //-------------------------------

}

void PlayState::changeScenarioQ(){
	cout << "Cambio de escenario" << endl;

	//Volvemos a poner al personaje en la posición (0,0,0)
	cout << "Ajustando posicion del PJ..." << endl;
	_movementManager->repositionHero(btVector3(0,0,0),_hero->getRigidBody()->getBulletRigidBody()->getOrientation());
	//------------------------

	//Cambio de escenario---
	//Primero hay que borrar el escenario anterior y luego cargar el nuevo.

	SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();
	Ogre::SceneManager::MovableObjectIterator iterator = _sceneMgr->getMovableObjectIterator("Entity");

	cout << "Vengo de: " << _currentScenario << endl;
	deleteScenario();

	cout << "Voy a : " << _nextScenario << endl;
	createScenario();

}

bool PlayState::deleteScenario(){

	std::cout << "Borrando escenario " << _currentScenario <<std::endl;

	/*Borrar vector*/
		for(unsigned int i=0; i<_vScenario.size(); i++){ //borrar los trozos de escenario
			delete _vScenario.at(i);
		}

		_vScenario.clear(); //limpiar vector

	/*Borrar nodos*/

	SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();
	Ogre::SceneManager::MovableObjectIterator iterator = _sceneMgr->getMovableObjectIterator("Entity");
	std::string sAux = "";

	switch(_currentScenario) {
	    case Menu:
	      /* circle stuff */
	      break;
	    case LevelGarden:
	    	//iterator = _sceneMgr->getMovableObjectIterator("Entity");
	    	while(iterator.hasMoreElements()){
	    	        Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
	    	        if(Ogre::StringUtil::startsWith(e->getName(),"LevelGardenEnt")){
	    	        	_sceneMgr->destroyEntity(e);
	    	        }
	    	}
	    	while (it.hasMoreElements()){
	    		sAux = it.getNext()->getName();
	    		if(Ogre::StringUtil::startsWith(sAux,"LevelGardenSN")){
	    			_sceneMgr->getSceneNode(sAux)->removeAndDestroyAllChildren();
	    			_sceneMgr->destroySceneNode(sAux);
	    		}
	    	}
	    	break;
	    case LevelRoom:
	    	while(iterator.hasMoreElements()){
	    		Ogre::Entity* e = static_cast<Ogre::Entity*>(iterator.getNext());
	    		if(Ogre::StringUtil::startsWith(e->getName(),"LevelRoomEnt")){
	    			_sceneMgr->destroyEntity(e);
	    		}
	    	}
	    	while (it.hasMoreElements()){
	    		sAux = it.getNext()->getName();
	    		if(Ogre::StringUtil::startsWith(sAux,"LevelRoomSN")){
	    			_sceneMgr->getSceneNode(sAux)->removeAndDestroyAllChildren();
	    		 	_sceneMgr->destroySceneNode(sAux);
	    		}
	    	}
	    	break;
	}

	printAll();
	return _vScenario.empty();
}

void PlayState::createScenario(){
	//por si acaso, antes de crear, borrar.
	//deleteScenario();

	std::cout << "Creando escenario " << _nextScenario <<std::endl;
	//Creo el nuevo escenario---
	Entity* _ground = _sceneMgr->getEntity("planeEnt");
	switch(_nextScenario) {
		case Menu:
			/* circle stuff */
			break;
		case LevelGarden:
			for(int i=0;i<3;i++ ){
				String aux=Ogre::StringConverter::toString(i + _numModules);
				Entity* _entScn = _sceneMgr->createEntity("LevelGardenEnt"+aux, "escenario2.mesh");
				SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("LevelGardenSN"+aux);
				_nodeScn->attachObject(_entScn);
				_nodeScn->yaw(Degree(270));
				_nodeScn->setScale(Vector3(2.5,2.5,2.5));
				_nodeScn->translate(Vector3(230*i,0,0));

				_vScenario.push_back(_nodeScn);
			}
			_ground->setMaterialName("Ground");
			_currentScenario = _nextScenario;
			_nextScenario = LevelRoom;
			_numModules += 3;
			break;
		case LevelRoom:
			for(int i=0;i<3;i++ ){
				String aux=Ogre::StringConverter::toString(i + _numModules);
				Entity* _entScn = _sceneMgr->createEntity("LevelRoomEnt"+aux, "escenario.mesh");
				SceneNode*_nodeScn = _sceneMgr->getRootSceneNode()->createChildSceneNode("LevelRoomSN"+aux);
				_nodeScn->attachObject(_entScn);
				_nodeScn->yaw(Degree(270));
				_nodeScn->setScale(Vector3(2.5,2.5,2.5));
				_nodeScn->translate(Vector3(230*i,0,0));

				_vScenario.push_back(_nodeScn);
			}
			_ground->setMaterialName("GroundRoom");
			_currentScenario = _nextScenario;
			_nextScenario = LevelGarden;
			_numModules += 3;
			break;
	}
	//-------------------------------
	std::cout << "currentScenario: "<< _currentScenario << " nextScenario "<< _nextScenario <<std::endl;
	printAll();
}

void PlayState::printAll(){
	SceneNode::ChildNodeIterator it = _sceneMgr->getRootSceneNode()->getChildIterator();
	std::cout << "Printing all children of root" << std::endl;
	while (it.hasMoreElements()){
		String  _aux = it.getNext()->getName();
		std::cout << "	-"<< _aux << std::endl;
	}
}

void PlayState::createAllWalls(){
	Ogre::Vector3 position(0,1.5,0);
	WallType name;
	GameEntity* gameEntity = new GameEntity();
	Wall* wall;
	_walls = new std::vector<Wall*>();
	_walls->clear();

	//Muro de la izquierda
	position.z = WALLL_POSITION_Z;
	position.y = WALLL_POSITION_Y;
	name = LeftWall;
	gameEntity = createGameEntity("WallL", "cube.mesh", position);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	_walls->push_back(wall);

	//Muro de la derecha
	position.z = WALLR_POSITION_Z;
	position.y = WALLR_POSITION_Y;
	name = RightWall;
	gameEntity = createGameEntity("WallR", "cube.mesh", position);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	_walls->push_back(wall);

	//Suelo
	position.z = FLOOR_POSITION_Z;
	position.y = FLOOR_POSITION_Y;
	name = Floor;
	gameEntity = createGameEntity("Floor", "cube.mesh", position);
	wall = new Wall();
	wall->setSceneNode(gameEntity->getSceneNode());
	wall->setRigidBody(gameEntity->getRigidBody());
	_walls->push_back(wall);

}

GameEntity* PlayState::createGameEntity(std::string name, std::string mesh, Ogre::Vector3 position){
	std::cout << "Creando GameEntity " << name << std::endl;

	GameEntity* gameEntity;
	Ogre::SceneNode* node = _sceneMgr->getRootSceneNode()->createChildSceneNode("SN_" + name);
	Entity *entity = _sceneMgr->createEntity("E_" + name, "cube.mesh");
	node->attachObject(entity);

	OgreBulletDynamics::RigidBody* rigidBody;
	OgreBulletCollisions::BoxCollisionShape* bodyShape;

	if(Ogre::StringUtil::startsWith(name,"Wall") || Ogre::StringUtil::startsWith(name,"Floor")){
		if(Ogre::StringUtil::startsWith(name,"WallB")){
			//para la pared trasera, (1,10,20) quiza
			node->scale(1,10,20);
			bodyShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(1,10,20));
		}
		else if(Ogre::StringUtil::startsWith(name,"Floor")){
			//para el suelo, (40,1,20) quiza (cambiar el 20 por la longitud de 2 0 3 modulos)
			node->scale(40,1,16);
			bodyShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(40,1,16));
		}
		else{
			node->scale(40,10,1); //para paredes laterales //cambiar el 40 por la longitud de 2 o 3 modulos de nivel
			bodyShape = new OgreBulletCollisions::BoxCollisionShape(Ogre::Vector3(40,10,1));
		}

		rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name, _world);
		rigidBody->setShape(node, bodyShape, 0.0f /*Restitucion*/, 0.6f/*Friccion*/, 100.0f/*Masa*/, position);
	}
	else{
		OgreBulletCollisions::StaticMeshToShapeConverter *trimeshConverter =
					new OgreBulletCollisions::StaticMeshToShapeConverter(entity);
		OgreBulletCollisions::CollisionShape *bodyShape = trimeshConverter->createConvex();

		OgreBulletDynamics::RigidBody* rigidBody = new OgreBulletDynamics::RigidBody("RB_" + name, _world);

		rigidBody->setShape(node, bodyShape,
				0.0 /* Restitucion */, 0.9 /* Friccion */,
				100.0 /* Masa */, position /* Posicion inicial */,
				Quaternion::IDENTITY /* Orientacion */);
	}


	rigidBody->getBulletRigidBody()->setAngularFactor(btVector3(0,0,0));
	rigidBody->disableDeactivation();

	gameEntity = new GameEntity();
	gameEntity->setSceneNode(node);
	gameEntity->setRigidBody(rigidBody);

	return gameEntity;
}
