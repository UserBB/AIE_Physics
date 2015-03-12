/***************************************************************************/
/*                                                                         */
/*  File: Game.cpp                                                         */
/*  Date: 06-02-07                                                         */
/*                                                                         */
/***************************************************************************/
/*
	Main Game File - This is where we update all our elements in the game
	do any creation of things, such as land, tanks and call render
	on them all or update....heart of the whole game is here.

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
#include "Maths.h"
#include "Game.h"


#include "Cube.h"
#include "CubeCollision.h"


// Externals

// DirectX Device/Render Handles
extern LPDIRECT3D9			g_pD3D;			// main.cpp
extern LPDIRECT3DDEVICE9	g_pD3DDevice;	// main.cpp

// Globals
bool		g_wireView		= true;
float		g_timeStep		= 1.0f;			// Timing 
bool		g_paused		= true;
bool		g_singleStep	= false;
float		g_totalTime		= 0.0f;
bool		g_floorCollisions = false;


int			g_numCubes		= 0;
Cube		g_cubes[20];



/***************************************************************************/


void Reset(int selected)
{
	Release();

	switch (selected)
	{
		case 0:
		case 1: // Single Cube On Plane
		{
			Cube cubes[2] = 
				{
					Cube ( D3DXVECTOR3(0,5,0),		D3DXVECTOR3(0,0,0),			D3DXVECTOR3(5,5,5),		1.0f),
					Cube ( D3DXVECTOR3(0,-0.5f,0),	D3DXVECTOR3(0,0,0),			D3DXVECTOR3(50, 1, 50), 100000.0f )
				};
			g_numCubes = sizeof(cubes)/sizeof(Cube);
			memcpy(g_cubes, cubes, sizeof(Cube)*g_numCubes);
		}
		break;

		case 2: // Couple of cubes stacked
		{
			Cube cubes[3] = 
			{
				Cube ( D3DXVECTOR3(0,7.5,0),	D3DXVECTOR3(0,0.1f,0),		D3DXVECTOR3(5,5,5),		1.0f),
				Cube ( D3DXVECTOR3(0,2.5,0),	D3DXVECTOR3(0,0,0),			D3DXVECTOR3(5,5,5),		1.0f),
				Cube ( D3DXVECTOR3(0,-0.5f,0),	D3DXVECTOR3(0,0,0),			D3DXVECTOR3(50, 1, 50), 100000.0f )
			};
			g_numCubes = sizeof(cubes)/sizeof(Cube);
			memcpy(g_cubes, cubes, sizeof(Cube)*g_numCubes);
		}
		break;


		default:
		{
		}
		break;
	}

	
}

/***************************************************************************/

bool Create()
{	
	Reset(0);

	return true;
}// End Create

/***************************************************************************/

bool Release()
{
	return true;
}// End Release()


/***************************************************************************/
/*                                                                         */
/*  Name: KeyBoard() & KeyDown()										   */
/*                                                                         */
/*  Used to accept keyboard input and alter some global variables - used   */
/*  for simple analysis of our code - turning wire frame rendering on/off  */
/*  adjusting variables which are used in the demo.                        */
/*                                                                         */
/***************************************************************************/
static void KeyBoard()
{
	if( GetKeyState(VK_SPACE)   & 0x80 ) Reset(0);

}//End KeyBoard()

//==========================================================================

// So we can catch single key presses for single time step
void KeyDown()
{
	if ( GetKeyState('P') & 0x80 )	g_paused			= !g_paused;
	if ( GetKeyState('S') & 0x80 )	g_singleStep		= true;
	if ( GetKeyState('W') & 0x80 )	g_wireView			= !g_wireView;

	for (int i=0; i<10; i++) // Keys 1..9
	{
		if( GetKeyState('0'+i)	& 0x80 ) Reset(i);
	}
}//KeyDown(..)


/***************************************************************************/


static void UpdateTiming()
{
	// We use a more accurate timing information, QueryPerformanceCounter
	// instead of GetTickCount() so we get smoother animations
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	LARGE_INTEGER nTime;
	QueryPerformanceCounter(&nTime);

	static LARGE_INTEGER oTime = nTime;

	double delay = (double(nTime.QuadPart - oTime.QuadPart)/freq.QuadPart);
	oTime=nTime;

	g_timeStep = (float)delay;

	if (g_timeStep > 0.07f)
	{
		g_timeStep = 0.07f;
	}

}

