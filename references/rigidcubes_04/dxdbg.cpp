/**********************************************************************************/
/*                                                                                */
/*  File:    dxdbg.cpp                                                            */
/*  Author:  bkenwright@xbdev.net                                                 */
/*  Web:     www.xbdev.net                                                        */
/*  Date:    10-12-2005 (xmas)                                                    */
/*                                                                                */
/**********************************************************************************/
/*
   See .h file for details

   Updated: 19-12-2005 (xmas)
*/
/**********************************************************************************/

#include "dxdbg.h"



#include <stdio.h>								//sprintf(..)
#include <assert.h>								//assert(..)
#include <windows.h>							//MessageBox(..)


//Saving debug information to a log file
//void abc(char *str)
//{
//      FILE *fp = fopen("output.txt", "a+");
//      fprintf(fp, "%s\n", str);
//      fclose(fp);
//}


void ErrorMessageA(char * str)
{
	MessageBox(NULL, str, str, 0 );
}

void ErrorMessageB(char * str, 
				   char* szFile, 
				   char* szFunction, 
				   long  nLine)
{
	char buf[300]; // large temp buffer

	sprintf(buf, "ERROR: %s\n\n"
				 "File: %s\n"
				 "Function: %s\n"
				 "Line: %d\n",
				 str, szFile, szFunction, nLine);

	MessageBox(NULL, 
			   buf,  // Dialog Content
			   str,  // Window Title
			   0 );
}//End ErrorMessageB(..)



void StatsDisplay(IDirect3DDevice  * pDevice,
				 char * str)
{
	/*
	static int   right_pos  = 480;
	static int   left_pos   = 5;
	static int   bottom_pos = 445;
	static int   max_string_len = 1000;

	static debugtxt txt(pDevice);
	
	ID3DXFont* pFont = txt.pFont;
	TEXTMETRICA pp;
	pFont->GetTextMetricsA(&pp);
	long w = pp.tmAveCharWidth;
	

	int len = (int)strlen(str);
	if( len > max_string_len ) assert("String Overflow Error");

	static float pos = (float)right_pos;

	pos-=0.1f;

	static int char_pos = 0;

	int cwidth = w;
	if( pos <= left_pos )
	{
		pos+=cwidth;
		char_pos += 1;
	}

	if( char_pos >= len )
	{
		char_pos=0;
		pos = (float)right_pos;
	}

	static char buf[1000];
	sprintf(buf, "%s", str+int(char_pos) );


	txt.printf(buf, (int)pos, bottom_pos , 0);
	*/
}


void TriDisplay(IDirect3DDevice  * pDevice,
				 int nTris)
{
	static char buff[100] = {0};
	sprintf(buff, "tris : %d", nTris);
	//printf(pDevice, buff, 10, 23  );

	static debugtxt txt(pDevice);
	txt.printf(buff, 10, 24, 0xffffffff);
}





void WireSquare(IDirect3DDevice  * pDevice, 
				float x, 
				float y, 
				float w, 
				float h,
				unsigned int col )
{
		struct TLVERTEX
		{
			float x,y,z,rhw;
			unsigned int colour;
			enum{ FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
		};

		//pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		float scale = 128.0f;
		TLVERTEX Vertex[4] = 
		{
			//   x      y    z    rhw     colour
			{    x,     y,   0,  1.0f,   col},
			{    x+w,   y,   0,  1.0f,   col},
			{    x+w,   y+h, 0,  1.0f,   col},
			{    x,     y+h, 0,  1.0f,   col},
		};
		pDevice->SetTexture( 0, NULL);
		
#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->SetFVF( TLVERTEX::FVF_TLVERTEX );
#else
		pDevice->SetVertexShader( TLVERTEX::FVF_TLVERTEX );
#endif // DIRECT3D_VERSION

		
		pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 2, Vertex, sizeof( TLVERTEX ) );
		pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );
}


void FlatSquare(IDirect3DDevice  * pDevice, 
				float x, 
				float y, 
				float w, 
				float h,
				unsigned int col )
{
		struct TLVERTEX
		{
			float x,y,z,rhw;
			unsigned int colour;
			enum{ FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
		};

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		float scale = 128.0f;
		TLVERTEX Vertex[4] = 
		{
			//   x      y    z  rhw     colour
			{    x,     y,   0,  1.0f,   col},
			{    x+w,   y,   0,  1.0f,   col},
			{    x+w,   y+h, 0,  1.0f,   col},
			{    x,     y+h, 0,  1.0f,   col},
		};
		pDevice->SetTexture( 0, NULL);

		
#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->SetFVF( TLVERTEX::FVF_TLVERTEX );
#else
		pDevice->SetVertexShader( TLVERTEX::FVF_TLVERTEX );
#endif // DIRECT3D_VERSION

		HRESULT hr = pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TLVERTEX ) );
}

