/*********************************************************************
 * Módulo 1. Curso de Experto en Desarrollo de Videojuegos
 * Autor: David Vallejo Fernández    David.Vallejo@uclm.es
 *
 * Código modificado a partir de Managing Game States with OGRE
 * http://www.ogre3d.org/tikiwiki/Managing+Game+States+with+OGRE
 * Inspirado en Managing Game States in C++
 * http://gamedevgeek.com/tutorials/managing-game-states-in-c/
 *
 * You can redistribute and/or modify this file under the terms of the
 * GNU General Public License ad published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * and later version. See <http://www.gnu.org/licenses/>.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.  
 *********************************************************************/

#ifndef PlayState_H
#define PlayState_H

#include <Ogre.h>
#include <OIS/OIS.h>

#include "GameState.h"
#include "MovementManager.h"
#include "PhysicsManager.h"
#include "AI_Manager.h"
#include "AnimationManager.h"
#include "Enemy.h"
#include "Hero.h"
#include "Wall.h"

#include <CEGUI.h>
#include <RendererModules/Ogre/Renderer.h>

#include <OgreBulletDynamicsRigidBody.h>
#include <Shapes/OgreBulletCollisionsStaticPlaneShape.h>
#include <Shapes/OgreBulletCollisionsBoxShape.h>

#include <string>
#include "GameEntity.h"
#include "PhysicsMask.h"
#include "Scenario.h"

using namespace Ogre;

class PlayState : public Ogre::Singleton<PlayState>, public GameState{
 public:
  PlayState () {}

  void enter ();
  void exit ();
  void pause ();
  void resume ();
  
  void keyPressed (const OIS::KeyEvent &e);
  void keyReleased (const OIS::KeyEvent &e);

  void mouseMoved (const OIS::MouseEvent &e);
  void mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
  void mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);

  bool frameStarted (const Ogre::FrameEvent& evt);
  bool frameEnded (const Ogre::FrameEvent& evt);

  void changeScenario(Scenario::Scenario nextScenario);
  
  // Heredados de Ogre::Singleton.
  static PlayState& getSingleton ();
  static PlayState* getSingletonPtr ();

  CEGUI::MouseButton convertMouseButton(OIS::MouseButtonID id);
  
  void createBoss();
  void readEnemies(String path);
  void printAll();

  Scenario::Scenario getCurrentScenario();

 protected:
  Ogre::Root* _root;
  Ogre::SceneManager* _sceneMgr;
  Ogre::Viewport* _viewport;
  Ogre::Camera* _camera;
  Ogre::AnimationState *_animEnemy;
  Ogre::SceneNode* aureolaNode;
  
  Real _deltaT;
  bool _exitGame;

  OgreBulletDynamics::DynamicsWorld * _world;
  OgreBulletCollisions::DebugDrawer * _debugDrawer;
  
  int _numEntities;
  float _timeLastObject;
  float _arrowSpeed;
  bool _bossRoom;
  
  Hero* _hero;
  std::vector<Ogre::SceneNode*> _vScenario; //Vector que representa el escenario, compuesto por sceneNodes (Luego habrá que meterle cuerpos fisicos tambien);
  std::vector<Enemy*> _enemies;
  std::vector<Wall*> _walls;
  std::vector<GameEntity*> _gameEntities;
  std::vector<Boss*> _bossPieces;
  std::vector<Ogre::Vector3> _posEnemies;
  std::vector<std::string> _enemyTypes;

  MovementManager* _movementManager;
  PhysicsManager* _physicsManager;
  AnimationManager* _animationManager;

  Scenario::Scenario _currentScenario;
  Scenario::Scenario _nextScenario;
  int _numModules;
  bool _wallsAreVisible;
  bool _bossCreated;

  SceneNode* _cameraPivot;
  int _frameCounter;

  void CreateInitialWorld();

  void createGUI();
  void updateGUI();
  
  bool deleteCurrentScenario();
  void deleteScenarioContent();
  void createScenario(Scenario::Scenario nextScenario);
  void createAllWalls();
  void createBossRoom();

  void createTestGameEntities();
  GameEntity* createGameEntity(std::string name, std::string mesh, Ogre::Vector3 position, Ogre::Vector3 scale, double mass);
  OgreBulletDynamics::RigidBody* createRigidBody(Ogre::SceneNode* node, Ogre::Vector3 scale,Ogre::Vector3 position, double mass);

  void populateObstacles(String _path);
  void populateThreads(String _path);
  void populateSpike(String _path);
  void populateEnemies();

  void removeAllBehindBackWall();
  bool quit(const CEGUI::EventArgs &e);
  bool pauseB(const CEGUI::EventArgs &e);

};

#endif
