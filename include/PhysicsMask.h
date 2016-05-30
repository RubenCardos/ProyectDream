#ifndef PhysicsMask_H
#define PhysicsMask_H

namespace PhysicsMask{

//MASCARAS DE COLISION---
enum collisiontypes {
    COL_Player = 1,
    COL_Walls = 2,
    COL_Thread = 3,
    COL_Reel = 4,
	COL_Enemy = 5,
	COL_StaticWalls=6
};
//-----------------------

//GRUPOS DE COLISION-----
const short player_collides_with = COL_Walls | COL_StaticWalls | COL_Thread  | COL_Reel | COL_Enemy ;
const short enemy_collides_with =  COL_Walls | COL_StaticWalls | COL_Player ;
const short walls_collides_with =  COL_StaticWalls;
//-----------------------

}

#endif
