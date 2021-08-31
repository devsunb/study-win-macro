#pragma once
#include "EventList.h"

class CDlgEditDelay : public CDialog
{
	DECLARE_DYNAMIC(CDlgEditDelay)

public:
	CDlgEditDelay(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditDelay();

	enum { IDD = IDD_DLG_EDIT_DELAY };

	sMacroItem _item;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
