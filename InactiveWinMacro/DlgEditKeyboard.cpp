#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEditKeyboard.h"
#include "Common.h"
#include "virtual_key.h"
#include <assert.h>

// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static long _key   = 0x30;
static long _option = 0;


IMPLEMENT_DYNAMIC(CDlgEditKeyboard, CDialog)

CDlgEditKeyboard::CDlgEditKeyboard(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditKeyboard::IDD, pParent)
{
}

CDlgEditKeyboard::~CDlgEditKeyboard()
{
}

void CDlgEditKeyboard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditKeyboard, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_MESSAGE, &CDlgEditKeyboard::OnCbnSelchangeComboMessage)
	ON_BN_CLICKED(IDOK, &CDlgEditKeyboard::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditKeyboard::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgEditKeyboard::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == IT_NONE) {
		SetWindowText ("키보드 이벤트 추가");
		// 기본값 설정
		_item.type             = IT_KEY;
		_item.keybd.wi.message = WM_KEYDOWN;
		_item.keybd.vk         = _key;
		_item.keybd.option     = _option;
	}
	else {
		SetWindowText ("키보드 이벤트 편집");
	}

	int sel = 0;
	SendDlgItemMessage (IDC_COMBO_MESSAGE, WM_SETREDRAW, 0, 0);
	for (int i=0; i<4; i++) {
		SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_INSERTSTRING, i,  (LPARAM)g_windowMessage[i].str);
		if (_item.keybd.wi.message == g_windowMessage[i].message) sel = i;
	}
	SendDlgItemMessage (IDC_COMBO_MESSAGE, WM_SETREDRAW, 1, 0);

	SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_SETCURSEL, sel);

	sel = 0;
	SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, CB_INITSTORAGE, 256, 100);
	SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, WM_SETREDRAW, 0, 0);
	for (int i=0; g_vk_list[i].vk >= 0; i++) {
		SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, CB_INSERTSTRING, i,  (LPARAM)g_vk_list[i].desc);
		if (_item.keybd.vk == g_vk_list[i].vk) sel = i;
	}
	SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, WM_SETREDRAW, 1, 0);

	SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, CB_SETCURSEL, sel);

	SetDlgItemInt  (IDC_EDIT_REPEAT_COUNT,		_item.keybd.s.repeatCount, FALSE);
	SetDlgItemInt  (IDC_EDIT_SCAN_CODE,			_item.keybd.s.scanCode, FALSE);
	CheckDlgButton (IDC_CHECK_EXTENDED_KEY,		_item.keybd.s.extendedKey ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_CONTEXT_KEY,		_item.keybd.s.contextCode ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage (IDC_COMBO_PREVIOUS_KEY, CB_SETCURSEL, _item.keybd.s.previousKey);
	CheckDlgButton (IDC_CHECK_TRANSITION_STATE, _item.keybd.s.transactionState ? BST_CHECKED : BST_UNCHECKED);

	OnCbnSelchangeComboMessage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditKeyboard::OnCbnSelchangeComboMessage()
{
	int sel = SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_GETCURSEL);
	assert (sel >= 0);

	int message = g_windowMessage[sel].message;

	if (message == WM_KEYUP || message == WM_SYSKEYUP) {
		// WM_KEYUP, WM_SYSKEYUP 메시지에서는 repeat count는 항상 1이다. (편집 불가)
		SetDlgItemInt  (IDC_EDIT_REPEAT_COUNT, 1, FALSE);
		GetDlgItem (IDC_EDIT_REPEAT_COUNT)->EnableWindow (FALSE);

		// WM_KEYUP, WM_SYSKEYUP 메시지에서는 previous key는 항상 down(1)이다. (편집 불가)
		SendDlgItemMessage (IDC_COMBO_PREVIOUS_KEY, CB_SETCURSEL, 1);
		GetDlgItem (IDC_COMBO_PREVIOUS_KEY)->EnableWindow (FALSE);

		CheckDlgButton (IDC_CHECK_TRANSITION_STATE, BST_CHECKED);
	}
	else {
		GetDlgItem (IDC_EDIT_REPEAT_COUNT)->EnableWindow (TRUE);
		GetDlgItem (IDC_COMBO_PREVIOUS_KEY)->EnableWindow (TRUE);

		CheckDlgButton (IDC_CHECK_TRANSITION_STATE, BST_UNCHECKED);
	}

	if (message == WM_SYSKEYDOWN || message == WM_SYSKEYUP) {
		CheckDlgButton (IDC_CHECK_CONTEXT_KEY, BST_CHECKED);
	}
	else {
		CheckDlgButton (IDC_CHECK_CONTEXT_KEY, BST_UNCHECKED);
	}

	// context code와 transaction state는 편집 불가
	// 선택한 메시지에 의해 결정된다.
	GetDlgItem (IDC_CHECK_CONTEXT_KEY)->EnableWindow (FALSE);
	GetDlgItem (IDC_CHECK_TRANSITION_STATE)->EnableWindow (FALSE);
}

void CDlgEditKeyboard::OnBnClickedOk()
{
	int sel = SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_GETCURSEL);
	assert (sel >= 0);

	_item.keybd.wi.message = g_windowMessage[sel].message;

	sel = SendDlgItemMessage (IDC_COMBO_VIRTUAL_KEY, CB_GETCURSEL);
	_item.keybd.vk = g_vk_list[sel].vk;

	_item.keybd.s.repeatCount		= GetDlgItemInt (IDC_EDIT_REPEAT_COUNT, 0, FALSE);
	_item.keybd.s.scanCode			= GetDlgItemInt (IDC_EDIT_SCAN_CODE,	0, FALSE);
	_item.keybd.s.extendedKey		= IsDlgButtonChecked (IDC_CHECK_EXTENDED_KEY) == BST_CHECKED ? 1 : 0;
	_item.keybd.s.contextCode		= IsDlgButtonChecked (IDC_CHECK_CONTEXT_KEY ) == BST_CHECKED ? 1 : 0;
	_item.keybd.s.previousKey		= SendDlgItemMessage (IDC_COMBO_PREVIOUS_KEY, CB_GETCURSEL);
	_item.keybd.s.transactionState	= IsDlgButtonChecked (IDC_CHECK_TRANSITION_STATE) == BST_CHECKED ? 1 : 0;

	_key    = _item.keybd.vk;
	_option = _item.keybd.option;

	OnOK();
}

void CDlgEditKeyboard::OnBnClickedCancel()
{
	OnCancel();
}