int GetFPS()
{
	float fTimeNow = (float)GetTickCount();
	static float fOldTime = fTimeNow;

	static int nCounter = 0;
	nCounter++;

	static int nFPS = 0;
	if( fOldTime+1000.0f < fTimeNow )
	{
		fOldTime = fTimeNow;

		nFPS = nCounter;
		nCounter = 0;
	}//End if

	return nFPS;
}//End GetFPS(..)

void FPSDisplay(IDirect3DDevice  * pDevice)
{
	int fps = GetFPS();

	// Scale the width as values can go from 0 to 300+ etc
	int w = fps*2;

	// Typically about 30 to 300 fps, so we'll limit our bar width
	if( w < 50  ) w = 50;
	if( w > 250 ) w = 250;
	
	// Linear value.  Lets say our fps goes from 0 to 1000, then we can
	// scale it from 0 to 0xff
	int r = (int)(float(fps)/500.0f)*0xff;

	// Create a changing colour with fps value

	DWORD col = D3DCOLOR_ARGB(0x33, r, 55, 55);
	
	pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	pDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID );

	FlatSquare(pDevice, 5, 5, (float)w, 17, col);
	WireSquare(pDevice, 5, 5, (float)w, 17, 0x00ffffff);

	static char buff[100];
	sprintf(buff, "fps : %d", fps);
	//printf(pDevice, buff, 10, 6  );

	static debugtxt txt(pDevice);

	txt.printf(buff, 10, 6, 0xffffffff);

}



void FlatTexture(IDirect3DDevice  * pDevice,
				 IDirect3DTexture * pTexture )
{
		struct TLVERTEX
		{
			float x,y,z,rhw;
			float tu,tv;
		};
		#define	FVF_TLVERTEX (D3DFVF_XYZRHW | D3DFVF_TEX1)

		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		float scale = 128.0f;
		TLVERTEX Vertex[4] = 
		{
			// x  y  z rhw tu tv
			{    0,    0,0, 1, 0, 0,},
			{scale,    0,0, 1, 1, 0,},
			{scale,scale,0, 1, 1, 1,},
			{    0,scale,0, 1, 0, 1,},
		};

#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->SetFVF( FVF_TLVERTEX );
#else
		pDevice->SetVertexShader( FVF_TLVERTEX );
#endif // DIRECT3D_VERSION

		pDevice->SetTexture( 0, pTexture );
		pDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TLVERTEX ) );
}

void printf(IDirect3DDevice * pDevice,
			char * str, 
			int xpos /*= 10*/, 
			int ypos /*= 10*/, 
			unsigned int rgb /*= 0xffffff*/,
			int size /*=15*/)
{
	static ID3DXFont* pFont;

#if(DIRECT3D_VERSION >= 0x0900)

	
	D3DXCreateFont(pDevice,  // LPDIRECT3DDEVICE9
				size,        // Width
				0,           // Height
				FW_NORMAL,   // Weight
				1,           // MipLevels
				false,       // Italic
				DEFAULT_CHARSET,     // CharSet
				OUT_DEFAULT_PRECIS,  // OutputPrecision
				ANTIALIASED_QUALITY, // Quality
				FIXED_PITCH | FF_DONTCARE, // PitchAndFamily
				"Arial",     // pFacename
				&pFont);     // LPD3DXFONT
	

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
			size, //height
			0,  //width; 
			0,  // lfEscapement; 
			0,  //lfOrientation; 
			FW_BOLD, // lfWeight; 
			FALSE, // lfItalic; 
			FALSE, // lfUnderline; 
			FALSE, // lfStrikeOut; 
			DEFAULT_CHARSET, // lfCharSet; 
			OUT_DEFAULT_PRECIS, //lfOutPrecision; 
			CLIP_DEFAULT_PRECIS, // lfClipPrecision; 
			ANTIALIASED_QUALITY,// lfQuality; 
			FIXED_PITCH,// lfPitchAndFamily; 
			"Arial"// lfFaceName[LF_FACESIZE]; 
			};

	lfont.lfHeight = size;

   //This is all there is to creating a D3DXFont.  Hmmmm....
   D3DXCreateFontIndirect( pDevice ,&lfont,&pFont );

