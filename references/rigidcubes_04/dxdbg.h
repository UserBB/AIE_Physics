/**********************************************************************************/
/*                                                                                */
/*  File:    dxdbg.h                                                              */
/*  Author:  bkenwright@xbdev.net                                                 */
/*  Web:     www.xbdev.net                                                        */
/*  Date:    10-12-2005 (xmas)                                                    */
/*                                                                                */
/**********************************************************************************/
/*
    Updated: 12-12-05
	Simple debug functions/classes.

	Implemented so you can add simple debugging/testing output to various demos/
	games/test apps etc.  Most of the methods make use of static variables so
	functions can be called within main loops and won't create/release resources
	over and over again - this is so we don't have to create globals and also don't
	loose out due to inefficient creation/releasing of resources such as fonts
	bitmaps etc.

	Also designed for easy implementation of directx8 and directx9
*/
/**********************************************************************************/
/*
   Updated: 19-12-2005 (xmas)
*/
/**********************************************************************************/

#ifndef DXDBG_H
#define DXDBG_H

#include <d3dx9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//#pragma comment(lib, "d3d8.lib")					// DirectX Library Files
//#pragma comment(lib, "d3dx8.lib")
//#include <d3dx8.h>								// DirectX Header File

#include <stdio.h>									// sprintf(..)


#if(DIRECT3D_VERSION >= 0x0900)
#define			IDirect3DDevice						IDirect3DDevice9
#define			IDirect3DTexture					IDirect3DTexture9
#define			IDirect3DIndexBuffer				IDirect3DIndexBuffer9
#define			IDirect3DVertexBuffer				IDirect3DVertexBuffer9
#else
#define			IDirect3DDevice						IDirect3DDevice8
#define			IDirect3DTexture					IDirect3DTexture8
#define			IDirect3DIndexBuffer				IDirect3DIndexBuffer8
#define			IDirect3DVertexBuffer				IDirect3DVertexBuffer8
#endif // DIRECT3D_VERSION



void StatsDisplay(IDirect3DDevice  * pDevice,
				 char * str);

void FlatTexture(IDirect3DDevice  * pDevice,
				 IDirect3DTexture * pTexture );

void TriDisplay(IDirect3DDevice  * pDevice,
				 int nTris);


void printf(IDirect3DDevice * pDevice,
			char * str, 
			float xpos = 10, 
			float ypos = 10, 
			unsigned int rgb = 0xffffff,
			int size = 15);


void FPSDisplay(IDirect3DDevice  * pDevice);

void FlatSquare(IDirect3DDevice  * pDevice, 
				float x, 
				float y, 
				float w, 
				float h,
				unsigned int col );


void DrawLine2D(IDirect3DDevice  * pDevice, 
			    float x0, float y0,
			    float x1, float y1,
			    unsigned int col );


// Simple 3D Line Drawing Function
void DrawLine3D(IDirect3DDevice9  * pDevice, 
			    const D3DXVECTOR3& from,
		        const D3DXVECTOR3& to,
			    unsigned int col = 0xffff00ff,
			    const D3DXMATRIX& mat = D3DXMATRIX(1,0,0,0, 0,1,0,0,  0,0,1,0,  0,0,0,1));



void DrawArrowLine(IDirect3DDevice9  * pDevice, 
				   const D3DXVECTOR3& from,
				   const D3DXVECTOR3& to,
				   unsigned int col = 0xffff00ff);



//void DrawCube(IDirect3DDevice* pDevice, D3DXMATRIX* matWorld, D3DXVECTOR3 dim=D3DXVECTOR3(1,1,1));

#define DBG_HALT  {__asm { int 10 }}

#define  ErrMsg(str)   ErrorMessageB(str, __FILE__, __FUNCTION__, __LINE__); 
//#define  ErrMsg(str)   ErrorMessageA(str); 

