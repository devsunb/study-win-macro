#pragma once
#include "Definition.h"
#include <vector>

using namespace std;


class CDlgProcessList : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcessList)

public:
	CDlgProcessList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProcessList();

	enum { IDD = IDD_DLG_PROCESS };

	sProcessInfo *_selPi;

private:
	void UpdateProcessListView ();
	void UpdateListProcess ();
	int  GetProcessList ();

private:
	HWND _selProcess;

	CImageList _processIcon;
	CListCtrl  _listProcess;
	
	vector<sProcessInfo> _processList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
