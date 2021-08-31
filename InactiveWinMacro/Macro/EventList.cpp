#include "stdafx.h"
#include "EventList.h"
#include "WindowList.h"
#include "virtual_key.h"
#include "Common.h"
#include "MultimediaTimer.h"
#include <math.h>
#include <assert.h>
#include "../AppIni.h"


inline eItemType GetMessageType (int message)
{
	if (WM_KEYFIRST <= message && message <= WM_KEYLAST) {
		return IT_KEY;
	}
	else if (WM_MOUSEFIRST <= message && message <= WM_MOUSELAST) {
		return IT_MOUSE;
	}
	else if (WM_NCMOUSEMOVE	<= message && message <= WM_NCXBUTTONDBLCLK) {
		return IT_NC_MOUSE;
	}
	else {
		return IT_NONE;	// Unknown
	}
}

CEventList::CEventList() :
	_eventRecvTime(0), _eventDelay(0)
{
	clear ();
	reserve (10000);
}

void CEventList::ResetEventList ()
{
	// 파일로부터 데이터를 읽은 후 불필요한 데이터를 삭제한다.

	assert (_winList);

	_eventRecvTime = 0;
	_eventDelay    = 0;
}

int CEventList::SendKeybdMouse (int index, int dt)
{
	if (0 <= index && index < (int)size()) {
		sMacroItem &mi = at(index);

		if (mi.type == IT_KEY) {
			sKeybdEvent &key = mi.keybd;

			CWnd *wnd = _winList->GetTargetWindow (key.wi.winid, key.wi.order[0]);
			if (!wnd) return index + 1;  // 대상 윈도우를 찾지 못하면 이벤트를 지나간다.

			wnd = _winList->GetLastChildWindow (wnd, key.wi.depth, key.wi.order);
			if (!wnd) return index + 1; // 대상 윈도우에서 차일드를 찾지 못해도 이벤트를 지나간다.

			wnd->PostMessage (key.wi.message, key.vk, key.option);
			index++;
		}
		else if (mi.type == IT_MOUSE || mi.type == IT_NC_MOUSE) {
			sMouseEvent &mou = mi.mouse;

			CWnd *wnd = _winList->GetTargetWindow (mou.wi.winid, mou.wi.order[0]);
			if (!wnd) return index + 1;  // 대상 윈도우를 찾지 못하면 이벤트를 지나간다.

			wnd = _winList->GetLastChildWindow (wnd, mou.wi.depth, mou.wi.order);
			if (!wnd) return index + 1; // 대상 윈도우에서 차일드를 찾지 못해도 이벤트를 지나간다.

			wnd->PostMessage (mou.wi.message, mou.option, mou.pos);
			index++;
		}
		else if (mi.type == IT_DELAY) { // delay
			if (_eventDelay == 0) {
				_eventDelay = mi.delay.delay;
			}
			else {
				_eventDelay -= dt;
				if (_eventDelay <= 0) {
					_eventDelay = 0;
					index++;
				}
			}
		}
		else {
			//TRACE ("Undefined message: %s\n", pGetMessageStr (mi));
		}
		return index;
	}
	return -1;
}

inline bool IsBidir (CWnd *parent, CWnd *child)
{
	// 자식과 부모 윈도우가 서로를 가르키고 있는지 체크한다.
	if (parent) {
		for (parent = parent->GetWindow (GW_CHILD); parent; parent = parent->GetWindow (GW_HWNDNEXT)) {
			if (parent == child) {
				return true;
			}
		}
	}
	return false;
}

void CEventList::GetWindowsPath (vector<CWnd *> &wndStack, CWnd *wnd)
{
	// 트리 구조의 윈도우를 리프노드부터 루트까지 탐색
	wndStack.clear ();
	wndStack.reserve (MAX_ORDER);

	HWND hWndDesktop = ::GetDesktopWindow();
	CWnd *wndChild = 0;
	for ( ; wnd; wnd = wnd->GetParent ()) {
		if (wnd->m_hWnd == hWndDesktop) break;
		if (wndChild && !IsBidir(wnd, wndChild)) break;

		wndStack.push_back (wnd);
		wndChild = wnd;
	}
}

