// Legacy Project - bootstrapper (Deceit\LegacyProject.exe). Replaces the former launcher of the owner, kept in
// bin\win_x64\LegacyProject (same behaviour for the launch itself). See docs/loader.md, "Bootstrapper".
//
// Steam launch option of Deceit:   LegacyProject %command%
//   Steam starts this program in the Deceit folder, %command% being Steam's own command for the game. Without the
//   option, Steam starts the game as usual and Legacy Project stays off: that is how a player turns it on and off.
//
// Legacy version (bin\win_x64\Game.dll of Deceit 5.2.9, same test as the startup DLL):
//   1. LegacyProject\dinput8.dll (where it is kept while the game does not run) is moved to bin\win_x64, where the
//      game loads it;
//   2. bin\win_x64\Deceit.exe -project Deceit.crygame is started (the arguments of Steam's command follow), with
//      DFX_LP_BOOT=1 in its environment: the startup DLL only turns Legacy Project on in a game started this way
//      (or in a game-server process, -lp_dedicated). Otherwise it does nothing and puts itself back in LegacyProject;
//   3. this program ends at once (it never waits for the game). When the game has ended, the PartyServer agent puts
//      the DLL back into LegacyProject (Lp/LpBoot.cs: the game usually crashes while closing, so the DLL's own
//      DLL_PROCESS_DETACH rarely runs; it does it too when it does run).
// No log file (owner's decision): a message box only when the game cannot be started.
// Any other version (live): our dinput8.dll must not be in bin\win_x64 (it is put back into LegacyProject if a crash
// left it there), then Steam's command is started unchanged.
#include <windows.h>
#include <shellapi.h>
#include <cstdint>
#include <cstdio>
#include <cwchar>
#include <string>

static const uint32_t kLegacyGameDllTimestamp = 1628031901;     // Game.dll of Deceit 5.2.9 (loader/loader.cpp)
static const wchar_t kTitle[] = L"Legacy Project";

static std::wstring g_root, g_bin, g_ours;

// No log file: only a message when the game cannot be started.
static void Error(const std::wstring& text) {
    MessageBoxW(nullptr, text.c_str(), kTitle, MB_OK | MB_ICONERROR | MB_SETFOREGROUND);
}

static std::wstring Join(const std::wstring& a, const wchar_t* b) { return a + L"\\" + b; }
static bool Exists(const std::wstring& p) { return GetFileAttributesW(p.c_str()) != INVALID_FILE_ATTRIBUTES; }
static std::wstring Parent(const std::wstring& p) { size_t s = p.find_last_of(L'\\'); return s == std::wstring::npos ? p : p.substr(0, s); }

static std::wstring ErrorText(DWORD e) {
    wchar_t* buf = nullptr;
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, e,
                   MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), (LPWSTR)&buf, 0, nullptr);
    std::wstring s = buf ? buf : L"";
    if (buf) LocalFree(buf);
    while (!s.empty() && (s.back() == L'\r' || s.back() == L'\n' || s.back() == L' ')) s.pop_back();
    wchar_t code[32]; _snwprintf(code, 31, L" (error %lu)", e); code[31] = 0;
    return s + code;
}

// Same test as the startup DLL: the PE time stamp of Game.dll on disk.
static bool IsLegacyGame() {
    HANDLE h = CreateFileW(Join(g_bin, L"Game.dll").c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) return false;
    uint8_t buf[0x400] = {}; DWORD got = 0;
    ReadFile(h, buf, sizeof(buf), &got, nullptr); CloseHandle(h);
    if (got < 0x100 || buf[0] != 'M' || buf[1] != 'Z') return false;
    uint32_t lfanew = *(uint32_t*)(buf + 0x3c);
    if (lfanew + 12 > got || memcmp(buf + lfanew, "PE\0\0", 4) != 0) return false;
    return *(uint32_t*)(buf + lfanew + 8) == kLegacyGameDllTimestamp;
}

static bool Move(const std::wstring& from, const std::wstring& to, DWORD* err) {
    for (int i = 0; i < 5; ++i) {
        if (MoveFileExW(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING)) return true;
        *err = GetLastError();
        Sleep(200);
    }
    return false;
}

// bin\win_x64\dinput8.dll out of the way, into LegacyProject (as dinput8.dll when none is there, else as .old: the
// one of LegacyProject is the newer, written by an update). A file in use can be renamed, not replaced.
static bool StoreActive() {
    const std::wstring active = Join(g_bin, L"dinput8.dll"), stored = Join(g_ours, L"dinput8.dll");
    if (!Exists(active)) return true;
    const std::wstring to = Exists(stored) ? stored + L".old" : stored;
    DWORD err = 0;
    return Move(active, to, &err);
}

