#pragma once
#include "EventList.h"

class CDlgEditMouse : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditMouse)

public:
	CDlgEditMouse(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditMouse();

	enum { IDD = IDD_DLG_EDIT_MOUSE };

	sMacroItem _item;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboMessage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
