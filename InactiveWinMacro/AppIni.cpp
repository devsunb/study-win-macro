#include "stdafx.h"
#include "AppIni.h" 


CAppIni::CAppIni(const char *inipath)
: CIni(inipath)
{
	LoadData();
}

CAppIni::~CAppIni()
{
	SaveData();
}

void CAppIni::LoadData()
{
	TransferAllData (false);
}

void CAppIni::SaveData()
{
	TransferAllData (true);
}

void CAppIni::TransferAllData (bool bSave)
{
	Transfer (bSave, "options",				(DWORD &)options,		0x0);

	Transfer (bSave, "macroFileName",		macroFileName,			MAX_PATH,	"macro.iwm");

	Transfer (bSave, "windowSx",			windowSx,				0);
	Transfer (bSave, "windowSy",			windowSy,				0);

	Transfer (bSave, "macroSelected",		macroSelected,			0);
	Transfer (bSave, "eventTimerPeriod",	eventTimerPeriod,		33);

	Transfer (bSave, "recMinMouseDistance",	recMinMouseDistance,	10);
	Transfer (bSave, "recMinTimeInterval",	recMinTimeInterval,		100);
}


CAppIni g_ini("InactiveWinMacro.ini");

