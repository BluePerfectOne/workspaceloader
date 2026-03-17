#define UNICODE
#define _UNICODE

#include <windows.h>
#include <shellapi.h>

#include <algorithm>
#include <cwctype>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <fstream>
#include <json.hpp>

namespace {

struct Project {
    std::wstring name;
    std::wstring path;
};

struct ProjectResolved {
    Project project;
    std::optional<std::wstring> workspace;
};


constexpr int kListId = 1001;
constexpr int kOpenButtonId = 1002;
constexpr int kQuitButtonId = 1003;
constexpr int kRefreshButtonId = 1004;

std::vector<ProjectResolved> g_projects;

std::wstring ToLower(std::wstring value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](wchar_t ch) { return static_cast<wchar_t>(towlower(ch)); });
    return value;
}

std::optional<std::wstring> FindWorkspace(const std::wstring& dir) {
    try {
        if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
            return std::nullopt;
        }

        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const std::wstring ext = ToLower(entry.path().extension().wstring());
            if (ext == L".code-workspace") {
                return entry.path().wstring();
            }
        }
    } catch (...) {
        return std::nullopt;
    }

    return std::nullopt;
}

std::wstring FileNameOnly(const std::wstring& fullPath) {
    const std::filesystem::path p(fullPath);
    return p.filename().wstring();
}

void LaunchTarget(const std::wstring& target) {
    std::wstring args = L"--reuse-window \"" + target + L"\"";
    const HINSTANCE result = ShellExecuteW(nullptr, L"open", L"code", args.c_str(), nullptr, SW_SHOWNORMAL);

    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        MessageBoxW(nullptr,
                    L"Failed to launch VS Code with command 'code'.\n\n"
                    L"Make sure VS Code is installed and the 'code' command is available in PATH.",
                    L"Launch Error",
                    MB_ICONERROR | MB_OK);
    }
}

void OpenSelectedProject(HWND hwnd) {
    const HWND list = GetDlgItem(hwnd, kListId);
    const int index = static_cast<int>(SendMessageW(list, LB_GETCURSEL, 0, 0));
    if (index == LB_ERR || index < 0 || index >= static_cast<int>(g_projects.size())) {
        MessageBoxW(hwnd, L"Please select a project first.", L"No Selection", MB_ICONWARNING | MB_OK);
        return;
    }

    const auto& selected = g_projects[static_cast<size_t>(index)];
    if (selected.workspace.has_value()) {
        LaunchTarget(*selected.workspace);
    } else {
        LaunchTarget(selected.project.path);
    }
}

void PopulateProjects(HWND hwnd) {
    g_projects.clear();
    const HWND list = GetDlgItem(hwnd, kListId);
    // Clear the listbox before adding new items
    SendMessageW(list, LB_RESETCONTENT, 0, 0);

    std::vector<Project> projects;
    std::ifstream f("projects.json");
    if (!f) {
        MessageBoxW(hwnd, L"projects.json file not found.", L"Config Error", MB_ICONERROR | MB_OK);
        return;
    }
    try {
        nlohmann::json j;
        f >> j;
        // Debug: print raw JSON to console
        OutputDebugStringA(("Raw JSON: " + j.dump()).c_str());
        for (const auto& item : j) {
            try {
                std::string nameStr = item["name"].get<std::string>();
                std::string pathStr = item["path"].get<std::string>();
                // Debug: print each string before conversion
                OutputDebugStringA(("Name: " + nameStr).c_str());
                OutputDebugStringA(("Path: " + pathStr).c_str());
                Project p;
                p.name = std::wstring(nameStr.begin(), nameStr.end());
                p.path = std::wstring(pathStr.begin(), pathStr.end());
                projects.push_back(std::move(p));
            } catch (const std::exception& convErr) {
                MessageBoxA(nullptr, convErr.what(), "String Conversion Error", MB_ICONERROR | MB_OK);
            }
        }
    } catch (const std::exception& e) {
        MessageBoxA(nullptr, e.what(), "JSON Parse Error", MB_ICONERROR | MB_OK);
        return;
    }

    g_projects.reserve(projects.size());
    for (const auto& p : projects) {
        ProjectResolved resolved{p, FindWorkspace(p.path)};
        std::wstring label = p.name;
        if (resolved.workspace.has_value()) {
            label += L"  [" + FileNameOnly(*resolved.workspace) + L"]";
        } else {
            label += L"  [no workspace file found]";
        }
        SendMessageW(list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(label.c_str()));
        g_projects.push_back(std::move(resolved));
    }
    if (!g_projects.empty()) {
        SendMessageW(list, LB_SETCURSEL, 0, 0);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        CreateWindowW(L"STATIC",
                      L"Select a project to open in VS Code:",
                      WS_CHILD | WS_VISIBLE,
                      16,
                      16,
                      540,
                      22,
                      hwnd,
                      nullptr,
                      nullptr,
                      nullptr);

        CreateWindowW(L"LISTBOX",
                      nullptr,
                      WS_CHILD | WS_VISIBLE | LBS_NOTIFY | WS_BORDER | WS_VSCROLL,
                      16,
                      42,
                      540,
                      180,
                      hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(kListId)),
                      nullptr,
                      nullptr);

        CreateWindowW(L"BUTTON",
                      L"Open",
                      WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
                      380,
                      236,
                      80,
                      30,
                      hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(kOpenButtonId)),
                      nullptr,
                      nullptr);

        CreateWindowW(L"BUTTON",
                      L"Quit",
                      WS_CHILD | WS_VISIBLE,
                      476,
                      236,
                      80,
                      30,
                      hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(kQuitButtonId)),
                      nullptr,
                      nullptr);

        CreateWindowW(L"BUTTON",
                      L"Refresh",
                      WS_CHILD | WS_VISIBLE,
                      16,
                      236,
                      80,
                      30,
                      hwnd,
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(kRefreshButtonId)),
                      nullptr,
                      nullptr);

        PopulateProjects(hwnd);
        return 0;
    }

    case WM_COMMAND: {
        const int controlId = LOWORD(wParam);
        const int notifyCode = HIWORD(wParam);

        if (controlId == kOpenButtonId) {
            OpenSelectedProject(hwnd);
            return 0;
        }

        if (controlId == kQuitButtonId) {
            PostQuitMessage(0);
            return 0;
        }

        if (controlId == kRefreshButtonId) {
            PopulateProjects(hwnd);
            return 0;
        }

        if (controlId == kListId && notifyCode == LBN_DBLCLK) {
            OpenSelectedProject(hwnd);
            return 0;
        }

        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    default:
        break;
    }

    return DefWindowProcW(hwnd, message, wParam, lParam);
}

} // namespace

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    const wchar_t className[] = L"ProjectOpenerWindowClass";

    WNDCLASSW wc{};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);

    if (!RegisterClassW(&wc)) {
        MessageBoxW(nullptr, L"Failed to register window class.", L"Startup Error", MB_ICONERROR | MB_OK);
        return 1;
    }
    HWND hwnd = CreateWindowExW(
        0,
        className,
        L"Project Launcher",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        590,
        320,
        nullptr,
        nullptr,
        hInstance,
        nullptr);

    if (hwnd == nullptr) {
        MessageBoxW(nullptr, L"Failed to create main window.", L"Startup Error", MB_ICONERROR | MB_OK);
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}
