#pragma once
#include "afxwin.h"
#include "macro_def.h"

class CListCtrlEx : public CListCtrl {
public:
	CListCtrlEx ();
	
	void SetMacroItems (vector<sMacroItem> *items) { _items = items; }

	int GetFirstSel ();
	int GetLastSel ();
	void SetItemInfo (int index, sMacroItem &mi, bool sel);
	void SwapItem (int a, int b);

private:
	vector<sMacroItem> *_items;
	vector<sMacroItem> clipboard;

	void KeyCtrlA ();
	void KeyCtrlC ();
	void KeyCtrlV ();
	void KeyCtrlX ();

public:
	void KeyDelete ();
	void KeyMoveUp ();
	void KeyMoveDown ();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
};

