#include "shelllink.h"
#include <Windows.h>
#include <shlobj.h>
#include <winnls.h>
#include <shobjidl.h>
#include <objbase.h>
#include <objidl.h>
#include <shlguid.h>
#include <cstring>
#include <string>
#include <Psapi.h>
#include <wrl.h>
#include <propvarutil.h>
#include <Propkey.h>
using namespace Microsoft::WRL;

#define DEFAULT_SHELL_LINKS_PATH	L"\\Microsoft\\Windows\\Start Menu\\Programs\\"
#define DEFAULT_LINK_FORMAT			L".lnk"

namespace shortcut {
	HRESULT defaultShellLinksDirectory(WCHAR* path, DWORD nSize = MAX_PATH) {
		DWORD written = GetEnvironmentVariableW(L"APPDATA", path, nSize);
		HRESULT hr = written > 0 ? S_OK : E_INVALIDARG;
		if (SUCCEEDED(hr)) {
			errno_t result = wcscat_s(path, nSize, DEFAULT_SHELL_LINKS_PATH);
			hr = (result == 0) ? S_OK : E_INVALIDARG;
		}
		return hr;
	}

	HRESULT defaultShellLinkPath(const std::wstring& appname, WCHAR* path, DWORD nSize = MAX_PATH) {
		long hr = defaultShellLinksDirectory(path, nSize);
		if (SUCCEEDED(hr)) {
			const std::wstring appLink(appname + DEFAULT_LINK_FORMAT);
			errno_t result = wcscat_s(path, nSize, appLink.c_str());
			hr = (result == 0) ? S_OK : E_INVALIDARG;
		}
		return hr;
	}

	bool fileExists(const wchar_t* appname) {
		WCHAR p[MAX_PATH];
		if (!SUCCEEDED(defaultShellLinkPath(appname, p))) return 0;
		return PathFileExistsW(p);
	}

	long deleteLinkFileIfExists(const wchar_t* appname) {
		WCHAR p[MAX_PATH];
		long o = defaultShellLinkPath(appname, p);
		if (!SUCCEEDED(o)) return o;
		if (PathFileExistsW(p) != TRUE) return o;
		return (DeleteFileW(p) != 0) ? S_OK : E_ACCESSDENIED;
	}

	HRESULT defaultExecutablePath(WCHAR* path, DWORD nSize = MAX_PATH) {
		DWORD written = GetModuleFileNameExW(GetCurrentProcess(), nullptr, path, nSize);
		return (written > 0) ? S_OK : E_FAIL;
	}

	long CreateShortcut(const wchar_t* pszTargetargs, const wchar_t* pszDescription,

		int iShowmode, const wchar_t* pszIconfile, int iIconindex, const wchar_t* aumiD, const wchar_t* appname, const wchar_t* guid)
	{
		HRESULT       hRes;                  /* Returned COM result code */
		ComPtr<IShellLinkW> pShellLink;           /* IShellLink object pointer */
		ComPtr<IPersistFile> pPersistFile;          /* IPersistFile object pointer */
		hRes = CoCreateInstance(
			CLSID_ShellLink,     /* pre-defined CLSID of the IShellLink
									 object */
			NULL,                 /* pointer to parent interface if part of
									 aggregate */
			CLSCTX_INPROC_SERVER, /* caller and called code are in same
									 process */
			IID_IShellLink,      /* pre-defined interface of the
									 IShellLink object */
			(LPVOID*)&pShellLink);         /* Returns a pointer to the IShellLink
									 object */
		if (SUCCEEDED(hRes))
		{
			/* Set the fields in the IShellLink object */
			hRes = pShellLink->SetArguments(pszTargetargs);
			if (pszDescription != nullptr)
			{
				hRes = pShellLink->SetDescription(pszDescription);
			}
			if (iShowmode > 0)
			{
				hRes = pShellLink->SetShowCmd(iShowmode);
			}
			WCHAR exeDir[MAX_PATH];
			hRes = defaultExecutablePath(&exeDir[0]);
			if (SUCCEEDED(hRes))
			{
				hRes = pShellLink->SetPath(exeDir);
				std::wstring fn(exeDir);
				const size_t last_slash_idx = fn.rfind('\\');
				if (std::string::npos != last_slash_idx)
					hRes = pShellLink->SetWorkingDirectory(fn.substr(0, last_slash_idx).c_str());
			}
			if (pszIconfile != nullptr && iIconindex >= 0)
			{
				hRes = pShellLink->SetIconLocation(pszIconfile, iIconindex);
			}
			ComPtr<IPropertyStore> propertyStore;
			hRes = pShellLink.As(&propertyStore);
			if (SUCCEEDED(hRes)) {
				PROPVARIANT appIdPropVar;
				hRes = InitPropVariantFromString(aumiD, &appIdPropVar);
				if (SUCCEEDED(hRes)) {
					PROPVARIANT tg;
					hRes = InitPropVariantFromString(guid, &tg);
					if (SUCCEEDED(hRes)) {
						hRes = propertyStore->SetValue(PKEY_AppUserModel_ID, appIdPropVar);
						if (SUCCEEDED(hRes)) {
							hRes = propertyStore->SetValue(PKEY_AppUserModel_ToastActivatorCLSID, tg);
							if (SUCCEEDED(hRes)) {
								hRes = propertyStore->Commit();
								if (SUCCEEDED(hRes))
								{
									hRes = pShellLink.As(&pPersistFile);
									if (SUCCEEDED(hRes)) {
										WCHAR linkFile[MAX_PATH];
										hRes = defaultShellLinkPath(appname, linkFile);
										if (SUCCEEDED(hRes)) {
											hRes = pPersistFile->Save(linkFile, TRUE);
											pPersistFile->Release();
										}
									}
								}
							}
						}
					}
					PropVariantClear(&tg);
				}
				PropVariantClear(&appIdPropVar);
			}
			pShellLink->Release();
		}
		return hRes;
	}
}
