#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEventRec.h"
#include "DlgProcessList.h"
#include "KeyMouseHook.h"
#include "Definition.h"
#include "AppIni.h"
#include "MultimediaTimer.h"
#include <math.h>


IMPLEMENT_DYNAMIC(CDlgEventRec, CDialog)

CDlgEventRec::CDlgEventRec(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEventRec::IDD, pParent)
{
	_macroRec = false;
}

CDlgEventRec::~CDlgEventRec()
{
}

void CDlgEventRec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WINDOWS, _listWindows);
	DDX_Control(pDX, IDC_LIST_EVENTS, _listEvents);
	DDX_Control(pDX, IDC_STATIC_PROCESS_ICON, _staticProcessIcon);
}


BEGIN_MESSAGE_MAP(CDlgEventRec, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CDlgEventRec::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEventRec::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_INSTALL_HOOK, &CDlgEventRec::OnBnClickedButtonInstallHook)
	ON_BN_CLICKED(IDC_BUTTON_UNINSTALL_HOOK, &CDlgEventRec::OnBnClickedButtonUninstallHook)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PROCESS, &CDlgEventRec::OnBnClickedButtonSelectProcess)
END_MESSAGE_MAP()


BOOL CDlgEventRec::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name);

	_staticProcessIcon.SetIcon (LoadIcon (AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_UNKNOWN)));
	SetDlgItemText (IDC_EDIT_PROCESS, "...");

	_windowsIcon.Create (16, 16, ILC_COLOR24|ILC_MASK, 10,	10);

	_listWindows.SetExtendedStyle(_listWindows.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER | LVS_EX_SINGLEROW);
	_listWindows.SetImageList(&_windowsIcon, LVSIL_SMALL);
	_listWindows.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	
	_listWindows.InsertColumn(0, "#",			LVCFMT_LEFT, 30);
	_listWindows.InsertColumn(1, "타이틀",		LVCFMT_LEFT, 170);
	_listWindows.InsertColumn(2, "위치, 크기",	LVCFMT_LEFT, 150);
	_listWindows.InsertColumn(3, "클래스",		LVCFMT_LEFT, 150);
	_listWindows.InsertColumn(4, "스타일",		LVCFMT_RIGHT, 80);
	_listWindows.InsertColumn(5, "확장스타일",	LVCFMT_RIGHT, 80);


	CBitmap cBit;
	cBit.LoadBitmap (IDB_BITMAP_EVENTS);
	_eventsImage.Create (16, 16, ILC_COLOR24|ILC_MASK, 10,	10);
	_eventsImage.Add (&cBit, RGB(255,255,255));
	cBit.DeleteObject();

	_listEvents.SetExtendedStyle (_listEvents.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER | LVS_EX_SINGLEROW);
	_listEvents.SetImageList (&_eventsImage,	LVSIL_SMALL);
	_listEvents.SetItemState (0, LVIS_SELECTED, LVIS_SELECTED);
	
	_listEvents.InsertColumn (0, "메시지",		LVCFMT_LEFT, 150);
	_listEvents.InsertColumn (1, "파라미터",	LVCFMT_LEFT, 225);
	_listEvents.InsertColumn (2, "#",			LVCFMT_LEFT, 30);

	SetTextRecCount ();

	CheckDlgButton (IDC_CHECK_REC_KEY,			(g_ini.options.REC_KEYBD     ) ? BST_CHECKED : BST_UNCHECKED);	// Keyboard 입력 기록
	CheckDlgButton (IDC_CHECK_REC_MOUSE_BT,		(g_ini.options.REC_MOUSE_BT  ) ? BST_CHECKED : BST_UNCHECKED);	// Mouse Button 입력 기록
	CheckDlgButton (IDC_CHECK_REC_MOUSE_POS,	(g_ini.options.REC_MOUSE_POS ) ? BST_CHECKED : BST_UNCHECKED);	// Mouse 위치 변화 기록
	CheckDlgButton (IDC_CHECK_TIME,				(g_ini.options.REC_TIME      ) ? BST_CHECKED : BST_UNCHECKED);	// Keyboard나 Mouse 이벤트간 시간 간격 기록
	
	g_ini.recMinMouseDistance = BOUND (g_ini.recMinMouseDistance, 1, 10000);
	g_ini.recMinTimeInterval  = BOUND (g_ini.recMinTimeInterval,  1, 10000);

	SetDlgItemInt (IDC_EDIT_MOVE, g_ini.recMinMouseDistance);
	SetDlgItemInt (IDC_EDIT_TIME, g_ini.recMinTimeInterval);

	SetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT, _macro->repeat);
	EnableWindowItem();

	SetTimer (1000, 33, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CDlgEventRec::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_PROCESS_KEYBD: 
		_macro->_eventList.ProcessKeybd (wParam, lParam); 
		return TRUE;
	case WM_PROCESS_MOUSE: 
		_macro->_eventList.ProcessMouse (wParam, lParam); 
		return TRUE;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}


