/***************************************************************************/
/*                                                                         */
/*  File: Cube.cpp                                                         */
/*  Date: 24-10-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
	See .h file
*/
/***************************************************************************/


#include <windows.h>

#include <stdio.h>					// sprintf(..)
#include <d3dx9.h>					// Direct X Header Files
#pragma comment(lib, "d3d9.lib")	// Direct X Library Files	
#pragma comment(lib, "d3dx9.lib")

#include "dxdbg.h"
#include "Cube.h"




void Cube::Render(IDirect3DDevice9* pDevice)
{
	struct TLVERTEX
	{
		float x,y,z;
		float nx,ny,nz;
		unsigned int colour;
		enum{ FVF_TLVERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE };
	};

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if (g_wireView)	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else			pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDevice->SetTextureStageState(0,D3DTSS_COLOROP,		D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
	float w = m_e.x;
	float h = m_e.y;
	float d = m_e.z;

	//DWORD sleepFade = m_awake?0xffffffff:0xaa888888;
	DWORD col = m_colour;

	TLVERTEX Vertex[36] = 
	{
		//   x     y     z     normal		colour
		// Front
		{   -w,   -h,	-d,	   0,0,-1,    col},
		{   +w,   +h,	-d,    0,0,-1,    col},
		{   -w,   +h,	-d,    0,0,-1,    col},

		{   -w,   -h,	-d,    0,0,-1,    col},
		{   +w,   -h,	-d,    0,0,-1,    col},
		{   +w,   +h,	-d,    0,0,-1,    col},

		// Back
		{   -w,   -h,	+d,    0,0, 1,    col},
		{   -w,   +h,	+d,    0,0, 1,    col},
		{   +w,   +h,	+d,    0,0, 1,    col},

		{   -w,   -h,	+d,    0,0, 1,    col},
		{   +w,   +h,	+d,    0,0, 1,    col},
		{   +w,   -h,	+d,    0,0, 1,    col},

		// Top
		{   -w,   +h,	-d,    0,1,0,     col},
		{   +w,   +h,	+d,    0,1,0,     col},
		{   -w,   +h,	+d,    0,1,0,     col},

		{   -w,   +h,	-d,    0,1,0,     col},
		{   +w,   +h,	-d,    0,1,0,     col},
		{   +w,   +h,	+d,    0,1,0,     col},

		// Bottom
		{   -w,   -h,	-d,    0,-1,0,     col},
		{   -w,   -h,	+d,    0,-1,0,     col},
		{   +w,   -h,	+d,    0,-1,0,     col},
							   
		{   -w,   -h,	-d,    0,-1,0,     col},
		{   +w,   -h,	+d,    0,-1,0,     col},
		{   +w,   -h,	-d,    0,-1,0,     col},

		// Left
		{   -w,   -h,	-d,    -1,0,0,     col},
		{   -w,   +h,	-d,    -1,0,0,     col},
		{   -w,   +h,	+d,    -1,0,0,     col},

		{   -w,   -h,	-d,    -1,0,0,     col},
		{   -w,   +h,	+d,    -1,0,0,     col},
		{   -w,   -h,	+d,    -1,0,0,     col},

		// Right
		{   +w,   -h,	-d,    1,0,0,     col},
		{   +w,   +h,	+d,    1,0,0,     col},
		{   +w,   +h,	-d,    1,0,0,     col},
							   
		{   +w,   -h,	-d,    1,0,0,     col},
		{   +w,   -h,	+d,    1,0,0,     col},
		{   +w,   +h,	+d,    1,0,0,     col},
	};
	pDevice->SetTexture( 0, NULL);


	#if(DIRECT3D_VERSION >= 0x0900)
	pDevice->SetFVF( TLVERTEX::FVF_TLVERTEX );
	#else
	pDevice->SetVertexShader( TLVERTEX::FVF_TLVERTEX );
	#endif // DIRECT3D_VERSION


	UpdateMatrix();

	pDevice->SetTransform(D3DTS_WORLD, &m_matWorld);

	HRESULT hr = 
	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 12, Vertex, sizeof( TLVERTEX ) );

	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	RenderConstraints(pDevice);

	
}// End Render(..)


void Cube::RenderConstraints(IDirect3DDevice9* pDevice)
{
	// Draw a wire mesh for our constraints
	for (int i=0; i<m_numConstraints; i++)
	{
		stConstraint* c = &m_constraints[i];
		DrawLine3D( pDevice, 
					m_points[c->index0].curPos, 
					m_points[c->index1].curPos,
					0xffff00ff);
	}
}