#endif // DIRECT3D_VERSION



   static char szbuff[900];
   sprintf(szbuff, "%s", str );
   RECT rect = {xpos,ypos, ((int)xpos)+800,ypos+800};


#if(DIRECT3D_VERSION >= 0x0900)
   pFont->DrawTextA(NULL,
						szbuff,
						-1,
						&rect,
						DT_LEFT,
						0xFFBBBBFF);
#else
   pFont->DrawTextA(szbuff,
						-1,
						&rect,
						DT_LEFT,
						0xFFBBBBFF);
#endif // DIRECT3D_VERSION


	pFont->Release();
}



//static void WireBox(IDirect3DDevice  * pDevice, 
//					float x0, float y0,
//					float x1, float y1,
//					float x2, float y2,
//					float x3, float y3,
//					unsigned int col )
//{
//			struct TLVERTEX
//		{
//			float x,y,z,rhw;
//			unsigned int colour;
//			enum{ FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE };
//		};
//
//		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
//		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
//		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
//		
//		TLVERTEX Vertex[5] = 
//		{
//			//   x      y    z  rhw     colour
//			{    x0,   y0,   0,  1.0f,   col},
//			{    x1,   y1,   0,  1.0f,   col},
//			{    x2,   y2,   0,  1.0f,   col},
//			{    x3,   y3,   0,  1.0f,   col},
//			{    x0,   y0,   0,  1.0f,   col},
//		};
//		pDevice->SetTexture( 0, NULL);
//
//		
//#if(DIRECT3D_VERSION >= 0x0900)
//		pDevice->SetFVF( TLVERTEX::FVF_TLVERTEX );
//#else
//		pDevice->SetVertexShader( TLVERTEX::FVF_TLVERTEX );
//#endif // DIRECT3D_VERSION
//
//		pDevice->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, Vertex, sizeof( TLVERTEX ) );
//}


void DrawLine2D(  IDirect3DDevice  * pDevice, 
				  float x0, float y0,
				  float x1, float y1,
				  unsigned  int col )
{
		struct TLVERTEX
		{
			float x,y,z,rhw;
			unsigned int colour;
		};
		#define FVF_VERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		
		TLVERTEX Vertex[2] = 
		{
			//   x      y    z  rhw     colour
			{    x0,   y0,   0,  1.0f,   col},
			{    x1,   y1,   0,  1.0f,   col},
		};
		pDevice->SetTexture( 0, NULL);

		
#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->SetFVF( FVF_VERTEX);
#else
		pDevice->SetVertexShader( FVF_VERTEX );
#endif // DIRECT3D_VERSION

		pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, Vertex, sizeof( TLVERTEX ) );
}



/***************************************************************************/
/*                                                                         */
/*  Name: DrawLine()                                                       */
/*                                                                         */
/*  Simple function to draw a line in the world, for debugging/testing     */
/*                                                                         */
/***************************************************************************/

void DrawLine3D(IDirect3DDevice9  * pDevice, 
			    const D3DXVECTOR3& from,
		        const D3DXVECTOR3& to,
			    unsigned int col,
			    const D3DXMATRIX& mat /*= D3DXMATRIX(1,0,0,0, 0,1,0,0,  0,0,1,0,  0,0,0,1)*/)
{
		struct VERTEX
		{
			float x,y,z;
			unsigned int colour;
			enum { FVF_V = (D3DFVF_XYZ | D3DFVF_DIFFUSE) };
		};

		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//		pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
		
		VERTEX Vertex[2] = 
		{
			//   x		   y		z	  colour
			{  from.x,  from.y,  from.z,   col},
			{  to.x,    to.y,    to.z,     col},
		};
		pDevice->SetTexture( 0, NULL);

		
#if(DIRECT3D_VERSION >= 0x0900)
		pDevice->SetFVF( VERTEX::FVF_V );
#else
		pDevice->SetVertexShader( VERTEX::FVF_V );
#endif // DIRECT3D_VERSION

		//static D3DXMATRIX matWorld = D3DXMATRIX(1,0,0,0,
		//										0,1,0,0,
		//										0,0,1,0,
		//										0,0,0,1);
		pDevice->SetTransform(D3DTS_WORLD, &mat);
		pDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, Vertex, sizeof( VERTEX ) );

}// End DrawLine3D(..)


