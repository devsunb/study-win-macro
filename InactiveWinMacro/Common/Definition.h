#pragma once

inline int BOUND (int value, int lo, int hi)
{
	if (value < lo) value = lo;
	if (value > hi) value = hi;
	return value;
}

struct sProcessInfo 
{
	HWND hWnd;
	DWORD processID;
	DWORD threadID;

	sProcessInfo () :
		hWnd(0), processID(0), threadID(0) {}

	sProcessInfo (HWND hWnd_, DWORD processID_, DWORD threadID_) :
		hWnd(hWnd_), processID(processID_), threadID(threadID_) {}
};

struct sWindowProperty 
{
	HWND hWnd;
	int  sx, sy;
	int  dx, dy;
	int  cntChild;
	char caption[256+1];
	char className[256+1];
	DWORD style;
	DWORD exStyle;

	void GetWindowProperty (CWnd *wnd);
	bool Compare (const sWindowProperty &wp, bool ignoreHandle = false);
	int  ScoredCompare (const sWindowProperty &wp);
};
