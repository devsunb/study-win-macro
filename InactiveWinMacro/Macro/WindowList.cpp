#include "stdafx.h"
#include "WindowList.h"
#include <assert.h>


inline int CountZeroBits (DWORD value)
{
	int count = 0;
	for (int i=0; i<32; i++) {
		if (!((value>>i) & 1)) count++;
	}
	return count;
}

inline int CountChildWnd (CWnd *parent)
{
	// �ڽ� �������� ������ ����.
	int count = 0;

	for (parent = parent->GetWindow (GW_CHILD); parent; parent = parent->GetWindow (GW_HWNDNEXT)) {
		count++;
	}
	return count;
}


void sWindowProperty::GetWindowProperty (CWnd *wnd)
{
	hWnd = wnd->m_hWnd;

	CRect rect;
	wnd->GetWindowRect (&rect);
	sx = rect.left;
	sy = rect.top;
	dx = rect.Width ();
	dy = rect.Height ();

	cntChild = CountChildWnd (wnd);

	wnd->GetWindowText (caption, 256);
	caption[256] = '\0';

	::GetClassName (wnd->m_hWnd, className, 256);
	className[256] = '\0';

	style   = ::GetWindowLong(wnd->m_hWnd, GWL_STYLE);
	exStyle = ::GetWindowLong(wnd->m_hWnd, GWL_EXSTYLE);
}

bool sWindowProperty::Compare (const sWindowProperty &wp, bool ignoreHandle)
{
	return (
		(ignoreHandle || hWnd == wp.hWnd) &&
		strncmp (className, wp.className, 256) == 0) &&
		cntChild == wp.cntChild &&
		//strncmp (caption, wp.caption, 256) == 0 &&
		//style == wp.style &&
		//exStyle == wp.exStyle &&
		//sx == wp.sx && sy == wp.sy &&
		//dx == wp.dx && dy == wp.dy &&
		1;
}

