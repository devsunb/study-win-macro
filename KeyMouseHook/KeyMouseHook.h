#pragma once

#ifdef IN_DLL
#define DLL_DECL extern "C" __declspec(dllexport)
#else
#define DLL_DECL extern "C" __declspec(dllimport)
#endif

#define WM_PROCESS_KEYBD		(WM_USER + 1011)
#define WM_PROCESS_MOUSE		(WM_USER + 1012)

DLL_DECL int UninstallKeybdMouseHook();
DLL_DECL int InstallKeybdMouseHook(DWORD hThreadID, HWND hWnd, HWND eventRecvWnd);
DLL_DECL DWORD TestValue();
