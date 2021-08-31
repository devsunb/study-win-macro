#pragma once
#include "Ini.h"


struct sBitOptions {
	DWORD ALWAYS_ON_TOP		: 1;
	DWORD TRAY_AT_STARTUP	: 1;
//	DWORD MACRO_RUN_LOG		: 1;
	DWORD MACRO_RUN_NOTIFY	: 1;
	DWORD REC_KEYBD			: 1;
	DWORD REC_MOUSE_BT		: 1;
	DWORD REC_MOUSE_POS		: 1;
	DWORD REC_TIME			: 1;
};


class CAppIni: public CIni
{
public: 
	sBitOptions options;

	char	macroFileName[MAX_PATH+1];

	int		windowSx;
	int		windowSy;

	int		macroSelected;
	int		eventTimerPeriod;

	int		recMinMouseDistance;
	int		recMinTimeInterval;

public:
	CAppIni(const char *ini_path);
	virtual ~CAppIni();

	void LoadData();
	void SaveData();

private:
	void TransferAllData (bool bSave);
};

extern CAppIni g_ini;
