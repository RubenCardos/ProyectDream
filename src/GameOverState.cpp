#include "GameOverState.h"
#include "PlayState.h"
#include "IntroState.h"
#include "MenuState.h"

using namespace std;

template<> GameOverState* Ogre::Singleton<GameOverState>::msSingleton = 0;

string _name="";

void
GameOverState::enter ()
{
  _root = Ogre::Root::getSingletonPtr();

  // Se recupera el gestor de escena y la cámara.----------------
  _sceneMgr = _root->getSceneManager("SceneManager");
  _camera = _sceneMgr->createCamera("GameOverCamera");
  _viewport = _root->getAutoCreatedWindow()->addViewport(_camera);
  _sceneMgr->setAmbientLight(Ogre::ColourValue(0.4, 0.4, 0.4));
  
   
  //Camara--------------------
  _camera->setPosition(Ogre::Vector3(15,5,15));
  _camera->lookAt(Ogre::Vector3(0,5,0));
  _camera->setNearClipDistance(5);
  _camera->setFarClipDistance(10000);
  //-----------------------------
 
  createGUI();
  cout << "Creo GUI\n" << endl;
  _exitGame = false;

  cout << "Entro a GAME OVER!!!\n" << endl;
}

void
GameOverState::exit ()
{
  cout << "salgo de GAME OVER\n" << endl;
  //Salgo del estado------------------------------
  _sceneMgr->clearScene();
  _sceneMgr->destroyCamera("GameOverCamera");
  _root->getAutoCreatedWindow()->removeAllViewports();
  //--------------------------------------------

  //Destruyo GUI------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  sheet->destroyChild("sheetGameOver");
  //------------------------
}

void
GameOverState::pause()
{
}

void
GameOverState::resume()
{
  
}

void 
GameOverState::CreateInitialWorld() {

}

bool
GameOverState::frameStarted
(const Ogre::FrameEvent& evt)
{ 
  _deltaT = evt.timeSinceLastFrame;
  updateGUI();
  //cout << "frameStarted\n" << endl;
  //cout << Ogre::StringConverter::toString(GameManager::getSingletonPtr()->getPunt()) << endl;
  return true;
}


bool
GameOverState::frameEnded
(const Ogre::FrameEvent& evt)
{

  _deltaT = evt.timeSinceLastFrame;
  //cout << "FRAMEENDED\n" << endl;

  //Salir del juego--
  if (_exitGame)
    return false;
  //-----------------
  return true;
}

void
GameOverState::keyPressed
(const OIS::KeyEvent &e)
{ 

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectKeyDown(static_cast<CEGUI::Key::Scan>(e.key));
  CEGUI::System::getSingleton().getDefaultGUIContext().injectChar(e.text);
  //-------------------------------
}

void
GameOverState::keyReleased
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
GameOverState::mouseMoved
(const OIS::MouseEvent &e)
{

  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseMove(e.state.X.rel, e.state.Y.rel);
  //-------------------------------
}

void
GameOverState::mousePressed
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonDown(convertMouseButton(id));
  //-------------------------------
  
}

void
GameOverState::mouseReleased
(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
  //CEGUI--------------------------
  CEGUI::System::getSingleton().getDefaultGUIContext().injectMouseButtonUp(convertMouseButton(id));
  //-------------------------------

}

GameOverState*
GameOverState::getSingletonPtr ()
{
return msSingleton;
}

GameOverState&
GameOverState::getSingleton ()
{ 
  assert(msSingleton);
  return *msSingleton;
}

CEGUI::MouseButton GameOverState::convertMouseButton(OIS::MouseButtonID id)//METODOS DE CEGUI
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

bool
GameOverState::quit(const CEGUI::EventArgs &e)
{
  _exitGame=true;
  return true;
}


