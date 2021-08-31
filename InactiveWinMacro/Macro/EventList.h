#pragma once
#include <vector>

using namespace std;


#define MAX_ORDER	32

struct sWindowInfo {
	HWND  hWnd;
	ULONG message;
	long  winid;
	long  depth;
	long  order[MAX_ORDER];
};


struct sKeybdEvent {
	sWindowInfo wi;

	DWORD vk;
	union {
		DWORD option;
		struct {
			DWORD repeatCount		: 16;
			DWORD scanCode			: 8;
			DWORD extendedKey		: 1;
			DWORD notUsed			: 4;
			DWORD contextCode		: 1;
			DWORD previousKey		: 1;
			DWORD transactionState	: 1;
		} s;
	};
};

struct sMouseEvent {
	sWindowInfo wi;

	DWORD option;
	DWORD pos;
};

struct sDelayItem {
	DWORD options;
	DWORD delay;
};

enum eItemType { IT_NONE = 0, IT_KEY, IT_MOUSE, IT_NC_MOUSE, IT_DELAY };

struct sMacroItem {
	eItemType type;

	union {
		sKeybdEvent keybd;
		sMouseEvent mouse;
		sDelayItem  delay;
	};

	sMacroItem () : type (IT_NONE) { }
};


class CWindowList;

class CEventList : public vector<sMacroItem> {
public:
	CEventList();

	void SetWindowList (CWindowList *winList) { _winList = winList; }

	void ResetEventList ();
	int  SendKeybdMouse (int index, int dt);

	void ProcessMouse (WPARAM wParam, LPARAM lParam);
	void ProcessKeybd (WPARAM wParam, LPARAM lParam);

private:
	int  MouseDistance (int px, int py);
	void InsertDelay ();
	
	void GetWindowsPath (vector<CWnd *> &wndStack, CWnd *wnd);
	void BuildWindowsDepthInfo (const vector<CWnd *> &wndStack, sWindowInfo &wi);

private:
	CWindowList *_winList;

	DWORD _eventRecvTime;
	long  _eventDelay;
};

extern const char *pGetMessageStr (sMacroItem &mi);
extern CString GetItemDescription (const sMacroItem &mi);
extern CString GetWinIdStr (const sMacroItem &mi);