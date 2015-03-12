/***************************************************************************/
/*                                                                         */
/*  File: CubeCollision.cpp                                                */
/*  Date: 10-11-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
	See .h file
*/
/***************************************************************************/

#include <math.h>		// cosf(..) sinf(..), sqrt(..)
#include <stdio.h>      // sprintf(..)

#include <windows.h>    // opengl has issues if we don't include it!

#include <d3d9.h>
#include <d3dx9.h>					// DirectX Header Files
#pragma comment(lib, "d3d9.lib")	// DirectX Library Files
#pragma comment(lib, "d3dx9.lib")					

#include <stdio.h>					// sprintf(..)

#include "dxdbg.h"					// Various dx debug drawing functions
#include "Cube.h"
#include "Maths.h"
#include "Game.h"



// Externals

// DirectX Device/Render Handles
extern LPDIRECT3D9			g_pD3D;			// main.cpp
extern LPDIRECT3DDEVICE9	g_pD3DDevice;	// main.cpp


/***************************************************************************/


/***************************************************************************/
/*
bool CubeFloorCollisionCheck(const Cube& box0, 
							 D3DXVECTOR3* hitPoint,
							 float* penetration,
							 D3DXVECTOR3* hitNormal)
{

	float x = box0.m_e.x;
	float y = box0.m_e.y;
	float z = box0.m_e.z;

	D3DXVECTOR3 Vertex[8] = 
	{
		D3DXVECTOR3(   x,    y,   z	),
		D3DXVECTOR3(  -x,    y,	  z	),
		D3DXVECTOR3(   x,   -y,	  z	),
		D3DXVECTOR3(  -x,   -y,	  z	),

		D3DXVECTOR3(   x,    y,	 -z	),
		D3DXVECTOR3(  -x,    y,	 -z	),
		D3DXVECTOR3(   x,   -y,	 -z	),
		D3DXVECTOR3(  -x,   -y,	 -z	)
	};

	for (int i=0; i<8; i++)
	{
		D3DXVec3TransformCoord(&Vertex[i], &Vertex[i], &box0.m_matWorld);
	}


	float penDist = 0.0f;

	D3DXVECTOR3 verts[8];
	int numVerts = 0;

	for (int i=0; i<8; i++)
	{
		float d = D3DXVec3Dot(&Vertex[i], &D3DXVECTOR3(0,1,0));

		if (d < 0.0f)
		{
			verts[numVerts] = Vertex[i];
			numVerts++;
			penDist = max(penDist, abs(d));
		}
	}

	// Check if we have had a collision
	if (numVerts > 0)
	{
		*hitNormal = D3DXVECTOR3(0,1,0);
		*penetration = penDist;

		*hitPoint = D3DXVECTOR3(0,0,0);
		for (int i=0; i<numVerts; i++)
		{
			*hitPoint += verts[i];
		}
		*hitPoint = (*hitPoint) * (1.0f/numVerts);

		return true;
	}

	// No Collision
	return false;
}
*/

/***************************************************************************/

inline void CalculateInterval(const Cube& box, const D3DXVECTOR3& axis, float& min, float& max)
{
	float x = box.m_e.x;
	float y = box.m_e.y;
	float z = box.m_e.z;
	D3DXVECTOR3 Vertex[8] = 
	{
		D3DXVECTOR3(   x,    y,   z	),
		D3DXVECTOR3(  -x,    y,	  z	),
		D3DXVECTOR3(   x,   -y,	  z	),
		D3DXVECTOR3(  -x,   -y,	  z	),

		D3DXVECTOR3(   x,    y,	 -z	),
		D3DXVECTOR3(  -x,    y,	 -z	),
		D3DXVECTOR3(   x,   -y,	 -z	),
		D3DXVECTOR3(  -x,   -y,	 -z	)
	};

	for (int i=0; i<8; i++)
	{
		D3DXVec3TransformCoord(&Vertex[i], &Vertex[i], &box.m_matWorld);
	}

	min = max = D3DXVec3Dot(&Vertex[0], &axis);
	for (int i=0; i<8; i++)
	{
		float d = D3DXVec3Dot(&Vertex[i], &axis);

		if (d<min) min = d;
		if (d>max) max = d;
	}
}

/***************************************************************************/

