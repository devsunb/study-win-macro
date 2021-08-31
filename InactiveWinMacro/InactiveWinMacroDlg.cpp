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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	_mmtms._hWndTarget = m_hWnd;

	// 타이틀 바(캡션바)에서 종료버튼을 비활성화
	//CMenu *pMenu = GetSystemMenu(FALSE);
	//pMenu->RemoveMenu(SC_CLOSE, MF_BYCOMMAND);
	
	// 트레이 아이콘을 만듭니다.
	m_hIcon2 = LoadIcon (AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_MAINFRAME2));
	TrayIconCreate(m_hWnd, m_hIcon, "키보드/마우스 매크로 프로그램 - 더블클릭 하세요.");

	// 윈도우의 위치를 복원한다.
	DlgInScreen ();
	SetWindowPos (NULL, g_ini.windowSx, g_ini.windowSy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// 윈도우의 각종 컨트롤을 설정한다.
	SetDlgItemText (IDC_EDIT_MACRO_FILE, g_ini.macroFileName);
	SetDlgItemText (IDC_STATIC_VERSION, GetVersionInfo(NULL, "ProductVersion"));

	_staticHomepage.SetURL("http://blog.daum.net/pg365/287");
	_staticHomepage.SetToolTipText("비활성 매크로 프로그램 웹페이지 방문");
	_staticHomepage.SetLinkCursor(::LoadCursor(0, MAKEINTRESOURCE(IDC_HAND)));

	// 만일 트레이 아이콘으로 최소화 설정이 있으면, 트레이 아이콘으로 최소화 한다.
	if (g_ini.options.TRAY_AT_STARTUP) {
		ShowWindow (SW_SHOWMINIMIZED);
		PostMessage (WM_SHOWWINDOW, FALSE, SW_OTHERUNZOOM);
	}

	// 파일에서 매크로를 불러와 리스트 컨트롤에 표시한다.
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

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.
void CInactiveWinMacroDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);

		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width()  - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else {
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
		AfxMessageBox ("실행할 매크로를 선택하세요.");
		return;
	}

	_runMacro = &g_macros[sel];
	
	if (!_runMacro->start ()) {
		_runMacro = NULL;

		AfxMessageBox ("대상 윈도우를 찾을 수 없습니다.");
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
	_snprintf (text, 256, "%d개 매크로 기록됨.", g_macros.size());
	text[256] = '\0';

	SetDlgItemText (IDC_STATIC_REC_COUNT2, text);
}

void CInactiveWinMacroDlg::OnLbnDblclkListMacros()
{
	OnBnClickedButtonEditMacro();
}

void CInactiveWinMacroDlg::OnBnClickedButtonMacroFile()
{
	// 새로운 매크로를 선택하기 전에, 현재 매크로가 변경되었을 때는 사용자에게 알림
	if (_macro_changed) {
		int ret = AfxMessageBox ("매크로가 변경되었습니다. 저장할까요?", MB_YESNO|MB_ICONQUESTION);
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
	// 기능 사용안함.
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
		int ret = AfxMessageBox ("매크로가 변경되었습니다. 저장할까요?", MB_YESNO|MB_ICONQUESTION);
		if (ret == IDYES) {
			OnBnClickedButtonSave ();
		}
	}

	// 여기서 종료
	OnOK ();
}
