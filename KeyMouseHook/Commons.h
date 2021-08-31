#pragma once

inline ULONG ProcIDFromWnd(HWND hWnd) 
{  
	// 윈도우 핸들로 프로세스 아이디 얻기
	ULONG idProc;  
	GetWindowThreadProcessId( hWnd, &idProc );  
	return idProc;  
}

inline long GetKeyDownState ()
{
	long option = 0;
	
	if (0x8000 & GetAsyncKeyState (VK_LBUTTON))  option |= MK_LBUTTON;	// 0x0001 - The left mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_RBUTTON))  option |= MK_RBUTTON;	// 0x0002 - The right mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_SHIFT))    option |= MK_SHIFT;		// 0x0004 - The SHIFT key is down.
	if (0x8000 & GetAsyncKeyState (VK_CONTROL))  option |= MK_CONTROL;	// 0x0008 - The CTRL key is down.
	if (0x8000 & GetAsyncKeyState (VK_MBUTTON))  option |= MK_MBUTTON;	// 0x0010 - The middle mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_XBUTTON1)) option |= MK_XBUTTON1;	// 0x0020 - Windows 2000/XP: The first X button is down.
	if (0x8000 & GetAsyncKeyState (VK_XBUTTON2)) option |= MK_XBUTTON2;	// 0x0040 - Windows 2000/XP: The second X button is down.

	return option;
}

inline long GetNcHitTest (HWND hWnd, int x, int y)
{
	return SendMessage (hWnd, WM_NCHITTEST, 0, MAKELONG(x, y));
}