inline int FindSiblingIndex (CWnd *wnd, HWND hWndTarget)
{
	CWnd *wndThis = wnd;

	int count = 0;
	for ( ; wnd; wnd = wnd->GetWindow(GW_HWNDNEXT)) {
		if (wnd->m_hWnd == hWndTarget) return count;
		count--;
	}
	count = 1;
	wnd = wndThis->GetWindow(GW_HWNDPREV);
	for ( ; wnd; wnd = wnd->GetWindow(GW_HWNDPREV)) {
		if (wnd->m_hWnd == hWndTarget) return count;
		count++;
	}
	TRACE ("윈도우 이웃 탐색 실패\n");
	return -1000;
}

void CEventList::BuildWindowsDepthInfo (const vector<CWnd *> &wndStack, sWindowInfo &wi)
{
	HWND hWndTarget = _winList->GetTargetWindow ();

	int n = (int)wndStack.size();
	CWnd *wnd = wndStack[--n];
	wi.winid = _winList->FindAndSaveIfDiffWindow (wnd);

	int depth = 0;
	wi.order[depth++] = FindSiblingIndex (wnd, hWndTarget);

	while (n > 0 && depth < MAX_ORDER) {
		CWnd *wnd = wndStack[--n];

		int order = 0;
		for ( ; wnd; wnd = wnd->GetNextWindow (GW_HWNDPREV)) order++;
		wi.order[depth++] = order;
	}
	wi.depth = depth;
}

int CEventList::MouseDistance (int px, int py)
{
	if (size() > 0) {
		sMacroItem &mi = at(size()-1);
		
		if ((mi.type == IT_MOUSE || mi.type == IT_NC_MOUSE) && 
			(mi.mouse.wi.message == WM_MOUSEMOVE || mi.mouse.wi.message == WM_NCMOUSEMOVE)) {
			long mx = GET_X_LPARAM(mi.mouse.pos); 
			long my = GET_Y_LPARAM(mi.mouse.pos); 

			int  dx = mx - px;
			int  dy = my - py;

			return (int)sqrt((double)dx*dx + dy*dy);
		}
	}
	return 1000000;
}

void CEventList::InsertDelay ()
{
	DWORD tc = CMultimediaTimer::GetTime();
	if (size() > 0) {
		long dt = tc - _eventRecvTime;
		if (dt >= g_ini.recMinTimeInterval) {
			sMacroItem mi;
			mi.type = IT_DELAY;
			mi.delay.delay   = dt;
			mi.delay.options = 0;
	
			push_back (mi);
		}
	}
	_eventRecvTime = tc;
}

void CEventList::ProcessKeybd (WPARAM wParam, LPARAM lParam)
{
	/*
	wParam - Specifies the virtual-key code of the key that generated the keystroke message.
	lParam - Specifies the repeat count, scan code, extended-key flag, context code, 
			 previous key-state flag, and transition-state flag. For more information about 
			 the lParam parameter, see Keystroke Message Flags. This parameter can be one or 
			 more of the following values. 
		0-15 	Specifies the repeat count. The value is the number of times the keystroke 
				is repeated as a result of the user's holding down the key.
		16-23	Specifies the scan code. The value depends on the OEM.
		24		Specifies whether the key is an extended key, such as a function key or a key 
				on the numeric keypad. The value is 1 if the key is an extended key; otherwise, 
				it is 0.
		25-28	Reserved.
		29		Specifies the context code. The value is 1 if the ALT key is down; otherwise, 
				it is 0.
		30		Specifies the previous key state. The value is 1 if the key is down before the 
				message is sent; it is 0 if the key is up.
		31		Specifies the transition state. The value is 0 if the key is being pressed and 
				1 if it is being released.
	*/

	// 키보드가 입력되고 있는 윈도우 핸들을 얻으려면 focus 된 윈도우 핸들을 얻으면 된다.
	HWND hWnd = GetFocus ();
	if (!hWnd) { TRACE ("키 입력에서 Focused 윈도우를 얻는데 실패\n"); return; }

	sMacroItem mi;
	mi.type = IT_KEY;
	mi.keybd.wi.hWnd = hWnd;
	mi.keybd.wi.message = (lParam & 0x20000000) ?
		((lParam & 0x80000000) ? WM_SYSKEYUP : WM_SYSKEYDOWN) :
		((lParam & 0x80000000) ? WM_KEYUP    : WM_KEYDOWN) ;
	mi.keybd.vk = (DWORD)wParam;
	mi.keybd.option = (DWORD)lParam;

	// 트리 구조의 윈도우를 리프노드부터 루트까지 탐색
	vector<CWnd *> wndStack;
	GetWindowsPath (wndStack, CWnd::FromHandle (hWnd));

	// 루트부터 다시 역추적
	int n = (int)wndStack.size();
	if (n > 0) {
		BuildWindowsDepthInfo (wndStack, mi.keybd.wi);

		if (g_ini.options.REC_KEYBD) {
			if (g_ini.options.REC_TIME) {
				InsertDelay ();
			}

			push_back (mi);
		}
	}
}

