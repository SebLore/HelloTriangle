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
    static HWND handle = SetupWindow(1200, 800, 560, 200, hInstance);
    static DXHandler dxh;
    InitializeD3D11(dxh, handle);
    util::DeltaTimer deltaTime;
    ShowWindow(handle, nCmdShow);

    // ==============================================
    // INSERT IMGUI STUFF HERE
    // ==============================================
	bool show_demo_window = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(handle);
	ImGui_ImplDX11_Init(dxh.GetDevice(), dxh.GetContext());

    bool done = false;
   
    // Main render loop
    while (!done) 
    {   
        MSG msg{};
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		ImGui::Render();
        dxh.Render(deltaTime.Delta());

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        dxh.Present();
        // rendering operations happen here
            
        // ======================================
        // INSERT IMGUI RENDER CALLS HERE
        // ======================================
    }
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

 
    return EXIT_SUCCESS;
}


// WINDOW STUFF
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


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		return true;

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
