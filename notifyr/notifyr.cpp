#include <Windows.h>
#include "shelllink.h"
#include "m.h"
#include <iostream>

// The UUID CLSID must be unique to your app. Create a new GUID if copying this code.
class DECLSPEC_UUID(_guid) NotificationActivator WrlSealed WrlFinal
	: public RuntimeClass<RuntimeClassFlags<ClassicCom>, INotificationActivationCallback>
{
public:
	virtual HRESULT STDMETHODCALLTYPE Activate(
		_In_ LPCWSTR appUserModelId,
		_In_ LPCWSTR invokedArgs,
		_In_reads_(dataCount) const NOTIFICATION_USER_INPUT_DATA * data,
		ULONG dataCount) override
	{
		return S_OK;
	}
};

// Flag class as COM creatable
CoCreatableClass(NotificationActivator);

void msg(const char* t, HRESULT hr) {
	if (hr == ERROR_SUCCESS)
		std::cout << t << ": OK" << "\n";
	else
		std::cerr << "Error duing " << t << "\n";
}


void rmReg() {
	msg("remove Reg Key", DesktopNotificationManagerCompat::_UNRegisterComServer(__uuidof(NotificationActivator)));
	CoUninitialize();
}

BOOL WINAPI HandlerRoutine(
	_In_ DWORD dwCtrlType
) {
	std::cout << "Process exit command recieved" <<
		((dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT || dwCtrlType == CTRL_CLOSE_EVENT) ? " from User" : "")
		<< ", doing cleanup!\n";
	rmReg();

	return FALSE;
}

#ifndef iconindex
#define iconindex 0
#endif
#ifndef shortcutTargetArgs
#define shortcutTargetArgs NULL
#endif

const wchar_t* GetWC(const char* c)
{
	const size_t cSize = strlen(c) + 1;
	wchar_t* wc = new wchar_t[cSize];
#pragma warning(suppress : 4996)
	mbstowcs(wc, c, cSize);

	return wc;
}
#include <VersionHelpers.h>

#define strcmp(x, y) (std::string(x) == std::string(y))

int main(int ac, char** agv)
{
	if (IsWindows10OrGreater()) {
		std::cerr << "You need Windows 10 Or Greater to run this!" << std::endl;
		return -1;
	}
	msg("INIT", CoInitialize(NULL));//Needed for Shortcut and Register
	if (!shortcut::fileExists(appname)) {
		auto w = GetWC(_guid);
		msg("Create Shortcut", shortcut::CreateShortcut(shortcutTargetArgs, _desc, showCmdMode, iconfile, iconindex, aumiID, appname, w));
		delete w;
	}
	msg("Register Sharkapp", DesktopNotificationManagerCompat::RegisterAumidAndComServer(aumiID, __uuidof(NotificationActivator)));
	msg("Activator Register", DesktopNotificationManagerCompat::RegisterActivator());
	SetConsoleCtrlHandler(HandlerRoutine, TRUE);

	int r = mainfunc(ac, agv);

	rmReg();
	if (ac > 1) {
		if(strcmp(agv[1], "-clean")){
			msg("Cleanup Shortcut", shortcut::deleteLinkFileIfExists(appname));
		}
	}
	return r;
}