int Cube::AddPoint(const D3DXVECTOR3& p, int index/*=-1*/)
{
	if (index==-1) index=m_numPoints;

	if (index<0 || index>=MAX_POINTS) DBG_HALT;
	m_points[index].curPos = p;
	m_points[index].oldPos = p;
	m_points[index].unmovable = m_mass > 1000.0f ? true : false;
	int val = m_numPoints;
	m_numPoints++;
	return val;
}

int Cube::AddConstraint(int i0, int i1)
{
	// Basic error checking
	if (m_numConstraints<0 || m_numConstraints>=MAX_CONSTRAINTS) DBG_HALT;
	if (i0<0 || i0>=m_numPoints) DBG_HALT;
	if (i1<0 || i1>=m_numPoints) DBG_HALT;


	float len = D3DXVec3Length(&(m_points[i0].curPos - m_points[i1].curPos));

	m_constraints[m_numConstraints].index0 = i0;
	m_constraints[m_numConstraints].index1 = i1;
	m_constraints[m_numConstraints].restLength = len;
	int constraintIndex = m_numConstraints;
	m_numConstraints++;
	return constraintIndex;
}

void Cube::SetupVerlet()
{
	m_numTempPoints = 0;
	m_numTempConstraints = 0;

	m_numPoints = 0;
	m_numConstraints = 0;

	float w = m_e.x;
	float h = m_e.y;
	float d = m_e.z;


	AddPoint(D3DXVECTOR3( w,  h, -d), 0);
	AddPoint(D3DXVECTOR3( w, -h, -d), 1);
	AddPoint(D3DXVECTOR3(-w, -h, -d), 2);
	AddPoint(D3DXVECTOR3(-w,  h, -d), 3);

	AddPoint(D3DXVECTOR3( w,  h,  d), 4);
	AddPoint(D3DXVECTOR3( w, -h,  d), 5);
	AddPoint(D3DXVECTOR3(-w, -h,  d), 6);
	AddPoint(D3DXVECTOR3(-w,  h,  d), 7);

	AddPoint(D3DXVECTOR3(0,   0,  0), 8); // Centre

	// Offset our cube
	for (int i=0; i<m_numPoints; i++)
	{
		m_points[i].curPos += m_c;
		m_points[i].oldPos += m_c;
	}


	// Link all the corners to the centre
	/*
	for (int i=0; i<(9-1); i++)
	{
		AddConstraint(8, i);
	}
	*/

	// Link all the conerns to each other
	for (int i=0; i<m_numPoints; i++)
	{
		for (int k=0; k<m_numPoints; k++)
		{
			if (i!=k)
			{
				AddConstraint(i, k);
			}
		}
	}

	m_numTempPoints = m_numPoints;
	m_numTempConstraints = m_numConstraints;

	/*
	m_numTempPoints = 0;
	m_numPoints = MAX_POINTS;
	for (int i=0; i<m_numPoints; i++)
	{
		D3DXVECTOR3 pos = points[i];
		m_points[i].curPos = pos;
		m_points[i].oldPos = pos;
		m_points[i].unmovable = m_mass > 1000.0f ? true : false;
	}
	*/



	/*
	m_numTempConstraints = 0;
	m_numConstraints = MAX_CONSTRAINTS;
	for (int i=0; i<m_numConstraints; i++)
	{
		int i0 = i;
		int i1 = m_numPoints-1;

		float len = D3DXVec3Length(&(m_points[i0].curPos - m_points[i1].curPos));

		m_constraints[i].index0 = i0;
		m_constraints[i].index1 = i1;
		m_constraints[i].restLength = len;
	}
	*/



}// End SetupVerlet()


//float Limit(float val, float min, float max)
//{
//	if (val > max)
//	{
//		val = max;
//	}
//	if (val < min)
//	{
//		val = min;
//	}
//	return val;
//}

//D3DXVECTOR3 Limit(D3DXVECTOR3 val, float min, float max)
//{
//	val.x = Limit(val.x, min, max);
//	val.y = Limit(val.y, min, max);
//	val.z = Limit(val.z, min, max);
//	return val;
//}



void Cube::Update(float dt)
{
	UpdateMatrix();

	VerletIntegrate(dt);
	SatisfyConstraints();

	ClearTemps();
}


