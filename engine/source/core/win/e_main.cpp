#include <windows.h>
#include <direct.h>
#include "core/render/r_render.h" // screen sizes there
#include "platform.h"
#include "resource.h"

#ifndef GET_X_LPARAM
# define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif

#ifndef GET_Y_LPARAM
# define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

using namespace mt;
//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE   g_hInst = NULL;  
HWND        g_hWnd  = NULL;

const float g_delta = 0.05f;
float       g_yaw   = 0.0f;
float       g_pitch = 0.0f;

int         g_downCount = 0;

//static const v2i16 gWindowSize(1024, 768);  // 4:3
static const v2i16 gWindowSize(1365, 768);  // 16:9
//static const v2i16 gWindowSize(1920, 1080); // FullHD
//static const v2i16 gWindowSize(2208, 1242); // iPhone 6 Plus
//static const v2i16 gWindowSize(1334,  750); // iPhone 6
//static const v2i16 gWindowSize(1136,  640); // iPhone 5
//static const v2i16 gWindowSize( 960,  640); // iPhone 4
//static const v2i16 gWindowSize(2048, 1536); // iPad Retina
//static const v2i16 gWindowSize(2560, 1600); // Kindle
//static const v2i16 gWindowSize(2880, 1800); // Mac
//static const v2i16 gWindowSize(1024/8,768/8); // tiny mode

//char g_loc[2] = {'e','n'};
//char g_loc[2] = {'d','e'};
//char g_loc[2] = {'r','u'};
//char g_loc[2] = {'p','t'};
//char g_loc[2] = {'e','s'};
char g_loc[2] = {'i','t'};

//////////////////////////////////////////////////////////////////////////
// File stuff

extern "C" FILE* open_file(const char* szFile, bool bRead, bool bIsResourceFile)
{
  if(bIsResourceFile && !bRead)
  {
    EALWAYS_ASSERT("Resources cannot be opened for writing");
    return NULL;
  }

  std::string path("_userdata/");
  if(!bIsResourceFile) {
    _mkdir("_userdata");
    path += szFile;
    szFile = path.c_str();
  }

  FILE* pFile    = NULL;
  errno_t result = fopen_s(&pFile, szFile, bRead ? "rb" : "wb");
  if(NULL != pFile)
    return pFile;
  if(!bIsResourceFile && !bRead)
    result = fopen_s(&pFile, szFile, "wb");
  return pFile;
}

//////////////////////////////////////////////////////////////////////////
// debug stuff

bool AssertFun(bool& enabled, const char* FileName, int LineNum, const char* TextCondition, ...){
  char Expr[2048];
  char TextBuffer[2048];

  va_list args;
  va_start(args, TextCondition);
  _vsnprintf_s(Expr, sizeof(Expr)/sizeof(Expr[0]), TextCondition, args );
  va_end(args);

  _snprintf_s(TextBuffer, sizeof(TextBuffer) / sizeof(TextBuffer[0]), "ASSERT(%s) failed in %s:%i\n", Expr, FileName, LineNum);
  switch(::MessageBox(g_hWnd, TextBuffer, "ASSERT", MB_YESNOCANCEL | MB_ICONWARNING))
  {
  case IDYES:
    return !!IsDebuggerPresent();
  case IDNO:
    return false;
  case IDCANCEL:
    enabled = false;
  }
  return false;
}

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

void SetDefaultWorkDir();

