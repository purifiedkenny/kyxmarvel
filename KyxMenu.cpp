// ============================================================================
//  KyxMenu.cpp  –  MentalityV2 sidebar+tab navigation shell for KyxMarvel
//
//  Based on PortableMenu, customized for Marvel Rivals DMA Overlay.
//  All settings bound to KyxMarvel::Settings via config singleton.
// ============================================================================

#include "PortableMenu.h"

// MentalityV2 includes
#include <src/GUI/main.h>

// KyxMarvel config
#include "Config.hpp"
#include "Globals.hpp"
#include "CloudConfig.hpp"
#include "Auth.hpp"
#include <Features/RaycastVisCheck.h>

#include <map>

// ============================================================================
//  Font loading  –  13 Poppins slots + Icomoon icon merges
// ============================================================================
void PortableMenu_LoadFonts()
{
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig cfg;
    cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_NoHinting
                         | ImGuiFreeTypeBuilderFlags_LightHinting
                         | ImGuiFreeTypeBuilderFlags_LoadColor;

    static ImWchar icomoon_ranges[] = { 0x1, 0x10FFFD, 0 };

    static ImFontConfig icomoon_config;
    icomoon_config.OversampleH = icomoon_config.OversampleV = 1;
    icomoon_config.MergeMode   = true;
    icomoon_config.GlyphOffset.y = 3.f;
    icomoon_config.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;

    const ImWchar* textRanges = io.Fonts->GetGlyphRangesChineseSimplifiedCommon();

    // Base atlas slot
    io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 18.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges);

    font::default_r = io.Fonts->AddFontFromMemoryTTF(PoppinsRegular, sizeof(PoppinsRegular), 18.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges);

    font::default_m = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 18.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges);

    font::default_s = io.Fonts->AddFontFromMemoryTTF(PoppinsSemiBold, sizeof(PoppinsSemiBold), 18.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 18.f, &icomoon_config, icomoon_ranges);

    font::esp_font = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 15.f, &cfg, textRanges);

    icomoon_config.GlyphOffset.y = 0.f;
    font::description_font = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 16.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 16.f, &icomoon_config, icomoon_ranges);

    icomoon_config.GlyphOffset.y = 3.f;
    font::regular_m = io.Fonts->AddFontFromMemoryTTF(PoppinsRegular, sizeof(PoppinsRegular), 23.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 23.f, &icomoon_config, icomoon_ranges);

    icomoon_config.GlyphOffset.y = 0.f;
    font::regular_l = io.Fonts->AddFontFromMemoryTTF(PoppinsRegular, sizeof(PoppinsRegular), 35.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 35.f, &icomoon_config, icomoon_ranges);

    font::s_inter_semibold = io.Fonts->AddFontFromMemoryTTF(PoppinsSemiBold, sizeof(PoppinsSemiBold), 17.f, &cfg, textRanges);
    font::inter_bold       = io.Fonts->AddFontFromMemoryTTF(PoppinsSemiBold, sizeof(PoppinsSemiBold), 18.f, &cfg, textRanges);

    font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(PoppinsSemiBold, sizeof(PoppinsSemiBold), 29.f, &cfg, textRanges);
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(icomoon_compressed_data_base85, 27.f, &icomoon_config, icomoon_ranges);

    font::small_font   = io.Fonts->AddFontFromMemoryTTF(PoppinsSemiBold, sizeof(PoppinsSemiBold), 14.f, &cfg, textRanges);
    font::inter_medium = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium,   sizeof(PoppinsMedium),   17.f, &cfg, textRanges);
}

// ============================================================================
//  One-time init
// ============================================================================
void PortableMenu_Initialize()
{
    LoadImages();
}

// ============================================================================
//  Sidebar helpers
// ============================================================================

static void SidebarSectionHeader(const char* label) {
    ImGui::PushFont(font::small_font);
    ImGui::SetCursorPosX(18.f);
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 6.f);
    ImGui::TextColored(ImVec4(c::text::description::default_color.Value.x,
        c::text::description::default_color.Value.y,
        c::text::description::default_color.Value.z, 0.7f), label);
    ImGui::PopFont();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.f);
}

static bool SidebarItem(const char* icon, const char* label, bool active) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;

    const float item_h = 32.f;
    const float sidebar_w = 200.f;
    ImVec2 pos = window->DC.CursorPos;
    ImRect bb(pos + ImVec2(8, 0), pos + ImVec2(sidebar_w - 8, item_h));

    ImGuiID id = window->GetID(label);
    ImGui::ItemSize(bb, 0);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    static std::map<ImGuiID, float> bg_alpha;
    float& alpha = bg_alpha[id];
    float target = active ? 1.0f : (hovered ? 0.4f : 0.0f);
    alpha = ImLerp(alpha, target, g.IO.DeltaTime * 12.f);

    if (alpha > 0.01f) {
        ImColor bg = utils::GetColorWithAlpha(c::anim::active, alpha * 0.2f);
        window->DrawList->AddRectFilled(bb.Min, bb.Max, bg, 6.f);
    }

    ImColor icon_col = active ? c::anim::active : (hovered ? c::text::label::hovered : c::text::label::default_color);
    ImGui::PushFont(font::default_r);
    ImVec2 icon_pos(bb.Min.x + 10.f, bb.Min.y + (item_h - ImGui::CalcTextSize(icon).y) * 0.5f);
    window->DrawList->AddText(icon_pos, icon_col, icon);

    ImColor text_col = active ? c::anim::active : (hovered ? c::text::label::hovered : c::text::label::default_color);
    ImVec2 text_pos(bb.Min.x + 34.f, bb.Min.y + (item_h - ImGui::CalcTextSize(label).y) * 0.5f);
    window->DrawList->AddText(text_pos, text_col, label);
    ImGui::PopFont();

    return pressed;
}

