#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


class CInactiveWinMacroApp : public CWinAppEx
{
public:
	CInactiveWinMacroApp();

	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

extern CInactiveWinMacroApp theApp;