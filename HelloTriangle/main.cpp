// Sebastian Lorensson 2022-11-27
// HELLO TRIANGLE MAIN
#include "pch.h"

#include "dxh.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"


// Forward declaration
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// creates window and return a handle
HWND SetupWindow(int width, int height, int x, int y, HINSTANCE hInstance) 
{
	TCHAR szWindowClass[] = _T("Hello Triangle");
	TCHAR szTitle[] = _T("Hello Triangle");

	WNDCLASSEX wcex{ 0 };

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		util::ErrorMessageBox("Could not register WNDCLASSEX.");
		exit(-1);
	}

	HWND hWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW,
		x, y, //center, a bit up
		width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (!hWnd)
	{
		util::ErrorMessageBox("Window setup failed. Last err: " + LASTERR);
		exit(-1);
	}
	return hWnd;

}

// texture names
#define SAMPLE_TEXTURE1 "sampletexture.png"
#define SAMPLE_TEXTURE2 "sampletexture2.png"

void InitializeD3D11(DXHandler& handler, HWND handle)
{
    handler.Initialize(handle);
	handler.AddTexture(SAMPLE_TEXTURE1);
	handler.AddTexture(SAMPLE_TEXTURE2);
}

// ********************************************************************************
// MAIN
// ********************************************************************************
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	// Initialization ======================================================
    static HWND handle = SetupWindow(800, 600, 100, 50, hInstance);
    static DXHandler dxh;
    InitializeD3D11(dxh, handle);
    util::DeltaTimer deltaTime;
    ShowWindow(handle, nCmdShow);
	// =====================================================================



	// Main render loop ====================================================
    bool running = true; 

    while (running)
    {
        MSG msg{};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }
        if (!running) // exit the loop if the window has been closed
            break;

        // any changes to the handler or mesh are applied inside this function
        dxh.Update(deltaTime.Delta());
        
        // Render the scene and anything else
        dxh.Render();

        // Finally present the rendered scene
        dxh.Present();
    }
	//=========================================================================
	
	// App exit
    return EXIT_SUCCESS;
}


// This handles window events
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// insert external window procs here

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}