inline bool SpanIntersect(	const Cube& box0, 
							const Cube& box1,
							const D3DXVECTOR3& axisc,
							float* minPenetration = NULL,
							D3DXVECTOR3* axisPenetration = NULL,
							float* pen = NULL)
{

	D3DXVECTOR3 axis = axisc;

	float lq = D3DXVec3LengthSq(&axis);
	if (lq <= 0.02f)
	{
		if (pen) *pen = 100000.0f;
		return true;
	}

	axis = Normalize(axis);

	float mina, maxa;
	float minb, maxb;
	CalculateInterval(box0, axis, mina, maxa);
	CalculateInterval(box1, axis, minb, maxb);

	float lena = maxa - mina;
	float lenb = maxb - minb;

	float minv = min( mina, minb );
	float maxv = max( maxa, maxb );
	float lenv = maxv - minv;

	if ( lenv > (lena + lenb) )
	{
		// Collision
		return false;
	}

	/*
	// You could do this test, but I prefair the above
	// method
	if (mina>maxb || minb>maxa)
	{
		return false;
	}
	*/

	float penetration = (lena + lenb) - lenv;

	if (pen)
	{
		*pen = penetration;
	}

	if (minPenetration && axisPenetration)
	{
		if (penetration < *minPenetration)
		{
			*minPenetration  = penetration;
			*axisPenetration = axis;

			// BoxA pushes BoxB away in the correct Direction
			if (minb < mina)
			{
				*axisPenetration *= -1;
			}
		}
	}


	// Colllision
	return true;
}

/***************************************************************************/

inline int GetNumHitPoints( const Cube& box0,
							const D3DXVECTOR3& hitNormal,
							const float penetration,
							D3DXVECTOR3 verts[8],
							int* vertIndexs)
{

	{
		DrawLine3D(g_pD3DDevice, box0.m_c, (box0.m_c + hitNormal*3.0f), 0xffff00ff);
	}


	float x = box0.m_e.x;
	float y = box0.m_e.y;
	float z = box0.m_e.z;
	D3DXVECTOR3 Vertex[8] = 
	{
		D3DXVECTOR3(  -x,   -y,   -z	),
		D3DXVECTOR3(   x,   -y,	  -z	),
		D3DXVECTOR3(   x,   -y,	   z	),
		D3DXVECTOR3(  -x,   -y,	   z	),

		D3DXVECTOR3(  -x,    y,	  -z	),
		D3DXVECTOR3(   x,    y,	  -z	),
		D3DXVECTOR3(   x,    y,	   z	),
		D3DXVECTOR3(  -x,    y,	   z	)
	};

	for (int i=0; i<8; i++)
	{
		D3DXVec3TransformCoord(&Vertex[i], &Vertex[i], &box0.m_matWorld);
	}

	D3DXVECTOR3 planePoint = Vertex[0];
	float maxdist = D3DXVec3Dot(&Vertex[0], &hitNormal);

	for (int i=0; i<8; i++)
	{
		float d = D3DXVec3Dot(&Vertex[i], &hitNormal);
		if (d > maxdist)
		{
			maxdist = d;
			planePoint = Vertex[i];
		}
	}

	// Plane Equation (A dot N) - d = 0;

	float d = D3DXVec3Dot(&planePoint, &hitNormal);
	d -= penetration + 0.01f;


	int numVerts = 0;
	for (int i=0; i<8; i++)
	{
		float side = D3DXVec3Dot(&Vertex[i], &hitNormal) - d;

		if ( side > 0 )
		{
			verts[numVerts] = Vertex[i];
			vertIndexs[numVerts] = i;
			numVerts++;
		}
	}

	return numVerts;
}

/***************************************************************************/

inline void SortVertices(D3DXVECTOR3* verts0,
						 int* vertIndexs0)
{
	int faces[6][4] =
	{
		{4,0,3,7},
		{1,5,6,2},
		{0,1,2,3},
		{7,6,5,4},
		{5,1,0,4},
		{6,7,3,2}
	};

	int faceSet = -1;
	D3DXVECTOR3 temp[4]; // New correct clockwise order

	// Work out which face to use
	for (int i=0; i<6 && faceSet==-1; i++)
	{
		int numFound = 0;
		for (int j=0; j<4; j++)
		{
			if (vertIndexs0[j]==faces[i][j])
			{
				temp[numFound] = verts0[j];
				numFound++;

				if (numFound==4)
				{
					faceSet = i;
					break;
				}
			}
		}
	}

	if (faceSet < 0)
	{
		int errorHasOccured = 1;
	}
	else
	{
		for (int i=0; i<4; i++)
		{
			verts0[i] = temp[i];
		}
	}
}