/***************************************************************************/
/*                                                                         */
/*  Name: DrawArrowLine()                                                  */
/*                                                                         */
/*  Draws a 2D Line from point to point, with an arrow on one end          */
/*                                                                         */
/***************************************************************************/
void DrawArrowLine(IDirect3DDevice9  * pDevice, 
				   const D3DXVECTOR3& from,
				   const D3DXVECTOR3& to,
				   unsigned int col/* = 0xffff00ff*/)
{
	// Draw Line From-To
	DrawLine3D(pDevice, from, to, col);

	// Build the Arrow Head
	D3DXVECTOR3 N = from - to;

	D3DXVec3Normalize(&N, &N);


	D3DXVECTOR3 misc = N;
	misc.x += 20.0f;
	D3DXVec3Normalize(&misc, &misc);

	D3DXVECTOR3 perp;
	D3DXVec3Cross(&perp, &N, &misc);

	D3DXMATRIX matRot;
	D3DXMatrixRotationAxis(&matRot, &misc, 3.14f/5.0f);

	D3DXVECTOR3 N0, N1;
	D3DXVec3TransformCoord(&N0, &N, &matRot);

	D3DXMatrixRotationAxis(&matRot, &misc, -(3.14f/5.0f));
	D3DXVec3TransformCoord(&N1, &N, &matRot);


	DrawLine3D(pDevice, to, to + N0*0.5f, col);
	DrawLine3D(pDevice, to, to + N1*0.5f, col);
}// End DrawArrowLine(..)


/***************************************************************************/
/*                                                                         */
/*  Name: DrawCube()                                                       */
/*                                                                         */
/*  Simple function to draw a 3D Cube in the world, for debugging/testing  */
/*                                                                         */
/***************************************************************************/

//void DrawCube(IDirect3DDevice* pDevice, D3DXMATRIX* matWorld, D3DXVECTOR3 dim/*=(0.5,0.5,0.5*/)
//{
//
//  	struct stVERTEX
//	{
//		float x,y,z;
//		unsigned int colour;
//		enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE };
//	};
//
//	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
//	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
//	pDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
//	pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_DIFFUSE);
//	float w = dim.x/2.0f;
//	float h = dim.y/2.0f;
//	float d = dim.z/2.0f;
//	DWORD col = 0xffff00ff;
//
//	stVERTEX Vertex[36] = 
//	{
//		//   x     y      z       colour
//		// Front
//		{   -w,   -h,	-d,    0xffff0000},
//		{   +w,   +h,	-d,    0xffff0000},
//		{   -w,   +h,	-d,    0xffff0000},
//
//		{   -w,   -h,	-d,    0xffff0000},
//		{   +w,   -h,	-d,    0xffff0000},
//		{   +w,   +h,	-d,    0xffff0000},
//
//		// Back
//		{   -w,   -h,	+d,    0xff00ff00},
//		{   -w,   +h,	+d,    0xff00ff00},
//		{   +w,   +h,	+d,    0xff00ff00},
//
//		{   -w,   -h,	+d,    0xff00ff00},
//		{   +w,   +h,	+d,    0xff00ff00},
//		{   +w,   -h,	+d,    0xff00ff00},
//
//		// Top
//		{   -w,   +h,	-d,    0xff0000ff},
//		{   +w,   +h,	+d,    0xff0000ff},
//		{   -w,   +h,	+d,    0xff0000ff},
//					  
//		{   -w,   +h,	-d,    0xff0000ff},
//		{   +w,   +h,	-d,    0xff0000ff},
//		{   +w,   +h,	+d,    0xff0000ff},
//
//		// Bottom
//		{   -w,   -h,	-d,    0xffffff0f},
//		{   -w,   -h,	+d,    0xffffff0f},
//		{   +w,   -h,	+d,    0xffffff0f},
//					  
//		{   -w,   -h,	-d,    0xffffff00},
//		{   +w,   -h,	+d,    0xffffff00},
//		{   +w,   -h,	-d,    0xffffff00},
//
//		// Left
//		{   -w,   -h,	-d,    0xffffffff},
//		{   -w,   +h,	-d,    0xffffffff},
//		{   -w,   +h,	+d,    0xffffffff},
//				  
//		{   -w,   -h,	-d,    0xffffffff},
//		{   -w,   +h,	+d,    0xffffffff},
//		{   -w,   -h,	+d,    0xffffffff},
//
//		// Right
//		{   +w,   -h,	-d,    0xff000000},
//		{   +w,   +h,	+d,    0xff000000},
//		{   +w,   +h,	-d,    0xff000000},
//			  
//		{   +w,   -h,	-d,    0xff000000},
//		{   +w,   -h,	+d,    0xff000000},
//		{   +w,   +h,	+d,    0xff000000},
//	};
//	pDevice->SetTexture( 0, NULL);
//
//		
//	#if(DIRECT3D_VERSION >= 0x0900)
//		pDevice->SetFVF( stVERTEX::FVF );
//	#else
//		pDevice->SetVertexShader( stVERTEX::FVF );
//	#endif // DIRECT3D_VERSION
//
//
//	//D3DXMATRIX matMove, matScale, matRot, matWorld;
//	//D3DXMatrixTranslation(&matMove, pos.x, pos.y, pos.z);
//	//D3DXMatrixScaling(&matScale, dim.x, dim.y, dim.z);
//	//D3DXMatrixRotationYawPitchRoll(&matRot, rot.y, rot.x, rot.z);
//	//matWorld = world * matRot * matScale * matMove;
//
//	pDevice->SetTransform(D3DTS_WORLD, matWorld);
//
//	HRESULT hr = 
//	pDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 12, Vertex, sizeof( stVERTEX ) );
//
//}//End DrawCube(..)


