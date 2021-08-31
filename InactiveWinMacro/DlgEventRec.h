#pragma once
#include "macro_def.h"
#include "afxwin.h"
#include "DlgProcessList.h"


class CDlgEventRec : public CDialog
{
	DECLARE_DYNAMIC(CDlgEventRec)

public:
	CDlgEventRec(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEventRec();

	sMacro *_macro;

	enum { IDD = IDD_DLG_EVENT_REC };

private:
	void SetTextRecCount ();
	void MacroRecStateChange (bool rec);
	void EnableWindowItem ();
	void UpdateListWindows ();
	void UpdateListEvents ();

private:
	CImageList _windowsIcon;
	CListCtrl  _listWindows;

	CImageList _eventsImage;
	CListCtrl  _listEvents;

	CStatic    _staticProcessIcon;

	sProcessInfo _selProcess;

	bool _macroRec;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonInstallHook();
	afx_msg void OnBnClickedButtonUninstallHook();
	afx_msg void OnBnClickedButtonSelectProcess();
};
