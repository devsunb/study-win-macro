#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEditDelay.h"
#include "Definition.h"

// 프로그램이 실행되는 동안 대화상자 아이템에 
// 설정된 값을 저장하고 있는 변수들을 static으로 선언
static long _delay   = 10;
static long _options = 0;


IMPLEMENT_DYNAMIC(CDlgEditDelay, CDialog)

CDlgEditDelay::CDlgEditDelay(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditDelay::IDD, pParent)
{
}

CDlgEditDelay::~CDlgEditDelay()
{
}

void CDlgEditDelay::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditDelay, CDialog)
	ON_BN_CLICKED(IDOK, &CDlgEditDelay::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEditDelay::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgEditDelay::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (_item.type == IT_NONE) {
		SetWindowText ("시간 지연 추가");
		// 기본값 설정
		_item.type = IT_DELAY;
		_item.delay.delay   = _delay;
		_item.delay.options = _options;
	}
	else {
		SetWindowText ("시간 지연 편집");
	}

	SetDlgItemInt (IDC_EDIT_DELAY, _item.delay.delay, TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditDelay::OnBnClickedOk()
{
	_delay = BOUND (GetDlgItemInt (IDC_EDIT_DELAY, NULL, TRUE), 1, 24*60*60*1000);
	_item.delay.delay = _delay;

	OnOK();
}

void CDlgEditDelay::OnBnClickedCancel()
{
	OnCancel();
}