void
GameOverState::createGUI()
{
  //Limpiar interfaz del estado anterior-------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

  CEGUI::Window* sheetGameOver = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticImage","sheetGameOver");
  sheetGameOver->setPosition(CEGUI::UVector2(cegui_reldim(0),cegui_reldim(0)));
  sheetGameOver->setSize(CEGUI::USize(cegui_reldim(1),cegui_reldim(1)));
  sheetGameOver->setProperty("Image","BackgroundImageGameOver");
  sheetGameOver->setProperty("FrameEnabled","False");
  sheetGameOver->setProperty("BackgroundEnabled", "False");

  CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(sheetGameOver->createChild("OgreTray/Editbox","ebox"));
  eb->setPosition(CEGUI::UVector2(CEGUI::UDim(0.68f, 0.0f),CEGUI::UDim(0.35f, 0)));
  eb->setSize(CEGUI::USize(CEGUI::UDim(0.25,0),CEGUI::UDim(0.07,0)));
  eb->setFont("SPIDER MONKEY-18");
  
  CEGUI::Window* acceptButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","acceptButton");
  acceptButton->setText("[font='SPIDER MONKEY-18'] Accept");
  acceptButton->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.06,0)));
  acceptButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55f, 0.0f),CEGUI::UDim(0.62f, 0)));
  acceptButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&GameOverState::accept,this));

  CEGUI::Window* menuButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","MenuButton");
  menuButton->setText("[font='SPIDER MONKEY-18'] Play Again");
  menuButton->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.06,0)));
  menuButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.55,0),CEGUI::UDim(0.70,0)));
  menuButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&GameOverState::goBackMenu,this));

  CEGUI::Window* exitButton = CEGUI::WindowManager::getSingleton().createWindow("OgreTray/Button","exitButtonGO");
  exitButton->setText("[font='SPIDER MONKEY-18'] Exit");
  exitButton->setSize(CEGUI::USize(CEGUI::UDim(0.20,0),CEGUI::UDim(0.06,0)));
  exitButton->setPosition(CEGUI::UVector2(CEGUI::UDim(0.78,0),CEGUI::UDim(0.70,0)));
  exitButton->subscribeEvent(CEGUI::PushButton::EventClicked,CEGUI::Event::Subscriber(&GameOverState::quit,this));

  CEGUI::Window* text = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text");
  text->setText("[font='SPIDER MONKEY-18']"+IntroState::getSingletonPtr()->readRecords());
  text->setSize(CEGUI::USize(CEGUI::UDim(0.30,0),CEGUI::UDim(0.50,0)));
  text->setPosition(CEGUI::UVector2(CEGUI::UDim(0.10,0),CEGUI::UDim(0.32,0)));
  text->setProperty("FrameEnabled","False");
  text->setProperty("BackgroundEnabled", "False");
  text->setProperty("HorzFormatting", "CenterAligned");

  CEGUI::Window* textRecords = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","text2");
  textRecords->setText("[font='SPIDER MONKEY-28'] Records");
  textRecords->setSize(CEGUI::USize(CEGUI::UDim(0.30,0),CEGUI::UDim(0.50,0)));
  textRecords->setPosition(CEGUI::UVector2(CEGUI::UDim(0.15,0),CEGUI::UDim(0.10,0)));
  textRecords->setProperty("FrameEnabled","False");
  textRecords->setProperty("BackgroundEnabled", "False");
  textRecords->setProperty("HorzFormatting", "CenterAligned");

  CEGUI::Window* textScores = CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText","textScores");
  textScores->setText("[font='SPIDER MONKEY-50']"+Ogre::StringConverter::toString(GameManager::getSingletonPtr()->getPunt()));
  textScores->setSize(CEGUI::USize(CEGUI::UDim(0.40,0),CEGUI::UDim(0.15,0)));
  textScores->setPosition(CEGUI::UVector2(CEGUI::UDim(0.68,0),CEGUI::UDim(0.48,0)));
  textScores->setProperty("FrameEnabled","False");
  textScores->setProperty("BackgroundEnabled", "False");
  textScores->setProperty("HorzFormatting", "CenterAligned");

  sheetGameOver->addChild(exitButton);
  sheetGameOver->addChild(text);
  sheetGameOver->addChild(textRecords);
  sheetGameOver->addChild(menuButton);
  sheetGameOver->addChild(acceptButton);
  sheetGameOver->addChild(eb);
  sheetGameOver->addChild(textScores);
  sheet->addChild(sheetGameOver);
  

}