/***************************************************************************/

void SetCamera()
{
	extern bool  g_LMousePress;
	extern bool  g_RMousePress;
	extern int	 g_mousePosX;
	extern int   g_mousePosY;
	extern int   g_zoom;

	float s = 0.02f;
	D3DXVECTOR3 vMove = D3DXVECTOR3((float)g_mousePosX*s, (float)g_mousePosY*s, 0);

	static D3DXVECTOR3 vDir			= D3DXVECTOR3(0, 20, -50);
	static D3DXVECTOR3 vDirOld		= vDir;


	if (g_LMousePress || g_RMousePress)
	{
		if (g_LMousePress)
		{
			D3DXMATRIX rot;
			D3DXMatrixRotationYawPitchRoll(&rot, vMove.x, vMove.y, 0);
			D3DXVec3TransformCoord(&vDir, &vDirOld, &rot);
		}
		else if (g_RMousePress)
		{
			vDir = vDirOld;
			D3DXVECTOR3 vDirNorm;
			D3DXVec3Normalize(&vDirNorm, &vDir);
			float scale = 1.0f;
			vDir += (float)g_zoom*vDirNorm*scale;
		}
	}
	else
	{
		vDirOld = vDir;
	}

	
	static D3DXVECTOR3 vLookAt		= D3DXVECTOR3(0,0,0);
	D3DXVECTOR3 vCamPosition		= vLookAt + vDir;


    // [1] D3DTS_VIEW
    D3DXMATRIX v;
    g_pD3DDevice->SetTransform(	D3DTS_VIEW, 
								D3DXMatrixLookAtLH(&v, 
								&vCamPosition, 
                                &vLookAt, 
                                &D3DXVECTOR3(0,1,0)));  
    // [2] D3DTS_PROJECTION
    D3DXMATRIX p;
    g_pD3DDevice->SetTransform( D3DTS_PROJECTION, 
								D3DXMatrixPerspectiveFovLH( &p,  
								D3DX_PI/4,  
								1.0f,
                                1.0f,1000.0f));
}//End SetCamera(..)

void SetMaterial()
{
      D3DMATERIAL9 matMaterial;

      D3DCOLORVALUE rgbaDiffuse  = {1.0, 1.0, 1.0, 0.0};  //RGBA
      D3DCOLORVALUE rgbaAmbient  = {1.0, 1.0, 1.0, 0.0};
      D3DCOLORVALUE rgbaSpecular = {0.0, 0.0, 0.0, 0.0};

      // emissive is for a glow effect
      D3DCOLORVALUE rgbaEmissive = {0.0, 0.0, 0.0, 0.0};

      matMaterial.Diffuse = rgbaDiffuse;
      matMaterial.Ambient = rgbaAmbient;
      matMaterial.Specular = rgbaSpecular;
      matMaterial.Emissive = rgbaEmissive;
      matMaterial.Power = 100.0f;

      g_pD3DDevice->SetMaterial(&matMaterial);
}// End SetMaterial(..)


void SetLight()
{
	//Set a light up?
	D3DLIGHT9 d3dLight;
	//Initialize the light structure.
	ZeroMemory(&d3dLight, sizeof(D3DLIGHT9));
	//Set up a white point light at (0, 0, -10).
	d3dLight.Type = D3DLIGHT_POINT;
	// Actual colour of the light. (e.g. white).
	d3dLight.Diffuse.r = 1.0f;
	d3dLight.Diffuse.g = 1.0f;
	d3dLight.Diffuse.b = 1.0f;

	// Colour when no light hits the object.
	d3dLight.Ambient.r = 0.2f;
	d3dLight.Ambient.g = 0.2f;
	d3dLight.Ambient.b = 0.2f;

	// Shiny effect.
	d3dLight.Specular.r = 0.0f;
	d3dLight.Specular.g = 0.0f;
	d3dLight.Specular.b = 0.0f;

	d3dLight.Position.x =  2.0f;
	d3dLight.Position.y =  10.0f;
	d3dLight.Position.z = -10.0f;

	d3dLight.Attenuation0 = 0.6f; 
	d3dLight.Attenuation1 = 0.0f; 
	d3dLight.Attenuation2 = 0.0f; 
	d3dLight.Range = 500.0f;

	g_pD3DDevice->SetLight(0, &d3dLight);
	g_pD3DDevice->LightEnable(0,TRUE);
	//Turn on lighting
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);


	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_XRGB(5,100,32));
}// SetLight()


