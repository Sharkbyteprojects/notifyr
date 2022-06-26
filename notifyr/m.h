#pragma once
//SETTINGS:
#define _guid "3d01fb3a-b81c-4861-b776-ab105d24dfe6"
#define appname L"notifyr"
#define _desc L"An Notification Test App"
#define iconfile 0 //If icon provided, link to it as wideString, else 0
#define iconindex 0//For iconfile, to select specific icon in the icon file
#define aumiID L"sharkbyteprojects.notifyr" //company.app
#define shortcutTargetArgs L""

//UNCOMMENT FOLLOWING LINE TO KEEP CREATED SHORTCUT IN WIN MENU
//#define keepShortcut

//ADVANCED SETTINGS
#define showCmdMode 0
//SETTINGS END



#include "DesktopNotificationManagerCompat.h"
#include <NotificationActivationCallback.h>
#include <windows.ui.notifications.h>

using namespace ABI::Windows::Data::Xml::Dom;
using namespace ABI::Windows::UI::Notifications;
using namespace Microsoft::WRL;


int mainfunc(int argc, char** argv);

