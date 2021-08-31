#include "stdafx.h"
#include "InactiveWinMacro.h"
#include "DlgEditMessage.h"


IMPLEMENT_DYNAMIC(CDlgEditMessage, CDialog)

CDlgEditMessage::CDlgEditMessage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditMessage::IDD, pParent)
{
}

CDlgEditMessage::~CDlgEditMessage()
{
}

void CDlgEditMessage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgEditMessage, CDialog)
END_MESSAGE_MAP()
