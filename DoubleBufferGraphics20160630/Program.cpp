#include <windows.h>
#include <tchar.h>

#include <sti.h>
#include <gdiplus.h>
using namespace Gdiplus;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR szClassNme[] = _T("DoubleBufferGraphics20160630");

GdiplusStartupInput gdiSI;
ULONG_PTR gdiToken;

HWND ghWnd;
Bitmap* gOffScreenBitmap;
Graphics* gOffScreenGraphics;

bool isDownSpaceKey = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst,
                   LPSTR lpszCmdLine, int nCmdShow)
{
	GdiplusStartup(&gdiToken, &gdiSI, NULL);

    MSG msg;
    WNDCLASS myProg;
    if (!hPreInst) {
        myProg.style            = CS_HREDRAW | CS_VREDRAW;
        myProg.lpfnWndProc      = WndProc;
        myProg.cbClsExtra       = 0;
        myProg.cbWndExtra       = 0;
        myProg.hInstance        = hInstance;
        myProg.hIcon            = NULL;
        myProg.hCursor          = LoadCursor(NULL, IDC_ARROW);
		myProg.hbrBackground    = (HBRUSH)(COLOR_WINDOW+1);
        myProg.lpszMenuName     = NULL;
        myProg.lpszClassName    = szClassNme;
        if (!RegisterClass(&myProg))
            return FALSE;
    }
    ghWnd = CreateWindow(szClassNme,	// class名
		_T("DoubleBufferを高速にした"),		// タイトル
        WS_OVERLAPPEDWINDOW,		// Style
        CW_USEDEFAULT,				// X
        CW_USEDEFAULT,				// Y
        CW_USEDEFAULT,				// Width
        CW_USEDEFAULT,				// Height
        NULL,						// 親ウィンドウまたはオーナーウィンドウのハンドル
        NULL,						// メニューハンドルまたは子ウィンドウ ID
        hInstance,					// アプリケーションインスタンスのハンドル
        NULL);						// ウィンドウ作成データ
    ShowWindow(ghWnd, nCmdShow);
    UpdateWindow(ghWnd);
	
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

	GdiplusShutdown(gdiToken);

    return (msg.wParam);
}

void OnPaint(HDC hdc)
{
    Graphics g(hdc);
	
    gOffScreenGraphics->Clear(Color::White);

    Gdiplus::Pen pen1(Color(255, 255, 0, 0), 5);
	gOffScreenGraphics->DrawRectangle(&pen1, 0, 0, 200, 300);
	
	Gdiplus::Font font(L"ＭＳ ゴシック", 18);
	Gdiplus::SolidBrush blackBrush(Color(255, 0, 0, 0));
	Gdiplus::StringFormat format;
	format.SetAlignment(StringAlignmentCenter);
	if (isDownSpaceKey) {
		gOffScreenGraphics->DrawString(_T("よくできました！"), -1, &font, RectF(0.0f, 0.0f, 200.0f, 300.0f), &format, &blackBrush);
	}
	else {
		gOffScreenGraphics->DrawString(_T("Spaceキーを押してください。"), -1, &font, RectF(0.0f, 0.0f, 200.0f, 300.0f), &format, &blackBrush);
	}

	g.DrawImage(gOffScreenBitmap, 0, 0);
	
    return;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg) {
        case WM_PAINT:
            hdc = BeginPaint(ghWnd, &ps);
            OnPaint(hdc);
            EndPaint(ghWnd, &ps);
            break;
        case WM_KEYDOWN:
            switch (wParam) {
                case VK_SPACE:
					isDownSpaceKey = true;
					InvalidateRect(ghWnd,0,false);
					break;
				default:
					break;
			}
			break;
        case WM_KEYUP:
            switch (wParam) {
                case VK_SPACE:
					isDownSpaceKey = false;
					InvalidateRect(ghWnd,0,false);
					break;
				default:
					break;
			}
			break;
		case WM_CREATE:
			gOffScreenBitmap = new Bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
			gOffScreenGraphics = Graphics::FromImage(gOffScreenBitmap);
			break;
		case WM_ERASEBKGND:
			return TRUE;
        case WM_DESTROY:
			gOffScreenGraphics->Flush();
			delete gOffScreenBitmap;
			delete gOffScreenGraphics;
            PostQuitMessage(0);
            break;
        default:
            return(DefWindowProc(hWnd, msg, wParam, lParam));
    }
    return (0L);
}
