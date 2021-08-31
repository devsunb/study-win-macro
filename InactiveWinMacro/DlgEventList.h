#pragma once
#include "macro_def.h"
#include "ListCtrlEx.h"


class CDlgEventList : public CDialog
{
	DECLARE_DYNAMIC(CDlgEventList)

public:
	CDlgEventList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEventList();

	enum { IDD = IDD_DLG_EVENT };

	sMacro *_macro;

private:
	CImageList _windowsIcon;
	CListCtrl  _listWindows;

	CImageList _eventsImage;
	CListCtrlEx _listEvents;

	int GetAddIndex ();
	void UpdateListEvents ();
	void UpdateListWindows ();
	void SetTextRecCount();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonInsKeybd();
	afx_msg void OnBnClickedButtonInsMouse();
	afx_msg void OnBnClickedButtonInsDelay();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
