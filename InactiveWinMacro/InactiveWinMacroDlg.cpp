#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "InactiveWinMacroDlg.h"
#include "DlgEventList.h"
#include "DlgEventRec.h"
#include "DlgConfig.h"
#include "TrayIcon.h"
#include "AppIni.h"
#include "macro_def.h"
#include "macro_file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CInactiveWinMacroDlg::CInactiveWinMacroDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInactiveWinMacroDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_runMacro = NULL;
	_macro_changed = false;

	srand((DWORD)time(0)+(DWORD)getpid());
}

void CInactiveWinMacroDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MACROS, _listMacros);
	DDX_Control(pDX, IDC_STATIC_HOMEPAGE, _staticHomepage);
}

BEGIN_MESSAGE_MAP(CInactiveWinMacroDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CInactiveWinMacroDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CInactiveWinMacroDlg::OnBnClickedButtonStop)
	ON_LBN_DBLCLK(IDC_LIST_MACROS, &CInactiveWinMacroDlg::OnLbnDblclkListMacros)
	ON_BN_CLICKED(IDC_BUTTON_MACRO_FILE, &CInactiveWinMacroDlg::OnBnClickedButtonMacroFile)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CInactiveWinMacroDlg::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_REC_MACRO, &CInactiveWinMacroDlg::OnBnClickedButtonRecMacro)
	ON_BN_CLICKED(IDC_BUTTON_ADD_MACRO, &CInactiveWinMacroDlg::OnBnClickedButtonAddMacro)
	ON_BN_CLICKED(IDC_BUTTON_EDIT_MACRO, &CInactiveWinMacroDlg::OnBnClickedButtonEditMacro)
	ON_BN_CLICKED(IDC_BUTTON_COPY_MACRO, &CInactiveWinMacroDlg::OnBnClickedButtonCopyMacro)
	ON_BN_CLICKED(IDC_BUTTON_DELETE_MACRO, &CInactiveWinMacroDlg::OnBnClickedButtonDeleteMacro)
	ON_BN_CLICKED(IDC_BUTTON_UP2, &CInactiveWinMacroDlg::OnBnClickedButtonUp2)
	ON_BN_CLICKED(IDC_BUTTON_DOWN2, &CInactiveWinMacroDlg::OnBnClickedButtonDown2)
	ON_BN_CLICKED(IDC_BUTTON_CONFIG, &CInactiveWinMacroDlg::OnBnClickedButtonConfig)
	ON_BN_CLICKED(ID_QUIT, &CInactiveWinMacroDlg::OnBnClickedQuit)
END_MESSAGE_MAP()


void CInactiveWinMacroDlg::DlgInScreen ()
{
	RECT rect;
	GetWindowRect (&rect);

	int cx = GetSystemMetrics(SM_CXVIRTUALSCREEN) - (rect.right - rect.left) - 1;
	int cy = GetSystemMetrics(SM_CYVIRTUALSCREEN) - (rect.bottom - rect.top) - 1;

	if (g_ini.windowSx < 0) g_ini.windowSx = 0;
	if (g_ini.windowSx > cx) g_ini.windowSx = cx;

	if (g_ini.windowSy < 0) g_ini.windowSy = 0;
	if (g_ini.windowSy > cy) g_ini.windowSy = cy;
}

