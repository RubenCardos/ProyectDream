#ifndef PhysicsMask_H
#define PhysicsMask_H

namespace PhysicsMask{

//MASCARAS DE COLISION---
enum collisiontypes {
    COL_Player = 1,
    COL_Walls = 2,
    COL_Thread = 4,
    COL_Reel = 8,
	COL_Enemy = 16,
	COL_StaticWalls=32,
	COL_Boss = 64,
	COL_Obs = 128,
	COL_BackWall = 256
};
//-----------------------

//GRUPOS DE COLISION-----
const short player_collides_with = COL_Walls | COL_StaticWalls | COL_Thread  | COL_Reel | COL_Enemy | COL_Boss | COL_Obs | COL_BackWall;
const short enemy_collides_with =  COL_Walls | COL_StaticWalls | COL_Player | COL_Obs ;
const short walls_collides_with =  COL_StaticWalls | COL_Player | COL_Enemy | COL_Walls | COL_Boss;
const short boss_collides_with = COL_StaticWalls | COL_Walls | COL_Player | COL_Boss;
const short staticwalls_collides_with =  COL_Walls | COL_Player | COL_Enemy | COL_Boss | COL_Obs |COL_BackWall;
const short thread_collides_with = COL_Player;
const short reel_collides_with = COL_Player;
const short obs_collides_with = COL_Player | COL_Enemy | COL_StaticWalls | COL_Obs;
const short backwall_collides_with = COL_Player | COL_StaticWalls;
//-----------------------

}

#endif