static bool PageTab(const char* label, bool active) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    ImGui::PushFont(font::default_m);
    ImVec2 text_sz = ImGui::CalcTextSize(label);
    ImGui::PopFont();

    const float pad_x = 18.f;
    const float pad_y = 6.f;
    ImVec2 pos = window->DC.CursorPos;
    ImVec2 sz(text_sz.x + pad_x * 2, text_sz.y + pad_y * 2);
    ImRect bb(pos, pos + sz);

    ImGuiID id = window->GetID(label);
    ImGui::ItemSize(bb, 0);
    if (!ImGui::ItemAdd(bb, id)) return false;

    bool hovered, held;
    bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held);

    static std::map<ImGuiID, float> tab_alpha;
    float& alpha = tab_alpha[id];
    float target_a = active ? 1.0f : (hovered ? 0.5f : 0.0f);
    alpha = ImLerp(alpha, target_a, g.IO.DeltaTime * 12.f);

    if (alpha > 0.01f) {
        ImColor bg = utils::GetColorWithAlpha(c::anim::active, alpha * 0.85f);
        window->DrawList->AddRectFilled(bb.Min, bb.Max, bg, sz.y * 0.5f);
    }

    ImGui::PushFont(font::default_m);
    ImColor text_col = active ? ImColor(255, 255, 255, 255) :
                       (hovered ? c::text::label::hovered : c::text::label::default_color);
    ImVec2 text_pos = bb.Min + ImVec2(pad_x, pad_y);
    window->DrawList->AddText(text_pos, text_col, label);
    ImGui::PopFont();

    return pressed;
}

static void SectionHeader(const char* label) {
    ImGui::PushFont(font::inter_bold);
    ImGui::Text("%s", label);
    ImGui::PopFont();
    ImGui::Spacing();
}

// ============================================================================
//  User info card
// ============================================================================
static void DrawUserInfoCard(ImDrawList* dl, ImVec2 win_pos, float sidebar_w, float win_h)
{
    const float card_h = 80.f;
    const float pad = 10.f;
    ImVec2 mn(win_pos.x + 8, win_pos.y + win_h - card_h - pad);
    ImVec2 mx(win_pos.x + sidebar_w - 8, win_pos.y + win_h - pad);

    dl->AddRectFilled(mn, mx, ImGui::ColorConvertFloat4ToU32(c::child::background), 8.f);

    // Avatar circle
    ImVec2 ac(mn.x + 22, mn.y + 22);
    dl->AddCircleFilled(ac, 15, ImColor(c::anim::active), 32);
    ImGui::PushFont(font::small_font);
    const char* initials = "KM";
    ImVec2 its = ImGui::CalcTextSize(initials);
    dl->AddText(ac - its * 0.5f, ImColor(255, 255, 255, 255), initials);
    ImGui::PopFont();

    dl->AddCircleFilled(ImVec2(ac.x + 11, ac.y + 11), 3.5f,
        Globals::DmaInitialized.load() ? ImColor(80, 200, 80, 255) : ImColor(200, 80, 80, 255));

    ImGui::PushFont(font::default_m);
    dl->PushClipRect(ImVec2(mn.x + 44, mn.y), ImVec2(mx.x - 8 - 38 - 6, mn.y + card_h), true); dl->AddText(ImVec2(mn.x + 44, mn.y + 8), ImColor(c::text::label::active), "Kyx - Marvel Rivals"); dl->PopClipRect();
    ImGui::PopFont();
    ImGui::PushFont(font::small_font);
    dl->AddText(ImVec2(mn.x + 44, mn.y + 28), ImColor(c::text::description::default_color), "#DMA");
    ImGui::PopFont();

    // Badge
    ImVec2 bp(mx.x - 8 - 38, mn.y + 10);
    ImVec2 bs(38, 17);
    dl->AddRectFilled(bp, bp + bs, ImColor(c::anim::active), 4.f);
    ImGui::PushFont(font::small_font);
    const char* badge = Globals::DmaInitialized.load() ? "LIVE" : "OFF";
    ImVec2 bt = ImGui::CalcTextSize(badge);
    dl->AddText(bp + (bs - bt) * 0.5f, ImColor(255, 255, 255, 255), badge);
    ImGui::PopFont();

    // Status row
    float by = mn.y + 52;
    float tw = (mx.x - mn.x) / 3.f;
    ImGui::PushFont(font::small_font);
    struct SI { const char* v; const char* l; };
    static char fpsStr[16] = "--";
    {
        float fps = ImGui::GetIO().Framerate;
        if (fps > 0.f) snprintf(fpsStr, sizeof(fpsStr), "%.0f", fps);
    }
    SI si[] = { {"DMA","TYPE"}, {fpsStr,"FPS"}, {Globals::SdkInitialized.load() ? "OK" : "ERR","SDK"} };
    for (int i = 0; i < 3; i++) {
        float x = mn.x + tw * i;
        ImVec2 vs = ImGui::CalcTextSize(si[i].v);
        ImVec2 ls = ImGui::CalcTextSize(si[i].l);
        dl->AddText(ImVec2(x + (tw - vs.x) * 0.5f, by), ImColor(c::anim::active), si[i].v);
        dl->AddText(ImVec2(x + (tw - ls.x) * 0.5f, by + 12), ImColor(c::text::description::default_color), si[i].l);
    }
    ImGui::PopFont();
}

