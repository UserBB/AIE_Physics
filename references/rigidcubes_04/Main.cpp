/*****************************************************************************/
/*
	Main.cpp
	bkenwright@xbdev.net
	www.xbdev.net

	Quote: Apple doesn't fall far from the tree!

	Simple Program Entry Point :)

	Code for fun!...its the only way...
/*
/*****************************************************************************/



// Windows Header Files:
#include <windows.h>            
//#include <assert.h>
#include <math.h>				// abs(..)


#include <d3dx9.h>				// Direct X Header Files

								// Direct X Library Files
#pragma comment(lib, "d3d9.lib")	
#pragma comment(lib, "d3dx9.lib")


// These few lines are compiler directives that include the windows librarys
// during linking.  You could instead inside visual studio goto Project->
// settings" menu and under the "Link" tab add the necessary librarys instead.
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Winmm.lib")


#define SafeRelease(pObject) if(pObject != NULL) {pObject->Release(); pObject=NULL;}



#include "Game.h"



/*****************************************************************************/
/*
   GLOBALS
   As much as we all hate them, its sometimes a little easier...so we've got
   a few globals for the essential of our code.
/*
/*****************************************************************************/

HDC						g_hdc;				// global device context
HGLRC					g_hrc;				// global rendering context

HWND					g_hwnd		= NULL;	// handle of our window

											
LPDIRECT3D9				g_pD3D		= NULL; // DirectX Device/Render Handles
LPDIRECT3DDEVICE9		g_pD3DDevice= NULL;


int						g_mousePosX = 0;
int						g_mousePosY = 0;
bool					g_LMousePress = false;
bool					g_RMousePress = false;
int						g_zoom	      = 0;

/***************************************************************************/
// PROTOTYPES                                                              //
/***************************************************************************/


bool CreateX(HWND hWnd);			// 
bool ReleaseX();					// 
void GameLoopX();					// 

extern void KeyDown();


/***************************************************************************/
/*                                                                         */
/* Handle all messages for the main window here                            */
/*                                                                         */
/***************************************************************************/
long _stdcall MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int mousePosPressX;
	static int mousePosPressY;
	static int zoomPress;

	static HCURSOR hcursor = NULL;

    switch( uMsg )
    {
		case WM_CREATE:
		{
			hcursor = LoadCursor(NULL,IDC_ARROW);
		}
		break;

        case WM_KEYDOWN:
        {
			KeyDown();

            switch( wParam )
            {
                case VK_ESCAPE:         // Exit on pressing Escape Key
                    PostQuitMessage(0);
                    break;
			}
			break;
        }

		case WM_LBUTTONDOWN:
		{
			mousePosPressX = LOWORD(lParam);
			mousePosPressY = HIWORD(lParam);
			g_LMousePress=true;
			break;
		}
		case WM_LBUTTONUP:
		{
			g_LMousePress=false;
			break;
		}
		case WM_RBUTTONDOWN:
		{
			zoomPress = HIWORD(lParam);
			g_RMousePress=true;
			break;
		}
		case WM_RBUTTONUP:
		{
			g_RMousePress=false;
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (hcursor)
			{
				SetCursor(hcursor);
			}

			int mousePosX = LOWORD(lParam);
			int mousePosY = HIWORD(lParam);
			int zoom	  =	HIWORD(lParam);

			if (g_LMousePress)
			{
				g_mousePosX = mousePosX - mousePosPressX;
				g_mousePosY = mousePosY - mousePosPressY;
			}
			else if (g_RMousePress)
			{
				g_zoom = zoom - zoomPress;
			}
			else
			{
				mousePosPressX = mousePosX;
				mousePosPressY = mousePosY;
				zoomPress      = zoom;
				g_mousePosX = 0;
				g_mousePosY = 0;
				g_zoom=0;
			}
			break;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}


	}//End switch(..)


    return (long)DefWindowProc(hWnd, uMsg, wParam, lParam);

}//End MsgProc(..)