/***************************************************************************/

inline bool VertInsideFace(D3DXVECTOR3* verts0, D3DXVECTOR3& p0, float planeErr = 0.0f)
{
	// Work out the normal for the face
	D3DXVECTOR3 v0 = verts0[1] - verts0[0];
	D3DXVECTOR3 v1 = verts0[2] - verts0[0];
	D3DXVECTOR3 n  = Cross(v1, v0);
	n = Normalize(n);

	for (int i=0; i<4; i++)
	{
		D3DXVECTOR3 s0 = verts0[i];
		D3DXVECTOR3 s1 = verts0[(i+1)%4];
		D3DXVECTOR3 sx = s0 + n*10.0f;

		// Work out the normal for the face
		D3DXVECTOR3 sv0 = s1 - s0;
		D3DXVECTOR3 sv1 = sx - s0;
		D3DXVECTOR3 sn  = Cross(sv1, sv0);
		sn = Normalize(sn);

		float d  = Dot(s0, sn);
		float d0 = Dot(p0, sn) - d;

		// Outside the plane
		if (d0 > planeErr)
		{
			return false;
		}
	}

	return true;
}


inline void ClipFaceFaceVerts(	D3DXVECTOR3* verts0,
								int* vertIndexs0,
								D3DXVECTOR3* verts1,
								int* vertIndexs1,
								D3DXVECTOR3* vertsX,
								int* numVertsX)
{
	SortVertices(verts0, vertIndexs0);
	SortVertices(verts1, vertIndexs1);

	// Work out the normal for the face
	D3DXVECTOR3 v0 = verts0[1] - verts0[0];
	D3DXVECTOR3 v1 = verts0[2] - verts0[0];
	D3DXVECTOR3 n  = Cross(v1, v0);
	n = Normalize(n);

	// Project all the vertices onto a shared plane, plane0
	D3DXVECTOR3 vertsTemp1[4];
	for (int i=0; i<4; i++)
	{
		vertsTemp1[i] = verts1[i] + (n * Dot(n, verts0[0]-verts1[i]));
	}


	static D3DXVECTOR3 temp[50];
	int numVerts = 0;

	for (int c=0; c<2; c++)
	{
		D3DXVECTOR3* vertA = vertsTemp1;
		D3DXVECTOR3* vertB = verts0;
		if (c==1)
		{
			vertA = verts0;
			vertB = vertsTemp1;
		}

		// Work out the normal for the face
		D3DXVECTOR3 v0 = vertA[1] - vertA[0];
		D3DXVECTOR3 v1 = vertA[2] - vertA[0];
		D3DXVECTOR3 n  = Cross(v1, v0);
		n = Normalize(n);

		for (int i=0; i<4; i++)
		{
			D3DXVECTOR3 s0 = vertA[i];
			D3DXVECTOR3 s1 = vertA[(i+1)%4];
			D3DXVECTOR3 sx = s0 + n*10.0f;

			// Work out the normal for the face
			D3DXVECTOR3 sv0 = s1 - s0;
			D3DXVECTOR3 sv1 = sx - s0;
			D3DXVECTOR3 sn  = Cross(sv1, sv0);
			sn = Normalize(sn);

			float d = Dot(s0, sn);


			for (int j=0; j<4; j++)
			{
				D3DXVECTOR3 p0 = vertB[j];
				D3DXVECTOR3 p1 = vertB[(j+1)%4]; // Loops back to the 0th for the last one

				float d0 = Dot(p0, sn) - d;
				float d1 = Dot(p1, sn) - d;

				// Check there on opposite sides of the plane
				if ( (d0 * d1) < 0.0f)
				{
					D3DXVECTOR3 pX = p0   +   ( Dot(sn, (s0-p0)) /  Dot(sn, (p1-p0)) )  *  (p1 - p0);

					if (VertInsideFace(vertA, pX, 0.1f))
					{
						temp[numVerts] = pX;
						numVerts++;
					}

				}

				
				if (VertInsideFace(vertA, p0))
				{
					temp[numVerts] = p0;
					numVerts++;
				}
				

				if (numVerts > 40)
				{
					// We have a fixed array we pass in, which has a max size of 50
					// so if we go past this we'll have memory corruption

					// temp above is size 50 as well
					DBG_HALT;
				}
			}
		}

		

	}

	// Remove verts which are very close to each other
	for (int i=0; i<numVerts; i++)
	{
		for (int j=i; j<numVerts; j++)
		{
			if (i!=j)
			{
				float dist = D3DXVec3LengthSq(&(temp[i] - temp[j]));

				if (dist < 6.5f)
				{

					for (int k=j; k<numVerts; k++)
					{
						temp[k] = temp[k+1];
					}
					numVerts--;
				}
			}
		}
	}

	*numVertsX = numVerts;
	for (int i=0; i<numVerts; i++)
	{
		vertsX[i] = temp[i];
	}
}

