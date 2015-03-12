/***************************************************************************/
/*                                                                         */
/*  File: Game.h                                                           */
/*  Date: 06-02-07                                                         */
/*                                                                         */
/***************************************************************************/
/*

*/
/***************************************************************************/
#ifndef GAME_H
#define GAME_H



// Called Once at Game Startup
bool Create();

// Called Once when Game Ends and program closes
bool Release();

// Called for each frame update
void Update();


// Timing value declared in Game.cpp
extern float g_timeStep;




#endif //GAME_H

