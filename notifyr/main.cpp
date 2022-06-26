#include <Windows.h>
#include "m.h"
#include <iostream>

int mainfunc(int argc, char** argv) {
    const wchar_t bt[] = L"🦈 Hello World!\n";
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), bt, lstrlenW(bt), NULL, NULL);

    //TOAST
    ComPtr<IXmlDocument> doc;
    //EMOJI: WIN + .
    HRESULT hr = DesktopNotificationManagerCompat::CreateXmlDocumentFromString(
        L"<toast><visual><binding template='ToastGeneric'>"
        L"<text>😂 Hello world from a Toast Notification</text>"
        L"<text>😘 How are You?</text>"
        L"<text>🐗🦝🐋🦈 Run with argument \"-clean\" (as first argument) to cleanup the startmenuicon</text>"
        L"</binding></visual></toast>",
        &doc);
    if (SUCCEEDED(hr))
    {
        // See full code sample to learn how to inject dynamic text, buttons, and more

        // Create the notifier
        // Classic Win32 apps MUST use the compat method to create the notifier
        ComPtr<IToastNotifier> notifier;
        hr = DesktopNotificationManagerCompat::CreateToastNotifier(&notifier);
        if (SUCCEEDED(hr))
        {
            // Create the notification itself (using helper method from compat library)
            ComPtr<IToastNotification> toast;
            hr = DesktopNotificationManagerCompat::CreateToastNotification(doc.Get(), &toast);

            if (SUCCEEDED(hr))
            {
                // And show it!
                hr = notifier->Show(toast.Get());
                if (!SUCCEEDED(hr))
                    std::cerr << "!THE FIRST RUN MAY NOT WORK, BECAUSE OF WINDOWS SECURITY RULES\n\t"
                    "Start it again to see the Toast Notification\nRun with argument \"-clean\" (as first argument) to cleanup the startmenuicon\n";
            }
        }
    }
    //TOAST END

    Sleep(1 << 9);
	return 0;
}