/***************************************************************************/


inline void ClipFaceFaceVertsOld(D3DXVECTOR3* verts0,
							     int* vertIndexs0,
							     D3DXVECTOR3* verts1,
							     int* vertIndexs1)
{
	SortVertices(verts0, vertIndexs0);
	SortVertices(verts1, vertIndexs1);

	// Work out the normal for the face
	D3DXVECTOR3 v0 = verts1[1] - verts1[0];
	D3DXVECTOR3 v1 = verts1[2] - verts1[0];
	D3DXVECTOR3 n  = Cross(v1, v0);
	n = Normalize(n);

	//DrawLine(g_pD3DDevice, v0, v0 + n*4.0f);

	D3DXVECTOR3 temp[4];

	for (int i=0; i<4; i++)
	{
		D3DXVECTOR3 s0 = verts1[i];
		D3DXVECTOR3 s1 = verts1[(i+1)%4];
		D3DXVECTOR3 sx = s0 + n*10.0f;

		// Work out the normal for the face
		D3DXVECTOR3 sv0 = s1 - s0;
		D3DXVECTOR3 sv1 = sx - s0;
		D3DXVECTOR3 sn  = Cross(sv1, sv0);
		sn = Normalize(sn);


		DrawLine3D(g_pD3DDevice, (s0 + (s1-s0)*0.5f), (s0 + (s1-s0)*0.5f) + sn*1.0f);

		float d = Dot(s0, sn);

		for (int r=0; r<4; r++)
		{
			temp[r] = verts0[r];
		}

		for (int j=0; j<4; j++)
		{
			D3DXVECTOR3 p0 = verts0[j];
			D3DXVECTOR3 p1 = verts0[(j+1)%4]; // Loops back to the 0th for the last one

			float d0 = Dot(p0, sn) - d;
			float d1 = Dot(p1, sn) - d;

			// Check there on opposite sides of the plane
			if ( (d0 * d1) < 0.0f)
			{
				D3DXVECTOR3 pX = p0   +   ( Dot(sn, (s0-p0)) /  Dot(sn, (p1-p0)) )  *  (p1 - p0);

				if (d0 > 0.0f)
				{	
					//verts0[j] = pX;
					temp[j] = pX;
				}
				else
				{
					//verts0[(j+1)%4] = pX;
					temp[(j+1)%4] = pX;
				}
			}
		}

		for (int r=0; r<4; r++)
		{
			verts0[r] = temp[r];
		}
	}

	static debugsphere sphere(g_pD3DDevice);
	for (int i=0; i<4; i++)
	{
		sphere.Draw(g_pD3DDevice, verts0[i].x, verts0[i].y, verts0[i].z, 0.4f);
	}

}

/***************************************************************************/

inline void ClosestPtPointOBB(const D3DXVECTOR3& point, 
					   const Cube& box0,
					   D3DXVECTOR3* closestP)
{
	D3DXVECTOR3 d = point - box0.m_c;

	D3DXVECTOR3 q = box0.m_c;


	for (int i=0; i<3; i++)
	{
		float dist = Dot(d, box0.m_u[i]);

		if (dist >  box0.m_e[i]) dist =  box0.m_e[i];
		if (dist < -box0.m_e[i]) dist = -box0.m_e[i];

		q += dist * box0.m_u[i];
	}

	*closestP = q;
}

/***************************************************************************/

