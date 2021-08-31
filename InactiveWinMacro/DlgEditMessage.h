#pragma once
#include "EventList.h"

class CDlgEditMessage : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditMessage)

public:
	CDlgEditMessage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditMessage();

	enum { IDD = IDD_DLG_EDIT_MESSAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