inline long GetKeyDownState ()
{
	long option = 0;
	
	if (0x8000 & GetAsyncKeyState (VK_LBUTTON))  option |= MK_LBUTTON;	// 0x0001 - The left mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_RBUTTON))  option |= MK_RBUTTON;	// 0x0002 - The right mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_SHIFT))    option |= MK_SHIFT;	// 0x0004 - The SHIFT key is down.
	if (0x8000 & GetAsyncKeyState (VK_CONTROL))  option |= MK_CONTROL;	// 0x0008 - The CTRL key is down.
	if (0x8000 & GetAsyncKeyState (VK_MBUTTON))  option |= MK_MBUTTON;	// 0x0010 - The middle mouse button is down.
	if (0x8000 & GetAsyncKeyState (VK_XBUTTON1)) option |= MK_XBUTTON1;	// 0x0020 - Windows 2000/XP: The first X button is down.
	if (0x8000 & GetAsyncKeyState (VK_XBUTTON2)) option |= MK_XBUTTON2;	// 0x0040 - Windows 2000/XP: The second X button is down.

	return option;
}

void CEventList::ProcessMouse (WPARAM wParam, LPARAM lParam)
{
	/*
	wParam - Specifies the identifier of the mouse message. 
	lParam - Pointer to a MOUSEHOOKSTRUCT structure. 

	typedef struct {
		POINT pt;				// Specifies a POINT structure that contains the x- and y-coordinates of the cursor, in screen coordinates. 
		HWND hWnd;				// Handle to the window that will receive the mouse message corresponding to the mouse event. 
		UINT wHitTestCode;		// Specifies the hit-test value. For a list of hit-test values, see the description of the WM_NCHITTEST message. 
		ULONG_PTR dwExtraInfo;	// Specifies extra information associated with the message. 
	} MOUSEHOOKSTRUCT;
	*/
	MOUSEHOOKSTRUCT *ms = (MOUSEHOOKSTRUCT *)lParam;

	CWnd *wnd = CWnd::FromHandle (ms->hwnd);
	if (!wnd) { TRACE ("마우스 입력에서 대상 윈도우를 얻는데 실패\n"); return; }

	eItemType type = GetMessageType (wParam);

	int dx = 0, dy = 0;
	if (type != IT_NC_MOUSE) {
		CRect rect;
		wnd->GetClientRect (&rect);
		wnd->ClientToScreen (&rect);
		dx = rect.left;
		dy = rect.top;
	}

	int mx = ms->pt.x - dx;
	int my = ms->pt.y - dy;

	sMacroItem mi;
	mi.type = type;
	mi.mouse.wi.hWnd = ms->hwnd;
	mi.mouse.wi.message = (ULONG)wParam;
	mi.mouse.option = (type == IT_MOUSE) ? GetKeyDownState() : ms->wHitTestCode;
	mi.mouse.pos = MAKELONG (mx, my);

	// 트리 구조의 윈도우를 리프노드부터 루트까지 탐색
	vector<CWnd *> wndStack;
	GetWindowsPath (wndStack, CWnd::FromHandle (ms->hwnd));

	// 루트부터 다시 역추적
	int n = (int)wndStack.size();
	if (n > 0) {
		BuildWindowsDepthInfo (wndStack, mi.mouse.wi);
		
		if ((wParam == WM_MOUSEMOVE) || (wParam == WM_NCMOUSEMOVE)) {
			if (g_ini.options.REC_MOUSE_POS && MouseDistance (mx, my) >= g_ini.recMinMouseDistance) {
				if (g_ini.options.REC_TIME) {
					InsertDelay ();
				}
				push_back (mi);
			}
		}
		else {
			if (g_ini.options.REC_MOUSE_BT) {
				if (g_ini.options.REC_TIME) {
					InsertDelay ();
				}
				push_back (mi);
			}
		}
	}
}