void ClipLinePlane(	const D3DXVECTOR3*	verts0,	// 2 verts
					const int*			vertIndexs0,
					const Cube&			box0,
					const D3DXVECTOR3*	verts1, // 4 verts
					int*				vertIndexs1,
					const Cube&			box1,
					D3DXVECTOR3*		vertsX,
					int*				numVertX)
{

	// Work out the normal for the face
	//D3DXVECTOR3 v0 = verts1[1] - verts1[0];
	//D3DXVECTOR3 v1 = verts1[2] - verts1[0];
	//D3DXVECTOR3 N  = Cross(v1, v0);
	//N = Normalize(N);


	//D3DXVECTOR3 P0 = verts0[0];
	//D3DXVECTOR3 P1 = verts0[1];
	//D3DXVECTOR3 P2 = verts1[0];

	//D3DXVECTOR3 Px = P0   +   ( Dot(N, (P2-P0)) /  Dot(N, (P1-P0)) )  *   (P1 - P0);


	//float d = Dot(P2, N);

	//float t0 = Dot(P0, N) - d;
	//float t1 = Dot(P1, N) - d;


	ClosestPtPointOBB(verts0[0], box1, &vertsX[0]);
	ClosestPtPointOBB(verts0[1], box1, &vertsX[1]);
	*numVertX = 2;
	
}

/***************************************************************************/

void ClosestPointLineLine( const D3DXVECTOR3*	verts0,	// 2 verts
						   const D3DXVECTOR3*	verts1, // 2 verts
						   D3DXVECTOR3*			vertsX,
						   int*					numVertX)
{
	D3DXVECTOR3 p1 = verts0[0];
	D3DXVECTOR3 q1 = verts0[1];
	D3DXVECTOR3 p2 = verts1[0];
	D3DXVECTOR3 q2 = verts1[1];

	D3DXVECTOR3 d1 = q1 - p1;
	D3DXVECTOR3 d2 = q2 - p2;
	D3DXVECTOR3 r  = p1 - p2;
	float a = Dot(d1, d1);
	float e = Dot(d2, d2);
	float f = Dot(d2, r);

	const float epsilon = 0.00001f;

	float s, t;
	D3DXVECTOR3 c1, c2;

	if (a <= epsilon && e <= epsilon)
	{
		s = t = 0.0f;
		c1 = p1;
		c2 = p2;

		vertsX[0] = c1;
		*numVertX = 1;
		return;
	}

	if (a <= epsilon)
	{
		s = 0.0f;
		t = f / e;
		t = Clamp(t, 0.0f, 1.0f);
	}
	else
	{
		float c = Dot(d1, r);
		if (e <= epsilon)
		{
			t = 0.0f;
			s = Clamp(-c/a, 0.0f, 1.0f);
		}
		else
		{
			float b = Dot(d1, d2);
			float denom = a*e - b*b;

			if (denom != 0.0f)
			{
				s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
			}
			else
			{
				s = 0.0f;
			}

			t = (b*s + f) / e;

			if (t < 0.0f)
			{
				t = 0.0f;
				s = Clamp(-c / a , 0.0f, 1.0f);
			}
			else if (t > 1.0f)
			{
				t = 1.0f;
				s = Clamp((b-c) / a, 0.0f, 1.0f);
			}
		}
	}

	c1 = p1 + d1 * s;
	c2 = p2 + d2 * t;

	vertsX[0] = (c1 + c2) * 0.5f;
	*numVertX = 1;
}

/***************************************************************************/

