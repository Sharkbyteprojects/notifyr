#pragma once

namespace shortcut {
	long CreateShortcut(const wchar_t * pszTargetargs, const wchar_t * pszDescription,

		int iShowmode, const wchar_t * pszIconfile,
 int iIconindex,
		const wchar_t* aumiD, const wchar_t*, const wchar_t* __guid);
	bool fileExists(const wchar_t*);
	long deleteLinkFileIfExists(const wchar_t*);
}
