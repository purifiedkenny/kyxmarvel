#include "Pch.hpp"
#include "Overlay.hpp"
#include "PortableMenu.h"
#include "Config.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// =====================================================================
// Window Procedure — matches KyxApex exactly: forward to ImGui, then
// DefWindowProc.  No WM_NCHITTEST tricks, no message filtering.
// =====================================================================

LRESULT CALLBACK Overlay::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
        return true;

    switch (msg) {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// =====================================================================
// Device Creation (FLIP_DISCARD with tearing, fallback to legacy)
// =====================================================================

bool Overlay::CreateDevice() {
    constexpr DXGI_SWAP_EFFECT FLIP_DISCARD = static_cast<DXGI_SWAP_EFFECT>(4);
    constexpr UINT ALLOW_TEARING = 0x800;

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount        = 2;
    sd.BufferDesc.Width   = static_cast<UINT>(m_width);
    sd.BufferDesc.Height  = static_cast<UINT>(m_height);
    sd.BufferDesc.Format  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate = { 0, 1 };
    sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow       = m_hwnd;
    sd.SampleDesc         = { 1, 0 };
    sd.Windowed           = TRUE;
    sd.SwapEffect         = FLIP_DISCARD;
    sd.Flags              = ALLOW_TEARING;

    D3D_FEATURE_LEVEL featureLevel;
    constexpr D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        levels, 1, D3D11_SDK_VERSION,
        &sd, &m_swapChain, &m_device, &featureLevel, &m_context);

    if (SUCCEEDED(hr)) {
        m_tearingSupported = true;
    } else {
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags      = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
            levels, 1, D3D11_SDK_VERSION,
            &sd, &m_swapChain, &m_device, &featureLevel, &m_context);
        m_tearingSupported = false;
    }
    if (FAILED(hr)) return false;

    // Minimize DXGI frame queue
    IDXGIDevice1* dxgiDevice = nullptr;
    if (SUCCEEDED(m_device->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDevice))) {
        dxgiDevice->SetMaximumFrameLatency(1);
        dxgiDevice->Release();
    }

    ID3D11Texture2D* backBuffer = nullptr;
    m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (!backBuffer) return false;
    m_device->CreateRenderTargetView(backBuffer, nullptr, &m_rtv);
    backBuffer->Release();
    return m_rtv != nullptr;
}

void Overlay::DestroyDevice() {
    if (m_rtv)       { m_rtv->Release();       m_rtv = nullptr; }
    if (m_swapChain) { m_swapChain->Release();  m_swapChain = nullptr; }
    if (m_context)   { m_context->Release();    m_context = nullptr; }
    if (m_device)    { m_device->Release();     m_device = nullptr; }
}

// =====================================================================
// ImGui Init — MentalityV2 4-function API
// =====================================================================

bool Overlay::CreateImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    // Step 1: Load MentalityV2 fonts (BEFORE backends)
    PortableMenu_LoadFonts();

    // Step 2: Init backends
    if (!ImGui_ImplWin32_Init(m_hwnd)) return false;
    if (!ImGui_ImplDX11_Init(m_device, m_context)) return false;

    // Step 3: Bridge DX11 state into MentalityV2 globals
    BridgeMentalityGlobals();

    // Step 4: Apply MentalityV2 style
    ImGuiStyle& s = ImGui::GetStyle();
    s.FramePadding     = ImVec2(15, 20);
    s.ItemSpacing      = ImVec2(10, 10);
    s.WindowPadding    = ImVec2(0, 0);
    s.ScrollbarSize    = 1;
    s.SeparatorTextPadding = ImVec2(10, 10);
    s.FrameRounding    = 2.f;
    s.WindowRounding   = 20.f;
    s.PopupRounding    = 5.f;
    s.ChildBorderSize  = 1.f;
    s.WindowBorderSize = 0.f;
    s.PopupBorderSize  = 0.f;
    s.WindowShadowSize       = 100.0f;
    s.WindowShadowOffsetDist = 0.0f;
    s.WindowShadowOffsetAngle = 3.14159265f * 0.25f;
    s.Colors[ImGuiCol_Border]       = ImVec4(0, 0, 0, 0);
    s.Colors[ImGuiCol_Separator]    = ImVec4(1, 1, 1, 0.2f);
    s.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);
    s.Colors[ImGuiCol_WindowShadow] = ImVec4(0.08f, 0.08f, 0.08f, 0.35f);

    // Step 5: Load textures
    PortableMenu_Initialize();
    return true;
}

void Overlay::DestroyImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Overlay::BridgeMentalityGlobals() {
    PortableMenu_SetD3D11(m_device, m_context, m_swapChain, m_rtv, m_hwnd);
}

