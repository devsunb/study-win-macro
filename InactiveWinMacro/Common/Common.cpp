#include "stdafx.h"
#include "Common.h"


HICON GetWindowsIcon (HWND hWnd)
{
	HICON hIcon = NULL;

	if (SendMessageTimeout (hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 1000, (DWORD_PTR *)&hIcon)) {
		if (hIcon) return hIcon;
	}
	if (SendMessageTimeout(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 1000, (DWORD_PTR *)&hIcon)) {
		if (hIcon) return hIcon;
	}

	hIcon = (HICON)GetClassLong(hWnd, GCL_HICON);
	if (hIcon) return hIcon;

	hIcon = (HICON)GetClassLong(hWnd, GCL_HICONSM);
	if (hIcon) return hIcon;

	hIcon = LoadIcon(NULL, IDI_APPLICATION);
	return hIcon;
}

struct sWindowMessage g_windowMessage[] = {
	{ WM_KEYDOWN		,	"WM_KEYDOWN"			}, 
	{ WM_KEYUP			,	"WM_KEYUP"				}, 
	{ WM_SYSKEYDOWN		,	"WM_SYSKEYDOWN"			}, 
	{ WM_SYSKEYUP		,	"WM_SYSKEYUP"			}, 

	{ WM_MOUSEMOVE		,	"WM_MOUSEMOVE"			}, 
	{ WM_LBUTTONDOWN	,	"WM_LBUTTONDOWN"		}, 
	{ WM_LBUTTONUP		,	"WM_LBUTTONUP"			}, 
	{ WM_LBUTTONDBLCLK	,	"WM_LBUTTONDBLCLK"		}, 
	{ WM_RBUTTONDOWN	,	"WM_RBUTTONDOWN"		}, 
	{ WM_RBUTTONUP		,	"WM_RBUTTONUP"			}, 
	{ WM_RBUTTONDBLCLK	,	"WM_RBUTTONDBLCLK"		}, 
	{ WM_MBUTTONDOWN	,	"WM_MBUTTONDOWN"		}, 
	{ WM_MBUTTONUP		,	"WM_MBUTTONUP"			}, 
	{ WM_MBUTTONDBLCLK	,	"WM_MBUTTONDBLCLK"		}, 
	{ WM_MOUSEWHEEL		,	"WM_MOUSEWHEEL"			}, 
	{ WM_XBUTTONDOWN	,	"WM_XBUTTONDOWN"		}, 
	{ WM_XBUTTONUP		,	"WM_XBUTTONUP"			}, 

	{ WM_NCMOUSEMOVE	,	"WM_NCMOUSEMOVE"		}, 
	{ WM_NCLBUTTONDOWN	,	"WM_NCLBUTTONDOWN"		}, 
	{ WM_NCLBUTTONUP	,	"WM_NCLBUTTONUP"		}, 
	{ WM_NCLBUTTONDBLCLK,	"WM_NCLBUTTONDBLCLK"	}, 
	{ WM_NCRBUTTONDOWN	,	"WM_NCRBUTTONDOWN"		}, 
	{ WM_NCRBUTTONUP	,	"WM_NCRBUTTONUP"		}, 
	{ WM_NCRBUTTONDBLCLK,	"WM_NCRBUTTONDBLCLK"	}, 
	{ WM_NCMBUTTONDOWN	,	"WM_NCMBUTTONDOWN"		}, 
	{ WM_NCMBUTTONUP	,	"WM_NCMBUTTONUP"		}, 
	{ WM_NCMBUTTONDBLCLK,	"WM_NCMBUTTONDBLCLK"	}, 
	{ WM_NCXBUTTONDOWN	,	"WM_NCXBUTTONDOWN"		}, 
	{ WM_NCXBUTTONUP	,	"WM_NCXBUTTONUP"		}, 
	{ WM_NCXBUTTONDBLCLK,	"WM_NCXBUTTONDBLCLK"	}, 
};

int g_nWindowMessage = sizeof(g_windowMessage)/sizeof(sWindowMessage);


