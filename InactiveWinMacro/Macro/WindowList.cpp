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
	// 자식 윈도우의 개수를 센다.
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
	// 첫 윈도우를 저장한다.
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

	// 모든 윈도우 핸들을 초기화 한다.
	for (int i=0, n=(int)size(); i<n; i++) {
		at(i).hWnd = 0;
	}

	_hWndTarget			= hWnd;
	_targetProcessID	= processID;
	return true;
}

void CWindowList::ResetWindowList ()
{
	// 파일로부터 데이터를 읽은 후 불필요한 데이터를 삭제한다.

	// 모든 윈도우 핸들을 초기화 한다.
	for (int i=0, n=(int)size(); i<n; i++) {
		at(i).hWnd = 0;
	}

	_hWndTarget			= 0;
	_targetProcessID	= 0;
}

HWND CWindowList::SearchMaxMatchedWnd (sWindowProperty &wp, bool doNotComparePID)
{
	// 최고로 유사한 윈도우 핸들
	int maxScore = 0;
	CWnd *maxWnd = NULL;

	// 데스크탑 윈도우 아래 있는 최상위 자식 윈도우를 얻어온다.
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
		// 윈도우 목록에서 다음 윈도우를 얻어온다.
		pWnd = pWnd->GetWindow(GW_HWNDNEXT);
	}

	if (maxScore > 200) {	// 최소한 Class 이름과 자식 윈도우의 개수가 같아야 한다. 
							// 그리고 하나더 같으면 조건이 만족한다.
		// 찾았다.
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
		TRACE ("대상 윈도우 ID가 범위를 벗어남\n");
		return NULL;
	}

	sWindowProperty &wp = at(id);

	// id가 0이면 프로세서의 메인 윈도우이다. 
	// 무조건 프로세서의 메인 윈도우 핸들을 리턴한다.
	if (id == 0) {
		// 없으면 프로세서 정보로부터 메인 윈도우 핸들을 얻어온다.
		if (wp.hWnd == 0) {
			wp.hWnd = _hWndTarget;
		}
	}
	else {
		// 윈도우 핸들이 비어있다면(0) 윈도우를 검색한다.
		if (wp.hWnd == 0) {
			wp.hWnd = SearchMaxMatchedWnd (wp);
		}
	}

	// 윈도우 핸들이 있다면, 이미 한번 검색한 윈도우다.
	// 0인 경우는 검색 실패한 윈도우다.
	if (!wp.hWnd) {
		return NULL;
	}

	// -1과 0이 아니라면 윈도우 핸들이 정상인지 체크하여 리턴하면 된다.
	if (IsWindow (wp.hWnd)) {
		return CWnd::FromHandle (wp.hWnd);
	}
	wp.hWnd = (HWND)-1;  // 찾기 실패
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
	// 이 함수는 항상 성공한다.

	sWindowProperty wp;
	wp.GetWindowProperty (wnd);

	// 저장된 윈도우 목록과 비교하여 유사한 윈도우를 찾는다.
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