// =====================================================================
// Overlay Window
// =====================================================================

bool Overlay::CreateOverlayWindow() {
    m_width  = GetSystemMetrics(SM_CXSCREEN);
    m_height = GetSystemMetrics(SM_CYSCREEN);

    Screen[0] = static_cast<float>(m_width);
    Screen[1] = static_cast<float>(m_height);
    ScreenCenter[0] = Screen[0] / 2.0f;
    ScreenCenter[1] = Screen[1] / 2.0f;

    m_renderer.SetScreenSize(static_cast<double>(m_width), static_cast<double>(m_height));

    m_wc = {};
    m_wc.cbSize        = sizeof(WNDCLASSEX);
    m_wc.style         = CS_CLASSDC;
    m_wc.lpfnWndProc   = WndProc;
    m_wc.hInstance      = GetModuleHandle(nullptr);
    m_wc.lpszClassName  = L"KyxMarvelOverlay";
    RegisterClassEx(&m_wc);

    // Start WITHOUT WS_EX_TRANSPARENT since m_menuOpen defaults true.
    // KyxApex pattern: toggle WS_EX_TRANSPARENT via SetWindowLongPtr.
    m_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        m_wc.lpszClassName, L"",
        WS_POPUP,
        0, 0, m_width, m_height,
        nullptr, nullptr, m_wc.hInstance, nullptr);

    if (!m_hwnd) {
        LOG_ERROR("Failed to create overlay window");
        return false;
    }

    // KyxApex pattern: LWA_ALPHA(255) — entire window is a normal input
    // target.  Transparency comes from the swap chain clear color + DWM,
    // NOT from color-keying (which makes matching pixels click-through
    // at the compositor level before WndProc ever fires).
    SetLayeredWindowAttributes(m_hwnd, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    // DWM frame extension — use KyxApex's calculated margins instead of
    // {-1}, which causes DefWindowProc to return HTCAPTION for the whole
    // client area, turning every click into a title-bar drag.
    {
        RECT clientArea{}, windowArea{};
        GetClientRect(m_hwnd, &clientArea);
        GetWindowRect(m_hwnd, &windowArea);

        POINT diff{};
        ClientToScreen(m_hwnd, &diff);

        const MARGINS margins{
            windowArea.left + (diff.x - windowArea.left),
            windowArea.top  + (diff.y - windowArea.top),
            clientArea.right,
            clientArea.bottom
        };
        DwmExtendFrameIntoClientArea(m_hwnd, &margins);
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    return true;
}

bool Overlay::CreateOverlay() {
    if (!CreateOverlayWindow()) {
        LOG_ERROR("Failed to create overlay window");
        return false;
    }
    if (!CreateDevice()) {
        LOG_ERROR("Failed to create D3D11 device");
        return false;
    }
    if (!CreateImGui()) {
        LOG_ERROR("Failed to create ImGui");
        return false;
    }
    LOG_INFO("Overlay created: {}x{}", m_width, m_height);
    return true;
}

void Overlay::DestroyOverlay() {
    DestroyImGui();
    DestroyDevice();
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
        UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
        m_hwnd = nullptr;
    }
}

// =====================================================================
// Render Frame
// =====================================================================

bool Overlay::StartRender() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT) {
            Globals::Running = false;
            return false;
        }
    }

    // Toggle menu with INSERT (edge-detected)
    // KyxApex pattern: add/remove WS_EX_TRANSPARENT via SetWindowLongPtr
    static bool insertWasDown = false;
    bool insertDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
    if (insertDown && !insertWasDown) {
        m_menuOpen = !m_menuOpen;

        if (m_menuOpen) {
            // Remove WS_EX_TRANSPARENT so clicks reach our WndProc
            SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
                WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
            SetForegroundWindow(m_hwnd);
        } else {
            // Add WS_EX_TRANSPARENT so clicks pass through to the game
            SetWindowLongPtr(m_hwnd, GWL_EXSTYLE,
                WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
            config.SaveConfig();
        }
    }
    insertWasDown = insertDown;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    constexpr float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_context->OMSetRenderTargets(1, &m_rtv, nullptr);
    m_context->ClearRenderTargetView(m_rtv, clearColor);

    return true;
}

void Overlay::RenderMenu() {
    if (!m_menuOpen) return;
    PortableMenu_Render();
}

ImDrawList* Overlay::GetBackgroundDrawList() {
    return ImGui::GetBackgroundDrawList();
}

void Overlay::EndRender() {
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    m_swapChain->Present(0, m_tearingSupported ? 0x200u : 0u);
}
