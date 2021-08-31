#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "HyperLink.h"
#include "macro_def.h"
#include "MultimediaTimer.h"


#define WM_MM_TIMER	(WM_USER + 1250)

// Multimedia timer로 Macro 실행에 실시간성 확보
class CMmtMacros: public CMultimediaTimer 
{
public:
	virtual void OnTimer (UINT timerId, UINT msg) 
	{
		if (_hWndTarget) ::PostMessage (_hWndTarget, WM_MM_TIMER, 0, 0);
	}

	HWND _hWndTarget;
};


class CInactiveWinMacroDlg : public CDialog
{
public:
	CInactiveWinMacroDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_INACTIVEWINMACRO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	void UpdateListEvents ();
	void PlayKeybdMouse (DWORD dt);

private:
	CMmtMacros _mmtms;

	HICON m_hIcon;
	HICON m_hIcon2;

	DWORD _tp;
	CListBox _listMacros;
	CHyperLink _staticHomepage;

	sMacro *_runMacro;
	bool _macro_changed;

	void DlgInScreen ();
	void SetTextRecCount();
	void MacroRunNotify ();
	void EnableDlgItem(BOOL enable);
	void CheckAlwaysontop();

protected:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnLbnDblclkListMacros();
	afx_msg void OnBnClickedButtonMacroFile();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonRecMacro();
	afx_msg void OnBnClickedButtonAddMacro();
	afx_msg void OnBnClickedButtonEditMacro();
	afx_msg void OnBnClickedButtonCopyMacro();
	afx_msg void OnBnClickedButtonDeleteMacro();
	afx_msg void OnBnClickedButtonUp2();
	afx_msg void OnBnClickedButtonDown2();
	afx_msg void OnBnClickedButtonConfig();
	afx_msg void OnBnClickedQuit();
};
