#pragma once
// =====================================================================
// Overlay.hpp — DX11 transparent overlay with MentalityV2 menu
// =====================================================================

#include "Render/Renderer.hpp"
#include <d3d11.h>
#include <atomic>

class Overlay {
public:
    static Overlay& Get() {
        static Overlay instance;
        return instance;
    }

    bool CreateOverlay();
    void DestroyOverlay();

    bool StartRender();
    void EndRender();
    void RenderMenu();

    ImDrawList* GetBackgroundDrawList();

    bool IsMenuOpen() const { return m_menuOpen; }
    HWND GetHwnd() const { return m_hwnd; }
    ImGuiRenderer* GetRenderer() { return &m_renderer; }

    int GetWidth()  const { return m_width; }
    int GetHeight() const { return m_height; }

    ID3D11Device*           GetDevice()        const { return m_device; }
    ID3D11DeviceContext*    GetDeviceContext()  const { return m_context; }
    IDXGISwapChain*         GetSwapChain()     const { return m_swapChain; }
    ID3D11RenderTargetView* GetRenderTarget()  const { return m_rtv; }

private:
    Overlay() = default;

    bool CreateOverlayWindow();
    bool CreateDevice();
    void DestroyDevice();
    bool CreateImGui();
    void DestroyImGui();
    void BridgeMentalityGlobals();

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND                     m_hwnd        = nullptr;
    WNDCLASSEX               m_wc          = {};
    int                      m_width       = 2560;
    int                      m_height      = 1440;

    ID3D11Device*            m_device      = nullptr;
    ID3D11DeviceContext*     m_context     = nullptr;
    IDXGISwapChain*          m_swapChain   = nullptr;
    ID3D11RenderTargetView*  m_rtv         = nullptr;
    bool                     m_tearingSupported = false;

    bool                     m_menuOpen    = true;

    ImGuiRenderer            m_renderer;
};

inline Overlay& overlay = Overlay::Get();