void CDlgEventRec::OnBnClickedButtonSelectProcess()
{
	CDlgProcessList dlg;
	
	if (dlg.DoModal () == IDOK) {
		_selProcess = *dlg._selPi;
	
		_macro->ReadyToEventRec (_selProcess);

		// GetFocus() 함수를 올바르게 작동시키기 위해,
		// 키보드, 마우스 훅을 설치할 쓰레드를 현재 쓰레드에 연결한다. 
		DWORD fromId = GetCurrentThreadId();
		AttachThreadInput(fromId, _selProcess.threadID, TRUE);

		////////////////////////////////////////////////////////////////////////////////
		CWnd *wndTarget = CWnd::FromHandle (_selProcess.hWnd);

		_staticProcessIcon.SetIcon (GetWindowsIcon (_selProcess.hWnd));

		CString strCaption;
		wndTarget->GetWindowText (strCaption); 
		SetDlgItemText (IDC_EDIT_PROCESS, strCaption);
	}

	EnableWindowItem();
}

void CDlgEventRec::OnBnClickedButtonInstallHook()
{
	if (_selProcess.hWnd) {
		MacroRecStateChange (true);
		EnableWindowItem ();

		InstallKeybdMouseHook (_selProcess.threadID, _selProcess.hWnd, m_hWnd);
	}
}

void CDlgEventRec::OnBnClickedButtonUninstallHook()
{
	UninstallKeybdMouseHook ();
	MacroRecStateChange (false);
	EnableWindowItem ();
}

void CDlgEventRec::SetTextRecCount()
{
	char text[256+1];
	_snprintf (text, 256, "%d개 항목이 기록됨", _macro->_eventList.size());
	text[256] = '\0';

	SetDlgItemText (IDC_STATIC_REC_COUNT, text);
}


void CDlgEventRec::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		UpdateListWindows ();
		UpdateListEvents ();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgEventRec::UpdateListWindows ()
{
	int count = _listWindows.GetItemCount ();

	while ((int)_macro->_windowList.size() > count) {
		sWindowProperty &wp = _macro->_windowList[count];

		CString buff;
		buff.Format ("%d", count);
		_listWindows.InsertItem  (count, buff);
		_listWindows.SetItemData (count, (DWORD_PTR)wp.hWnd);
		_listWindows.SetItem     (count, 0, LVIF_IMAGE, NULL, count, 0, 0, 0);

		_listWindows.SetItemText (count, 1, wp.caption);
		buff.Format ("%d, %d - %d, %d", wp.sx, wp.sy, wp.dx, wp.dy);
		_listWindows.SetItemText (count, 2, buff);
		_listWindows.SetItemText (count, 3, wp.className);
		buff.Format ("%X", wp.style);
		_listWindows.SetItemText (count, 4, buff);
		buff.Format ("%X", wp.exStyle);
		_listWindows.SetItemText (count, 5, buff);

		HICON icon = GetWindowsIcon (wp.hWnd);
		_windowsIcon.Add (icon);

		count++;
	}
}

