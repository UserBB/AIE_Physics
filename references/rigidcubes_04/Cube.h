/***************************************************************************/
/*                                                                         */
/*  File: Cube.h                                                           */
/*  Date: 24-10-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
*/
/***************************************************************************/
#ifndef CUBE_H
#define CUBE_H


#include <d3dx9.h>				// Direct X Header Files

								// Direct X Library Files
#pragma comment(lib, "d3d9.lib")	
#pragma comment(lib, "d3dx9.lib")


#include "Maths.h"



extern bool		g_wireView; // Game.cpp

//*****************************************************************************


class Cube
{
public:

	struct stPoint
	{
		D3DXVECTOR3 curPos;
		D3DXVECTOR3 oldPos;
		bool		unmovable;
	};

	struct stConstraint
	{
		int		index0;
		int		index1;
		float	restLength;
	};

	#define MAX_POINTS 50
	#define	MAX_CONSTRAINTS 200

	stPoint		 m_points[MAX_POINTS];				// 8 Corners of our cube
	int			 m_numPoints;
	stConstraint m_constraints[MAX_CONSTRAINTS];	// Fixed cube constraints
	int			 m_numConstraints;

	int			 m_numTempConstraints;
	int			 m_numTempPoints;


	D3DXVECTOR3 m_c;			// Center Of Cube
	D3DXVECTOR3 m_u[3];			// Local x-y-z Axes
	D3DXVECTOR3 m_e;			// Positive halfwidths along each axis

	D3DXMATRIX  m_matWorld;

	float		m_mass;
	float		m_radius;	
	DWORD		m_colour;


	Cube()
	{
		Cube( D3DXVECTOR3(0,0,0),  D3DXVECTOR3(0,0,0),		 D3DXVECTOR3(5,5,5), 1.0f);
	}

	Cube(D3DXVECTOR3& pos,
		 D3DXVECTOR3& rot,
		 D3DXVECTOR3& size,
		 float mass,
		 DWORD colour = RandomColour())
	{
		m_c = pos;
		m_e = size;
		m_e *= 0.5f;

		m_mass = mass;
		m_radius = sqrtf(size.x*size.x + size.y*size.y + size.z*size.z) + 0.4f;
		m_colour = colour;

		SetupVerlet();

		UpdateMatrix();
	}


	void UpdateMatrix()
	{
		// Work out the centre of the cube, so we render it at the correct place
		m_c = D3DXVECTOR3(0,0,0);
		for (int i=0; i<m_numPoints; i++)
		{
			m_c += m_points[i].curPos;
		}
		m_c *= (1.0f/m_numPoints);


		// Next work out its rotation
		D3DXVECTOR3 z0 = (m_points[0].curPos + m_points[1].curPos + m_points[2].curPos + m_points[3].curPos)*(1/4.0f);
		D3DXVECTOR3 z1 = (m_points[4].curPos + m_points[5].curPos + m_points[6].curPos + m_points[7].curPos)*(1/4.0f);
		D3DXVECTOR3 zdir = (z1-z0);
		zdir = Normalize(zdir);

		D3DXVECTOR3 x0 = (m_points[7].curPos + m_points[3].curPos + m_points[2].curPos + m_points[6].curPos)*(1/4.0f);
		D3DXVECTOR3 x1 = (m_points[4].curPos + m_points[0].curPos + m_points[1].curPos + m_points[5].curPos)*(1/4.0f);
		D3DXVECTOR3 xdir = (x1-x0);
		xdir = Normalize(xdir);

		D3DXVECTOR3 ydir = Cross(zdir, xdir);


		D3DXMATRIX matR;
		D3DXMatrixIdentity(&matR);

		matR._11 = xdir.x;
		matR._12 = xdir.y;
		matR._13 = xdir.z;

		matR._21 = ydir.x;
		matR._22 = ydir.y;
		matR._23 = ydir.z;

		matR._31 = zdir.x;
		matR._32 = zdir.y;
		matR._33 = zdir.z;


		m_u[0] = D3DXVECTOR3(1,0,0);
		m_u[1] = D3DXVECTOR3(0,1,0);
		m_u[2] = D3DXVECTOR3(0,0,1);
		D3DXMATRIX matT;
		D3DXVec3TransformCoord(&m_u[0], &m_u[0], &matR);
		D3DXVec3TransformCoord(&m_u[1], &m_u[1], &matR);
		D3DXVec3TransformCoord(&m_u[2], &m_u[2], &matR);
		D3DXMatrixTranslation(&matT, m_c.x, m_c.y, m_c.z);
		m_matWorld = matR * matT;
	}

	void RenderConstraints(IDirect3DDevice9* pDevice);
	void Render(IDirect3DDevice9* pDevice);
	void Update(float dt);
	void SetupVerlet();

