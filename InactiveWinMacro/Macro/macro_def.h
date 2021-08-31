#pragma once
#include "EventList.h"
#include "WindowList.h"


class sMacro {
public:
	// 파일에 저장되는 변수들
	char name[256];
	DWORD repeat;
	DWORD options;

	CWindowList  _windowList;
	CEventList   _eventList;

public:
	// 매크로 실행시 필요한 변수들
	long index;
	long run_count;

	void init_runtime_value () 
	{
		index = 0;
		run_count = 0;
	}

public:
	sMacro ();
	sMacro (const sMacro &macro);
	sMacro &operator = (const sMacro &macro);

	void ReadyToEventRec (sProcessInfo &process)
	{
		_windowList.SetTargetProcess (process.hWnd, process.processID);
		_eventList.SetWindowList (&_windowList);
	}

	bool MacroStep (int dt);

	bool start (int index_ = 1) 
	{
		if (!_windowList.SetStartProcess ()) {
			return false;
		}
		_eventList.SetWindowList (&_windowList);

		init_runtime_value();
		index = index_;

		return true;
	}

	void stop () 
	{
		index = _eventList.size();
		run_count = 0;
	}

	bool is_running() 
	{
		return 0 <= index && index < (int)_eventList.size();
	}

};

extern vector<sMacro> g_macros;