bool
GameOverState::accept(const CEGUI::EventArgs &e)
{
  //Obtengo el nombre ----------------------------------------------------------------------
  CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
  _name=sheet->getChild("sheetGameOver")->getChild("ebox")->getText().c_str() ;
  //-----------------------------------------------------------------------------------------

  if(_name!="" ){
    if(_name!=" "){
      requestScore();
      sheet->getChild("sheetGameOver")->getChild("acceptButton")->setVisible(false); 
      CEGUI::Editbox* eb = static_cast<CEGUI::Editbox*>(sheet->getChild("sheetGameOver")->getChild("ebox"));
      eb->setReadOnly(true); //Cuando le damos aceptar la ventana se activa a solo lectura.
    }
  }
  
  sheet->getChild("sheetGameOver")->getChild("text")->setText("[font='SPIDER MONKEY-18']"+IntroState::getSingletonPtr()->readRecords());
  
  return true;
}

bool 
GameOverState::goBackMenu(const CEGUI::EventArgs &e)
{
  changeState(PlayState::getSingletonPtr());
  return true;
}

void
GameOverState::updateGUI()
{

  if(GameManager::getSingletonPtr()->getWin()){
    CEGUI::Window* sheet=CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();
    sheet->getChild("sheetGameOver")->setProperty("Image","BackgroundImageWinner");
  }
   
}

void
GameOverState::requestScore()
{
    int puntuacion=GameManager::getSingletonPtr()->getPunt();

    fstream fichero;//Fichero
    string frase;//Auxiliar
    bool reWrite=false;//Si debemos re-escribir el fichero o no
    String nombre;
    std::vector <String> puntuaciones;//vector para guardar puntuaciones

    fichero.open ( "Records.txt" , ios::in); //Suponemos que ya existe
    if (fichero.is_open()) {//se debe lanzar primero el metodo de lectura para comprobar que existe el fichero
      while (getline (fichero,frase)) {
        String nombre=_name;
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        if(puntuacion>punt && reWrite == false){
          cout << "Nuevo Record!!!"<< endl;
          reWrite=true;
          
          String aux=nombre+" "+Ogre::StringConverter::toString(puntuacion);
          cout << aux << endl;
          puntuaciones.push_back(aux);
        }
        puntuaciones.push_back(frase);
      }
      fichero.close();    
    }
    else{

      IntroState::getSingletonPtr()->createRecordsFile();

      fichero.open ( "Records.txt" , ios::in);
      while (getline (fichero,frase)) {
        String nombre=Ogre::StringUtil::split (frase," ")[0];
        int punt = Ogre::StringConverter::parseInt(Ogre::StringUtil::split (frase," ")[1]);
        cout << "\tNombre: "<< nombre << " Puntuacion " << punt << endl;
        if(puntuacion>punt && reWrite == false){
          cout << "Nuevo Record!!!"<< endl;
          reWrite=true;

          cout << "\nNombre?\n" << endl;
          //cin >> nombre;
          
          String aux=nombre+" "+Ogre::StringConverter::toString(puntuacion);
          cout << aux << endl;
          puntuaciones.push_back(aux);
        }
        puntuaciones.push_back(frase);
      }
      fichero.close();
    }

    //Se debe re-escribir el fichero
    if(reWrite==true){
      puntuaciones.pop_back();//saco la ultima

      ofstream archivo;  // objeto de la clase ofstream
      archivo.open("Records.txt");
      for(unsigned int i=0;i<puntuaciones.size();i++){
          archivo  << puntuaciones[i] << endl;
      }
      archivo.close();
    }
}

