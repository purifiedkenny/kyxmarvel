#pragma once
// =====================================================================
// Globals — game-specific constants and shared state
// Mirrors project-d/Source/Globals.hpp pattern
// =====================================================================

// Game process name — set at runtime by Auth::Init() (NOP-proof XOR decode).
// In DEV_AUTH_BYPASS mode this is set to plaintext immediately.
// Default value is the known process name for local-only / no-auth runs.
inline const char* g_GameProcessName = "Marvel-Win64-Shipping.exe";
#define GAME_NAME g_GameProcessName

// --- Screen constants (updated by overlay at startup) ---
inline float Screen[2]       = { 2560.0f, 1440.0f };
inline float ScreenCenter[2] = { 1280.0f, 720.0f };

namespace Globals {
    inline std::atomic<bool>     Running        = true;
    inline std::atomic<uint64_t> ModuleBase     = 0;
    inline std::atomic<uint64_t> ModuleSize     = 0;
    inline std::atomic<uint64_t> GNamesBase     = 0;
    inline std::atomic<bool>     DmaInitialized = false;
    inline std::atomic<bool>     SdkInitialized = false;
}

namespace ProcInfo {
    inline int  PID      = 0;
    inline bool CR3Fixed = false;
}