// ============================================================================
//  RENDER  –  Sidebar + Sub-Tab Navigation + KyxMarvel Content
// ============================================================================
void PortableMenu_Render()
{
    static bool gui_init = false;
    static float menu_alpha = 1.f;
    static bool menu_active = true;
    static CNotifications p_notif;

    static int sidebar_sel = 0;
    static int sub_tab     = 0;

    // Tab labels per sidebar page
    static const char* tabs_aimbot[]  = { "General", "Targeting", "Advanced" };
    static const char* tabs_visuals[] = { "ESP", "Skeleton", "Extras" };
    static const char* tabs_radar[]   = { "Radar", "Colors" };
    static const char* tabs_misc[]    = { "Settings", "Thickness", "Config", "Cloud" };
    static const char* tabs_theme[]   = { "Theme", "Presets", "Info" };

    struct TabSet { const char** labels; int count; };
    static TabSet tab_sets[] = {
        { tabs_aimbot,  3 },
        { tabs_visuals, 3 },
        { tabs_radar,   2 },
        { tabs_misc,    4 },
        { tabs_theme,   3 },
    };

    if (!gui_init) {
        c::bg::menu_bb = ImRect(ImVec2(100, 100),
                                ImVec2(100 + c::bg::size.x, 100 + c::bg::size.y));
        gui_init = true;
    }

    auto& settings = config.GetSettings();
    ImGuiStyle& s = ImGui::GetStyle();

    const float header_h  = 55.f;
    const float sidebar_w = 200.f;
    const float content_w = c::bg::size.x - sidebar_w;
    const float content_h = c::bg::size.y - header_h;
    const float col_w     = (content_w - 30.f) * 0.5f;

    ImGuiWindowFlags wf = ImGuiWindowFlags_NoTitleBar
        | ImGuiWindowFlags_NoBringToFrontOnFocus
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowSize(c::bg::size);
    ImGui::Begin("KYX - MARVEL RIVALS MENU", nullptr, wf);
    {
        c::anim::speed = ImGui::GetIO().DeltaTime * 12.f;
        c::bg::menu_bb = ImGui::GetCurrentWindow()->Rect();

        const ImVec2& pos = ImGui::GetWindowPos();
        menu_alpha = ImLerp(menu_alpha, menu_active ? 1.f : 0.f, c::anim::speed);
        s.Alpha = menu_alpha;

        draw_background_blur(ImGui::GetWindowDrawList(), g_pSwapChain,
            g_pd3dDevice, g_pd3dDeviceContext,
            pos, pos + c::bg::size,
            c::bg::rounding, ImDrawFlags_RoundCornersAll);

        handle_input();

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, pos + c::bg::size,
            utils::GetColorWithAlpha(c::window_bg_color,
                c::window_bg_color.Value.w * s.Alpha),
            c::bg::rounding, ImDrawFlags_RoundCornersAll);

        UpdateTheme(bTheme, ImGui::GetIO().DeltaTime * 12.f);

        // ============================================================
        //  HEADER BAR
        // ============================================================
        {
            ImGui::GetWindowDrawList()->AddRectFilled(
                pos, pos + ImVec2(c::bg::size.x, header_h),
                utils::GetColorWithAlpha(c::window_bg_color,
                    c::window_bg_color.Value.w * 0.6f * s.Alpha),
                c::bg::rounding, ImDrawFlags_RoundCornersTop);

            ImGui::GetWindowDrawList()->AddLine(
                pos + ImVec2(0, header_h),
                pos + ImVec2(c::bg::size.x, header_h),
                ImGui::GetColorU32(c::separator), 1.f);

            ImGui::PushFont(font::inter_bold);
            const char* title = "KYX - MARVEL RIVALS"; float title_w = ImGui::CalcTextSize(title).x;
            ImVec2 tp(pos.x + 20,
                      pos.y + (header_h - ImGui::CalcTextSize(title).y) * 0.5f);
            ImGui::GetWindowDrawList()->AddText(tp,
                ImColor(c::text::label::active), title);
            ImGui::PopFont();

            // Sub-tabs
            TabSet& ts = tab_sets[sidebar_sel];
            ImGui::SetCursorPos(ImVec2(20.f + title_w + 20.f, (header_h - 30.f) * 0.5f));
            for (int i = 0; i < ts.count; i++) {
                if (PageTab(ts.labels[i], sub_tab == i))
                    sub_tab = i;
                ImGui::SameLine(0, 6);
            }

            // Utility buttons
            ImGui::SetCursorPos(ImVec2(c::bg::size.x - 170,
                (header_h - 35) * 0.5f));
            custom::ToggleButton(ICON_SUN_2_FILL, ICON_MOONLIGHT_FILL,
                &bTheme, ImVec2(35, 35));
            ImGui::SameLine(0, 4);
            if (custom::InActiveButton(ICON_SETTINGS_3_FILL, ImVec2(35, 35)))
                settings_popup.open();
            ImGui::SameLine(0, 4);
            custom::InActiveButton(ICON_LAYOUT_LEFT_FILL, ImVec2(35, 35));

            // Status dots
            ImVec2 dp(pos.x + c::bg::size.x - 42, pos.y + header_h * 0.5f);
            ImGui::GetWindowDrawList()->AddCircleFilled(
                dp, 5, Globals::SdkInitialized.load() ? ImColor(80, 200, 80, 255) : ImColor(245, 195, 65, 255));
            ImGui::GetWindowDrawList()->AddCircleFilled(
                dp + ImVec2(16, 0), 5, ImColor(c::anim::active));
        }

        // Vertical separator
        ImGui::GetWindowDrawList()->AddLine(
            pos + ImVec2(sidebar_w, header_h),
            pos + ImVec2(sidebar_w, c::bg::size.y),
            ImGui::GetColorU32(c::separator), 1.f);

        // ============================================================
        //  SIDEBAR
        // ============================================================
        ImGui::SetCursorPos(ImVec2(0, header_h + 8));
        ImGui::BeginChild("##SIDEBAR", ImVec2(sidebar_w, content_h - 8), false,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        {
            ImGuiWindow* sw = ImGui::GetCurrentWindow();
            sw->DrawList->AddRectFilled(
                sw->Pos, sw->Pos + ImVec2(sidebar_w, content_h),
                utils::GetColorWithAlpha(c::window_bg_color,
                    c::window_bg_color.Value.w * 0.3f),
                0, ImDrawFlags_RoundCornersBottomLeft);

            SidebarSectionHeader("COMBAT");
            if (SidebarItem(ICON_AIMING_FILL, "Aimbot", sidebar_sel == 0))
                { sidebar_sel = 0; sub_tab = 0; }

            ImGui::Spacing();

            SidebarSectionHeader("VISUALS");
            if (SidebarItem(ICON_EYE_FILL, "ESP", sidebar_sel == 1))
                { sidebar_sel = 1; sub_tab = 0; }
            if (SidebarItem(ICON_COMPASS_FILL, "Radar", sidebar_sel == 2))
                { sidebar_sel = 2; sub_tab = 0; }

            ImGui::Spacing();

            SidebarSectionHeader("SETTINGS");
            if (SidebarItem(ICON_SETTINGS_3_FILL, "Misc", sidebar_sel == 3))
                { sidebar_sel = 3; sub_tab = 0; }
            if (SidebarItem(ICON_LAYOUT_LEFT_FILL, "Theme", sidebar_sel == 4))
                { sidebar_sel = 4; sub_tab = 0; }
        }
        ImGui::EndChild();

        DrawUserInfoCard(ImGui::GetWindowDrawList(), pos, sidebar_w, c::bg::size.y);

        // ============================================================
        //  CONTENT
        // ============================================================
        ImGui::SetCursorPos(ImVec2(sidebar_w + 10, header_h + 10));
        ImGui::BeginChild("##CONTENT", ImVec2(content_w - 20, content_h - 20),
            false, ImGuiWindowFlags_NoScrollbar);
        {
            auto PushCol = []() {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, c::child::background);
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 12));
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(13, 8));
            };
            auto PopCol = []() {
                ImGui::PopStyleVar(3);
                ImGui::PopStyleColor();
            };

            // ===========================================================
            //  PAGE 0: AIMBOT
            // ===========================================================
            if (sidebar_sel == 0)
            {
                auto& aim = settings.Aimbot;

                if (sub_tab == 0)
                {
                    // General
                    PushCol();
                    ImGui::BeginChild("##AIM_GEN_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("AIMBOT");
                        custom::Checkbox("Enabled", &aim.Enabled);
                        custom::Checkbox("Visible Check", &aim.VisibleCheck);
                        custom::Checkbox("Prediction", &aim.Prediction);

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("FOV & SMOOTHING");
                        custom::SliderFloat("FOV", &aim.FOV, 1.0f, 200.0f, "%.0f px");
                        custom::SliderFloat("Smooth", &aim.SmoothValue, 1.0f, 50.0f, "%.1f");
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##AIM_GEN_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("HITBOX");
                        static const char* hitboxNames[] = { "Closest", "Head", "Neck", "Upper Body", "Middle" };
                        custom::Combo("Hitbox", reinterpret_cast<int*>(&aim.HitboxTarget), hitboxNames, IM_ARRAYSIZE(hitboxNames));

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("HOTKEYS");
                        custom::SliderInt("First Hotkey (VK)", &aim.FirstHotkey, 0x01, 0x06, "0x%02X");
                        custom::SliderInt("Second Hotkey (VK)", &aim.SecondHotkey, 0x01, 0x06, "0x%02X");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 1)
                {
                    // Targeting
                    PushCol();
                    ImGui::BeginChild("##AIM_TGT_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("FOV MODE");
                        static const char* fovModes[] = { "Regular", "Dynamic" };
                        custom::Combo("FOV Mode", reinterpret_cast<int*>(&aim.FovMode), fovModes, IM_ARRAYSIZE(fovModes));

                        if (static_cast<int>(aim.FovMode) == 1) {
                            custom::SliderFloat("Dynamic Min", &aim.DynamicFovMin, 1.0f, 50.0f, "%.0f px");
                            custom::SliderFloat("Dynamic Max", &aim.DynamicFovMax, 10.0f, 200.0f, "%.0f px");
                        }

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("SMOOTH TYPE");
                        static const char* smoothTypes[] = { "Linear", "Dynamic", "Koestep" };
                        custom::Combo("Smooth Type", reinterpret_cast<int*>(&aim.Smooth), smoothTypes, IM_ARRAYSIZE(smoothTypes));

                        if (static_cast<int>(aim.Smooth) == 2) {
                            custom::SliderFloat("Accel", &aim.KoestepAccel, 0.1f, 5.0f, "%.2f");
                            custom::SliderFloat("Decel", &aim.KoestepDecel, 0.1f, 5.0f, "%.2f");
                        }
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##AIM_TGT_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("PRIORITY");
                        static const char* priorities[] = { "FOV", "FOV + Distance" };
                        custom::Combo("Priority", reinterpret_cast<int*>(&aim.Priority), priorities, IM_ARRAYSIZE(priorities));

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("DISTANCES");
                        custom::SliderFloat("1st Hotkey Dist", &aim.FirstHotkeyDistance, 10.0f, 500.0f, "%.0f m");
                        custom::SliderFloat("2nd Hotkey Dist", &aim.SecondHotkeyDistance, 10.0f, 500.0f, "%.0f m");
                        custom::SliderFloat("Target Switch", &aim.TargetSwitchDelay, 0.0f, 2000.0f, "%.0f ms");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 2)
                {
                    // Advanced
                    PushCol();
                    ImGui::BeginChild("##AIM_ADV_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("DEADZONE");
                        custom::SliderFloat("Deadzone FOV", &aim.DeadzoneFOV, 0.0f, 10.0f, "%.1f px");
                        custom::SliderFloat("Deadzone Smooth", &aim.DeadzoneSmooth, 0.1f, 5.0f, "%.2f");
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##AIM_ADV_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("INPUT DEVICE");
                        auto& inp = settings.Input;
                        static const char* deviceTypes[] = { "None", "KMBox Net", "Memory Aim" };
                        custom::Combo("Device##input", &inp.DeviceType, deviceTypes, IM_ARRAYSIZE(deviceTypes));

                        if (inp.DeviceType == 1) { // KMBox Net
                            ImGui::InputText("KMBox IP", inp.KMBoxIP, sizeof(inp.KMBoxIP));
                            custom::SliderInt("KMBox Port", &inp.KMBoxPort, 1024, 65535);
                            custom::SliderFloat("Sensitivity", &inp.Sensitivity, 0.1f, 5.0f, "%.2f");
                        }
                        else if (inp.DeviceType == 2) { // Memory Aim
                            custom::SliderFloat("Mem Aim Smooth", &inp.MemAimSmooth, 1.0f, 30.0f, "%.1f");
                        }

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("PANIC");
                        custom::Checkbox("Panic Turn Off", &aim.AllowPanicTurnOff);
                        if (aim.AllowPanicTurnOff) {
                            custom::SliderInt("Panic Hotkey (VK)", &aim.PanicHotkey, 0x01, 0xFF);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();
                }
            }

            // ===========================================================
            //  PAGE 1: VISUALS
            // ===========================================================
            else if (sidebar_sel == 1)
            {
                auto& vis = settings.Visuals;

                if (sub_tab == 0)
                {
                    // ESP
                    PushCol();
                    ImGui::BeginChild("##VIS_ESP_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("ESP");
                        custom::Checkbox("Enabled##vis", &vis.Enabled);
                        custom::Checkbox("Show Team", &vis.ShowTeam);
                        custom::Checkbox("Only Visible", &vis.OnlyVisible);
                        custom::Checkbox("Ignore Knocked", &vis.IgnoreKnocked);
                        custom::SliderFloat("Max Distance", &vis.MaxDistance, 10.0f, 500.0f, "%.0f m");

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("BOX ESP");
                        custom::Checkbox("Box", &vis.Box);
                        if (vis.Box) {
                            static const char* boxTypes[] = { "2D Box", "2D Corner", "2D Round Corner", "3D Box" };
                            custom::Combo("Box Style", reinterpret_cast<int*>(&vis.BoxStyle), boxTypes, IM_ARRAYSIZE(boxTypes));
                            custom::ColorEdit4("Box Visible", &vis.BoxVisibleColor.R, picker_flags);
                            custom::ColorEdit4("Box Invisible", &vis.BoxInvisibleColor.R, picker_flags);
                            static const char* fillTypes[] = { "Regular", "HP", "Gradient" };
                            custom::Combo("Box Fill", reinterpret_cast<int*>(&vis.BoxFill), fillTypes, IM_ARRAYSIZE(fillTypes));
                            custom::ColorEdit4("Fill Color", &vis.BoxFillColor.R, picker_flags);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##VIS_ESP_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("HERO INFO");
                        custom::Checkbox("Hero Name", &vis.HeroESP);
                        if (vis.HeroESP) {
                            custom::ColorEdit4("Name Visible", &vis.NameVisibleColor.R, picker_flags);
                            custom::ColorEdit4("Name Invisible", &vis.NameInvisibleColor.R, picker_flags);
                        }
                        custom::Checkbox("Player Name", &vis.ShowPlayerName);
                        if (vis.ShowPlayerName) {
                            custom::ColorEdit4("PName Visible", &vis.PlayerNameVisColor.R, picker_flags);
                            custom::ColorEdit4("PName Invisible", &vis.PlayerNameInvisColor.R, picker_flags);
                        }
                        custom::Checkbox("Show Distance##vis", &vis.ShowDistance);
                        custom::Checkbox("Show Rank", &vis.ShowRank);

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("HEALTH BAR");
                        static const char* hbarPos[] = { "Left", "Right", "Bottom" };
                        custom::Combo("Bar Position", reinterpret_cast<int*>(&vis.HBarPosition), hbarPos, IM_ARRAYSIZE(hbarPos));
                        custom::SliderFloat("Bar Width", &vis.HealthBarWidth, 1.0f, 8.0f, "%.1f");

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("TEXT & FONT");
                        custom::Checkbox("Text Shadow", &vis.TextShadow);
                        custom::SliderFloat("Name Size", &vis.NameFontSize, 8.0f, 24.0f, "%.0f");
                        custom::SliderFloat("Player Name Size", &vis.PlayerNameFontSize, 8.0f, 24.0f, "%.0f");
                        custom::SliderFloat("Distance Size", &vis.DistFontSize, 8.0f, 20.0f, "%.0f");
                        custom::SliderFloat("Global Font##vis", &vis.FontSize, 8.0f, 24.0f, "%.0f");

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("CROSSHAIR");
                        custom::Checkbox("Crosshair", &vis.Crosshair);
                        if (vis.Crosshair) {
                            custom::SliderInt("Type", &vis.CrosshairType, 0, 3);
                            custom::ColorEdit4("Color##xhair", &vis.CrosshairColor.R, picker_flags);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 1)
                {
                    // Skeleton
                    PushCol();
                    ImGui::BeginChild("##VIS_SKEL_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("SKELETON");
                        custom::Checkbox("Skeleton##skel", &vis.Skeleton);
                        if (vis.Skeleton) {
                            custom::ColorEdit4("Visible", &vis.SkeletonVisColor.R, picker_flags);
                            custom::ColorEdit4("Invisible", &vis.SkeletonInvisColor.R, picker_flags);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##VIS_SKEL_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("SNAPLINES");
                        custom::Checkbox("Snaplines", &vis.Snaplines);
                        if (vis.Snaplines)
                            custom::SliderFloat("Distance##snap", &vis.SnaplinesDistance, 10.0f, 500.0f, "%.0f m");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 2)
                {
                    // Extras
                    PushCol();
                    ImGui::BeginChild("##VIS_EXT_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("EXTRAS");
                        custom::Checkbox("Head Circle", &vis.HeadCircle);
                        custom::Checkbox("Aimbot FOV Circle", &vis.AimbotFovDraw);
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##VIS_EXT_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("COLORS");
                        if (vis.HeadCircle)
                            custom::ColorEdit4("Head Circle", &vis.HeadCircleColor.R, picker_flags);
                        if (vis.AimbotFovDraw)
                            custom::ColorEdit4("FOV Circle", &vis.AimbotFovColor.R, picker_flags);
                    }
                    ImGui::EndChild();
                    PopCol();
                }
            }

            // ===========================================================
            //  PAGE 2: RADAR
            // ===========================================================
            else if (sidebar_sel == 2)
            {
                auto& rad = settings.Radar;

                if (sub_tab == 0)
                {
                    PushCol();
                    ImGui::BeginChild("##RAD_GEN_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("RADAR");
                        custom::Checkbox("Enabled##rad", &rad.Enabled);
                        custom::Checkbox("Healthbar##rad", &rad.Healthbar);
                        custom::Checkbox("Show Distance##rad", &rad.ShowDistance);
                        custom::Checkbox("Direction Lines", &rad.DirectionLines);
                        custom::Checkbox("Custom Position", &rad.CustomPosEnabled);
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##RAD_GEN_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("LAYOUT");
                        custom::SliderFloat("Position X", &rad.PosX, 0.0f, 3840.0f, "%.0f");
                        custom::SliderFloat("Position Y", &rad.PosY, 0.0f, 2160.0f, "%.0f");
                        custom::SliderFloat("Size", &rad.Size, 50.0f, 500.0f, "%.0f");
                        custom::SliderFloat("Scale", &rad.Scale, 0.1f, 5.0f, "%.2f");
                        custom::SliderFloat("Rounding##rad", &rad.Rounding, 0.0f, 20.0f, "%.0f");

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("SIZING");
                        custom::SliderFloat("Local Size", &rad.LocalSize, 3.0f, 20.0f, "%.0f");
                        custom::SliderFloat("Enemy Size", &rad.EnemySize, 3.0f, 20.0f, "%.0f");
                        custom::SliderFloat("Font Size##rad", &rad.FontSize, 6.0f, 18.0f, "%.0f");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 1)
                {
                    PushCol();
                    ImGui::BeginChild("##RAD_COL_F", ImVec2(-1, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("RADAR COLORS");
                        custom::ColorEdit4("Enemy Visible", &rad.EnemyVisColor.R, picker_flags);
                        custom::ColorEdit4("Enemy Invisible", &rad.EnemyInvisColor.R, picker_flags);
                        custom::ColorEdit4("Local Player", &rad.LocalColor.R, picker_flags);
                    }
                    ImGui::EndChild();
                    PopCol();
                }
            }

            // ===========================================================
            //  PAGE 3: MISC
            // ===========================================================
            else if (sidebar_sel == 3)
            {
                auto& misc = settings.Misc;

                if (sub_tab == 0)
                {
                    PushCol();
                    ImGui::BeginChild("##MISC_SET_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("PERFORMANCE");
                        custom::Checkbox("DMA Acceleration", &misc.DMAAcceleration);
                        custom::Checkbox("VSync", &misc.OverlayVSync);
                        custom::Checkbox("Limit CPU", &misc.LimitCPU);
                        if (misc.LimitCPU)
                            custom::SliderFloat("Target Frame Time", &misc.CPULimitMs, 1.0f, 33.0f, "%.1f ms");
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##MISC_SET_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("DISPLAY");
                        custom::Checkbox("Widescreen Support", &misc.WidescreenSupport);
                        custom::Checkbox("Info Widget", &misc.InfoWidget);
                        custom::Checkbox("Firmware Checker", &misc.FirmwareChecker);

                        static const char* overlayModes[] = { "FUSER", "Transparent" };
                        custom::Combo("Overlay Mode", reinterpret_cast<int*>(&misc.Overlay), overlayModes, IM_ARRAYSIZE(overlayModes));

                        static const char* mouseModes[] = { "Default", "Inverted" };
                        custom::Combo("Mouse Mode", reinterpret_cast<int*>(&misc.Mouse), mouseModes, IM_ARRAYSIZE(mouseModes));

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("RAYCAST (EMBREE)");
                        custom::Checkbox("Enabled##Raycast", &settings.Raycast.Enabled);
                        custom::Checkbox("Debug Wireframe", &settings.Raycast.DebugWireframe);
                        custom::SliderFloat("Max Distance (m)", &settings.Raycast.MaxDistance, 50.0f, 2000.0f, "%.0f m");
                        custom::SliderInt("Rebuild Interval (ms)", &settings.Raycast.RebuildIntervalMs, 50, 1000, "%d ms");

                        // Status
                        {
                            auto& rv = KyxMarvel::RaycastVisCheck::Get();
                            int geo = rv.GetGeometryCount();
                            int tri = rv.GetTriangleCount();
                            char buf[80];
                            snprintf(buf, sizeof(buf), "Geo: %d | Tri: %d | %s", geo, tri,
                                     rv.IsInitialized() ? "Active" : "Idle");
                            ImGui::TextColored(rv.IsInitialized() ? ImVec4(0.3f,0.8f,0.3f,1) : ImVec4(0.5f,0.5f,0.5f,1), "%s", buf);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 1)
                {
                    PushCol();
                    ImGui::BeginChild("##MISC_THK_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("THICKNESS");
                        custom::SliderFloat("Box", &misc.BoxThickness, 0.5f, 5.0f, "%.1f");
                        custom::SliderFloat("Snapline", &misc.SnaplineThickness, 0.5f, 5.0f, "%.1f");
                        custom::SliderFloat("Skeleton Vis", &misc.SkeletonVisThick, 0.5f, 5.0f, "%.1f");
                        custom::SliderFloat("Skeleton Invis", &misc.SkeletonInvisThick, 0.5f, 5.0f, "%.1f");
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##MISC_THK_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("CROSSHAIR / FOV");
                        custom::SliderFloat("Crosshair Size", &misc.CrosshairSize, 2.0f, 20.0f, "%.0f");
                        custom::SliderFloat("Crosshair Thick", &misc.CrosshairThickness, 0.5f, 5.0f, "%.1f");
                        custom::SliderFloat("Aim FOV Thick", &misc.AimFovThickness, 0.5f, 5.0f, "%.1f");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 2)
                {
                    // Config
                    PushCol();
                    ImGui::BeginChild("##MISC_CFG_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("CONFIG");
                        static int selectedProfile = 0;
                        custom::SliderInt("Profile", &selectedProfile, 0, 7);

                        ImGui::Spacing();
                        if (custom::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            config.SaveConfig(selectedProfile);
                        }
                        if (custom::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            config.LoadConfig(selectedProfile);
                        }
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##MISC_CFG_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("QUICK BINDS");
                        for (int i = 0; i < 4; i++) {
                            ImGui::PushID(i);
                            char lbl[32];
                            snprintf(lbl, sizeof(lbl), "Bind %d Hotkey", i + 1);
                            custom::SliderInt(lbl, &misc.ConfigBinds[i].Hotkey, 0, 0xFF);
                            snprintf(lbl, sizeof(lbl), "Bind %d Profile", i + 1);
                            custom::SliderInt(lbl, &misc.ConfigBinds[i].Profile, 0, 7);
                            ImGui::PopID();
                        }
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 3)
                {
                    // Cloud Config
                    static char cloudStatus[128] = "";
                    static char shareCode[16] = "";
                    static char importCode[16] = "";
                    static char shareName[65] = "My Config";
                    static char importInfo[256] = "";

                    PushCol();
                    ImGui::BeginChild("##CLOUD_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("CLOUD CONFIG");
                        ImGui::TextWrapped("User: %s", Auth::Username.empty() ? "(not authenticated)" : Auth::Username.c_str());
                        ImGui::Spacing();

                        if (custom::Button("Save to Cloud", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            if (!Auth::Username.empty()) {
                                auto resp = CloudConfig::SaveConfig(Auth::Username, settings);
                                if (resp.Ok())
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Saved to cloud OK");
                                else if (resp.status == 0)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Connection failed (err %lu)", resp.winError);
                                else if (resp.status == 401)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Auth failed (invalid API key)");
                                else
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Save failed (HTTP %d)", resp.status);
                            } else {
                                snprintf(cloudStatus, sizeof(cloudStatus), "Not authenticated");
                            }
                        }
                        if (custom::Button("Load from Cloud", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            if (!Auth::Username.empty()) {
                                auto resp = CloudConfig::LoadConfig(Auth::Username, settings);
                                if (resp.Ok())
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Loaded from cloud OK");
                                else if (resp.status == 404)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "No cloud config found");
                                else if (resp.status == 0)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Connection failed (err %lu)", resp.winError);
                                else if (resp.status == 401)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Auth failed (invalid API key)");
                                else
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Load failed (HTTP %d)", resp.status);
                            } else {
                                snprintf(cloudStatus, sizeof(cloudStatus), "Not authenticated");
                            }
                        }
                        if (custom::Button("Delete from Cloud", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            if (!Auth::Username.empty()) {
                                auto resp = CloudConfig::DeleteConfig(Auth::Username);
                                if (resp.Ok())
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Cloud config deleted");
                                else if (resp.status == 0)
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Connection failed (err %lu)", resp.winError);
                                else
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Delete failed (HTTP %d)", resp.status);
                            }
                        }

                        ImGui::Spacing();
                        if (cloudStatus[0])
                            ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.4f, 1.0f), "%s", cloudStatus);
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##CLOUD_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("SHARE CONFIG");
                        ImGui::InputText("Display Name", shareName, sizeof(shareName));
                        if (custom::Button("Share##cloud", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
                            if (!Auth::Username.empty()) {
                                // Save first, then share
                                CloudConfig::SaveConfig(Auth::Username, settings);
                                auto code = CloudConfig::ShareConfig(Auth::Username, shareName);
                                if (code.has_value()) {
                                    snprintf(shareCode, sizeof(shareCode), "%s", code->c_str());
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Shared! Code: %s", code->c_str());
                                    // Copy to clipboard
                                    if (OpenClipboard(nullptr)) {
                                        EmptyClipboard();
                                        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, code->size() + 1);
                                        if (hg) {
                                            memcpy(GlobalLock(hg), code->c_str(), code->size() + 1);
                                            GlobalUnlock(hg);
                                            SetClipboardData(CF_TEXT, hg);
                                        }
                                        CloseClipboard();
                                    }
                                } else {
                                    snprintf(cloudStatus, sizeof(cloudStatus), "Share failed");
                                }
                            }
                        }
                        if (shareCode[0]) {
                            ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.6f, 1.0f), "Code: %s", shareCode);
                            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(copied to clipboard)");
                        }

                        ImGui::Spacing(); ImGui::Spacing();
                        SectionHeader("IMPORT CONFIG");
                        ImGui::InputText("Share Code", importCode, sizeof(importCode));
                        if (custom::Button("Import##cloud", ImVec2(ImGui::GetContentRegionAvail().x, 35))) {
                            auto imported = CloudConfig::ImportSharedConfig(importCode);
                            if (imported.has_value()) {
                                CloudConfig::INIToSettings(imported->content, settings);
                                snprintf(importInfo, sizeof(importInfo), "Imported '%s' by %s",
                                    imported->name.c_str(), imported->author.c_str());
                                snprintf(cloudStatus, sizeof(cloudStatus), "Import OK");
                            } else {
                                snprintf(importInfo, sizeof(importInfo), "");
                                snprintf(cloudStatus, sizeof(cloudStatus), "Code not found");
                            }
                        }
                        if (importInfo[0])
                            ImGui::TextColored(ImVec4(0.4f, 0.8f, 0.9f, 1.0f), "%s", importInfo);
                    }
                    ImGui::EndChild();
                    PopCol();
                }
            }

            // ===========================================================
            //  PAGE 4: THEME / INFO
            // ===========================================================
            else if (sidebar_sel == 4)
            {
                if (sub_tab == 0)
                {
                    PushCol();
                    ImGui::BeginChild("##THM_L", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("THEME");
                        custom::ToggleButton(ICON_MOONLIGHT_FILL, ICON_SUN_2_FILL,
                            &bTheme, ImVec2(ImGui::GetContentRegionAvail().x, 40));
                        custom::ColorEdit4("Accent",
                            (float*)&c::anim::active, picker_flags);
                        custom::ColorEdit4("Secondary",
                            (float*)&c::dark_color, picker_flags);

                        static const char* fw[] = { "Regular", "Medium", "Semibold" };
                        static ImFont* df[] = {
                            font::default_r, font::default_m, font::default_s };
                        custom::Combo("Font Weight", &c::selected_font,
                            fw, IM_ARRAYSIZE(fw));
                        ImGui::GetIO().FontDefault = df[c::selected_font];
                    }
                    ImGui::EndChild();
                    PopCol();

                    ImGui::SameLine();

                    PushCol();
                    ImGui::BeginChild("##THM_R", ImVec2(col_w, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("MENU COLORS");
                        custom::ColorEdit4("Menu Main", &settings.Misc.MenuMainColor.R, picker_flags);
                        custom::ColorEdit4("Menu Text", &settings.Misc.MenuTextColor.R, picker_flags);
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 1)
                {
                    PushCol();
                    ImGui::BeginChild("##PRE_F", ImVec2(-1, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        SectionHeader("PRESETS");
                        ImGui::TextWrapped("Config presets: use the Config sub-tab under Misc "
                            "to save/load profiles 0-7.");
                    }
                    ImGui::EndChild();
                    PopCol();
                }
                else if (sub_tab == 2)
                {
                    PushCol();
                    ImGui::BeginChild("##INFO_F", ImVec2(-1, -1), false,
                        ImGuiWindowFlags_AlwaysUseWindowPadding);
                    {
                        ImGui::PushFont(font::regular_m);
                        ImGui::TextColored(ImVec4(1, 1, 1, 0.9f), "Welcome to");
                        ImGui::SameLine();
                        ImGui::TextColored(
                            ImVec4(c::anim::active.Value.x,
                                   c::anim::active.Value.y,
                                   c::anim::active.Value.z, 1.f),
                            "Kyx - Marvel Rivals!");
                        ImGui::PopFont();

                        ImGui::PushFont(font::description_font);
                        ImGui::TextColored(
                            ImVec4(c::text::description::default_color.Value.x,
                                   c::text::description::default_color.Value.y,
                                   c::text::description::default_color.Value.z, 0.8f),
                            "Marvel Rivals DMA Overlay");
                        ImGui::PopFont();

                        ImGui::Spacing(); ImGui::Spacing();

                        // Status table
                        ImGui::PushStyleColor(ImGuiCol_ChildBg,
                            ImGui::ColorConvertFloat4ToU32(
                                ImVec4(c::child::top_bg.x, c::child::top_bg.y,
                                       c::child::top_bg.z, 0.4f)));
                        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.f);
                        ImGui::BeginChild("##info_tbl", ImVec2(-1, 70), false);
                        {
                            float third = ImGui::GetContentRegionAvail().x / 3.f;
                            struct Col { const char* h; const char* v; ImVec4 vc; };
                            Col cols[] = {
                                { "DMA", Globals::DmaInitialized.load() ? "OK" : "ERR",
                                  { c::anim::active.Value.x, c::anim::active.Value.y,
                                    c::anim::active.Value.z, 1.f } },
                                { "SDK", Globals::SdkInitialized.load() ? "OK" : "ERR",
                                  { c::anim::active.Value.x, c::anim::active.Value.y,
                                    c::anim::active.Value.z, 1.f } },
                                { "BUILD", "OK",
                                  { 0.3f, 0.85f, 0.3f, 1.f } }
                            };
                            for (int i = 0; i < 3; i++) {
                                ImGui::SetCursorPos(ImVec2(third * i + 15, 12));
                                ImGui::PushFont(font::small_font);
                                ImGui::TextColored(
                                    ImVec4(c::text::description::default_color.Value.x,
                                           c::text::description::default_color.Value.y,
                                           c::text::description::default_color.Value.z, 0.7f),
                                    "%s", cols[i].h);
                                ImGui::PopFont();
                                ImGui::SetCursorPos(ImVec2(third * i + 15, 34));
                                ImGui::PushFont(font::inter_bold);
                                ImGui::TextColored(cols[i].vc, "%s", cols[i].v);
                                ImGui::PopFont();
                            }
                        }
                        ImGui::EndChild();
                        ImGui::PopStyleVar();
                        ImGui::PopStyleColor();

                        ImGui::Spacing(); ImGui::Spacing();
                        ImGui::PushFont(font::description_font);
                        static char modBase[32];
                        snprintf(modBase, sizeof(modBase), "Module: 0x%llX", Globals::ModuleBase.load());
                        ImGui::Text("%s", modBase);
                        ImGui::PopFont();

                        ImGui::Spacing();
                        if (custom::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 40))) {
                            Globals::Running = false;
                        }
                    }
                    ImGui::EndChild();
                    PopCol();
                }
            }
        }
        ImGui::EndChild();

        // Notifications
        p_notif.Render();

        // Settings popup
        if (settings_popup.begin(340.0f)) {
            custom::ColorEdit4("Accent##sp",
                (float*)&c::anim::active, picker_flags);
            custom::ColorEdit4("Secondary##sp",
                (float*)&c::dark_color, picker_flags);

            static const char* fw_p[] = { "Regular", "Medium", "Semibold" };
            static ImFont* df_p[] = {
                font::default_r, font::default_m, font::default_s };

            custom::Combo("Font##sp", &c::selected_font,
                fw_p, IM_ARRAYSIZE(fw_p));
            ImGui::GetIO().FontDefault = df_p[c::selected_font];

            static const char* os_p[] = {
                ICON_ARROWS_RIGHT_LINE " From the left",
                ICON_ARROWS_LEFT_LINE " From the right" };
            custom::Combo("Anim##sp", &c::anim_offset_side,
                os_p, IM_ARRAYSIZE(os_p));
            custom::ToggleButton(ICON_MOONLIGHT_FILL, ICON_SUN_2_FILL,
                &bTheme, ImVec2(ImGui::GetContentRegionAvail().x, 40));

            if (custom::Button("Save##sp",
                    ImVec2(ImGui::GetContentRegionAvail().x, 50)))
                settings_popup.close();
            settings_popup.end();
        }
    }
    ImGui::End();
}

// ============================================================================
//  Bridge DX11 globals
// ============================================================================
void PortableMenu_SetD3D11(ID3D11Device* device, ID3D11DeviceContext* ctx,
                           IDXGISwapChain* swapChain, ID3D11RenderTargetView* rtv,
                           HWND__* window)
{
    g_pd3dDevice            = device;
    g_pd3dDeviceContext     = ctx;
    g_pSwapChain            = swapChain;
    g_mainRenderTargetView  = rtv;
    hwnd                    = window;
}