	int  AddConstraint(int i0, int i1);
	int  AddPoint(const D3DXVECTOR3& p, int index=-1);

	void ClearTemps()
	{
		m_numConstraints = m_numTempConstraints;
		m_numPoints = m_numTempPoints;
	}

	void AddCollisionPoint(const D3DXVECTOR3& point, const D3DXVECTOR3& hitNormal, float penetrationDepth)
	{
	
		// Attach any collision points to the centre of the cube only!
		int iCentre = 8; //Centre point of the cube
		D3DXVECTOR3 centre = m_points[iCentre].curPos;
		
		int iTemp = AddPoint(point);
		int iTempConstraint = AddConstraint(iCentre, iTemp);


		D3DXVECTOR3 dir = point - centre;	// Work out the direction to the centre of the cube
		float len = D3DXVec3Length(&dir);	// Distance from centre to the point
		D3DXVec3Normalize(&dir, &dir);		// Normalize direction
		D3DXVECTOR3 p0 = point + dir*(len + penetrationDepth);
		D3DXVECTOR3 p1 = point;
		len = D3DXVec3Length(&(p1-p0));
		
		m_constraints[iTempConstraint].restLength = len;



		/*
		// As an alternative method!...doesn't seem to work right though
		// but link each of the collision points to all the other particle
		// points instead of just the centre.

		int iTemp = AddPoint(point);

		// Link it to all the other points
		for (int i=0; i<m_numTempPoints; i++)
		{
			int constraintIndex = AddConstraint(i, iTemp);
			
			
			D3DXVECTOR3 centre = m_points[i].curPos;
			D3DXVECTOR3 dir = point - centre;	// Work out the direction to the centre of the cube
			float len = D3DXVec3Length(&dir);	// Distance from centre to the point
			D3DXVec3Normalize(&dir, &dir);		// Normalize direction
			D3DXVECTOR3 p0 = point + dir*(len + penetrationDepth);
			D3DXVECTOR3 p1 = point;
			len = D3DXVec3Length(&(p1-p0));
			
			
			//D3DXVECTOR3 centre = m_points[i].curPos;
			//D3DXVECTOR3 dirC = centre - point;
			//D3DXVec3Normalize(&dirC, &dirC);
			//D3DXVECTOR3 norP = point + hitNormal*(penetrationDepth+0.01f);
			//D3DXVECTOR3 dirP = norP - point;
			//float lenToC = D3DXVec3Dot(&dirP, &dirC);
			//D3DXVECTOR3 pX = point + dirC*lenToC;

			//D3DXVECTOR3 p0 = centre;
			//D3DXVECTOR3 p1 = pX;
			//float len = D3DXVec3Length(&(p1-p0));

			m_constraints[constraintIndex].restLength = len;
		}
		*/
			
	}// AddCollisionPoint(..)


	void VerletIntegrate(float dt)
	{
		for (int i=0; i<m_numPoints; i++)
		{
			if (!m_points[i].unmovable)
			{
				D3DXVECTOR3 oldPos = m_points[i].oldPos;
				D3DXVECTOR3 curPos = m_points[i].curPos;
				D3DXVECTOR3 a = D3DXVECTOR3(0,-9.8f,0);

				//curPos = 2*curPos - oldPos + a*dt*dt;
				static float drag = 0.01f;
				curPos = (2.0f-drag)*curPos - (1.0f-drag)*oldPos + a*dt*dt;

				m_points[i].oldPos = m_points[i].curPos;
				m_points[i].curPos = curPos;
			}
		}
	}// End VerletIntegrate(..)

	void SatisfyConstraints()
	{
		const int numIterations = 20;

		for (int i=0; i<numIterations; i++)
		{
			for (int k=0; k< m_numConstraints; k++)
			{
				// Constraint 1 (Floor)
				//for (int v=0; v<m_numPoints; v++)
				//{
				//	if (m_points[v].curPos.y < 0.0f) m_points[v].curPos.y = 0.0f;
				//}

				// Constraint 2 (Links)
				stConstraint* c = &m_constraints[k];
				D3DXVECTOR3& p0 = m_points[c->index0].curPos;
				D3DXVECTOR3& p1 = m_points[c->index1].curPos;
				D3DXVECTOR3 delta = p1-p0;
				float len = D3DXVec3Length(&delta);
				if (len>0.000001f)
				{
					float diff = (len - c->restLength) / len;
					p0 += delta*0.5f*diff;
					p1 -= delta*0.5f*diff;
				}


				if (m_points[c->index0].unmovable)
				{
					m_points[c->index0].curPos = m_points[c->index0].oldPos;
				}
				if (m_points[c->index1].unmovable)
				{
					m_points[c->index1].curPos = m_points[c->index1].oldPos;
				}
			}
		}
	}// End SatisfyConstraints(..)

};

    

#endif //CUBE_H




