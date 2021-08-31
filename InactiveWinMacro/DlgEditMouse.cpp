#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEditMouse.h"
#include "Common.h"
#include <assert.h>

// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static DWORD _option = 0;
static DWORD _pos    = MAKELONG (0, 0);


IMPLEMENT_DYNAMIC(CDlgEditMouse, CDialog)

CDlgEditMouse::CDlgEditMouse(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditMouse::IDD, pParent)
{
}

CDlgEditMouse::~CDlgEditMouse()
{
}

void CDlgEditMouse::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditMouse, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_MESSAGE, &CDlgEditMouse::OnCbnSelchangeComboMessage)
	ON_BN_CLICKED(IDOK, &CDlgEditMouse::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditMouse::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgEditMouse::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == IT_NONE) {
		SetWindowText ("마우스 이벤트 추가");
		// 기본값 설정
		_item.type             = IT_MOUSE;
		_item.mouse.wi.message = WM_MOUSEMOVE;
		_item.mouse.option     = _option;
		_item.mouse.pos        = _pos;
	}
	else {
		SetWindowText ("마우스 이벤트 편집");
	}

	int sel = 0;
	SendDlgItemMessage (IDC_COMBO_MESSAGE, WM_SETREDRAW, 0, 0);
	for (int i=4; i<g_nWindowMessage; i++) {
		SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_INSERTSTRING, i-4,  (LPARAM)g_windowMessage[i].str);
		if (_item.mouse.wi.message == g_windowMessage[i].message) sel = i-4;
	}
	SendDlgItemMessage (IDC_COMBO_MESSAGE, WM_SETREDRAW, 1, 0);

	SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_SETCURSEL, sel);
	sel += 4;

	CheckDlgButton (IDC_CHECK_CONTROL,	(_item.mouse.option & MK_CONTROL)  ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_LBUTTON,	(_item.mouse.option & MK_LBUTTON)  ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_MBUTTON,	(_item.mouse.option & MK_MBUTTON)  ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_RBUTTON,	(_item.mouse.option & MK_RBUTTON)  ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_SHIFT,	(_item.mouse.option & MK_SHIFT)    ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_XBUTTON1,	(_item.mouse.option & MK_XBUTTON1) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton (IDC_CHECK_XBUTTON2,	(_item.mouse.option & MK_XBUTTON2) ? BST_CHECKED : BST_UNCHECKED);

	sel = 0;
	SendDlgItemMessage (IDC_COMBO_HIT_TEST, WM_SETREDRAW, 0, 0);
	for (int i=0; i<g_nNcHitTest; i++) {
		SendDlgItemMessage (IDC_COMBO_HIT_TEST, CB_INSERTSTRING, i,  (LPARAM)g_ncHitTest[i].name);
		if (LOWORD (_item.mouse.option) == g_ncHitTest[i].code) sel = i;
	}
	SendDlgItemMessage (IDC_COMBO_HIT_TEST, WM_SETREDRAW, 1, 0);

	SendDlgItemMessage (IDC_COMBO_HIT_TEST, CB_SETCURSEL, sel);

	SetDlgItemInt  (IDC_EDIT_WHEEL_DELTA, GET_WHEEL_DELTA_WPARAM(_item.mouse.option), TRUE);
	SetDlgItemInt  (IDC_EDIT_CURSOR_X,	GET_X_LPARAM(_item.mouse.pos), TRUE);
	SetDlgItemInt  (IDC_EDIT_CURSOR_Y,	GET_Y_LPARAM(_item.mouse.pos), TRUE);

	OnCbnSelchangeComboMessage();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditMouse::OnCbnSelchangeComboMessage()
{
	int sel = SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_GETCURSEL);
	assert (sel >= 0);

	sel += 4;

	bool enable = sel < 17;
	GetDlgItem(IDC_CHECK_CONTROL )->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_LBUTTON )->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_MBUTTON )->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_RBUTTON )->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_SHIFT 	 )->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_XBUTTON1)->EnableWindow (enable);
	GetDlgItem(IDC_CHECK_XBUTTON2)->EnableWindow (enable);
	GetDlgItem(IDC_COMBO_HIT_TEST)->EnableWindow (!enable);

	enable = g_windowMessage[sel].message == WM_MOUSEWHEEL;
	GetDlgItem(IDC_EDIT_WHEEL_DELTA)->EnableWindow (enable);
}

void CDlgEditMouse::OnBnClickedOk()
{
	int sel = SendDlgItemMessage (IDC_COMBO_MESSAGE, CB_GETCURSEL);
	assert (sel >= 0);

	sel += 4;

	int state = 0;
	if (sel < 17) {
		// WM_xxx 메시지일 때
		state |= (IsDlgButtonChecked (IDC_CHECK_CONTROL)  == BST_CHECKED) ? MK_CONTROL  : 0; 
		state |= (IsDlgButtonChecked (IDC_CHECK_LBUTTON)  == BST_CHECKED) ? MK_LBUTTON  : 0; 
		state |= (IsDlgButtonChecked (IDC_CHECK_MBUTTON)  == BST_CHECKED) ? MK_MBUTTON  : 0; 
		state |= (IsDlgButtonChecked (IDC_CHECK_RBUTTON)  == BST_CHECKED) ? MK_RBUTTON  : 0; 
		state |= (IsDlgButtonChecked (IDC_CHECK_SHIFT)    == BST_CHECKED) ? MK_SHIFT    : 0;   
		state |= (IsDlgButtonChecked (IDC_CHECK_XBUTTON1) == BST_CHECKED) ? MK_XBUTTON1 : 0;
		state |= (IsDlgButtonChecked (IDC_CHECK_XBUTTON2) == BST_CHECKED) ? MK_XBUTTON2 : 0;
	}
	else {
		// WM_NCxxx 메시지일 때
		state = SendDlgItemMessage (IDC_COMBO_HIT_TEST, CB_GETCURSEL);
		state = g_ncHitTest[state].code;
	}

	int delta = 0;
	if (g_windowMessage[sel].message == WM_MOUSEWHEEL) {
		delta = GetDlgItemInt  (IDC_EDIT_WHEEL_DELTA, 0, TRUE);
	}
	_item.mouse.option = MAKELONG (state, delta);

	int cursor_x = GetDlgItemInt  (IDC_EDIT_CURSOR_X, 0, TRUE);
	int cursor_y = GetDlgItemInt  (IDC_EDIT_CURSOR_Y, 0, TRUE);
	_item.mouse.pos = MAKELONG (cursor_x, cursor_y);

	_option = _item.mouse.option;
	_pos    = _item.mouse.pos;

	OnOK();
}

void CDlgEditMouse::OnBnClickedCancel()
{
	OnCancel();
}
