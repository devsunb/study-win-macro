#pragma once
#include "Common.h"
#include "Definition.h"
#include <vector>

using namespace std;

class CWindowList : public vector<sWindowProperty> {
public:
	CWindowList();

	void SetTargetProcess (HWND hWndTarget, DWORD processID);
	bool SetStartProcess ();
	void ResetWindowList ();
	int  FindAndSaveIfDiffWindow (CWnd *wnd);
	
	HWND GetTargetWindow ();
	CWnd *GetTargetWindow (int id, int order);
	CWnd *GetLastChildWindow (CWnd *wnd, long depth, long order[]);

private:
	HWND SearchMaxMatchedWnd (sWindowProperty &wp, bool doNotComparePID = false);

private:
	HWND  _hWndTarget;
	DWORD _targetProcessID;
};