void ErrorMessageA(char * str);
void ErrorMessageB(char * str, 
				   char* szFile, 
				   char* szFunction, 
				   long  nLine);


void DrawText(IDirect3DDevice9 * pDevice,
				char * str, 
				int xpos = 10, 
				int ypos = 10, 
				unsigned int argb = 0xffffffff,
				int size =15);

class debugtxt
{
public:
	debugtxt(IDirect3DDevice * pDevice){ Create(pDevice); };
	~debugtxt(){ Release(); };

public:
	ID3DXFont* pFont;

public:

	void printf(char * str, 
			int xpos = 10, 
			int ypos = 10, 
			unsigned int rgb = 0xffffffff)
	{
			static char szbuff[900];
			sprintf(szbuff, "%s", str );
			RECT rect = {xpos,ypos, xpos+800,ypos+800};


			#if(DIRECT3D_VERSION >= 0x0900)
			pFont->DrawTextA(NULL, szbuff,
									-1,
									&rect,
									DT_LEFT,
									rgb);
			#else
			pFont->DrawTextA(szbuff,
									-1,
									&rect,
									DT_LEFT,
									rgb);
			#endif // DIRECT3D_VERSION

	}

	
protected:

	bool Create(IDirect3DDevice * pDevice)
	{
			#if(DIRECT3D_VERSION >= 0x0900)

				D3DXCreateFont(pDevice,			// LPDIRECT3DDEVICE9
							15,					// Width
							0,					// Height
							FW_NORMAL,			// Weight
							1,					// MipLevels
							false,				// Italic
							ANSI_CHARSET,		// CharSet
							OUT_DEFAULT_PRECIS,	// OutputPrecision
							DEFAULT_QUALITY,	// Quality
							FIXED_PITCH ,		// PitchAndFamily
							"Arial",			// pFacename
							&pFont);			// LPD3DXFONT
				

				/*
				static LOGFONT lfont={
						15,						// height
						0,						// width; 
						0,						// lfEscapement; 
						0,						// lfOrientation; 
						FW_BOLD,				// lfWeight; 
						FALSE,					// lfItalic; 
						FALSE,					// lfUnderline; 
						FALSE,					// lfStrikeOut; 
						DEFAULT_CHARSET,		// lfCharSet; 
						OUT_DEFAULT_PRECIS,		// lfOutPrecision; 
						CLIP_DEFAULT_PRECIS,	// lfClipPrecision; 
						ANTIALIASED_QUALITY,	// lfQuality; 
						FIXED_PITCH,			// lfPitchAndFamily; 
						"Arial"					// lfFaceName[LF_FACESIZE]; 
						};

				lfont.lfHeight = 15;

				//This is all there is to creating a D3DXFont.  Hmmmm....
				D3DXCreateFontIndirect( pDevice ,&lfont,&pFont );
				*/



			#else
				static LOGFONT lfont={
						15,						// height
						0,						// width; 
						0,						// lfEscapement; 
						0,						// lfOrientation; 
						FW_BOLD,				// lfWeight; 
						FALSE,					// lfItalic; 
						FALSE,					// lfUnderline; 
						FALSE,					// lfStrikeOut; 
						DEFAULT_CHARSET,		// lfCharSet; 
						OUT_DEFAULT_PRECIS,		// lfOutPrecision; 
						CLIP_DEFAULT_PRECIS,	// lfClipPrecision; 
						ANTIALIASED_QUALITY,	// lfQuality; 
						FIXED_PITCH,			// lfPitchAndFamily; 
						"Arial"					// lfFaceName[LF_FACESIZE]; 
						};

				lfont.lfHeight = 15;

			//This is all there is to creating a D3DXFont.  Hmmmm....
			D3DXCreateFontIndirect( pDevice ,&lfont,&pFont );

			#endif // DIRECT3D_VERSION

			return true;

	}//End Create(..)

	void Release()
	{
		pFont->Release();
	}
};


