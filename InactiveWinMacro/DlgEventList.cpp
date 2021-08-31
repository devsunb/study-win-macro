#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEventList.h"
#include "EventList.h"
#include "Definition.h"
#include "virtual_key.h"
#include "DlgEditDelay.h"
#include "DlgEditKeyboard.h"
#include "DlgEditMessage.h"
#include "DlgEditMouse.h"


IMPLEMENT_DYNAMIC(CDlgEventList, CDialog)

CDlgEventList::CDlgEventList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEventList::IDD, pParent)
{
}

CDlgEventList::~CDlgEventList()
{
}

void CDlgEventList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_WINDOWS, _listWindows);
	DDX_Control(pDX, IDC_LIST_EVENTS, _listEvents);
}


BEGIN_MESSAGE_MAP(CDlgEventList, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_INS_KEYBD, &CDlgEventList::OnBnClickedButtonInsKeybd)
	ON_BN_CLICKED(IDC_BUTTON_INS_MOUSE, &CDlgEventList::OnBnClickedButtonInsMouse)
	ON_BN_CLICKED(IDC_BUTTON_INS_DELAY, &CDlgEventList::OnBnClickedButtonInsDelay)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &CDlgEventList::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgEventList::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_UP, &CDlgEventList::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &CDlgEventList::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDOK, &CDlgEventList::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgEventList::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgEventList::OnInitDialog()
{
	CDialog::OnInitDialog();

	//assert (_eventList);
	SetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name);

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

	_listEvents.SetExtendedStyle(_listEvents.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER /* | LVS_EX_SINGLEROW*/ );
	_listEvents.SetImageList(&_eventsImage, LVSIL_SMALL);
	_listEvents.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	
	_listEvents.InsertColumn(0, "메시지",	LVCFMT_LEFT, 150);
	_listEvents.InsertColumn(1, "파라미터",	LVCFMT_LEFT, 225);
	_listEvents.InsertColumn(2, "#",		LVCFMT_LEFT, 30);

	_listEvents.SetMacroItems (&_macro->_eventList);
	// _listEvents.DeleteAllItems ();
	UpdateListEvents ();
	UpdateListWindows ();

	SetTextRecCount();

	SetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT, _macro->repeat);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CDlgEventList::SetTextRecCount()
{
	char text[256+1];

	_snprintf (text, 256, "%d개 항목이 기록됨", _macro->_eventList.size());
	text[256] = '\0';

	SetDlgItemText (IDC_STATIC_REC_COUNT, text);
}

void CDlgEventList::UpdateListWindows ()
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

void CDlgEventList::UpdateListEvents ()
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
		// Ensure that the last item is visible.
		int nCount = _listEvents.GetItemCount();
		if (nCount > 0) _listEvents.EnsureVisible(nCount-1, FALSE);
	}
}

int CDlgEventList::GetAddIndex ()
{
	int sel = _listEvents.GetLastSel ();
	if (0 <= sel) {
		return sel+1; // 만일 선택된 경우, 선택된 항목 뒤에다 추가하려고 1 증가
	}
	else {
		return _listEvents.GetItemCount ();
	}
}

void CDlgEventList::OnBnClickedButtonInsKeybd ()
{
	int sel = GetAddIndex ();

	CDlgEditKeyboard dlg;
	if (dlg.DoModal () == IDOK) {
		 _macro->_eventList.insert (_macro->_eventList.begin() + sel, dlg._item);

		 _listEvents.InsertItem  (sel, "");
		_listEvents.SetItemInfo (sel, dlg._item, true);

		SetTextRecCount();
		_listEvents.SetFocus ();
	}
}

void CDlgEventList::OnBnClickedButtonInsMouse ()
{
	int sel = GetAddIndex ();

	CDlgEditMouse dlg;
	if (dlg.DoModal () == IDOK) {
		 _macro->_eventList.insert (_macro->_eventList.begin() + sel, dlg._item);

		 _listEvents.InsertItem  (sel, "");
		_listEvents.SetItemInfo (sel, dlg._item, true);

		SetTextRecCount();
		_listEvents.SetFocus ();
	}
}

void CDlgEventList::OnBnClickedButtonInsDelay ()
{
	int sel = GetAddIndex ();

	CDlgEditDelay dlg;
	if (dlg.DoModal () == IDOK) {
		 _macro->_eventList.insert (_macro->_eventList.begin() + sel, dlg._item);

		_listEvents.InsertItem  (sel, "");
		_listEvents.SetItemInfo (sel, dlg._item, true);

		SetTextRecCount();
		_listEvents.SetFocus ();
	}
}

void CDlgEventList::OnBnClickedButtonEdit ()
{
	int sel = _listEvents.GetNextItem (-1, LVNI_SELECTED);
	if (sel >= 0) {
		sMacroItem &mi = _macro->_eventList[sel];

		if (mi.type == IT_KEY) {			// keyboard
			CDlgEditKeyboard dlg;
			dlg._item = mi;

			if (dlg.DoModal () == IDOK) {
				mi = dlg._item;
				_listEvents.SetItemInfo (sel, dlg._item, true);
			}
		}
		else if ((mi.type == IT_MOUSE) ||	// mouse
			(mi.type == IT_NC_MOUSE)) {		// nc mouse
			CDlgEditMouse dlg;
			dlg._item = mi;

			if (dlg.DoModal () == IDOK) {
				mi = dlg._item;
				_listEvents.SetItemInfo (sel, dlg._item, true);
			}
		}
		else if (mi.type == IT_DELAY) {		// delay
			CDlgEditDelay dlg;
			dlg._item = mi;

			if (dlg.DoModal () == IDOK) {
				mi = dlg._item;
				_listEvents.SetItemInfo (sel, dlg._item, true);
			}
		}
	}
}

void CDlgEventList::OnBnClickedButtonDelete ()
{
	_listEvents.KeyDelete ();

	SetTextRecCount();
	_listEvents.SetFocus ();
}

void CDlgEventList::OnBnClickedButtonUp ()
{
	_listEvents.KeyMoveUp ();
	_listEvents.SetFocus ();
}

void CDlgEventList::OnBnClickedButtonDown ()
{
	_listEvents.KeyMoveDown ();
	_listEvents.SetFocus ();
}

void CDlgEventList::OnBnClickedOk()
{
	if (_macro->_eventList.size() <= 1) {
		AfxMessageBox ("기록된 매크로가 없습니다.");
		return;
	}

	GetDlgItemText (IDC_EDIT_MACRO_NAME, _macro->name, 255);
	_macro->name[255] = '\0';

	_macro->repeat  = BOUND(GetDlgItemInt (IDC_EDIT_MACRO_REPEAT_COUNT), 0, 1000000);
	_macro->options = 0;

	OnOK();
}

void CDlgEventList::OnBnClickedCancel()
{
	OnCancel();
}
