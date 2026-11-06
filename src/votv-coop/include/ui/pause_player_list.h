// ui/pause_player_list.h -- the session's players, listed beside VOTV's pause (ESC) menu.
//
// A passive surface on ui::imgui_overlay: while the native pause menu is up and a session runs,
// a small read-only panel in the top-right corner names every player in the session (host in
// gold, a custom nick colour where one is set), marks your own row and shows each ping. It takes
// no input and no cursor, so the pause menu under it keeps working as before; the interactive
// list stays the tilde scoreboard.
//
// Principle 7: pure presentation over coop::roster's snapshot.

#pragma once

namespace ui::pause_player_list {

// Draw the panel. Called inside the overlay's ImGui frame (render thread) while the pause menu
// is open; draws nothing out of session.
void Render();

}  // namespace ui::pause_player_list
