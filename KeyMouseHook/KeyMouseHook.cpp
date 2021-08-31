#include "stdafx.h"
#include "Commons.h"
#define IN_DLL
#include "KeyMouseHook.h"

// ���� ������ ����Ǵ� ������ ��� �ʱ�ȭ �Ǿ�� �Ѵ�.
#pragma data_seg(".shared")

HINSTANCE g_hInstance = NULL;
HHOOK g_hKeybdHook = NULL;
HHOOK g_hMouseHook = NULL;

HWND  g_eventRecvWnd = NULL;

MOUSEHOOKSTRUCT g_mouseHookStruct = { { 0, 0}, 0, 0 };

#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")

bool ent = false;

LRESULT CALLBACK KeybdProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		SendMessage (g_eventRecvWnd, WM_PROCESS_KEYBD, wParam, lParam);
	}
	return CallNextHookEx (g_hKeybdHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		// ����ü�� ������(lParam) ���� ���, �ٸ� ���μ������� ������ �Ұ����ϴ�.
		// �׷��Ƿ� ���� �޸𸮷� �ű�� �� �޸��� �����͸� ������ �Ѵ�.
		memcpy ((void *)&g_mouseHookStruct, (void *)lParam, sizeof(MOUSEHOOKSTRUCT));
		SendMessage (g_eventRecvWnd, WM_PROCESS_MOUSE, wParam, (LPARAM)&g_mouseHookStruct);
	}
	return CallNextHookEx (g_hMouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK CbtProc (int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		// ����ü�� ������(lParam) ���� ���, �ٸ� ���μ������� ������ �Ұ����ϴ�.
		// �׷��Ƿ� ���� �޸𸮷� �ű�� �� �޸��� �����͸� ������ �Ѵ�.
		memcpy ((void *)&g_mouseHookStruct, (void *)lParam, sizeof(MOUSEHOOKSTRUCT));
		SendMessage (g_eventRecvWnd, WM_PROCESS_MOUSE, wParam, (LPARAM)&g_mouseHookStruct);
	}
	return CallNextHookEx (g_hMouseHook, nCode, wParam, lParam);
}


DLL_DECL int UninstallKeybdMouseHook()
{
	if (g_hKeybdHook) {
		UnhookWindowsHookEx(g_hKeybdHook);
		g_hKeybdHook = NULL;
	}
	if (g_hMouseHook) {
		UnhookWindowsHookEx(g_hMouseHook);
		g_hMouseHook = NULL;
	}
	return 0;  
}

DLL_DECL int InstallKeybdMouseHook(DWORD hThreadID, HWND hWnd, HWND eventRecvWnd)
{
	UninstallKeybdMouseHook();

	g_eventRecvWnd = eventRecvWnd;

	g_hMouseHook = SetWindowsHookEx(WH_MOUSE,    MouseProc, g_hInstance, hThreadID);
	g_hKeybdHook = SetWindowsHookEx(WH_KEYBOARD, KeybdProc, g_hInstance, hThreadID);

	return 0;
}

DLL_DECL DWORD TestValue()
{
	return 0;
}
