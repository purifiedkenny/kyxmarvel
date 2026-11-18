#pragma once
// =====================================================================
// Config.hpp — JSON config manager singleton (project-d style)
// 8 save slots, JSON files in configs/ directory
// =====================================================================

#include "Structs.hpp"
#include <filesystem>
#include <fstream>
#include <string>

// LOG macros — pull from spdlog if available, otherwise no-op
#ifdef SPDLOG_LOGGER_CALL
  // spdlog available via PCH
#else
  #ifndef LOG_INFO
    #define LOG_INFO(...)  do {} while(0)
  #endif
  #ifndef LOG_WARN
    #define LOG_WARN(...)  do {} while(0)
  #endif
  #ifndef LOG_ERROR
    #define LOG_ERROR(...) do {} while(0)
  #endif
#endif

class Config {
public:
    static Config& Get() {
        static Config instance;
        return instance;
    }

    bool Init(const std::string& configDir = "configs") {
        m_configDir = configDir;
        std::filesystem::create_directories(m_configDir);
        LOG_INFO("Config directory: {}", m_configDir);

        // Try to load default profile
        LoadConfig(0);
        return true;
    }

    void SaveConfig(int profile = -1) {
        if (profile < 0) profile = m_settings.Misc.ActiveConfig;
        if (profile < 0 || profile > 7) return;

        try {
            nlohmann::json j = m_settings;
            auto path = GetProfilePath(profile);
            std::ofstream ofs(path);
            if (ofs.is_open()) {
                ofs << j.dump(4);
                LOG_INFO("Config saved: {}", path);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to save config: {}", e.what());
        }
    }

    void LoadConfig(int profile = 0) {
        if (profile < 0 || profile > 7) return;

        auto path = GetProfilePath(profile);
        if (!std::filesystem::exists(path)) {
            LOG_WARN("Config file not found: {}", path);
            return;
        }

        try {
            std::ifstream ifs(path);
            if (ifs.is_open()) {
                nlohmann::json j;
                ifs >> j;
                m_settings = j.get<KyxMarvel::Settings>();
                m_settings.Misc.ActiveConfig = profile;
                LOG_INFO("Config loaded: {}", path);
            }
        } catch (const std::exception& e) {
            LOG_ERROR("Failed to load config: {}", e.what());
        }
    }

    void DeleteConfig(int profile) {
        if (profile < 0 || profile > 7) return;
        auto path = GetProfilePath(profile);
        if (std::filesystem::exists(path)) {
            std::filesystem::remove(path);
            LOG_INFO("Config deleted: {}", path);
        }
    }

    KyxMarvel::Settings&       GetSettings()       { return m_settings; }
    const KyxMarvel::Settings& GetSettings() const { return m_settings; }

private:
    Config() = default;

    std::string GetProfilePath(int profile) const {
        return m_configDir + "/profile_" + std::to_string(profile) + ".json";
    }

    KyxMarvel::Settings m_settings;
    std::string         m_configDir = "configs";
};

inline Config& config = Config::Get();
