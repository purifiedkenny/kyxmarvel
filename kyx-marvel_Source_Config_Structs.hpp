#pragma once
// =====================================================================
// Structs.hpp — JSON-serializable config structs overlay
// Wraps KyxMarvel::Settings for nlohmann/json serialization
// =====================================================================

#include <Features/Settings.h>
#include <nlohmann/json.hpp>

// JSON serialization for KyxMarvel settings
namespace KyxMarvel {

    // --- FColor ---
    inline void to_json(nlohmann::json& j, const FColor& c) {
        j = nlohmann::json{ {"r", c.R}, {"g", c.G}, {"b", c.B}, {"a", c.A} };
    }
    inline void from_json(const nlohmann::json& j, FColor& c) {
        j.at("r").get_to(c.R); j.at("g").get_to(c.G);
        j.at("b").get_to(c.B); j.at("a").get_to(c.A);
    }

    // --- AimbotSettings ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(AimbotSettings,
        Enabled, VisibleCheck, Prediction, HitboxTarget,
        FirstHotkey, SecondHotkey,
        DeadzoneFOV, DeadzoneSmooth,
        Priority, FovMode, Smooth,
        FOV, SmoothValue, TargetSwitchDelay,
        FirstHotkeyDistance, SecondHotkeyDistance,
        AllowPanicTurnOff, PanicHotkey,
        DynamicFovMin, DynamicFovMax,
        KoestepAccel, KoestepDecel
    )

    // --- VisualSettings ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(VisualSettings,
        Enabled, Box, BoxStyle, BoxVisibleColor, BoxInvisibleColor, BoxFill, BoxFillColor,
        HBarPosition, HealthBarWidth,
        HeroESP, NameVisibleColor, NameInvisibleColor,
        ShowTeam, OnlyVisible, IgnoreKnocked, MaxDistance,
        ShowRank, RankVisibleColor, RankInvisibleColor,
        ShowPlayerName, PlayerNameVisColor, PlayerNameInvisColor,
        ShowDistance, DistVisibleColor, DistInvisibleColor,
        Skeleton, SkeletonVisColor, SkeletonInvisColor,
        Snaplines, SnaplinesDistance,
        HeadCircle, HeadCircleColor,
        AimbotFovDraw, AimbotFovColor,
        Crosshair, CrosshairColor, CrosshairType,
        TextShadow,
        NameFontSize, PlayerNameFontSize, DistFontSize,
        FontIndex, FontSize
    )

    // --- RadarSettings ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RadarSettings,
        Enabled, DrawMesh, Healthbar,
        EnemyVisColor, EnemyInvisColor,
        ShowDistance, DistVisColor, DistInvisColor,
        LocalColor, LocalSize, EnemySize,
        DirectionLines,
        PosX, PosY, Size, Scale, CustomPosEnabled, Rounding,
        FontIndex, FontSize
    )

    // --- MiscSettings::ConfigBind ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(MiscSettings::ConfigBind,
        Hotkey, Profile
    )

    // --- MiscSettings ---
    inline void to_json(nlohmann::json& j, const MiscSettings& s) {
        j = nlohmann::json{
            {"DMAAcceleration", s.DMAAcceleration}, {"OverlayVSync", s.OverlayVSync},
            {"LimitCPU", s.LimitCPU}, {"CPULimitMs", s.CPULimitMs},
            {"WidescreenSupport", s.WidescreenSupport}, {"InfoWidget", s.InfoWidget},
            {"Overlay", s.Overlay}, {"Mouse", s.Mouse},
            {"MenuMainColor", s.MenuMainColor}, {"MenuTextColor", s.MenuTextColor},
            {"BoxThickness", s.BoxThickness}, {"SnaplineThickness", s.SnaplineThickness},
            {"SkeletonVisThick", s.SkeletonVisThick}, {"SkeletonInvisThick", s.SkeletonInvisThick},
            {"CrosshairSize", s.CrosshairSize}, {"CrosshairThickness", s.CrosshairThickness},
            {"AimFovThickness", s.AimFovThickness},
            {"FirmwareChecker", s.FirmwareChecker},
            {"ConfigBind0", s.ConfigBinds[0]}, {"ConfigBind1", s.ConfigBinds[1]},
            {"ConfigBind2", s.ConfigBinds[2]}, {"ConfigBind3", s.ConfigBinds[3]},
            {"ActiveConfig", s.ActiveConfig}
        };
    }
    inline void from_json(const nlohmann::json& j, MiscSettings& s) {
        if (j.contains("DMAAcceleration"))   j.at("DMAAcceleration").get_to(s.DMAAcceleration);
        if (j.contains("OverlayVSync"))      j.at("OverlayVSync").get_to(s.OverlayVSync);
        if (j.contains("LimitCPU"))          j.at("LimitCPU").get_to(s.LimitCPU);
        if (j.contains("CPULimitMs"))        j.at("CPULimitMs").get_to(s.CPULimitMs);
        if (j.contains("WidescreenSupport")) j.at("WidescreenSupport").get_to(s.WidescreenSupport);
        if (j.contains("InfoWidget"))        j.at("InfoWidget").get_to(s.InfoWidget);
        if (j.contains("Overlay"))           j.at("Overlay").get_to(s.Overlay);
        if (j.contains("Mouse"))             j.at("Mouse").get_to(s.Mouse);
        if (j.contains("MenuMainColor"))     j.at("MenuMainColor").get_to(s.MenuMainColor);
        if (j.contains("MenuTextColor"))     j.at("MenuTextColor").get_to(s.MenuTextColor);
        if (j.contains("BoxThickness"))      j.at("BoxThickness").get_to(s.BoxThickness);
        if (j.contains("SnaplineThickness")) j.at("SnaplineThickness").get_to(s.SnaplineThickness);
        if (j.contains("SkeletonVisThick"))  j.at("SkeletonVisThick").get_to(s.SkeletonVisThick);
        if (j.contains("SkeletonInvisThick")) j.at("SkeletonInvisThick").get_to(s.SkeletonInvisThick);
        if (j.contains("CrosshairSize"))     j.at("CrosshairSize").get_to(s.CrosshairSize);
        if (j.contains("CrosshairThickness")) j.at("CrosshairThickness").get_to(s.CrosshairThickness);
        if (j.contains("AimFovThickness"))   j.at("AimFovThickness").get_to(s.AimFovThickness);
        if (j.contains("FirmwareChecker"))   j.at("FirmwareChecker").get_to(s.FirmwareChecker);
        if (j.contains("ConfigBind0"))       j.at("ConfigBind0").get_to(s.ConfigBinds[0]);
        if (j.contains("ConfigBind1"))       j.at("ConfigBind1").get_to(s.ConfigBinds[1]);
        if (j.contains("ConfigBind2"))       j.at("ConfigBind2").get_to(s.ConfigBinds[2]);
        if (j.contains("ConfigBind3"))       j.at("ConfigBind3").get_to(s.ConfigBinds[3]);
        if (j.contains("ActiveConfig"))      j.at("ActiveConfig").get_to(s.ActiveConfig);
    }

    // --- RaycastSettings ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RaycastSettings,
        Enabled, DebugWireframe, MaxDistance, RebuildIntervalMs
    )

    // --- InputSettings ---
    inline void to_json(nlohmann::json& j, const InputSettings& s) {
        j = nlohmann::json{
            {"DeviceType", s.DeviceType}, {"KMBoxIP", std::string(s.KMBoxIP)},
            {"KMBoxPort", s.KMBoxPort}, {"Sensitivity", s.Sensitivity},
            {"MemAimSmooth", s.MemAimSmooth}
        };
    }
    inline void from_json(const nlohmann::json& j, InputSettings& s) {
        if (j.contains("DeviceType"))   j.at("DeviceType").get_to(s.DeviceType);
        if (j.contains("KMBoxIP")) {
            std::string ip = j.at("KMBoxIP").get<std::string>();
            strncpy_s(s.KMBoxIP, ip.c_str(), sizeof(s.KMBoxIP) - 1);
        }
        if (j.contains("KMBoxPort"))    j.at("KMBoxPort").get_to(s.KMBoxPort);
        if (j.contains("Sensitivity"))  j.at("Sensitivity").get_to(s.Sensitivity);
        if (j.contains("MemAimSmooth")) j.at("MemAimSmooth").get_to(s.MemAimSmooth);
    }

    // --- Master Settings ---
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Settings,
        Aimbot, Visuals, Radar, Misc, Raycast, Input, MenuToggleKey
    )

} // namespace KyxMarvel
