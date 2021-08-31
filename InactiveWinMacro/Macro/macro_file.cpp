#include "stdafx.h"
#include "macro_def.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void LoadMacros (const char *fileName)
{
	FILE *fp = fopen (fileName, "rb");
	if (!fp) return;

	long c = 0;
	fread (&c, sizeof(long), 1, fp);

	g_macros.resize(c);
	for (int i=0; i<c; ++i) {
		sMacro &m = g_macros[i];

		fread (m.name,			sizeof(char), 256, fp);
		fread (&m.repeat,		sizeof(DWORD), 1, fp);
		fread (&m.options,		sizeof(DWORD), 1, fp);
		
		long n = 0;
		fread (&n, sizeof(long), 1, fp);

		m._windowList.resize (n);
		for (int j=0; j<n; ++j) {
			fread (&m._windowList[j], sizeof(sWindowProperty), 1, fp);
		}

		fread (&n, sizeof(long), 1, fp);

		m._eventList.resize (n);
		for (int j=0; j<n; ++j) {
			fread (&m._eventList[j], sizeof(sMacroItem), 1, fp);
		}

		///////////////////////////////////////////////////////////
		m._windowList.ResetWindowList ();
		m._eventList.ResetEventList ();
	}
	fclose (fp);
}

void SaveMacro (const char *fileName)
{
	FILE *fp = fopen (fileName, "wb");
	if (!fp) return;

	long c = g_macros.size();
	fwrite (&c, sizeof(long), 1, fp);

	for (int i=0; i<c; ++i) {
		sMacro &m = g_macros[i];

		fwrite (m.name,			sizeof(char), 256, fp);
		fwrite (&m.repeat,		sizeof(DWORD), 1, fp);
		fwrite (&m.options,		sizeof(DWORD), 1, fp);
		
		long n = m._windowList.size();
		fwrite (&n, sizeof(long), 1, fp);

		for (int j=0; j<n; ++j) {
			fwrite (&m._windowList[j], sizeof(sWindowProperty), 1, fp);
		}

		n = m._eventList.size();
		fwrite (&n, sizeof(long), 1, fp);

		for (int j=0; j<n; ++j) {
			fwrite (&m._eventList[j], sizeof(sMacroItem), 1, fp);
		}
	}
	fclose (fp);
}
