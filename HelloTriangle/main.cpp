// Sebastian Lorensson 2022-11-27
#include "pch.h"

#define SAMPLE_TEXTURE1 "sampletexture.png"
#define SAMPLE_TEXTURE2 "sampletexture2.png"


#include "dxh.h"
#include "window.h"
// ********************************************************************************
// IGNORE THIS, IT IS NOT RELEVANT TO THE TEST!
// ********************************************************************************

void InitializeD3D11(DXHandler& handler, HWND handle)
{
    handler.Initialize(handle);
	handler.AddTexture(SAMPLE_TEXTURE1);
	handler.AddTexture(SAMPLE_TEXTURE2);
}

// ********************************************************************************

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
    static HWND handle = SetupWindow(800, 600, 560, 200, hInstance);
    static DXHandler dxh;
    InitializeD3D11(dxh, handle);
    util::DeltaTimer deltaTime;
    ShowWindow(handle, nCmdShow);

    // ==============================================
    // INSERT IMGUI STUFF HERE


    // ==============================================
    
    // Main render loop
    MSG msg{};
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else 
        {
            // rendering operations happen here
            dxh.Render(deltaTime.Delta());
            // ======================================
            // INSERT IMGUI RENDER CALLS HERE
            // ======================================
        }
    }
 
    return static_cast<int>(msg.wParam);
}