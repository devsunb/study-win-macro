#pragma once
#include "EventList.h"

class CDlgEditKeyboard : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditKeyboard)

public:
	CDlgEditKeyboard(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditKeyboard();

	enum { IDD = IDD_DLG_EDIT_KEYBOARD };

	sMacroItem _item;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboMessage();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
