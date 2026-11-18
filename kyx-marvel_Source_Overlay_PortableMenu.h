#pragma once
// ============================================================================
//  PortableMenu.h  –  MentalityV2 sidebar+tab navigation shell
//
//  Drop-in modular menu framework extracted from KyxFort's MentalityV2 UI.
//  Provides: sidebar navigation, sub-tab header, two-column card layout,
//            theme system (dark/light), blur background, user info card,
//            notifications, popups, and all custom:: widgets.
//
//  REQUIREMENTS:
//    - MentalityV2 ImGui fork (External/MentalityV2/)
//    - DirectX 11 device, context, swap chain already created
//    - FreeType-enabled ImGui font atlas (MentalityV2 default)
//
//  INTEGRATION:
//    1. #include "PortableMenu.h"
//    2. After ImGui context creation:    PortableMenu_LoadFonts();
//    3. After DX11 device creation:      PortableMenu_SetD3D11(dev, ctx, sc, rtv, hwnd);
//    4. One-time init:                   PortableMenu_Initialize();
//    5. Each frame inside NewFrame/Render: PortableMenu_Render();
//
//  CUSTOMIZATION POINTS (search for "USER:" in PortableMenu.cpp):
//    - Sidebar sections & items        (USER: SIDEBAR)
//    - Sub-tab definitions per section  (USER: TABS)
//    - Content pages                    (USER: CONTENT)
//    - User info card                   (USER: INFO CARD)
//    - Header branding                  (USER: HEADER)
// ============================================================================

// --- Public API (same shape as MentalityGui) ---

/// Load all Poppins + Icomoon fonts into the ImGui atlas.
/// Call BEFORE ImGui_ImplDX11_CreateDeviceObjects().
void PortableMenu_LoadFonts();

/// One-time texture/image init. Call after DX11 device is ready.
void PortableMenu_Initialize();

/// Render the full menu window. Call each frame between NewFrame/Render.
void PortableMenu_Render();

/// Bridge DX11 globals so blur and texture loading work.
void PortableMenu_SetD3D11(
    struct ID3D11Device*           device,
    struct ID3D11DeviceContext*     ctx,
    struct IDXGISwapChain*         swapChain,
    struct ID3D11RenderTargetView* rtv,
    struct HWND__*                 window);