/***************************************************************************/
/*                                                                         */
/* Program entry point.                                                    */
/*                                                                         */
/***************************************************************************/
int _stdcall WinMain(HINSTANCE i, HINSTANCE, char* k, int) 
{
    MSG msg;
    char szname[] = "Simple Physics Demo - www.xbdev.net";
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      szname, NULL };
    RegisterClassEx( &wc );
    g_hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                              szname, szname, 
                              WS_OVERLAPPEDWINDOW,//for fullscreen make into WS_POPUP
                              50, 50, 500,500,    //for full screen GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
                              GetDesktopWindow(), NULL, wc.hInstance, NULL);
    
    // Initilise or directX code here!
	CreateX(g_hwnd);

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);     
 

    // Message loop. Note that this has been modified to allow
    // us to execute if no messages are being processed.
    while(1)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            if (!GetMessage(&msg, NULL, 0, 0))
                break;

            DispatchMessage(&msg);
        }
		else
		{
			GameLoopX();
		}
    }

	// We destroy any directx memory we allocated etc, 
    // Tidy up before leaving.
	ReleaseX();

    return 0;
}// End WinMain(..)



/***************************************************************************/
/*                                                                         */
/* Create(..)                                                              */
/* Creatio of our DirectX Applications, called once at the start of our    */
/* program to setup directx.                                               */
/*                                                                         */
/***************************************************************************/

bool CreateX(HWND hWnd)
{
	g_hwnd = hWnd;
    //First of all, create the main D3D object. If it is created successfully 
	//we should get a pointer to an IDirect3D8 interface.
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if( g_pD3D==NULL )
	{
		MessageBox(NULL, "Error", "Error Creating DirectX Object Instance", 0);
		return false;
	}

    //Get the current display mode
    D3DDISPLAYMODE d3ddm;
	HRESULT hr = 
    g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if( hr!=S_OK )
	{
		MessageBox(NULL, "Error", "Failed Getting Display Mode", 0);
		return false;
	}

    //Create a structure to hold the settings for our device
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    //Fill the structure. 
    //We want our program to be windowed, and set the back buffer to a format
    //that matches our current display mode
    d3dpp.Windowed				 = TRUE;
    d3dpp.SwapEffect			 = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat		 = d3ddm.Format;
    //For depth buffering (e.g.) the z-buffer
    d3dpp.BackBufferCount		 = 1;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.EnableAutoDepthStencil = TRUE;
    
	//Create a Direct3D device
	hr =														// Error Checking
    g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,					// UINT Adapter
						 D3DDEVTYPE_HAL,						// D3DDEVTYPE DeviceType
						 hWnd,									// HWND hFocusWindow
						 D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// DWORD BehaviorFlags
						 &d3dpp,								// D3DPRESENT_PARAMETERS *
						 &g_pD3DDevice);						// IDirect3DDevice9 *
	if( hr!=S_OK )
	{
		MessageBox(NULL,"Error","Failed Creating Rendering Device", 0);
		return false;
	}


    //Turn on z-buffering
    //g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	DWORD val;
	//Set our culling mode to None
	g_pD3DDevice->GetRenderState(D3DRS_CULLMODE, &val);
	if (val != D3DCULL_NONE)
		g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	Create();

	return true;
}//End Create(..)


/***************************************************************************/
/*                                                                         */
/* ReleaseX()                                                              */
/* Release any allocated memory, and any directx api's, resources etc      */
/* before our program finally terminates.                                  */
/*                                                                         */
/***************************************************************************/

bool ReleaseX()
{
	Release();

	SafeRelease( g_pD3DDevice );

	SafeRelease( g_pD3D );

	return true;
}//End Release()

/***************************************************************************/
/*                                                                         */
/* Render()                                                                */
/* Basically our gameloop, which gets called repeatedly.                   */
/*                                                                         */
/***************************************************************************/

void GameLoopX()
{
	Update();

}// End Render()