class debugsphere
{
	IDirect3DVertexBuffer* m_pVertexBuffer;
	IDirect3DIndexBuffer*  m_pIndexBuffer;

	DWORD m_dwNumOfVertices;
	DWORD m_dwNumOfIndices;

	UINT D3DFVF_CUSTOMVERTEX; // (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);

	struct CUSTOMVERTEX
	{
		FLOAT x, y, z;				//Vertex position 
		FLOAT nx, ny, nz;			//Direction the vertex is facing(normal)
		DWORD colour;				//The vertex colour.
	};

public:
	debugsphere(IDirect3DDevice * pDevice, DWORD col=0xffff0000){ Create(pDevice, col); };
	~debugsphere(){ Release(); };

	int GetNumTris(){ return (m_dwNumOfIndices - 2); }

	void Draw(IDirect3DDevice  * pDevice, 
					const float centerx, 
					const float centery, 
					const float centerz, 
					const float radius )
	{
	  D3DXMATRIX matMove;
	  D3DXMatrixTranslation(&matMove, centerx, centery, centerz);
	  D3DXMATRIX matScale;
	  D3DXMatrixScaling(&matScale, radius, radius, radius);

	  D3DXMATRIX matWorld = matScale * matMove;
	  
	  Draw(pDevice, &matWorld, radius * 0.5f);
	}

    void Draw(IDirect3DDevice * pDevice,
			  D3DXMATRIX* matWorld,
			  float radius)
	{
		D3DXMATRIX matScale;
		D3DXMatrixScaling(&matScale, radius, radius, radius);

		pDevice->SetTransform(D3DTS_WORLD, &(matScale * (*matWorld)));

      //Select the index buffer
      DWORD dwNumOfPolygons = m_dwNumOfIndices - 2;

#if(DIRECT3D_VERSION >= 0x0900)

	  pDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

      pDevice->SetVertexShader(NULL);
	  pDevice->SetPixelShader(NULL);

	  pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));
	
	  pDevice->SetIndices(m_pIndexBuffer);


	  //Render the polygons from the index buffer
      pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 
									0, 
									0,
									m_dwNumOfVertices, 
									0,
                                    dwNumOfPolygons);
#else
	  pDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

	  pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(CUSTOMVERTEX));
	  pDevice->SetIndices(m_pIndexBuffer, 0);
      //Render the polygons from the index buffer
      pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 
									0,
									m_dwNumOfVertices, 
									0,
                                    dwNumOfPolygons);
#endif
 
	}

protected:

	void Release()
	{
		if (m_pIndexBuffer)  m_pIndexBuffer->Release();
		if (m_pVertexBuffer) m_pVertexBuffer->Release();
	}

	void Create(IDirect3DDevice  * pDevice, DWORD col)
	{
		D3DFVF_CUSTOMVERTEX =  (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE);

		//These two variables determine the quality of our sphere, try changing them,
		//if you put a large number in them you'll get a very details sphere...made
		//up of very small triangles.
		int nRings				= 8;
		int nSegments			= 8;

		m_dwNumOfVertices		= (nRings + 1) * (nSegments + 1);
		m_dwNumOfIndices		= 2 * nRings * (nSegments + 1);

		m_pVertexBuffer			= NULL;
		m_pIndexBuffer			= NULL;

		WORD *pIndices			= NULL;
		CUSTOMVERTEX *pVertex	= NULL;


#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pVertexBuffer,
									NULL);

		pDevice->CreateIndexBuffer( m_dwNumOfIndices * sizeof(WORD),
									0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
									&m_pIndexBuffer,
									NULL);

		m_pVertexBuffer->Lock(0,0, (void**)&pVertex, 0);
		m_pIndexBuffer->Lock(0,m_dwNumOfIndices, (void**)&pIndices, 0);

