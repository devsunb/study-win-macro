#include "stdafx.h"
#include "macro_def.h"
#include "ini.h"
#include "../AppIni.h"


vector<sMacro> g_macros;

sMacro::sMacro ()
{
	init_runtime_value();

	strcpy (name, "Macro");
	repeat = 1;
	options = 0;
}

sMacro::sMacro (const sMacro &macro)
{
	strncpy (name, macro.name, 255);
	name[255]   = '\0';
	repeat      = macro.repeat;
	options     = macro.options;
	_windowList	= macro._windowList;
	_eventList	= macro._eventList;

	init_runtime_value ();
}

sMacro &sMacro::operator = (const sMacro &macro) 
{
	if (this != &macro) {
		strncpy (name, macro.name, 255);
		name[255]   = '\0';
		repeat      = macro.repeat;
		options     = macro.options;
		_windowList	= macro._windowList;
		_eventList	= macro._eventList;

		init_runtime_value ();
	}
	return *this;
}


bool sMacro::MacroStep (int dt)
{
	if (0 < index && index < (int)_eventList.size()) {
		index = _eventList.SendKeybdMouse (index, dt);

		if ((int)_eventList.size() <= index) {
			run_count++;

			int repeat_count = repeat;
			if (repeat_count == 0 || run_count < repeat_count) {
				index = 1; // 처음부터 재시작
			}
			else {
				stop ();
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}


