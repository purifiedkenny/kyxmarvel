#pragma once

// =====================================================================
// Precompiled header — kyx-marvel
// Mirrors project-d style: system → windows → dx → vendor → project
// =====================================================================

// --- CRT / STL ---
#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <map>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include <algorithm>
#include <format>
#include <numbers>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>

// --- Windows ---
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwmapi.h>
#include <ShellScalingApi.h>

// DMALibrary vmmdll.h needs NTSTATUS which WIN32_LEAN_AND_MEAN strips
#ifndef _NTDEF_
typedef LONG NTSTATUS;
#endif

// --- Vendor: spdlog ---
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

// --- System: debug helpers ---
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

// --- System: multimedia timer ---
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// --- Vendor: nlohmann/json ---
#include <nlohmann/json.hpp>

// --- Vendor: imgui ---
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

// --- KyxMarvel library ---
#include <MarvelLib.h>

// --- Log macros (project-d style) ---
#define LOG_INFO(...)     spdlog::info(__VA_ARGS__)
#define LOG_WARN(...)     spdlog::warn(__VA_ARGS__)
#define LOG_ERROR(...)    spdlog::error(__VA_ARGS__)
#define LOG_DEBUG(...)    spdlog::debug(__VA_ARGS__)
#define LOG_CRITICAL(...) spdlog::critical(__VA_ARGS__)

// --- Project headers ---
#include <Globals.hpp>
#include <Exception.hpp>
#include <Timer.hpp>