/***************************************************************************/
/*                                                                         */
/* Update()                                                                */
/* Our main render loop, which gets called over and over agian to do our   */
/* drawing...clears the screen, draws the data, then presents it..         */
/*                                                                         */
/***************************************************************************/
void Update()
{

	UpdateTiming();

	KeyBoard();

	SetCamera();

	SetLight();

	SetMaterial();

	
	// Clear the back buffer to a blue color
	g_pD3DDevice->Clear( 0, 
						 NULL, 
						 D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
						 D3DCOLOR_XRGB(0,20,155), 
						 1.0f, 
						 0 );

	g_pD3DDevice->BeginScene();


	static debug_wire_ground ground(g_pD3DDevice, 12,12);
	ground.Draw(g_pD3DDevice, 0,0,0, 50.0f);


	if (g_wireView)	g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	else			g_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );





	float dt = g_timeStep;

	if (g_paused)
	{
		dt = 0.0f;
	}

	bool doSingleStep = false;
	if (g_singleStep)
	{
		doSingleStep = true;
		g_singleStep = false;
		dt = 0.01f;
	}

	if (!g_paused)
	{
		doSingleStep = true;
	}

	// Things go wierd if our timestep dt is to large!...so clamp it
	dt = Clamp(dt, 0.0f, 0.1f);

	g_totalTime += dt;


	if (doSingleStep)
	{
		// Do Collision Detection
		for (int i=0; i<g_numCubes; i++)
		{
			for (int k=0; k<g_numCubes; k++)
			{
				if (i!=k)
				{
					D3DXVECTOR3 hitPoints[50];
					int			numHitPoints		= 0;
					float		penetrationBox		= 0.0f;
					D3DXVECTOR3	hitNormalBox;

					bool hitBox = CubeCubeCollisionCheck(g_cubes[i], g_cubes[k], hitPoints, &numHitPoints, &penetrationBox, &hitNormalBox);

					if (hitBox)
					{
						for (int j=0; j<numHitPoints; j++)
						{
							g_cubes[i].AddCollisionPoint(hitPoints[j], hitNormalBox, penetrationBox); 
							g_cubes[k].AddCollisionPoint(hitPoints[j], hitNormalBox, penetrationBox);
						}
					}
				}
			}
		}
		
		// Update our cubes
		for (int i=0; i<g_numCubes; i++)
		{
			g_cubes[i].Update(dt);
		}
	}

	



	for (int i=0; i<g_numCubes; i++)
	{
		g_cubes[i].Render(g_pD3DDevice);
	}

	//if (g_model)
	//{
	//	if (doSingleStep)
	//		g_model->Update(dt);

	//	g_model->Render(g_pD3DDevice);
	//}

	//if (g_hair)
	//{
	//	if (doSingleStep)
	//		g_hair->Update(dt);

	//	g_hair->Render(g_pD3DDevice);
	//}

	
	//if (g_cloth)
	//{
	//	if (doSingleStep)
	//		g_cloth->Update(dt);

	//	g_cloth->Render(g_pD3DDevice);
	//}

	//debugsphere ball(g_pD3DDevice, 0xffffffff);
	//if (g_drawBall)
	//	ball.Draw(g_pD3DDevice, 
	//			  g_headSphere.x,
	//			  g_headSphere.y,
	//			  g_headSphere.z, 
	//			  4.5f);

	//if (g_drawHead)
	//	g_head.Render(g_pD3DDevice);


	DrawText(g_pD3DDevice, "Verlet Integration Demo", 10, 10); 
	char buf[256] = {'\0'};

	sprintf(buf, "Time: %.3f", g_totalTime);
	DrawText(g_pD3DDevice, buf, 10, 50);

	DrawText(g_pD3DDevice, "Keys 0-1 Select Config", 10, 70);
	DrawText(g_pD3DDevice, "P-Pause",				 10, 90);
	DrawText(g_pD3DDevice, "S-SingleStep",			 10, 110);

//	sprintf(buf, "F-Draw Head {%s}", g_drawHead?"ON":"OFF");
//	DrawText(g_pD3DDevice, buf,						 10, 130);





	g_pD3DDevice->EndScene();
	// After rendering the scene we display it.
	g_pD3DDevice->Present( NULL, NULL, NULL, NULL );

}//End Update()