void CDlgEventRec::UpdateListEvents ()
{
	bool inserted = false;
	int count = _listEvents.GetItemCount ();

	while ((int)_macro->_eventList.size() > count) {
		sMacroItem &mi = _macro->_eventList[count];
		_listEvents.InsertItem  (count, pGetMessageStr (mi));
		_listEvents.SetItem     (count, 0, LVIF_IMAGE, NULL, mi.type, 0, 0, 0);
		_listEvents.SetItemText (count, 1, GetItemDescription (mi));
		_listEvents.SetItemText (count, 2, GetWinIdStr (mi));

		count++;
		inserted = true;
	}
	if (inserted) {
		int nCount = _listEvents.GetItemCount();
		if (nCount > 0) _listEvents.EnsureVisible(nCount-1, FALSE);

		SetTextRecCount ();
	}
}

void CDlgEventRec::EnableWindowItem()
{
	BOOL enable = !_macroRec;
	BOOL sp_enable = !!_selProcess.hWnd;

	GetDlgItem(IDC_EDIT_MACRO_NAME)			->EnableWindow (enable);
	// GetDlgItem(IDC_LIST_EVENTS)			->EnableWindow (enable);

	GetDlgItem(IDC_BUTTON_SELECT_PROCESS)	->EnableWindow (enable && !sp_enable);

	GetDlgItem(IDC_CHECK_REC_KEY)			->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_MOUSE_BT)		->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_REC_MOUSE_POS)		->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_TIME)				->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_MOVE)				->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_TIME)				->EnableWindow (enable);
	GetDlgItem(IDC_EDIT_MACRO_REPEAT_COUNT) ->EnableWindow (enable);

	GetDlgItem(IDC_BUTTON_INSTALL_HOOK)		->EnableWindow (sp_enable &&  enable);
	GetDlgItem(IDC_BUTTON_UNINSTALL_HOOK)	->EnableWindow (sp_enable && !enable);
}

void CDlgEventRec::MacroRecStateChange(bool rec)
{
	g_ini.options.REC_KEYBD		= (IsDlgButtonChecked (IDC_CHECK_REC_KEY)		== BST_CHECKED) ? 1 : 0;	// Keyboard 입력 기록
	g_ini.options.REC_MOUSE_BT	= (IsDlgButtonChecked (IDC_CHECK_REC_MOUSE_BT)	== BST_CHECKED) ? 1 : 0;	// Mouse Button 입력 기록
	g_ini.options.REC_MOUSE_POS = (IsDlgButtonChecked (IDC_CHECK_REC_MOUSE_POS)	== BST_CHECKED) ? 1 : 0;	// Mouse 위치 변화 기록
	g_ini.options.REC_TIME		= (IsDlgButtonChecked (IDC_CHECK_TIME)			== BST_CHECKED) ? 1 : 0;	// Keyboard나 Mouse 이벤트간 시간 간격 기록

	g_ini.recMinMouseDistance = GetDlgItemInt (IDC_EDIT_MOVE);
	g_ini.recMinTimeInterval  = GetDlgItemInt (IDC_EDIT_TIME);

	g_ini.recMinMouseDistance = BOUND (g_ini.recMinMouseDistance, 1, 10000);
	g_ini.recMinTimeInterval  = BOUND (g_ini.recMinTimeInterval,  1, 10000);

	_macroRec = rec;
}

void CDlgEventRec::OnBnClickedOk()
{
	if (!_selProcess.hWnd || _macro->_eventList.size() <= 1) {
		AfxMessageBox ("매크로를 기록할 대상 프로세서가 선택되어 있지 않거나 매크로가 기록되지 않았습니다.");
		return;
	}

	OnBnClickedButtonUninstallHook();

	GetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name, 255);
	_macro->name[255] = '\0';

	MacroRecStateChange (false);

	_macro->repeat  = BOUND(GetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT), 0, 1000000);
	_macro->options = 0;

	OnOK();
}

void CDlgEventRec::OnBnClickedCancel()
{
	OnBnClickedButtonUninstallHook();

	MacroRecStateChange (false);

	OnCancel();
}