BOOL CInactiveWinMacroDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	_mmtms._hWndTarget = m_hWnd;

	// Ÿ��Ʋ ��(ĸ�ǹ�)���� �����ư�� ��Ȱ��ȭ
	//CMenu *pMenu = GetSystemMenu(FALSE);
	//pMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);
	
	// Ʈ���� �������� ����ϴ�.
	m_hIcon2 = LoadIcon (AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_MAINFRAME2));
	TrayIconCreate(m_hWnd, m_hIcon, "Ű����/���콺 ��ũ�� ���α׷� - ����Ŭ�� �ϼ���.");

	// �������� ��ġ�� �����Ѵ�.
	DlgInScreen ();
	SetWindowPos (NULL, g_ini.windowSx, g_ini.windowSy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// �������� ���� ��Ʈ���� �����Ѵ�.
	SetDlgItemText (IDC_EDIT_MACRO_FILE, g_ini.macroFileName);
	SetDlgItemText (IDC_STATIC_VERSION, GetVersionInfo(NULL, "ProductVersion"));

	_staticHomepage.SetURL("http://blog.daum.net/pg365/287");
	_staticHomepage.SetToolTipText("��Ȱ�� ��ũ�� ���α׷� �������� �湮");
	_staticHomepage.SetLinkCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));

	// ���� Ʈ���� ���������� �ּ�ȭ ������ ������, Ʈ���� ���������� �ּ�ȭ �Ѵ�.
	if (g_ini.options.TRAY_AT_STARTUP) {
		ShowWindow (SW_SHOWMINIMIZED);
		PostMessage (WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
	}

	// ���Ͽ��� ��ũ�θ� �ҷ��� ����Ʈ ��Ʈ�ѿ� ǥ���Ѵ�.
	LoadMacros (g_ini.macroFileName);

	_listMacros.ResetContent();
	for (int i=0; i<(int)g_macros.size(); ++i) {
		_listMacros.InsertString (i, g_macros[i].name);
	}
	_listMacros.SetCurSel(g_ini.macroSelected);

	EnableDlgItem (TRUE);

	SetTextRecCount();
	CheckAlwaysontop();

	SetTimer (1000, 500, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.
void CInactiveWinMacroDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width()  - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CInactiveWinMacroDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CInactiveWinMacroDlg::CheckAlwaysontop()
{
	SetWindowPos ((g_ini.options.ALWAYS_ON_TOP)? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CInactiveWinMacroDlg::OnDestroy()
{
	RECT rectWnd;

	GetWindowRect (&rectWnd);

	g_ini.windowSx = rectWnd.left;
	g_ini.windowSy = rectWnd.top;

	TrayIconClose();

	CDialog::OnDestroy();
}

void CInactiveWinMacroDlg::EnableDlgItem (BOOL enable)
{
	GetDlgItem (IDC_EDIT_MACRO_FILE)    ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_MACRO_FILE)  ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_SAVE)        ->EnableWindow (enable);

	GetDlgItem (IDC_BUTTON_REC_MACRO)   ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_ADD_MACRO)   ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_EDIT_MACRO)  ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_COPY_MACRO)  ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_DELETE_MACRO)->EnableWindow (enable);

	GetDlgItem (IDC_BUTTON_PLAY)        ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_STOP)        ->EnableWindow (!enable);

	GetDlgItem (IDC_BUTTON_UP2)         ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_DOWN2)       ->EnableWindow (enable);
	GetDlgItem (IDC_BUTTON_CONFIG)      ->EnableWindow (enable);
	GetDlgItem (ID_QUIT)                ->EnableWindow (enable);
}

void CInactiveWinMacroDlg::MacroRunNotify ()
{
	static int count = 0;

	if (_runMacro && _runMacro->is_running ()) {
		count++;
		if (count%2) {
			SetIcon (m_hIcon2, FALSE);
			SetIcon (m_hIcon2, TRUE);
		}
		else {
			SetIcon (m_hIcon, FALSE);
			SetIcon (m_hIcon, TRUE);
		}
	}
	else {
		if (count%2) {
			SetIcon (m_hIcon, FALSE);
			SetIcon (m_hIcon, TRUE);
		}
	}
}

void CInactiveWinMacroDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		if (g_ini.options.MACRO_RUN_NOTIFY) {
			MacroRunNotify ();
		}
	}

	CDialog::OnTimer(nIDEvent);
}