sNcHitTest g_ncHitTest[] = {
	{ HTERROR,			"HTERROR"		},	// 	On the screen background or on a dividing line between windows (same as HTNOWHERE, except that the DefWindowProc function produces a system beep to indicate an error).
	{ HTTRANSPARENT,	"HTTRANSPARENT"	},	// 	In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).
	{ HTNOWHERE,		"HTNOWHERE"		},	// 	On the screen background or on a dividing line between windows.
	{ HTCLIENT,			"HTCLIENT"		},	// 	In a client area.
	{ HTCAPTION,		"HTCAPTION"		},	// 	In a title bar.
	{ HTSYSMENU,		"HTSYSMENU"		},	// 	In a window menu or in a Close button in a child window.
	{ HTSIZE,			"HTSIZE"		},	// 	In a size box (same as HTGROWBOX).
//	{ HTGROWBOX,		"HTGROWBOX"		},	// 	In a size box (same as HTSIZE).
	{ HTMENU,			"HTMENU"		},	// 	In a menu.
	{ HTHSCROLL,		"HTHSCROLL"		},	// 	In a horizontal scroll bar.
	{ HTVSCROLL,		"HTVSCROLL"		},	// 	In the vertical scroll bar.
	{ HTMINBUTTON,		"HTMINBUTTON"	},	// 	In a Minimize button.
//	{ HTREDUCE,			"HTREDUCE"		},	// 	In a Minimize button.
	{ HTMAXBUTTON,		"HTMAXBUTTON"	},	// 	In a Maximize button.
//	{ HTZOOM,			"HTZOOM"		},	// 	In a Maximize button.
	{ HTLEFT,			"HTLEFT"		},	// 	In the left border of a resizable window (the user can click the mouse to resize the window horizontally).
	{ HTRIGHT,			"HTRIGHT"		},	// 	In the right border of a resizable window (the user can click the mouse to resize the window horizontally).
	{ HTTOP,			"HTTOP"			},	// 	In the upper-horizontal border of a window.
	{ HTTOPLEFT,		"HTTOPLEFT"		},	// 	In the upper-left corner of a window border.
	{ HTTOPRIGHT,		"HTTOPRIGHT"	},	// 	In the upper-right corner of a window border.
	{ HTBOTTOM,			"HTBOTTOM"		},	// 	In the lower-horizontal border of a resizable window (the user can click the mouse to resize the window vertically).
	{ HTBOTTOMLEFT,		"HTBOTTOMLEFT"	},	// 	In the lower-left corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
	{ HTBOTTOMRIGHT,	"HTBOTTOMRIGHT"	},	// 	In the lower-right corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
	{ HTBORDER,			"HTBORDER"		},	// In the border of a window that does not have a sizing border.
	{ HTCLOSE,			"HTCLOSE"		},	// 	In a Close button.
	{ HTHELP,			"HTHELP"		},	// 	In a Help button.
};

int g_nNcHitTest = sizeof(g_ncHitTest)/sizeof(sNcHitTest);


const char *GetncHitTestName (int code)
{
	for (int i=0; i<sizeof(g_ncHitTest)/sizeof(sNcHitTest); i++) {
		if (g_ncHitTest[i].code == code) return g_ncHitTest[i].name;
	}
	return "(Unknown)";
}


#pragma comment (lib, "Version.lib")

CString GetVersionInfo(HMODULE hLib, CString csEntry)
{
	CString csRet;

	if (hLib == NULL) hLib = AfxGetResourceHandle();

	HRSRC hVersion = FindResource( hLib, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION );
	if (hVersion != NULL) {
		HGLOBAL hGlobal = LoadResource( hLib, hVersion );
		if ( hGlobal != NULL) {

			LPVOID versionInfo = LockResource(hGlobal);
			if (versionInfo != NULL) {
				DWORD vLen,langD;
				BOOL retVal;

				LPVOID retbuf=NULL;
				static char fileEntry[256];
				sprintf(fileEntry,_T("\\VarFileInfo\\Translation"));

				retVal = VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen);
				if (retVal && vLen==4) {
					memcpy(&langD,retbuf,4);
					sprintf(fileEntry, _T("\\StringFileInfo\\%02X%02X%02X%02X\\%s"),
						(langD & 0xff00)>>8,langD & 0xff,(langD & 0xff000000)>>24,
						(langD & 0xff0000)>>16, csEntry);
				}
				else
					sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID(), (LPCTSTR)csEntry);

				if (VerQueryValue(versionInfo,fileEntry,&retbuf,(UINT *)&vLen))
					csRet = (char*)retbuf;
			}
		}

		UnlockResource( hGlobal );
		FreeResource( hGlobal );
	}

	return csRet;
}