//static int allochook( int /*allocType*/, void* /*userData*/, size_t size, int /*blockType*/, long /*requestNumber*/, const unsigned char* /*filename*/, int /*lineNumber*/)
//{
//  if ( size == 108 )
//    _asm int 3; 
//  return TRUE;
//}
//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE /*hPrevInstance*/,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
  g_hInst = hInstance;
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  //_CrtSetBreakAlloc(4578);
  //_CrtSetAllocHook(allochook);
  if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
    return 0;
 
   SetDefaultWorkDir();
   // shittiest cmd line parser one ever saw
   char* pcmd = lpCmdLine;
   while(*pcmd)
   {
     if('-' != *pcmd++)
       continue;

     const char option = *pcmd;
     if('d' == option) // set working directory
     {
       ++pcmd;
       char* dir = pcmd;
       while(*pcmd && ' ' != *pcmd) // skip to next option
         ++pcmd;
       if(*pcmd)
         *pcmd++ = NULL; // terminate path (note: I am modifying cmd line there!)
       SetCurrentDirectory(dir);
     }
     else if('l' == option) // locale
     {
       ++pcmd;
       char* loc = pcmd;
       while(*pcmd && ' ' != *pcmd) // skip to next option
         ++pcmd;
       if(2 == pcmd - loc)
         memcpy(g_loc, loc, 2);
     }
     else while(*pcmd && ' ' != *pcmd) // unknown option
       ++pcmd;

   }

  screen_resize(gWindowSize.x, gWindowSize.y);

  if(!game_init())
    return 0;

  // Main message loop:
  MSG msg = {0};
  for (;;)
  {
    const long long t = GetCPUCycles() + 1000 * GetCPUCyclesPerMSec() / 60;
    if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
    {
      if (GetMessage(&msg, NULL, 0, 0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }
      else
      {
        game_release();
        return (int)msg.wParam;
      }
    }
    game_tick();
    if(g_hWnd)
      game_render();
    const long long n = GetCPUCycles();
    if(t > n)
      ::Sleep(DWORD((t - n) / GetCPUCyclesPerMSec()));
  }
}

void SetDefaultWorkDir()
{
  TCHAR szFPath[MAX_PATH];
  size_t p = (size_t)GetModuleFileName(NULL, &szFPath[0], MAX_PATH);
  for(; 0 != p; --p)
  {
    if(L'\\' != szFPath[p])
      continue;
    szFPath[p] = 0;
    SetCurrentDirectory(szFPath);
    break;
  }
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
  // Register class
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX); 
  wcex.style          = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc    = WndProc;
  wcex.cbClsExtra     = 0;
  wcex.cbWndExtra     = 0;
  wcex.hInstance      = hInstance;
  wcex.hIcon          = LoadIcon(hInstance, (LPCTSTR)101);
  wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName   = NULL;
  wcex.lpszClassName  = "WindowClass";
  wcex.hIconSm        = LoadIcon(wcex.hInstance, (LPCTSTR)101);
  if( !RegisterClassEx(&wcex) )
    return E_FAIL;


  RECT rect;
  SetRect(&rect, 0, 0, gWindowSize.x, gWindowSize.y);
  AdjustWindowRect(&rect,  WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, false);
  g_hWnd = CreateWindowEx(/*WS_EX_TOPMOST*/0, "WindowClass", "Window", WS_CAPTION | WS_SYSMENU | WS_THICKFRAME,
    CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, hInstance, NULL);

  if( !g_hWnd )
    return E_FAIL;

  ShowWindow( g_hWnd, nCmdShow );

  return S_OK;
}

inline int X(LPARAM lParam)
{
  return int(GET_X_LPARAM(lParam));
}