LRESULT CInactiveWinMacroDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_MM_TIMER: {
		DWORD t = CMultimediaTimer::GetTime ();
		PlayKeybdMouse (t - _tp);
		_tp = t;
		return TRUE;
	}
	case WM_TRAY_ICON:
		if(LOWORD(lParam) == 0x203){
			// Double click
			PostMessage (WM_SHOWWINDOW, TRUE, SW_OTHERZOOM);
			SetForegroundWindow();
		}
		return 0;
	case WM_SYSCOMMAND:
		if(wParam==SC_MINIMIZE){
			PostMessage (WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
		}
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

void CInactiveWinMacroDlg::PlayKeybdMouse (DWORD dt)
{
	if (!_runMacro->MacroStep (dt)) {
		_mmtms.Stop ();
		EnableDlgItem (TRUE);
	}
}

void CInactiveWinMacroDlg::OnBnClickedButtonPlay()
{
	int sel = _listMacros.GetCurSel();
	if (sel < 0) {
		AfxMessageBox ("������ ��ũ�θ� �����ϼ���.");
		return;
	}

	_runMacro = &g_macros[sel];
	
	if (!_runMacro->start ()) {
		_runMacro = NULL;

		AfxMessageBox ("��� �����츦 ã�� �� �����ϴ�.");
		return;
	}

	EnableDlgItem (FALSE);

	_mmtms.Start (g_ini.eventTimerPeriod);
}

void CInactiveWinMacroDlg::OnBnClickedButtonStop()
{
	if (_runMacro && _runMacro->is_running ()) {
		_runMacro->stop ();
		_mmtms.Stop ();
	}

	EnableDlgItem (TRUE);
}

void CInactiveWinMacroDlg::SetTextRecCount()
{
	char text[256+1];
	_snprintf (text, 256, "%d�� ��ũ�� ��ϵ�.", g_macros.size());
	text[256] = '\0';

	SetDlgItemText (IDC_STATIC_REC_COUNT2, text);
}

void CInactiveWinMacroDlg::OnLbnDblclkListMacros()
{
	OnBnClickedButtonEditMacro();
}

void CInactiveWinMacroDlg::OnBnClickedButtonMacroFile()
{
	// ���ο� ��ũ�θ� �����ϱ� ����, ���� ��ũ�ΰ� ����Ǿ��� ���� ����ڿ��� �˸�
	if (_macro_changed) {
		int ret = AfxMessageBox ("��ũ�ΰ� ����Ǿ����ϴ�. �����ұ��?", MB_YESNO|MB_ICONQUESTION);
		if (ret == IDYES) {
			OnBnClickedButtonSave ();
		}
	}

	///////////////////////////////////////////////////////////////////////////////
	char szFilter[] = "Macro file (*.iwm)|*.iwm|All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, g_ini.macroFileName, OFN_HIDEREADONLY, szFilter);
	
	if(IDOK == dlg.DoModal()) {
		char curPath[MAX_PATH+1] = "";
		GetCurrentDirectory (MAX_PATH, curPath);
		curPath[MAX_PATH] = '\0';

		CString fileNameNew = "";

		if (strnicmp(curPath, (LPCSTR)dlg.GetPathName (), strlen(curPath)) == 0) {
			fileNameNew = (LPCSTR)dlg.GetPathName () + strlen(curPath) + 1;
		}
		else {
			fileNameNew = dlg.GetPathName();
		}

		if (dlg.GetFileExt ().GetLength () == 0) {
			fileNameNew += ".iwm";
		}

		g_macros.clear ();
		LoadMacros ((LPCSTR)fileNameNew);

		_listMacros.ResetContent();
		for (int i=0; i<(int)g_macros.size(); ++i) {
			_listMacros.InsertString (i, g_macros[i].name);
		}
		_listMacros.SetCurSel(0);

		strncpy (g_ini.macroFileName, (LPCSTR)fileNameNew, MAX_PATH);
		g_ini.macroFileName[MAX_PATH] = '\0';
		SetDlgItemText (IDC_EDIT_MACRO_FILE, g_ini.macroFileName);
	}
}

void CInactiveWinMacroDlg::OnBnClickedButtonSave()
{
	GetDlgItemText (IDC_EDIT_MACRO_FILE, g_ini.macroFileName, MAX_PATH);
	g_ini.macroFileName[MAX_PATH] = '\0';
	
	SaveMacro (g_ini.macroFileName);
	_macro_changed = false;
}

void CInactiveWinMacroDlg::OnBnClickedButtonRecMacro()
{
	CDlgEventRec dlg;

	sMacro temp;
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		int count = _listMacros.GetCount ();

		_listMacros.InsertString (count, temp.name);
		_listMacros.SetCurSel (count);

		g_macros.push_back (temp);
		_macro_changed = true;
	}

	SetTextRecCount();
}

