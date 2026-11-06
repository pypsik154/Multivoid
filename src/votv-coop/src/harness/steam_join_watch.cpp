// harness/steam_join_watch.cpp -- see harness/steam_join_watch.h.

#include "harness/steam_join_watch.h"

#include "coop/player/roster.h"
#include "coop/session/session_manager.h"
#include "ue_wrap/core/log.h"
#include "ue_wrap/core/paths.h"

#include <windows.h>

#include <atomic>
#include <chrono>
#include <string>
#include <thread>

namespace harness::steam_join_watch {
namespace {

std::atomic<bool> g_started{false};

// The request file's text: at most one short line. Empty when the file is missing or unreadable.
std::string ReadSmall(const std::wstring& path) {
    HANDLE h = ::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_DELETE, nullptr,
                             OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (h == INVALID_HANDLE_VALUE) return {};
    char buf[128] = {};
    DWORD got = 0;
    const BOOL ok = ::ReadFile(h, buf, sizeof(buf) - 1, &got, nullptr);
    ::CloseHandle(h);
    if (!ok) return {};
    return std::string(buf, got);
}

// "host:port" made of the characters an address can hold; anything else is refused whole.
std::string CleanAddress(const std::string& raw) {
    std::string out;
    for (char c : raw) {
        if (c == '\r' || c == '\n') break;
        if (c == ' ' || c == '\t') continue;
        const bool okChar = (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                            c == '.' || c == ':' || c == '-' || c == '[' || c == ']';
        if (!okChar) return {};
        out.push_back(c);
        if (out.size() > 80) return {};
    }
    return out;
}

void Loop(std::wstring path) {
    for (;;) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (::GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) continue;
        const std::string addr = CleanAddress(ReadSmall(path));
        ::DeleteFileW(path.c_str());
        if (addr.empty()) {
            UE_LOGW("steam_join: multivoid_join.txt held no usable address -- ignored");
            continue;
        }
        coop::roster::Snapshot s;
        coop::roster::GetSnapshot(s);
        if (s.inSession) {
            UE_LOGI("steam_join: a join to %s was asked for while a session runs -- ignored",
                    ue_wrap::log::Addr(addr).c_str());
            continue;
        }
        UE_LOGI("steam_join: the Steam bridge asked to join %s -- Direct connect",
                ue_wrap::log::Addr(addr).c_str());
        if (!coop::session_manager::ConnectDirect(addr))
            UE_LOGW("steam_join: Direct connect to %s was refused", ue_wrap::log::Addr(addr).c_str());
    }
}

}  // namespace

void Start() {
    if (g_started.exchange(true)) return;
    const std::wstring dir = ue_wrap::paths::ExeDir();
    if (dir.empty()) {
        UE_LOGW("steam_join: no exe folder -- the Steam join watch is off");
        return;
    }
    const std::wstring path = dir + L"\\multivoid_join.txt";
    ::DeleteFileW(path.c_str());  // a request left over from an earlier run is not this player's click now
    std::thread(Loop, path).detach();
    UE_LOGI("steam_join: watching for Steam invites (multivoid_join.txt beside the exe)");
}

}  // namespace harness::steam_join_watch
