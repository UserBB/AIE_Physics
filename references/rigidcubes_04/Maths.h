/***************************************************************************/
/*                                                                         */
/*  File: Maths.h                                                          */
/*  Date: 24-10-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
*/
/***************************************************************************/
#ifndef MATHS_H
#define MATHS_H


#include <d3dx9.h>				// Direct X Header Files

								// Direct X Library Files
#pragma comment(lib, "d3d9.lib")	
#pragma comment(lib, "d3dx9.lib")


 struct stModelData
 {
    int   numIndices;
    int   numVerts;
    int   *i;
    float *v;
 };




inline D3DXVECTOR3 Cross(const D3DXVECTOR3& vA, const D3DXVECTOR3& vB)
{
	D3DXVECTOR3 temp;
	D3DXVec3Cross(&temp, &vA, &vB);
	return temp;
}

inline float Dot(const D3DXVECTOR3& vA, const D3DXVECTOR3& vB)
{
	return D3DXVec3Dot(&vA, &vB);
}

inline D3DXVECTOR3 operator* (D3DXVECTOR3& vA, D3DXMATRIX& m)
{
	D3DXVECTOR3 temp;
	D3DXVec3TransformCoord(&temp, &vA, &m);
	return temp;
}

inline D3DXMATRIX Inverse(D3DXMATRIX& m)
{
	D3DXMATRIX temp;
	D3DXMatrixInverse(&temp, NULL, &m);
	return temp;
}

inline D3DXVECTOR3 Normalize(D3DXVECTOR3& vA)
{
	D3DXVECTOR3 temp;
	D3DXVec3Normalize(&temp, &vA);
	return temp;
}


inline float Clamp(float v, float min, float max)
{
	float res = v;
	res = v>max ? max : v;
	res = v<min ? min : v;
	return res;
};


// Random number in range [-1,1]
inline float Random()
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = 2.0f * r - 1.0f;
	return r;
}

inline float Random(float lo, float hi)
{
	float r = (float)rand();
	r /= RAND_MAX;
	r = (hi - lo) * r + lo;
	return r;
}

inline D3DXVECTOR3 Random(const D3DXVECTOR3& min, const D3DXVECTOR3& max)
{
	D3DXVECTOR3 res = D3DXVECTOR3( Random(min.x, max.x),
								   Random(min.y, max.y),
								   Random(min.z, max.z));
	return res;
}

inline DWORD RandomColour()
{
	DWORD a = (0xff<<24);
	DWORD r = (((unsigned char)(255.0f * Random(0,1)))<<16);
	DWORD g = (((unsigned char)(255.0f * Random(0,1)))<<8);
	DWORD b = (((unsigned char)(255.0f * Random(0,1)))<<0);

	DWORD col = a | r | g | b;
				
	return col;
}



#endif //MATHS_H