/*
WM_NULL,				
WM_CREATE,
WM_DESTROY,
WM_MOVE,
WM_SIZE,
WM_ACTIVATE,
WM_SETFOCUS,
WM_KILLFOCUS,
WM_ENABLE,
WM_SETREDRAW,
WM_SETTEXT, 
WM_GETTEXT, 
WM_GETTEXTLENGTH, 
WM_PAINT, 
WM_CLOSE, 
WM_QUERYENDSESSION, 
WM_QUIT, 
WM_QUERYOPEN, 
WM_ERASEBKGND, 
WM_SYSCOLORCHANGE, 
WM_ENDSESSION, 
WM_SYSTEMERROR, 
WM_SHOWWINDOW, 
WM_CTLCOLOR, 
WM_WININICHANGE, 
WM_SETTINGCHANGE,
WM_DEVMODECHANGE,
WM_ACTIVATEAPP,
WM_FONTCHANGE, 
WM_TIMECHANGE, 
WM_CANCELMODE, 
WM_SETCURSOR, 
WM_MOUSEACTIVATE,
WM_CHILDACTIVATE,
WM_QUEUESYNC, 
WM_GETMINMAXINFO,
WM_PAINTICON, 
WM_ICONERASEBKGND, 
WM_NEXTDLGCTL, 
WM_SPOOLERSTATUS, 
WM_DRAWITEM, 
WM_MEASUREITEM, 
WM_DELETEITEM, 
WM_VKEYTOITEM, 
WM_CHARTOITEM, 

WM_SETFONT, 
WM_GETFONT, 
WM_SETHOTKEY, 
WM_GETHOTKEY, 
WM_QUERYDRAGICON,
WM_COMPAREITEM,	
WM_COMPACTING,	
WM_WINDOWPOSCHANGING, 
WM_WINDOWPOSCHANGED, 
WM_POWER, 
WM_COPYDATA, 
WM_CANCELJOURNAL, 
WM_NOTIFY, 
WM_INPUTLANGCHANGEREQUEST, 
WM_INPUTLANGCHANGE, 
WM_TCARD, 
WM_HELP, 
WM_USERCHANGED, 
WM_NOTIFYFORMAT, 
WM_CONTEXTMENU, 
WM_STYLECHANGING, 
WM_STYLECHANGED, 
WM_DISPLAYCHANGE, 
WM_GETICON, 
WM_SETICON, 

WM_NCCREATE, 
WM_NCDESTROY, 
WM_NCCALCSIZE, 
WM_NCHITTEST, 
WM_NCPAINT, 
WM_NCACTIVATE, 
WM_GETDLGCODE, 
WM_NCMOUSEMOVE, 
WM_NCLBUTTONDOWN, 
WM_NCLBUTTONUP, 
WM_NCLBUTTONDBLCLK, 
WM_NCRBUTTONDOWN, 
WM_NCRBUTTONUP, 
WM_NCRBUTTONDBLCLK, 
WM_NCMBUTTONDOWN, 
WM_NCMBUTTONUP, 
WM_NCMBUTTONDBLCLK, 

WM_KEYFIRST, 
WM_KEYDOWN, 
WM_KEYUP, 
WM_CHAR, 
WM_DEADCHAR, 
WM_SYSKEYDOWN, 
WM_SYSKEYUP, 
WM_SYSCHAR, 
WM_SYSDEADCHAR, 
WM_KEYLAST, 

WM_IME_STARTCOMPOSITION, 
WM_IME_ENDCOMPOSITION, 
WM_IME_COMPOSITION, 
WM_IME_KEYLAST, 

WM_INITDIALOG, 
WM_COMMAND, 
WM_SYSCOMMAND, 
WM_TIMER, 
WM_HSCROLL, 
WM_VSCROLL, 
WM_INITMENU, 
WM_INITMENUPOPUP, 
WM_MENUSELECT, 
WM_MENUCHAR, 
WM_ENTERIDLE, 

WM_CTLCOLORMSGBOX, 
WM_CTLCOLOREDIT, 
WM_CTLCOLORLISTBOX, 
WM_CTLCOLORBTN, 
WM_CTLCOLORDLG, 
WM_CTLCOLORSCROLLBAR, 
WM_CTLCOLORSTATIC, 

WM_MOUSEFIRST, 
WM_MOUSEMOVE, 
WM_LBUTTONDOWN, 
WM_LBUTTONUP, 
WM_LBUTTONDBLCLK, 
WM_RBUTTONDOWN, 
WM_RBUTTONUP, 
WM_RBUTTONDBLCLK, 
WM_MBUTTONDOWN, 
WM_MBUTTONUP, 
WM_MBUTTONDBLCLK, 
WM_MOUSEWHEEL, 
WM_MOUSEHWHEEL, 

WM_PARENTNOTIFY, 
WM_ENTERMENULOOP, 
WM_EXITMENULOOP, 
WM_NEXTMENU, 
WM_SIZING, 
WM_CAPTURECHANGED, 
WM_MOVING, 
WM_POWERBROADCAST, 
WM_DEVICECHANGE, 

WM_MDICREATE, 
WM_MDIDESTROY, 
WM_MDIACTIVATE, 
WM_MDIRESTORE, 
WM_MDINEXT, 
WM_MDIMAXIMIZE, 
WM_MDITILE, 
WM_MDICASCADE, 
WM_MDIICONARRANGE, 
WM_MDIGETACTIVE, 
WM_MDISETMENU, 
WM_ENTERSIZEMOVE, 
WM_EXITSIZEMOVE, 
WM_DROPFILES, 
WM_MDIREFRESHMENU, 

WM_IME_SETCONTEXT, 
WM_IME_NOTIFY, 
WM_IME_CONTROL, 
WM_IME_COMPOSITIONFULL, 
WM_IME_SELECT, 
WM_IME_CHAR, 
WM_IME_KEYDOWN, 
WM_IME_KEYUP, 

WM_MOUSEHOVER, 
WM_NCMOUSELEAVE, 
WM_MOUSELEAVE, 

WM_CUT, 
WM_COPY, 
WM_PASTE,
WM_CLEAR,
WM_UNDO, 

WM_RENDERFORMAT, 
WM_RENDERALLFORMATS, 
WM_DESTROYCLIPBOARD, 
WM_DRAWCLIPBOARD, 
WM_PAINTCLIPBOARD, 
WM_VSCROLLCLIPBOARD, 
WM_SIZECLIPBOARD, 
WM_ASKCBFORMATNAME, 
WM_CHANGECBCHAIN, 
WM_HSCROLLCLIPBOARD, 
WM_QUERYNEWPALETTE, 
WM_PALETTEISCHANGING, 
WM_PALETTECHANGED, 

WM_HOTKEY, 
WM_PRINT, 
WM_PRINTCLIENT, 

WM_HANDHELDFIRST, 
WM_HANDHELDLAST, 
WM_PENWINFIRST, 
WM_PENWINLAST, 
WM_COALESCE_FIRST, 
WM_COALESCE_LAST, 
WM_DDE_FIRST, 
WM_DDE_INITIATE, 
WM_DDE_TERMINATE, 
WM_DDE_ADVISE, 
WM_DDE_UNADVISE, 
WM_DDE_ACK, 
WM_DDE_DATA, 
WM_DDE_REQUEST, 
WM_DDE_POKE,
WM_DDE_EXECUTE,
WM_DDE_LAST,
*/