inline int Y(LPARAM lParam)
{
  return int(GET_Y_LPARAM(lParam));
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
  bool handled = true;
  int downcount = g_downCount;

  const int screenW = gWindowSize.x;
  const int screenH = gWindowSize.y;

  switch (message) 
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;

  case WM_MOUSEMOVE:
    if(GetAsyncKeyState(VK_LBUTTON)& 0x8000)
      input_touch_move(X(lParam), Y(lParam), 0);
    if(GetAsyncKeyState(VK_RBUTTON)& 0x8000)
    {
      input_touch_move(X(lParam), Y(lParam), 1);
      input_touch_move(-X(lParam) + screenW, -Y(lParam) + screenH, 2);
    }
    break;
  case WM_LBUTTONDOWN:
    ++g_downCount;
    input_touch_begin(X(lParam), Y(lParam), 0);
    break;
  case WM_LBUTTONUP:
    --g_downCount;
    input_touch_end(X(lParam), Y(lParam), 0);
    break;
  case WM_RBUTTONDOWN:
    ++g_downCount;
    input_touch_begin(X(lParam), Y(lParam), 1);
    input_touch_begin(-X(lParam) + screenW, -Y(lParam) + screenH, 2);
    break;
  case WM_RBUTTONUP:
    --g_downCount;
    input_touch_end(X(lParam), Y(lParam), 1);
    input_touch_end(-X(lParam) + screenW, -Y(lParam) + screenH, 2);
    break;
  case WM_KEYDOWN:
    switch( wParam )
    {
    case VK_LEFT:
      g_yaw   -= g_delta;
      break;
    case VK_RIGHT:
      g_yaw   += g_delta;
      break;
    case VK_UP:
      g_pitch += g_delta;
      break;
    case VK_DOWN: 
      g_pitch -= g_delta;
      break;
    }
    break;
  case WM_SIZE:
    screen_resize(LOWORD(lParam), HIWORD(lParam));
    break;
  default:
    handled = false;
  }

  g_yaw = 0.0f;
  g_pitch = 0.0f;
  if(GetAsyncKeyState(VK_LEFT))
    g_yaw -= (f32)DEG2RAD(45.0f);
  if(GetAsyncKeyState(VK_RIGHT))
    g_yaw += (f32)DEG2RAD(45.0f);

  if(GetAsyncKeyState(VK_UP))
    g_pitch -= (f32)DEG2RAD(45.0f);
  if(GetAsyncKeyState(VK_DOWN))
    g_pitch += (f32)DEG2RAD(45.0f);

  g_yaw   = NormalizeAngle(g_yaw);
  g_pitch = NormalizeAngle(g_pitch);

  //float ax =  cos(g_pitch) * sin(g_yaw), 
  //      ay = -cos(g_pitch) * cos(g_yaw),
  //      az =  sin(g_pitch);

  input_acceleration(sin(g_pitch), sin(g_yaw), cos(g_yaw));

  if(0 == downcount && 0 != g_downCount)
    SetCapture(g_hWnd);
  else if(0 != downcount && 0 == g_downCount)
    ReleaseCapture();

  return handled ? 0 : DefWindowProc(hWnd, message, wParam, lParam);
}

namespace
{
  struct TextBox_t
  {
    TextBox_t(const wchar_t * const szTitle, wchar_t* buffer, unsigned int size)
      : szTitle(szTitle)
      , buffer (buffer)
      , size   (size){}
    const wchar_t * const szTitle;
    wchar_t*        buffer;
    unsigned int    size;
  private:
    TextBox_t& operator=(const TextBox_t&);
  };

  // Message handler for enter name box.
  INT_PTR CALLBACK EnterName(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
  {
    switch (message)
    {
    case WM_INITDIALOG:
      {
        SetWindowLongPtr(hDlg, GWL_USERDATA, (LONG)lParam);
        if(const TextBox_t* t = (const TextBox_t*)(lParam)) {
          SetWindowTextW(hDlg, t->szTitle);
          SetDlgItemTextW(hDlg, IDC_TEXTFLD, t->buffer);
        }
        return (INT_PTR)TRUE;
      }

    case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
      {
        if(LOWORD(wParam) == IDOK)
        {
          if(TextBox_t* t = (TextBox_t*)GetWindowLongPtr(hDlg, GWL_USERDATA))
            GetDlgItemTextW(hDlg, IDC_TEXTFLD, t->buffer, t->size);
        }
        EndDialog(hDlg, LOWORD(wParam));
        return (INT_PTR)TRUE;
      }
      break;
    }
    return (INT_PTR)FALSE;
  }
}

extern "C" void TextBox(const wchar_t * const szTitle, wchar_t* buffer, unsigned int size)
{
  TextBox_t t(szTitle, buffer, size);
  OnTextBoxResult((INT_PTR)TRUE == DialogBoxParam(g_hInst, MAKEINTRESOURCE(IDD_ENTERNAME), g_hWnd, EnterName, (LPARAM)&t));
}

extern "C" bool GetLocale(char loc[2])
{
  loc[0] = g_loc[0];
  loc[1] = g_loc[1];
  return true;
}
