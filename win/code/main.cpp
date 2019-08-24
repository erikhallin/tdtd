#define _WIN32_WINNT 0x0502 //Needed for GetConsoleWindow()
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>
#include <fcntl.h> //for console

#include "game.h"
#include "definitions.h"
#include "resource.h"

using namespace std;

string _version="0.1";
int _screen_width=1024;//800;
int _screen_height=768;//600;
game Game;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);


int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;
    bool debug_mode=false;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_TD));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "NotThePixels";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    hwnd = CreateWindowEx(0,
                          "NotThePixels",
                          "NotThePixels",
                          //WS_OVERLAPPEDWINDOW,
                          WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          _screen_width,
                          _screen_height,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    EnableOpenGL(hwnd, &hDC, &hRC);

    //debug
    HWND hwnd_console;
    if(debug_mode)
    {
        //Only output console
        AllocConsole() ;
        AttachConsole( GetCurrentProcessId() ) ;
        freopen( "CON", "w", stdout ) ;

        //Set console title
        SetConsoleTitle("Debug Console");
        hwnd_console=GetConsoleWindow();
        MoveWindow(hwnd_console,_screen_width,0,680,510,TRUE);

        cout<<"TD-TD debug console\n";
        cout<<"Version: "<<_version<<endl<<endl;
    }

    //init
    if(!Game.init(_screen_width,_screen_height))
    {
        return false;
    }
    clock_t time_now=clock();
    clock_t time_last=time_now;

    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            bool state_updated=false;
            time_now=clock();
            while( time_last+_time_dif*1000 <= time_now )
            {
                time_last+=_time_dif*1000;
                Game.update();
                state_updated=true;
            }
            if(state_updated)
            {
                Game.draw();
                SwapBuffers(hDC);
            }
        }
    }

    DisableOpenGL(hwnd, hDC, hRC);

    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_MOUSEMOVE:
        {
             Game.mouse_pos[0]=int( LOWORD(lParam));
             Game.mouse_pos[1]=int( HIWORD(lParam))+28;
        }
        break;

        case WM_LBUTTONDOWN:
        {
             Game.mouse_but[0]=true;
        }
        break;

        case WM_LBUTTONUP:
        {
             Game.mouse_but[0]=false;
        }
        break;

        case WM_RBUTTONDOWN:
        {
             Game.mouse_but[1]=true;
        }
        break;

        case WM_RBUTTONUP:
        {
             Game.mouse_but[1]=false;
        }
        break;

        case WM_MOUSEWHEEL:
        {
            if(HIWORD(wParam)>5000)
                Game.mouse_but[2]=true;
            if(HIWORD(wParam)>100&&HIWORD(wParam)<5000)
                Game.mouse_but[3]=true;
        }
        break;

        case WM_KEYDOWN:
		{
			Game.keys[int(wParam)]=true;

			if(wParam==VK_ESCAPE) PostQuitMessage(0);

			//cout<<"Button pressed: "<<int(wParam)<<endl;
		}
		break;

		case WM_KEYUP:
		{
			Game.keys[int(wParam)]=false;
		}
		break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    *hDC = GetDC(hwnd);

    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);

    //set 2D mode
    glClearColor(0.0,0.0,0.0,0.0);  //Set the cleared screen colour to black
    glViewport(0,0,_screen_width,_screen_height);   //This sets up the viewport so that the coordinates (0, 0) are at the top left of the window

    //Set up the orthographic projection so that coordinates (0, 0) are in the top left
    //and the minimum and maximum depth is -10 and 10. To enable depth just put in
    //glEnable(GL_DEPTH_TEST)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,_screen_width,_screen_height,0,-1,1);

    //Back to the modelview so we can draw stuff
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*//Enable antialiasing
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearStencil(0);*/
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}

