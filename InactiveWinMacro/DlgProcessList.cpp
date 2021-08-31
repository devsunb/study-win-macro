#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgProcessList.h"
#include "Common.h"


IMPLEMENT_DYNAMIC(CDlgProcessList, CDialog)

CDlgProcessList::CDlgProcessList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessList::IDD, pParent)
{
	_selProcess = NULL;
	_selPi = NULL;
}

CDlgProcessList::~CDlgProcessList()
{
}

void CDlgProcessList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROCESS, _listProcess);
}


BEGIN_MESSAGE_MAP(CDlgProcessList, CDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, &CDlgProcessList::OnBnClickedButtonRefresh)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PROCESS, &CDlgProcessList::OnNMDblclkListProcess)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PROCESS, &CDlgProcessList::OnNMClickListProcess)
	ON_BN_CLICKED(IDOK, &CDlgProcessList::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgProcessList::OnBnClickedCancel)
END_MESSAGE_MAP()


BOOL CDlgProcessList::OnInitDialog()
{
	CDialog::OnInitDialog();

	_processIcon.Create (16, 16, ILC_COLOR24|ILC_MASK, 10,	10);

	_listProcess.SetExtendedStyle(_listProcess.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_SUBITEMIMAGES | LVS_EX_DOUBLEBUFFER | LVS_EX_SINGLEROW);
	_listProcess.SetImageList(&_processIcon, LVSIL_SMALL);
	_listProcess.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
	
	_listProcess.InsertColumn(0, "���μ��� �̸�",	LVCFMT_LEFT, 330);
	_listProcess.InsertColumn(1, "����",			LVCFMT_LEFT, 75);

	// ���� ���α׷� ��Ͼ��
	GetProcessList ();
	UpdateProcessListView ();

	SetTimer (1000, 100, NULL);

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}


int CDlgProcessList::GetProcessList ()
{
	_processList.clear ();
	
	CWnd *mainWnd = AfxGetMainWnd ();

	// ó�� �ֻ��� �����츦 ��Ͽ��� ���´�.
	int count = 0;
	for (CWnd *pWnd = mainWnd->GetWindow(GW_HWNDFIRST); pWnd; pWnd = pWnd->GetWindow(GW_HWNDNEXT)) {
		if (!pWnd->GetOwner() && pWnd->IsWindowVisible() && pWnd->GetWindowTextLength()){
			// �������� ĸ���� ����		
			CString strCaption;
			pWnd->GetWindowText(strCaption); 

			if (strCaption == "InactiveWinMacro") continue;
			if (strCaption == "��Ȱ�� ������ ��ũ��") continue;
			if (strCaption == "Program Manager") continue;

			DWORD processID = 0;
			DWORD threadID  = ::GetWindowThreadProcessId (pWnd->m_hWnd, &processID);
			
			_processList.push_back (sProcessInfo(pWnd->m_hWnd, processID, threadID));
			count ++;
		}
	}

	return count;
}

void CDlgProcessList::UpdateProcessListView ()
{
	// ���μ����� ����� ������ ������ �̹����� ��� �����.
	for (int i=_processIcon.GetImageCount()-1; i>=0; i--) {
		_processIcon.Remove(i);
	}

	// ���� ���õ� �׸� ������ �д�.
	int sel = _listProcess.GetNextItem (-1, LVNI_SELECTED);

	// ����Ʈ ��Ʈ�ѿ� ��ϵ� ��� ���μ��� ��ϵ� �����.
	_listProcess.DeleteAllItems (); 

	// �˻��� ���μ��� ������� ����Ʈ ��Ʈ���� ���� ä���.
	for (int i=0; i<(int)_processList.size(); i++) {
		sProcessInfo &pi = _processList[i];
		CWnd *pWnd = CWnd::FromHandle (pi.hWnd);

		CString strCaption;
		pWnd->GetWindowText(strCaption); // �������� ĸ���� ����

		HICON icon = GetWindowsIcon (pi.hWnd);
		_processIcon.Add (icon);

		_listProcess.InsertItem  (i, strCaption);
		_listProcess.SetItemText (i, 1, "���� ��");
		_listProcess.SetItemData (i, pi.threadID);
		_listProcess.SetItem     (i, 0, LVIF_IMAGE, NULL, i, 0, 0, 0);
	}

	// �����찡 �ϳ� �̻� ������ ���� ó�� �����츦 �����Ѵ�.
	if (sel >= 0 && _listProcess.GetItemCount () > sel) {
		_listProcess.SetItemState (sel, LVIS_SELECTED, LVIS_SELECTED);
		_listProcess.EnsureVisible (sel-1, FALSE);
	}
}

void CDlgProcessList::UpdateListProcess ()
{
	for (int i=0; i<(int)_processList.size(); i++) {
		sProcessInfo &pi = _processList[i];
		
		DWORD tid = ::GetWindowThreadProcessId (pi.hWnd, NULL);

		_listProcess.SetItemText (i, 1, tid ? "���� ��": "����");
	}
}

void CDlgProcessList::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1000) {
		// ���μ����� ���������� ����Ǿ������� ���¸� ������Ʈ �Ѵ�.
		UpdateListProcess ();
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgProcessList::OnBnClickedButtonRefresh()
{
	// ���� ���α׷� ��Ͼ��
	GetProcessList ();
	UpdateProcessListView ();

	_selProcess = NULL;
}


void CDlgProcessList::OnNMClickListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	int sel = _listProcess.GetNextItem (-1, LVNI_SELECTED);
	if (0 <= sel && sel < (int)_processList.size()) {
		sProcessInfo &pi = _processList[sel];
		
		FLASHWINFO info;
		info.cbSize = sizeof(info);
		info.hwnd = pi.hWnd;
		info.dwFlags = FLASHW_ALL;
		info.dwTimeout = 0;
		info.uCount = 3;
		::FlashWindowEx(&info);
	}
	*pResult = 0;
}

void CDlgProcessList::OnNMDblclkListProcess(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	OnBnClickedOk();

	*pResult = 0;
}

void CDlgProcessList::OnBnClickedOk()
{
	int sel = _listProcess.GetNextItem (-1, LVNI_SELECTED);
	if (0 <= sel && sel < (int)_processList.size()) {
		sProcessInfo &pi = _processList[sel];
		
		// ���õ� ���μ����� ���� ���� ������ üũ�Ѵ�.
		DWORD tid = ::GetWindowThreadProcessId (pi.hWnd, NULL);
		if (tid) {
			// ���õ� ���μ��� ������ ���� �����͸� ������ �д�.
			// ��ȭ���ڸ� ȣ���� �����쿡 ���� ������ ����
			_selPi = &pi;

			OnOK();
		}
		else AfxMessageBox ("���� ���� ������ ���� ���μ����� ������ �� �����ϴ�.");
	}
	else AfxMessageBox ("���μ����� �������� �ʾҽ��ϴ�.");
}

void CDlgProcessList::OnBnClickedCancel()
{
	OnCancel();
}