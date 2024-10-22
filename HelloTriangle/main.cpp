// Sebastian Lorensson 2022-11-27
#include "pch.h"

#define SAMPLE_TEXTURE1 "sampletexture.png"
#define SAMPLE_TEXTURE2 "sampletexture2.png"


#include "dxh.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
// ********************************************************************************
// IGNORE THIS, IT IS NOT RELEVANT TO THE TEST!
// ********************************************************************************

HWND SetupWindow(int width, int height, int x, int y, HINSTANCE hInstance);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


void InitializeD3D11(DXHandler& handler, HWND handle)
{
    handler.Initialize(handle);
	handler.AddTexture(SAMPLE_TEXTURE1);
	handler.AddTexture(SAMPLE_TEXTURE2);
}

// ********************************************************************************

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    static HWND handle = SetupWindow(1200, 800, 100, 50, hInstance);
    static DXHandler dxh;
    InitializeD3D11(dxh, handle);
    util::DeltaTimer deltaTime;
    ShowWindow(handle, nCmdShow);


    bool running = true;
    // Main render loop
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
        if (!running)
            break;

        // any changes to the handler or mesh are applied here
        dxh.Update(deltaTime.Delta());
        
        // Render the scene
        dxh.Render();

        // Finally present the rendered scene
        dxh.Present();
    }
 
    return EXIT_SUCCESS;
}