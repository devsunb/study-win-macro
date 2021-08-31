#include "stdafx.h"
#include "ListCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CString GetItemDescription (const sMacroItem &item);
extern CString GetWinIdStr (const sMacroItem &mi);

CListCtrlEx::CListCtrlEx ()
{
}

void CListCtrlEx::DoDataExchange(CDataExchange* pDX)
{
	CListCtrl::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()


void CListCtrlEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool ctrl = (GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0;

	if (!ctrl) CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CListCtrlEx::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//TRACE("%X ", nChar);
	bool ctrl = (GetAsyncKeyState (VK_CONTROL) & 0x8000) != 0;

	if      (ctrl && nChar == 'A') KeyCtrlA ();
	else if (ctrl && nChar == 'C') KeyCtrlC ();
	else if (ctrl && nChar == 'X') KeyCtrlX ();
	else if (ctrl && nChar == 'V') KeyCtrlV ();
	else if (nChar == VK_DELETE)   KeyDelete ();
	//else if (nChar == VK_PRIOR)    KeyMoveUp ();
	//else if (nChar == VK_NEXT)     KeyMoveDown ();
	
	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CListCtrlEx::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

int CListCtrlEx::GetFirstSel ()
{
	return GetNextItem (-1, LVNI_SELECTED);
}

int CListCtrlEx::GetLastSel ()
{
	int last = -1;
	int sel = GetNextItem (-1, LVNI_SELECTED);

	while (sel >= 0) {
		last = sel;
		sel = GetNextItem (sel, LVNI_SELECTED);
	}
	return last;
}

void CListCtrlEx::SetItemInfo (int index, sMacroItem &mi, bool sel)
{
	SetItemText (index, 0, pGetMessageStr (mi));
	SetItemText (index, 1, GetItemDescription (mi));
	SetItemText (index, 2, GetWinIdStr (mi));
	
	SetItem     (index, 0, LVIF_IMAGE, NULL, mi.type, 0, 0, 0);
	
	if (sel) SetItemState (index,  LVIS_SELECTED, LVIS_SELECTED);
	else     SetItemState (index, ~LVIS_SELECTED, LVIS_SELECTED);
}

void CListCtrlEx::KeyCtrlA ()
{
	SetRedraw (FALSE);

	for (int i=0, n=GetItemCount(); i<n; i++) {
		SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
	}

	SetRedraw (TRUE);
}

void CListCtrlEx::KeyCtrlC ()
{
	UINT n = GetSelectedCount();
	if (!n) return;

	vector<sMacroItem> temp;
	temp.reserve (n);

	int  nItem = -1;
	for (UINT i=0; i<n; i++) {
		nItem = GetNextItem (nItem, LVNI_SELECTED);
		temp.push_back (_items->at(nItem));
	}

	// temp 벡터의 내용을 clipboard로 복사
	if (temp.size()) {
		clipboard.swap (temp);
	}
}

void CListCtrlEx::KeyCtrlV ()
{
	int sel = GetLastSel ();
	if (sel >= 0) {
		// 현재 선택된 항목들을 모두 선택 해제 한다.
		int s = GetNextItem (-1, LVNI_SELECTED);
		while (s >= 0) {
			SetItemState (s, 0, LVIS_SELECTED);
			s = GetNextItem (s, LVNI_SELECTED);
		}
		// 여러 항목이 선택되어 있기 때문에 마지막으로 선택된 항목 뒤에 붙여넣기 한다.
		sel++; 
	}
	else {
		// 선택된 항목이 없다면 마지막에 붙여넣기 한다.
		sel = GetItemCount ();
	}

	SetRedraw (FALSE);
	for (UINT i=0; i<clipboard.size(); i++) {
		_items->insert (_items->begin() + sel + i, clipboard[i]);
		InsertItem  (sel + i, "");
		SetItemInfo (sel + i, clipboard[i], true);
	}
	SetRedraw (TRUE);
}

void CListCtrlEx::KeyCtrlX ()
{
	UINT n = GetSelectedCount();
	if (!n) return;

	vector<int> sel;
	sel.reserve (n);

	vector<sMacroItem> temp;
	temp.reserve (n);

	int  nItem = -1;
	for (UINT i=0; i<n; i++) {
		nItem = GetNextItem (nItem, LVNI_SELECTED);
		temp.push_back (_items->at(nItem));
		sel.push_back (nItem);
	}

	// temp 벡터의 내용을 clipboard로 복사
	if (temp.size()) {
		clipboard.swap (temp);
	}

	SetRedraw (FALSE);
	// 지울 때는 마지막 선택 항목부터 거꾸로 지운다.
	for (int i=sel.size()-1; i>=0; i--) {
		DeleteItem (sel[i]);
		_items->erase (_items->begin() + sel[i]);
	}
	SetRedraw (TRUE);
}

void CListCtrlEx::KeyDelete ()
{
	UINT n = GetSelectedCount();
	if (!n) return;

	vector<int> sel;
	sel.reserve (n);

	int  nItem = -1;
	for (UINT i=0; i<n; i++) {
		nItem = GetNextItem (nItem, LVNI_SELECTED);
		sel.push_back (nItem);
	}

	SetRedraw (FALSE);
	// 지울 때는 마지막 선택 항목부터 거꾸로 지운다.
	for (int i=sel.size()-1; i>=0; i--) {
		DeleteItem (sel[i]);
		_items->erase (_items->begin() + sel[i]);
	}
	SetRedraw (TRUE);
}

void CListCtrlEx::SwapItem (int a, int b)
{
	std::swap ((*_items)[a], (*_items)[b]);

	SetItemInfo (a, (*_items)[a], true);
	SetItemInfo (b, (*_items)[b], false);
}

void CListCtrlEx::KeyMoveUp ()
{
	UINT n = GetSelectedCount();
	if (!n) return;

	vector<int> sel;
	sel.reserve (n);

	int  nItem = -1;
	for (UINT i=0; i<n; i++) {
		nItem = GetNextItem (nItem, LVNI_SELECTED);
		sel.push_back (nItem);
	}

	if (!sel.size()) return;

	// 두 번째 아이템부터 위로 올릴 수 있다.
	if (sel.at(0) < 1) return;

	SetRedraw (FALSE);
	for (int i=0; i<(int)sel.size(); i++) {
		int s = sel.at(i);
		SwapItem (s-1, s);
	}
	SetRedraw (TRUE);
}

void CListCtrlEx::KeyMoveDown ()
{
	UINT n = GetSelectedCount();
	if (!n) return;

	vector<int> sel;
	sel.reserve (n);

	int  nItem = -1;
	for (UINT i=0; i<n; i++) {
		nItem = GetNextItem (nItem, LVNI_SELECTED);
		sel.push_back (nItem);
	}

	if (!sel.size()) return;

	// 마지막 아이템은 아래로 내릴 수 없다.
	if (sel.at(sel.size()-1) >= GetItemCount ()-1) return;

	SetRedraw (FALSE);
	for (int i=sel.size()-1; i>=0; i--) {
		int s = sel.at(i);
		SwapItem (s+1, s);
	}
	SetRedraw (TRUE);
}