/***************************************************************************/
/*                                                                         */
/*  Name: DrawText(...                                                     */
/*                                                                         */
/*  Draws a 2D Circle in the 3D World around the X-Z Axis - you can apply  */
/*  a rotation so you can have it in any axis and any position in the      */
/*  world.                                                                 */
/*                                                                         */
/***************************************************************************/

void DrawText(IDirect3DDevice9 * pDevice,
				char * str, 
				int xpos /*= 10*/, 
				int ypos /*= 10*/, 
				unsigned int rgb /*= 0xffffff*/,
				int size /*=15*/)
{
	class font_c
	{
		public:
		font_c(){pFont=NULL;}
		~font_c(){pFont->Release();}
		ID3DXFont* pFont;
	};
	static font_c font;

	ID3DXFont** pFont = &font.pFont;

#if(DIRECT3D_VERSION >= 0x0900)

	if (*pFont==NULL)
	{
	D3DXCreateFont(pDevice,						// LPDIRECT3DDEVICE9
					size,						// Width
					0,							// Height
					FW_NORMAL,					// Weight
					1,							// MipLevels
					false,						// Italic
					DEFAULT_CHARSET,			// CharSet
					OUT_DEFAULT_PRECIS,			// OutputPrecision
					ANTIALIASED_QUALITY,		// Quality
					FIXED_PITCH | FF_DONTCARE,	// PitchAndFamily
					"Arial",					// pFacename
					pFont);						// LPD3DXFONT
	}
#else
	static LOGFONT lfont={
			size, //height
			0,  //width; 
			0,  // lfEscapement; 
			0,  //lfOrientation; 
			FW_BOLD, // lfWeight; 
			FALSE, // lfItalic; 
			FALSE, // lfUnderline; 
			FALSE, // lfStrikeOut; 
			DEFAULT_CHARSET, // lfCharSet; 
			OUT_DEFAULT_PRECIS, //lfOutPrecision; 
			CLIP_DEFAULT_PRECIS, // lfClipPrecision; 
			ANTIALIASED_QUALITY,// lfQuality; 
			FIXED_PITCH,// lfPitchAndFamily; 
			"Arial"// lfFaceName[LF_FACESIZE]; 
			};

	lfont.lfHeight = size;

   //This is all there is to creating a D3DXFont.  Hmmmm....
   D3DXCreateFontIndirect( pDevice ,&lfont,&pFont );

#endif // DIRECT3D_VERSION



   static char szbuff[900];
   sprintf(szbuff, "%s", str );
   RECT rect = {xpos,ypos, ((int)xpos)+800,ypos+800};


#if(DIRECT3D_VERSION >= 0x0900)
   (*pFont)->DrawTextA(NULL,
						szbuff,
						-1,
						&rect,
						DT_LEFT,
						0xFFBBBBFF);
#else
   pFont->DrawTextA(szbuff,
						-1,
						&rect,
						DT_LEFT,
						0xFFBBBBFF);
#endif // DIRECT3D_VERSION


	//(*pFont)->Release();

}// End DrawText(..)