const char *pGetMessageStr (sMacroItem &mi)
{
	if (mi.type == IT_DELAY) {
		return "Delay";
	}
	else if (mi.type == IT_KEY) {
		for (int i=0; i<g_nWindowMessage; i++) {
			if (g_windowMessage[i].message == mi.keybd.wi.message) {
				return g_windowMessage[i].str;
			}
		}
	}
	else if (mi.type == IT_MOUSE || mi.type == IT_NC_MOUSE) {
		for (int i=0; i<g_nWindowMessage; i++) {
			if (g_windowMessage[i].message == mi.keybd.wi.message) {
				return g_windowMessage[i].str;
			}
		}
	}
	
	return "(Unknown)";
}


///////////////////////////////////////////////////////////////////////////////

const char *pGetKeyStateStr (long state)
{
	int n = 0;
	static char str[1024];
	str[0] = '\0';

	if (state & MK_LBUTTON ) n += _snprintf (str + n, 1024 - n, "%s ", "MK_LBUTTON");
	if (state & MK_MBUTTON ) n += _snprintf (str + n, 1024 - n, "%s ", "MK_MBUTTON");
	if (state & MK_RBUTTON ) n += _snprintf (str + n, 1024 - n, "%s ", "MK_RBUTTON");
	if (state & MK_XBUTTON1) n += _snprintf (str + n, 1024 - n, "%s ", "MK_XBUTTON1");
	if (state & MK_XBUTTON2) n += _snprintf (str + n, 1024 - n, "%s ", "MK_XBUTTON2");
	if (state & MK_CONTROL ) n += _snprintf (str + n, 1024 - n, "%s ", "MK_CONTROL");
	if (state & MK_SHIFT   ) n += _snprintf (str + n, 1024 - n, "%s ", "MK_SHIFT");

	return str;
}

CString GetItemDescription (const sMacroItem &mi)
{
	CString param;

	if (mi.type == IT_KEY) { // keyboard
		const sKeybdEvent &key = mi.keybd;
		param.Format ("%-4s  %8X", pGetVkName (key.vk), key.option);
	}
	else if (mi.type == IT_MOUSE) { // mouse
		const sMouseEvent &mou = mi.mouse;
		if (mi.mouse.wi.message == WM_MOUSEWHEEL) {
			param.Format ("(%4d, %4d) %d %s", 
				(int)GET_X_LPARAM(mou.pos), (int)GET_Y_LPARAM(mou.pos), 
				(int)GET_WHEEL_DELTA_WPARAM(mou.option), 
				pGetKeyStateStr(GET_KEYSTATE_WPARAM(mou.option)));
		}
		else {
			param.Format ("(%4d, %4d) %s", 
				(int)GET_X_LPARAM(mou.pos), (int)GET_Y_LPARAM(mou.pos), 
				pGetKeyStateStr(GET_KEYSTATE_WPARAM(mou.option)));
		}
	}
	else if (mi.type == IT_NC_MOUSE) { // nc mouse
		const sMouseEvent &mou = mi.mouse;
		param.Format ("(%4d, %4d) %s", 
			(int)GET_X_LPARAM(mou.pos), (int)GET_Y_LPARAM(mou.pos), 
			GetncHitTestName(mou.option));
	}
	else if (mi.type == IT_DELAY) { // delay
		const sDelayItem &di = mi.delay;
		param.Format ("%d (ms)", di.delay);
	}
	return param;
}

CString GetWinIdStr (const sMacroItem &mi)
{
	CString winIdStr("");

	if (mi.type == IT_KEY){
		winIdStr.Format ("%d", mi.keybd.wi.winid);
	}
	else if (mi.type == IT_MOUSE || mi.type == IT_NC_MOUSE ){
		winIdStr.Format ("%d", mi.mouse.wi.winid);
	}
	return winIdStr;
}