void CInactiveWinMacroDlg::OnBnClickedButtonAddMacro()
{
	// ��� ������.
	/*
	CDlgEventList dlg;

	sMacro temp;
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		int count = _listMacros.GetCount ();

		_listMacros.InsertString (count, temp.name);
		_listMacros.SetCurSel (count);

		g_macros.push_back (temp);
		_macro_changed = true;
	}

	SetTextRecCount();
	*/
}

void CInactiveWinMacroDlg::OnBnClickedButtonEditMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	CDlgEventList dlg;

	sMacro temp = g_macros[sel];
	dlg._macro = &temp;

	if (dlg.DoModal () == IDOK) {
		_listMacros.DeleteString (sel);
		_listMacros.InsertString (sel, temp.name);
		_listMacros.SetCurSel (sel);

		g_macros[sel] = *dlg._macro;
		_macro_changed = true;
	}
}

void CInactiveWinMacroDlg::OnBnClickedButtonCopyMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	int count = _listMacros.GetCount ();
	sMacro &macro = g_macros[sel];

	_listMacros.InsertString (count, macro.name);
	_listMacros.SetCurSel (count);

	g_macros.push_back (macro);
	_macro_changed = true;

	SetTextRecCount();
}

void CInactiveWinMacroDlg::OnBnClickedButtonDeleteMacro()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < 0) return;

	_listMacros.DeleteString (sel);
	_listMacros.SetCurSel (sel);

	g_macros.erase (g_macros.begin() + sel);
	_macro_changed = true;

	SetTextRecCount();
}

void CInactiveWinMacroDlg::OnBnClickedButtonUp2()
{
	int sel = _listMacros.GetCurSel ();
	if (1 <= sel) {
		_listMacros.DeleteString (sel);
		_listMacros.InsertString (sel - 1, g_macros[sel].name);
		_listMacros.SetCurSel (sel - 1);

		std::swap (g_macros[sel], g_macros[sel - 1]);
		_macro_changed = true;
	}
}

void CInactiveWinMacroDlg::OnBnClickedButtonDown2()
{
	int sel = _listMacros.GetCurSel ();
	if (sel < _listMacros.GetCount () - 1) { 
		_listMacros.DeleteString (sel);
		_listMacros.InsertString (sel + 1, g_macros[sel].name);
		_listMacros.SetCurSel (sel + 1);

		std::swap (g_macros[sel], g_macros[sel + 1]);
		_macro_changed = true;
	}
}

void CInactiveWinMacroDlg::OnBnClickedButtonConfig()
{
	CDlgConfig dlg;

	if (dlg.DoModal () == IDOK) {
		CheckAlwaysontop();
	}
}

void CInactiveWinMacroDlg::OnBnClickedQuit()
{
	g_ini.macroSelected = _listMacros.GetCurSel ();

	if (_macro_changed) {
		int ret = AfxMessageBox ("��ũ�ΰ� ����Ǿ����ϴ�. �����ұ��?", MB_YESNO|MB_ICONQUESTION);
		if (ret == IDYES) {
			OnBnClickedButtonSave ();
		}
	}

	// ���⼭ ����
	OnOK ();
}
