#pragma once

HICON GetWindowsIcon (HWND hWnd);

// ----------------------------------------------------------------------------

struct sWindowMessage {
	int message;
	const char *str;
};

extern sWindowMessage g_windowMessage[];
extern int g_nWindowMessage;

// ----------------------------------------------------------------------------

struct sNcHitTest {
	int code;
	const char *name;
};

extern sNcHitTest g_ncHitTest[];
extern int g_nNcHitTest;

const char *GetncHitTestName (int code);

// ----------------------------------------------------------------------------

CString GetVersionInfo(HMODULE hLib, CString csEntry);





