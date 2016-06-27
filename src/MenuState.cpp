#include "MenuState.h"
#include "PlayState.h"
#include "GameManager.h"
#include "string"
using namespace std;

template<> MenuState* Ogre::Singleton<MenuState>::msSingleton = 0;
std::vector<string> _recorridos;

void
MenuState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("MenuCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
   
  //Camara--------------------
    _camera->setPosition(Ogre::Vector3(-40,10,0));
    _camera->lookAt(Ogre::Vector3(0,0,0));
    _camera->setNearClipDistance(5);
    _camera->setFarClipDistance(10000);
  //-----------------------------

  //Creacion de los elementos iniciales del mundo
  CreateInitialWorld();
  createGUI();

  _exitGame = false;
    
}

void
MenuState::exit ()
{
  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("MenuCamera");
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------

  //Elimino interfaz------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  //-------------------------------------
}

void
MenuState::pause(){}

void
MenuState::resume(){}

void 
MenuState::CreateInitialWorld() {
	//Luces--------------------------------------------------------------
	Light* _sunLight = _sceneMgr->createLight("SunLight");
	_sunLight->setPosition(200, 200, 200);
	_sunLight->setType(Light::LT_SPOTLIGHT);
	_sunLight->setDiffuseColour(.20, .20, 0);
	_sunLight->setSpotlightRange(Degree(30), Degree(50));

	Ogre::Vector3 _dir= -_sunLight->getPosition().normalisedCopy();
	_sunLight->setDirection(_dir);


	Light* _directionalLight = _sceneMgr->createLight("DirectionalLight");
	_directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    _directionalLight->setDirection(Ogre::Vector3(0, -1, 1));
     //------------------------------------------------------------------------------

     //Doors-------------------------------------------------------------------------------
     Entity* _entDoorR = _sceneMgr->createEntity("E_doorRoom","doorRoom.mesh");
     SceneNode* _sneceDoorR = _sceneMgr->createSceneNode("SN_doorRoom");
     _sneceDoorR->attachObject(_entDoorR);
     _sceneMgr->getRootSceneNode()->addChild(_sneceDoorR);
     //---------------------------------------------------------------------

}



bool
MenuState::frameStarted
(const Ogre::FrameEvent& evt)
{ 
  _deltaT = evt.timeSinceLastFrame;
  
  return true;
}


bool
MenuState::frameEnded
(const Ogre::FrameEvent& evt)
{
  _deltaT = evt.timeSinceLastFrame;

  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
MenuState::keyPressed
(const OIS::KeyEvent &e)
{
  // Tecla p --> PlayState.-------
  if (e.key == OIS::KC_P) {
    changeState(PlayState::getSingletonPtr());
  }
  //----------------- 

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
}

void
MenuState::keyReleased
(const OIS::KeyEvent &e)
{
  //Salgo del juego---------------
  if (e.key == OIS::KC_ESCAPE) {
    _exitGame = true;
  }
  //-------------------------------
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyUp(static_cast<CEGUI::Key::Scan>(e.key));
  //-------------------------------
}

void
MenuState::mouseMoved
(const OIS::MouseEvent &e)
{

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
MenuState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
  
}

void
MenuState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------

}

MenuState*
MenuState::getSingletonPtr ()
{
return msSingleton;
}

MenuState&
MenuState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CEGUI::MouseButton MenuState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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
MenuState::createGUI()
{
  //Limpiar interfaz del estado anterior-------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  //-------------------------------------------------------
  

}


void
MenuState::updateGUI()
{

  
}
