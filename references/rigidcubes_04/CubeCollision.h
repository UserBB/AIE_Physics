/***************************************************************************/
/*                                                                         */
/*  File: CubeCollision.h                                                  */
/*  Date: 10-11-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
	Because of the complexity of doing OBB-OBB collision detection, with
	the added determination of collision features, it was put into its own
	file.

	Basically, you pass in two OBB cubes, and the code will determine the
	collision points and the penetration depth and the direction of the
	collision to push the cubes appart.

	Its probably more complicated than it should be, and definetly needs a
	rewrite at some later date.
*/
/***************************************************************************/
#ifndef CUBECOLLISION_H
#define CUBECOLLISION_H

#include "Cube.h"

bool CubeCubeCollisionCheck(const Cube&		box0, 
							const Cube&		box1,
							D3DXVECTOR3*	hitPoints,
							int*			numHitPoints,
							float*			penetration,
							D3DXVECTOR3*	hitNormal);


//inline float Clamp(float v, float min, float max)
//{
//	float res = v;
//	res = v>max ? max : v;
//	res = v<min ? min : v;
//	return res;
//};



#endif //CUBECOLLISION_H