#else
		pDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pVertexBuffer);

		pDevice->CreateIndexBuffer( m_dwNumOfIndices * sizeof(WORD),
									0, D3DFMT_INDEX16, D3DPOOL_MANAGED,
									&m_pIndexBuffer);

		m_pVertexBuffer->Lock(0,0, (BYTE**)&pVertex, 0);
		m_pIndexBuffer->Lock(0,m_dwNumOfIndices, (BYTE**)&pIndices, 0);
#endif
	 


		WORD wVertexIndex = 0;
		D3DXVECTOR3 vNormal;

		//Setup some angles
		float rDeltaRingAngle   = (D3DX_PI / nRings);
		float rDeltaSegAngle    = (2.0f * D3DX_PI / nSegments);


		float red   = (static_cast<float>((col>>16)&0xff))/256.0f;
		float green = (static_cast<float>((col>>8)&0xff))/256.0f;
		float blue  = (static_cast<float>((col>>0)&0xff))/256.0f;


		//Generate the group of rings for the sphere
		for(int nCurrentRing = 0; nCurrentRing < nRings + 1; nCurrentRing++)
		{
				float r0 = sinf(nCurrentRing * rDeltaRingAngle);
				float y0 = cosf(nCurrentRing * rDeltaRingAngle);

				//OOooo Gerneate the group of segments for the current ring
				for(int nCurrentSegment=0; nCurrentSegment < nSegments + 1; nCurrentSegment++)
				{
					float x0 = r0 * sinf(nCurrentSegment * rDeltaSegAngle);
					float z0 = r0 * cosf(nCurrentSegment * rDeltaSegAngle);

					vNormal.x = x0;
					vNormal.y = y0;
					vNormal.z = z0;

					D3DXVec3Normalize(&vNormal, &vNormal);

					//Add one vector to the strip which makes up the sphere
					pVertex->x  = x0;
					pVertex->y  = y0;
					pVertex->z  = z0;

					pVertex->nx = vNormal.x;
					pVertex->ny = vNormal.y;
					pVertex->nz = vNormal.z;

	 

					pVertex->colour = D3DXCOLOR(red, green, blue, 1.0f);
					//red += 0.02f;
					//blue += 0.01f;
					//green -= 0.015f;


					/*
					//Alternatively you could set texture coordinates e.g:
					pVertex->tu = 1.0f - ( (float)nCurrentSegment / (float)nSegments );
					pVertex->tv = (float)nCurrent / (float)nRings;
					*/

					pVertex++;

					//Add two indices except for the last ring
					if(nCurrentRing != nRings)
					{
							*pIndices = wVertexIndex;
							pIndices++;

							*pIndices = wVertexIndex + (WORD)(nSegments + 1);
							pIndices++;

							wVertexIndex++;
					}
				}
		}

	 

		m_pIndexBuffer->Unlock();
		m_pVertexBuffer->Unlock();
	}
};



class debug_wire_ground
{
	IDirect3DVertexBuffer* m_pVertexBuffer;

	DWORD m_dwNumOfVertices;

	UINT D3DFVF_CUSTOMVERTEX; // (D3DFVF_XYZ|D3DFVF_DIFFUSE);

	struct CUSTOMVERTEX
	{
		FLOAT x, y, z;				//Vertex position 
		DWORD colour;				//The vertex colour.
	};

public:
	debug_wire_ground(	IDirect3DDevice * pDevice,	// pointer directx device
						int xres,					// num lines x plane
						int zres)					// num lines y plane
	{ 
		Create(pDevice, xres, zres); 
	};

	~debug_wire_ground(){ Release(); };

	int GetNumTris(){ return (m_dwNumOfVertices / 2); }