void CalculateHitPoint(  const Cube& box0, 
						 const Cube& box1,
						 const float penetration,
						 D3DXVECTOR3& hitNormal,
						 D3DXVECTOR3* hitPoints,
						 int*		  numHitPoints)
{
	D3DXVECTOR3 verts0[8];
	int vertIndex0[8];
	D3DXVECTOR3 norm0 = hitNormal;
	int numVerts0 = GetNumHitPoints( box0,
									 norm0,
									 penetration,
									 verts0,
									 vertIndex0);

	D3DXVECTOR3 verts1[8];
	int vertIndex1[8];
	D3DXVECTOR3 norm1 = -hitNormal;
	int numVerts1 = GetNumHitPoints( box1,
									 norm1,
									 penetration,
									 verts1,
									 vertIndex1);

	// This should never really happen!
	if (numVerts0==0 || numVerts1==0)
	{
		return;
	}


	int numVertsX		= numVerts0;
	D3DXVECTOR3* vertsX  = verts0;

	if (numVerts0>=4 && numVerts1>=4)
	{
		static D3DXVECTOR3 clipVerts[50];
		
		ClipFaceFaceVerts(	verts0, vertIndex0, 
							verts1, vertIndex1,
							clipVerts, &numVertsX);

		vertsX = clipVerts;
	}
	

	{
		// TO-DO - work out which one is the least number
		// of verts, and use that, if both have two, work out
		// the edge point exactly...if its just a single point, only
		// use that single vert

		// TO-DO** TO-DO
		//int numVertsX = numVerts0;
		//D3DXVECTOR3* vertsX = verts0;


		if (numVerts1 < numVerts0)
		{
			numVertsX = numVerts1;
			vertsX    = verts1;
			hitNormal = -norm1;
		}

		if (numVerts1==2 && numVerts0==2)
		{
			static D3DXVECTOR3 V[2];
			static int numV = 0;

			ClosestPointLineLine(verts0,
								 verts1,
								 V,		&numV);

			vertsX = V;
			numVertsX = numV;
		}

		if (numVerts0==2 && numVerts1==4)
		{
			ClipLinePlane(	verts0,		vertIndex0,  box0,
							verts1,		vertIndex1,  box1,
							vertsX,		&numVertsX);
		}

		if (numVerts0==4 && numVerts1==2)
		{
			ClipLinePlane(	verts1,		vertIndex1,   box1,
							verts0,		vertIndex0,   box0,
							vertsX,		&numVertsX);
		}



		/*
		static debugsphere sphere0(g_pD3DDevice, 0xff00ff00);
		for (int i=0; i<numVerts0; i++)
		{
			sphere0.Draw(g_pD3DDevice, verts0[i].x, verts0[i].y, verts0[i].z, 0.5f);
		}

		static debugsphere sphere1(g_pD3DDevice, 0xff005500);
		for (int i=0; i<numVerts1; i++)
		{
			sphere1.Draw(g_pD3DDevice, verts1[i].x, verts1[i].y, verts1[i].z, 0.5f);
		}
		*/


		char buf[128];
		sprintf(buf, "spheres: %d\n", numVertsX);
		
		static debugsphere sphere(g_pD3DDevice, 0xff000000);
		for (int i=0; i<numVertsX; i++)
		{
			sphere.Draw(g_pD3DDevice, vertsX[i].x, vertsX[i].y, vertsX[i].z, 0.5f);
		}
		


		/*
		*hitPoint = D3DXVECTOR3(0,0,0);
		// Work out the average hit point
		for (int i=0; i<numVertsX; i++)
		{
			*hitPoint += vertsX[i];
		}
		*hitPoint = (*hitPoint) * (1.0f/numVertsX);
		*/

		*numHitPoints = numVertsX;
		for (int i=0; i<numVertsX; i++)
		{
			hitPoints[i] = vertsX[i];
		}
	}


}

/***************************************************************************/

bool CubeCubeCollisionCheck(const Cube& box0, 
							const Cube& box1,
							D3DXVECTOR3* hitPoints,
							int* numHitPoints,
							float* penetration,
							D3DXVECTOR3* hitNormal)
{

	// Simple bounding sphere check first
	float len = (box0.m_radius+box1.m_radius);
	if ( D3DXVec3LengthSq(&(box1.m_c - box0.m_c)) > (len*len)  )
	{
		return false;
	}

	bool hit = true;
	float p = 10000.0f;
	D3DXVECTOR3 h;

	hit &= SpanIntersect(box0, box1, box0.m_u[0], &p, &h);
	hit &= SpanIntersect(box0, box1, box0.m_u[1], &p, &h);
	hit &= SpanIntersect(box0, box1, box0.m_u[2], &p, &h);

	hit &= SpanIntersect(box0, box1, box1.m_u[0], &p, &h);
	hit &= SpanIntersect(box0, box1, box1.m_u[1], &p, &h);
	hit &= SpanIntersect(box0, box1, box1.m_u[2], &p, &h);

	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[0], box1.m_u[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[0], box1.m_u[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[0], box1.m_u[2]), &p, &h);

	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[1], box1.m_u[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[1], box1.m_u[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[1], box1.m_u[2]), &p, &h);

	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[2], box1.m_u[0]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[2], box1.m_u[1]), &p, &h);
	hit &= SpanIntersect(box0, box1, Cross(box0.m_u[2], box1.m_u[2]), &p, &h);

	*numHitPoints = 0;

	if (penetration && hitNormal && hit)
	{
		CalculateHitPoint(  box0, 
							box1,
							p,
							h,
							hitPoints,
							numHitPoints);
		*penetration = p;
		*hitNormal = -h;
	}
	return hit;
}

/***************************************************************************/