//void Cube::UpdatePos(float dt)
//{
//	if (m_mass>1000.0f || !m_awake)
//	{
//		return;
//	}
//
//	m_c += (m_linVelocity * dt);
//
//	D3DXVECTOR3 angVel = m_angVelocity;
//
//	D3DXQUATERNION Qvel = 0.5f * m_rot * D3DXQUATERNION(angVel.x, angVel.y, angVel.z, 0);
//	m_rot += Qvel * dt;
//	D3DXQuaternionNormalize(&m_rot, &m_rot);
//
//
//	m_forces			= D3DXVECTOR3(0,0,0);
//	m_torques			= D3DXVECTOR3(0,0,0);
//
//
//	UpdateMatrix();
//}



//void Cube::UpdateVel(float dt)
//{
//	if (m_mass>1000.0f  || !m_awake)
//	{
//		return;
//	}
//
//	if (D3DXVec3Length(&m_linVelocity) < 0.0001f)  m_linVelocity = D3DXVECTOR3(0,0,0);
//	if (D3DXVec3Length(&m_angVelocity) < 0.0001f)  m_angVelocity = D3DXVECTOR3(0,0,0);
//
//
//
//	// Add Gravity
//	AddForce( m_c, D3DXVECTOR3(0, -9.8f * m_mass, 0) );
//	
//
//	// Update Angular
//	m_angVelocity += ((m_torques * m_invInertia) * dt);
//
//	// Update Linear
//	m_linVelocity += ((m_forces / m_mass) * dt);
//
//
//	// Bit Damping
//	const float damping = 0.98f;
//
//	m_linVelocity *= powf(damping, dt);
//	m_angVelocity *= powf(damping, dt);
//
//
//
//	UpdateMatrix();
//
//
//	// Update the kinetic energy store, and possibly put the body to sleep.
//	// can sleep bool?
//	{
//		float motion =  Dot(m_linVelocity, m_linVelocity) + 
//						Dot(m_angVelocity, m_angVelocity);
//
//		float bias = 0.96f;
//		m_rwaMotion = bias*m_rwaMotion + (1-bias)*motion;
//		if (m_rwaMotion> 50.0f) m_rwaMotion = 5.0f;
//
//		/*
//        if (m_rwaMotion < g_sleepEpsilon)
//		{
//			m_awake = false;
//			m_linVelocity = D3DXVECTOR3(0,0,0);
//			m_angVelocity = D3DXVECTOR3(0,0,0);
//		}
//        else if (m_rwaMotion > 10 * g_sleepEpsilon)
//		{
//			m_rwaMotion = 10 * g_sleepEpsilon;
//			m_awake = true;
//		}
//		*/
//    }
//
//	{
//		//	float vsmall = 0.00001f;
//		//	if ( abs(c0.m_linVelocity.x) < vsmall ) c0.m_linVelocity.x=0.0f;
//		//	if ( abs(c0.m_linVelocity.y) < vsmall ) c0.m_linVelocity.y=0.0f;
//		//	if ( abs(c0.m_linVelocity.z) < vsmall ) c0.m_linVelocity.z=0.0f;
//
//		//	if ( abs(c1.m_linVelocity.x) < vsmall ) c1.m_linVelocity.x=0.0f;
//		//	if ( abs(c1.m_linVelocity.y) < vsmall ) c1.m_linVelocity.y=0.0f;
//		//	if ( abs(c1.m_linVelocity.z) < vsmall ) c1.m_linVelocity.z=0.0f;
//
//		//	if ( abs(c0.m_angVelocity.x) < vsmall ) c0.m_angVelocity.x=0.0f;
//		//	if ( abs(c0.m_angVelocity.y) < vsmall ) c0.m_angVelocity.y=0.0f;
//		//	if ( abs(c0.m_angVelocity.z) < vsmall ) c0.m_angVelocity.z=0.0f;
//
//		//	if ( abs(c1.m_angVelocity.x) < vsmall ) c1.m_angVelocity.x=0.0f;
//		//	if ( abs(c1.m_angVelocity.y) < vsmall ) c1.m_angVelocity.y=0.0f;
//		//	if ( abs(c1.m_angVelocity.z) < vsmall ) c1.m_angVelocity.z=0.0f;
//
//
//
//		//	/*
//		//	if (D3DXVec3Length(&c0.m_linVelocity)<0.001) c0.m_linVelocity = D3DXVECTOR3(0,0,0);
//		//	if (D3DXVec3Length(&c1.m_linVelocity)<0.001) c1.m_linVelocity = D3DXVECTOR3(0,0,0);
//
//		//	if (D3DXVec3Length(&c0.m_angVelocity)<0.001) c0.m_angVelocity = D3DXVECTOR3(0,0,0);
//		//	if (D3DXVec3Length(&c1.m_angVelocity)<0.001) c1.m_angVelocity = D3DXVECTOR3(0,0,0);
//		//	*/
//	}
//}




