	void Draw(IDirect3DDevice  * pDevice, 
					float centerx, 
					float centery, 
					float centerz, 
					float scale )
	{
	  D3DXMATRIX matMove;
	  D3DXMatrixTranslation(&matMove, centerx, centery, centerz);
	  D3DXMATRIX matScale;
	  D3DXMatrixScaling(&matScale, scale, scale, scale);

	  D3DXMATRIX matWorld = matScale * matMove;
	  pDevice->SetTransform(D3DTS_WORLD, &matWorld);

	  pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	  pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	  pDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_DIFFUSE);

#if(DIRECT3D_VERSION >= 0x0900)
	  pDevice->SetFVF( D3DFVF_CUSTOMVERTEX );

      pDevice->SetVertexShader(NULL);
	  pDevice->SetPixelShader(NULL);

	  pDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(CUSTOMVERTEX));

	  pDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_dwNumOfVertices/2);

#else
	  pDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

	  pDevice->SetStreamSource(0, m_pVertexBuffer, sizeof(CUSTOMVERTEX));

      pDevice->DrawPrimitive(D3DPT_LINELIST, 0, m_dwNumOfVertices/2);
#endif
 
	}

protected:

	void Release()
	{
		m_pVertexBuffer->Release();
	}

	void Create(IDirect3DDevice  * pDevice,
		 int nXResolution,
		 int nZResolution)
	{
		D3DFVF_CUSTOMVERTEX =  (D3DFVF_XYZ|D3DFVF_DIFFUSE);

		//These two variables determine the quality of our sphere, try changing them,
		//if you put a large number in them you'll get a very details sphere...made
		//up of very small triangles.
		int nZ					= nZResolution;
		int nX					= nXResolution;

		m_dwNumOfVertices		= (nX+1) * (nZ+1) * 2;

		m_pVertexBuffer			= NULL;

		CUSTOMVERTEX *pVertex	= NULL;

#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pVertexBuffer,
									NULL);

		m_pVertexBuffer->Lock(0,0, (void**)&pVertex, 0);

#else
		pDevice->CreateVertexBuffer(m_dwNumOfVertices * sizeof(CUSTOMVERTEX),
									0, D3DFVF_CUSTOMVERTEX,
									D3DPOOL_DEFAULT, &m_pVertexBuffer);

		m_pVertexBuffer->Lock(0,0, (BYTE**)&pVertex, 0);
#endif

		//So our grid is scaled from 0 to 1
		float fDeltaX   = (1.0f / nX);
		float fDeltaZ   = (1.0f / nZ);

		//Generate the 

		DWORD dwColour = 0xff010101; // aarrggbb

		for(int nCurrentZ=0; nCurrentZ < nZ+1; nCurrentZ+=2)
		{
			float z0 = -0.5f + (nCurrentZ * fDeltaZ);
			float z1 = -0.5f + (nCurrentZ * fDeltaZ);

			float x0 = -0.5f;
			float x1 =  0.5f;

			float y0 = 0;
			float y1 = 0;


			//Add one vector to the strip which makes up the sphere
			pVertex->x  = x0;
			pVertex->y  = y0;
			pVertex->z  = z0;
			pVertex->colour = dwColour;
			pVertex++;

			pVertex->x  = x1;
			pVertex->y  = y1;
			pVertex->z  = z1;
			pVertex->colour = dwColour;
			pVertex++;
		}

		for(int nCurrentX=0; nCurrentX < nX+1; nCurrentX+=2)
		{
			float z0 = -0.5f;
			float z1 =  0.5f;

			float x0 = -0.5f + (nCurrentX * fDeltaX);
			float x1 = -0.5f + (nCurrentX * fDeltaX);

			float y0 = 0;
			float y1 = 0;


			//Add one vector to the strip which makes up the sphere
			pVertex->x  = x0;
			pVertex->y  = y0;
			pVertex->z  = z0;
			pVertex->colour = dwColour;
			pVertex++;

			pVertex->x  = x1;
			pVertex->y  = y1;
			pVertex->z  = z1;
			pVertex->colour = dwColour;
			pVertex++;
		}


		m_pVertexBuffer->Unlock();
	}
};

#endif // DXDBG_H