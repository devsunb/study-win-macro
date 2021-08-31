#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgConfig.h"
#include "AppIni.h"


IMPLEMENT_DYNAMIC(CDlgConfig, CDialog)

CDlgConfig::CDlgConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgConfig::IDD, pParent)
{
}

CDlgConfig::~CDlgConfig()
{
}

void CDlgConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgConfig, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgConfig::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgConfig::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgConfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	CheckDlgButton (IDC_CHECK_ALWAYS_ON_TOP,		g_ini.options.ALWAYS_ON_TOP    ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_TRAY_AT_STARTUP,		g_ini.options.TRAY_AT_STARTUP  ? BST_CHECKED : BST_UNCHECKED);
//	CheckDlgButton (IDC_CHECK_MACRO_LOG,			g_ini.options.MACRO_RUN_LOG    ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_MACRO_NOTIFY,			g_ini.options.MACRO_RUN_NOTIFY ? BST_CHECKED : BST_UNCHECKED);

	if (g_ini.eventTimerPeriod < 1) g_ini.eventTimerPeriod = 1;
	SetDlgItemInt (IDC_EDIT_EVENT_MIN_DELAY, g_ini.eventTimerPeriod);

	return TRUE;
}

void CDlgConfig::OnBnClickedOk()
{
	g_ini.options.ALWAYS_ON_TOP    = IsDlgButtonChecked (IDC_CHECK_ALWAYS_ON_TOP);
	g_ini.options.TRAY_AT_STARTUP  = IsDlgButtonChecked (IDC_CHECK_TRAY_AT_STARTUP);
//	g_ini.options.MACRO_RUN_LOG    = IsDlgButtonChecked (IDC_CHECK_MACRO_LOG);
	g_ini.options.MACRO_RUN_NOTIFY = IsDlgButtonChecked (IDC_CHECK_MACRO_NOTIFY);

	g_ini.eventTimerPeriod = GetDlgItemInt (IDC_EDIT_EVENT_MIN_DELAY);
	if (g_ini.eventTimerPeriod < 1) g_ini.eventTimerPeriod = 1;

	OnOK();
}

void CDlgConfig::OnBnClickedCancel()
{
	OnCancel();
}