int sWindowProperty::ScoredCompare (const sWindowProperty &wp)
{
	int score = 0;

	if (strncmp (className, wp.className, 256) == 0)	score += 100;
	if (cntChild == wp.cntChild)						score += 100;
	if (strncmp (caption, wp.caption, 256) == 0)		score += 30;
	score += CountZeroBits (style ^ wp.style);
	score += CountZeroBits (exStyle ^ wp.exStyle);
	if (dx == wp.dx && dy == wp.dy)						score += 2;
	if (sx == wp.sx && sy == wp.sy)						score += 1;

	return score;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CWindowList::CWindowList() :
	_hWndTarget(0), _targetProcessID(0)
{
	clear ();
	reserve (100);
}

void CWindowList::SetTargetProcess (HWND hWndTarget, DWORD processID)
{
	// ù �����츦 �����Ѵ�.
	CWnd *wnd = CWnd::FromHandle (hWndTarget);

	sWindowProperty wp;
	wp.GetWindowProperty (wnd);

	push_back (wp);

	_hWndTarget			= hWndTarget;
	_targetProcessID	= processID;
}

bool CWindowList::SetStartProcess ()
{
	if (!size()) return false;

	HWND hWnd = SearchMaxMatchedWnd (at(0), true);
	if (!hWnd) return false;

	DWORD processID = 0;
	::GetWindowThreadProcessId (hWnd, &processID);

	// ��� ������ �ڵ��� �ʱ�ȭ �Ѵ�.
	for (int i=0, n=(int)size(); i<n; i++) {
		at(i).hWnd = 0;
	}

	_hWndTarget			= hWnd;
	_targetProcessID	= processID;
	return true;
}

void CWindowList::ResetWindowList ()
{
	// ���Ϸκ��� �����͸� ���� �� ���ʿ��� �����͸� �����Ѵ�.

	// ��� ������ �ڵ��� �ʱ�ȭ �Ѵ�.
	for (int i=0, n=(int)size(); i<n; i++) {
		at(i).hWnd = 0;
	}

	_hWndTarget			= 0;
	_targetProcessID	= 0;
}

HWND CWindowList::SearchMaxMatchedWnd (sWindowProperty &wp, bool doNotComparePID)
{
	// �ְ�� ������ ������ �ڵ�
	int maxScore = 0;
	CWnd *maxWnd = NULL;

	// ����ũž ������ �Ʒ� �ִ� �ֻ��� �ڽ� �����츦 ���´�.
	CWnd *mainWnd = AfxGetMainWnd ();
	CWnd *pWnd = mainWnd->GetWindow(GW_HWNDFIRST);

	while (pWnd){
		DWORD processID = 0;
		::GetWindowThreadProcessId (pWnd->m_hWnd, &processID);

		if (doNotComparePID || _targetProcessID == processID) {
			sWindowProperty wpc;
			wpc.GetWindowProperty (pWnd);
			int score = wp.ScoredCompare (wpc);

			if (maxScore < score) {
				maxScore = score;
				maxWnd = pWnd;
			}
		}
		// ������ ��Ͽ��� ���� �����츦 ���´�.
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	if (maxScore > 200) {	// �ּ��� Class �̸��� �ڽ� �������� ������ ���ƾ� �Ѵ�. 
							// �׸��� �ϳ��� ������ ������ �����Ѵ�.
		// ã�Ҵ�.
		//TRACE ("FIND Target: %s\n", wpc.caption );
		return maxWnd->m_hWnd;
	}
	TRACE ("FIND Target failed\n");
	return (HWND)0;
}

HWND CWindowList::GetTargetWindow ()
{
	assert (size());
	assert (_hWndTarget);

	return _hWndTarget;
}

CWnd *CWindowList::GetTargetWindow (int id, int order)
{
	if (!(0 <= id && id < (int)size())) {
		TRACE ("��� ������ ID�� ������ ���\n");
		return NULL;
	}

	sWindowProperty &wp = at(id);

	// id�� 0�̸� ���μ����� ���� �������̴�. 
	// ������ ���μ����� ���� ������ �ڵ��� �����Ѵ�.
	if (id == 0) {
		// ������ ���μ��� �����κ��� ���� ������ �ڵ��� ���´�.
		if (wp.hWnd == 0) {
			wp.hWnd = _hWndTarget;
		}
	}
	else {
		// ������ �ڵ��� ����ִٸ�(0) �����츦 �˻��Ѵ�.
		if (wp.hWnd == 0) {
			wp.hWnd = SearchMaxMatchedWnd (wp);
		}
	}

	// ������ �ڵ��� �ִٸ�, �̹� �ѹ� �˻��� �������.
	// 0�� ���� �˻� ������ �������.
	if (!wp.hWnd) {
		return NULL;
	}

	// -1�� 0�� �ƴ϶�� ������ �ڵ��� �������� üũ�Ͽ� �����ϸ� �ȴ�.
	if (IsWindow (wp.hWnd)) {
		return CWnd::FromHandle (wp.hWnd);
	}
	wp.hWnd = (HWND)-1;  // ã�� ����
	TRACE ("FIND Target failed\n");
	return NULL;
}

CWnd *CWindowList::GetLastChildWindow (CWnd *wnd, long depth, long order[])
{
	for (int i=1; i<depth; i++) {
		wnd = wnd->GetWindow (GW_CHILD);
		if (!wnd) {
			TRACE ("FIND child failed\n");
			return NULL;
		}

		for (int j=1; j<order[i]; j++) {
			if (wnd) {
				wnd = wnd->GetWindow (GW_HWNDNEXT);
			}
			else {
				TRACE ("FIND child failed\n");
				return NULL;
			}
		}
	}
	//TRACE ("FIND child: %X\n", wnd->m_hWnd);
	return wnd;
}

int CWindowList::FindAndSaveIfDiffWindow (CWnd *wnd)
{
	// �� �Լ��� �׻� �����Ѵ�.

	sWindowProperty wp;
	wp.GetWindowProperty (wnd);

	// ����� ������ ��ϰ� ���Ͽ� ������ �����츦 ã�´�.
	int find = -1;
	for (int i=0, n=(int)size(); i<n; i++) {
		if (at(i).Compare (wp)) {
			find = i;
			break;
		}
	}

	if (find < 0) {
		// TRACE ("%d %d %s %X %X\n", wp.dx, wp.dy, wp.caption, wp.style, wp.exStyle);
		push_back (wp);
		find = (int)size() - 1;
	}
	return find;
}