// Steam's command without its program (the arguments that follow it), as given.
static std::wstring ArgumentsAfterProgram(const wchar_t* cmd) {
    const wchar_t* p = cmd;
    while (*p == L' ' || *p == L'\t') ++p;
    if (*p == L'"') { ++p; while (*p && *p != L'"') ++p; if (*p) ++p; }
    else while (*p && *p != L' ' && *p != L'\t') ++p;
    while (*p == L' ' || *p == L'\t') ++p;
    return p;
}
// The rest of our own command line after our program name: Steam's %command%.
static std::wstring SteamCommand() { return ArgumentsAfterProgram(GetCommandLineW()); }

static bool Start(const std::wstring& exe, const std::wstring& commandLine, const std::wstring& dir) {
    std::wstring cmd = commandLine;
    STARTUPINFOW si = {}; si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(exe.empty() ? nullptr : exe.c_str(), &cmd[0], nullptr, nullptr, FALSE, 0, nullptr, dir.c_str(), &si, &pi)) {
        Error(L"Deceit could not be started.\n\n" + (exe.empty() ? commandLine : exe) + L"\n\n" + ErrorText(GetLastError()));
        return false;
    }
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess);
    return true;
}

static int RunLegacy(const std::wstring& steamCommand) {
    const std::wstring active = Join(g_bin, L"dinput8.dll"), stored = Join(g_ours, L"dinput8.dll");
    // DeceitFix.dll is not required here: after an interrupted update it may be missing, and it is the startup DLL
    // that reinstalls it (loader/loader.cpp, "DeceitFix.dll missing, reinstalling"). Only a startup DLL is needed.
    if (Exists(stored)) {
        DWORD err = 0;
        // one left in bin\win_x64 (crash, or an older one): moved aside first, so the newer one can take its place
        if (Exists(active)) Move(active, stored + L".old", &err);
        if (!Move(stored, active, &err)) {
            Error(L"Legacy Project could not be started: dinput8.dll could not be moved into bin\\win_x64.\n\n" + ErrorText(err));
            return 1;
        }
    } else if (!Exists(active)) {
        Error(L"Legacy Project is not installed correctly: dinput8.dll is missing from bin\\win_x64\\LegacyProject.\n\n"
              L"Run the Legacy Project installer (Repair), or download the latest release again from the Legacy Project GitHub page.");
        return 1;
    }                                           // else: already in bin\win_x64 (a crash left it there)

    SetEnvironmentVariableW(L"DFX_LP_BOOT", L"1");
    // Steam's command on the legacy branch is "...\bin\win_x64\Deceit.exe -project Deceit.crygame": its arguments
    // are kept as they are; "-project Deceit.crygame" is added only when they do not have a -project already
    const std::wstring exe = Join(g_bin, L"Deceit.exe");
    const std::wstring args = steamCommand.empty() ? L"" : ArgumentsAfterProgram(steamCommand.c_str());
    std::wstring lower = args;
    for (auto& c : lower) c = towlower(c);
    std::wstring cmd = L"\"" + exe + L"\"";
    if (lower.find(L"-project") == std::wstring::npos) cmd += L" -project Deceit.crygame";
    if (!args.empty()) cmd += L" " + args;
    return Start(exe, cmd, g_bin) ? 0 : 1;
}

static int RunLive(const std::wstring& steamCommand) {
    StoreActive();
    SetEnvironmentVariableW(L"DFX_LP_BOOT", nullptr);
    if (!steamCommand.empty()) return Start(L"", steamCommand, g_root) ? 0 : 1;
    const std::wstring exe = Join(g_root, L"start_protected_game.exe");
    return Start(exe, L"\"" + exe + L"\" -project Deceit.crygame", g_root) ? 0 : 1;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) {
    wchar_t self[MAX_PATH * 4] = {};
    DWORD n = GetModuleFileNameW(nullptr, self, MAX_PATH * 4 - 1);
    if (!n || n >= MAX_PATH * 4 - 1) { MessageBoxW(nullptr, L"The path of LegacyProject.exe could not be read.", kTitle, MB_OK | MB_ICONERROR); return 1; }
    g_root = Parent(self);
    g_bin = g_root + L"\\bin\\win_x64";
    g_ours = Join(g_bin, L"LegacyProject");
    DeleteFileW(Join(Join(g_root, L"logbackups"), L"LegacyProject launcher.log").c_str());   // left by the first test build
    if (!Exists(Join(g_bin, L"Deceit.exe"))) {
        MessageBoxW(nullptr, L"LegacyProject.exe must be in the Deceit folder (the folder that contains \"bin\").\n\n"
                    L"Put the content of the Legacy Project zip into the Deceit folder.", kTitle, MB_OK | MB_ICONERROR);
        return 1;
    }
    const std::wstring steamCommand = SteamCommand();
    const bool legacy = IsLegacyGame();
    return legacy ? RunLegacy(steamCommand) : RunLive(steamCommand);
}
