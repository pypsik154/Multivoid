// ui/pause_player_list.cpp -- see ui/pause_player_list.h.

#include "ui/pause_player_list.h"

#include "coop/player/nick_color.h"
#include "coop/player/roster.h"
#include "l10n/l10n.h"
#include "ui/link_format.h"
#include "ui/scale.h"

#include "imgui.h"

#include <cstdio>

namespace ui::pause_player_list {
namespace {

using ui::scale::S;

ImVec4 NickColour(const coop::roster::Row& r) {
    // The scoreboard's rule: a custom nick colour wins, else the role colour.
    const uint32_t custom = r.isLocal ? coop::nick_color::LocalPacked()
                                      : coop::nick_color::PackedForSlot(r.slot);
    if (coop::nick_color::IsCustom(custom))
        return ImVec4(coop::nick_color::R(custom) / 255.f, coop::nick_color::G(custom) / 255.f,
                      coop::nick_color::B(custom) / 255.f, 1.0f);
    return r.isHost ? ImVec4(1.00f, 0.82f, 0.35f, 1.0f)   // host  = gold
                    : ImVec4(0.86f, 0.89f, 0.94f, 1.0f);  // client = soft white
}

}  // namespace

void Render() {
    coop::roster::Snapshot s;
    coop::roster::GetSnapshot(s);
    if (!s.inSession || s.count <= 0) return;

    const ImGuiIO& io = ImGui::GetIO();
    // Top-right, pinned: VOTV's pause menu sits on the left and in the middle.
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - S(24.0f), S(24.0f)), ImGuiCond_Always,
                            ImVec2(1.0f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(S(320.0f), 0.0f), ImGuiCond_Always);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(S(14.0f), S(11.0f)));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, S(8.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.07f, 0.09f, 0.90f));

    // Read-only: no inputs, no focus, never in front of the pause menu's own clicks.
    const ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus;

    if (ImGui::Begin("###coop_pause_players", nullptr, flags)) {
        char head[96];
        std::snprintf(head, sizeof(head), "%s  %d", l10n::T("Players in this session"), s.count);
        ImGui::TextUnformatted(head);
        ImGui::Separator();

        const char* const you = l10n::T("(you)");
        for (int i = 0; i < s.count; ++i) {
            const coop::roster::Row& r = s.rows[i];
            const char* nick = r.nick[0] ? r.nick : (r.isLocal ? l10n::T("Player") : l10n::T("Remote player"));

            ImGui::TextColored(NickColour(r), "%s", nick);
            if (r.isLocal) {
                ImGui::SameLine();
                ImGui::TextDisabled("%s", you);
            }
            if (!r.connected) {
                ImGui::SameLine();
                ImGui::TextDisabled("%s", l10n::T("(connecting)"));
            }

            char pb[16];
            ui::link_format::FormatPing(r.ping, r.linkKind, pb, sizeof(pb));
            ImGui::SameLine();
            const float tw = ImGui::CalcTextSize(pb).x;
            const float avail = ImGui::GetContentRegionAvail().x;
            if (avail > tw) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (avail - tw));
            ImGui::TextDisabled("%s", pb);
        }
    }
    ImGui::End();

    ImGui::PopStyleColor(1);
    ImGui::PopStyleVar(3);
}

}  // namespace ui::pause_player_list
