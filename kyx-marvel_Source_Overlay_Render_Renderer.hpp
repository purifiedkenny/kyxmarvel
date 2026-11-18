#pragma once
// =====================================================================
// Renderer.hpp — ImGui DX11 IRenderer implementation
// Bridges KyxMarvel::IRenderer → ImGui draw calls
// =====================================================================

#include <Render/IRenderer.h>
#include <imgui.h>

class ImGuiRenderer : public KyxMarvel::IRenderer {
public:
    void DrawRect(const KyxMarvel::FVector2D& min, const KyxMarvel::FVector2D& max,
                  const KyxMarvel::FColor& color, float thickness, bool filled) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        ImU32 col = ToImCol(color);
        ImVec2 a(static_cast<float>(min.X), static_cast<float>(min.Y));
        ImVec2 b(static_cast<float>(max.X), static_cast<float>(max.Y));
        if (filled)
            dl->AddRectFilled(a, b, col);
        else
            dl->AddRect(a, b, col, 0.0f, 0, thickness);
    }

    void DrawLine(const KyxMarvel::FVector2D& from, const KyxMarvel::FVector2D& to,
                  const KyxMarvel::FColor& color, float thickness) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        dl->AddLine(
            ImVec2(static_cast<float>(from.X), static_cast<float>(from.Y)),
            ImVec2(static_cast<float>(to.X), static_cast<float>(to.Y)),
            ToImCol(color), thickness);
    }

    void DrawCircle(const KyxMarvel::FVector2D& center, float radius,
                    const KyxMarvel::FColor& color, float thickness, bool filled) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        ImVec2 c(static_cast<float>(center.X), static_cast<float>(center.Y));
        ImU32 col = ToImCol(color);
        if (filled)
            dl->AddCircleFilled(c, radius, col, 64);
        else
            dl->AddCircle(c, radius, col, 64, thickness);
    }

    void DrawText(const KyxMarvel::FVector2D& pos, const std::string& text,
                  const KyxMarvel::FColor& color, float size, bool centered) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        ImVec2 p(static_cast<float>(pos.X), static_cast<float>(pos.Y));

        if (centered) {
            float scale = (size > 0.0f && ImGui::GetFontSize() > 0.0f)
                              ? size / ImGui::GetFontSize()
                              : 1.0f;
            ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
            p.x -= (textSize.x * scale) * 0.5f;
        }

        // Shadow
        dl->AddText(nullptr, size, ImVec2(p.x + 1, p.y + 1),
                    IM_COL32(0, 0, 0, 200), text.c_str());
        // Main
        dl->AddText(nullptr, size, p, ToImCol(color), text.c_str());
    }

    void DrawGradientRect(const KyxMarvel::FVector2D& min, const KyxMarvel::FVector2D& max,
                          const KyxMarvel::FColor& topColor, const KyxMarvel::FColor& bottomColor) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        ImVec2 a(static_cast<float>(min.X), static_cast<float>(min.Y));
        ImVec2 b(static_cast<float>(max.X), static_cast<float>(max.Y));
        dl->AddRectFilledMultiColor(a, b,
            ToImCol(topColor), ToImCol(topColor),
            ToImCol(bottomColor), ToImCol(bottomColor));
    }

    void DrawRoundedRect(const KyxMarvel::FVector2D& min, const KyxMarvel::FVector2D& max,
                         const KyxMarvel::FColor& color, float thickness,
                         float rounding, bool filled) override {
        auto* dl = ImGui::GetBackgroundDrawList();
        ImVec2 a(static_cast<float>(min.X), static_cast<float>(min.Y));
        ImVec2 b(static_cast<float>(max.X), static_cast<float>(max.Y));
        ImU32 col = ToImCol(color);
        if (filled)
            dl->AddRectFilled(a, b, col, rounding);
        else
            dl->AddRect(a, b, col, rounding, 0, thickness);
    }

    double GetScreenWidth() const override  { return m_screenW; }
    double GetScreenHeight() const override { return m_screenH; }

    void SetScreenSize(double w, double h) {
        m_screenW = w;
        m_screenH = h;
    }

private:
    double m_screenW = 2560.0;
    double m_screenH = 1440.0;

    static ImU32 ToImCol(const KyxMarvel::FColor& c) {
        return IM_COL32(
            static_cast<int>(c.R * 255.0f),
            static_cast<int>(c.G * 255.0f),
            static_cast<int>(c.B * 255.0f),
            static_cast<int>(c.A * 255.0f)
        );
    }
};
