
#include "elastos/droid/launcher2/UninstallShortcutReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherSettings::Favorites, Object, IBaseLauncherColumns);

void LauncherSettings::Favorites::InitStaticBlock()
{
    StringBuilder sb;
    sb += "content://";
    sb += LauncherProvider::AUTHORITY;
    sb += "/";
    sb += LauncherProvider::TABLE_FAVORITES;
    sb += "?";
    sb += LauncherProvider::PARAMETER_NOTIFY;
    sb += "=true";
    Uri::Parse(sb.ToString(), (IUri**)&CONTENT_URI);

    StringBuilder sb2;
    sb2 += "content://";
    sb2 += LauncherProvider::AUTHORITY;
    sb2 += "/";
    sb2 += LauncherProvider::TABLE_FAVORITES;
    sb2 += "?";
    sb2 += LauncherProvider::PARAMETER_NOTIFY;
    sb2 += "=false";
    Uri::Parse(sb2.ToString(), (IUri**)&CONTENT_URI_NO_NOTIFICATION);
}

const String LauncherSettings::Favorites::CONTAINER("container");

const Int32 LauncherSettings::Favorites::CONTAINER_DESKTOP = -100;
const Int32 LauncherSettings::Favorites::CONTAINER_HOTSEAT = -101;

const String LauncherSettings::Favorites::SCREEN("screen");

const String LauncherSettings::Favorites::CELLX("cellX");

const String LauncherSettings::Favorites::CELLY("cellY");

const String LauncherSettings::Favorites::SPANX("spanX");

const String LauncherSettings::Favorites::SPANY("spanY");

const String LauncherSettings::Favorites::PROFILE_ID("profileId");

const Int32 LauncherSettings::Favorites::ITEM_TYPE_FOLDER = 2;

const Int32 LauncherSettings::Favorites::ITEM_TYPE_LIVE_FOLDER = 3;

const Int32 LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET = 4;

const Int32 LauncherSettings::Favorites::ITEM_TYPE_WIDGET_CLOCK = 1000;

const Int32 LauncherSettings::Favorites::ITEM_TYPE_WIDGET_SEARCH = 1001;

const Int32 LauncherSettings::Favorites::ITEM_TYPE_WIDGET_PHOTO_FRAME = 1002;

const String LauncherSettings::Favorites::APPWIDGET_ID("appWidgetId");

const String LauncherSettings::Favorites::IS_SHORTCUT("isShortcut");

const String LauncherSettings::Favorites::URI("uri");

const String LauncherSettings::Favorites::DISPLAY_MODE("displayMode");

LauncherSettings::Favorites::GetContentUri(
    /* [in] */ Int64 id,
    /* [in] */ Boolean notify,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = NULL;

    StringBuilder sb;
    sb += "content://";
    sb += LauncherProvider::AUTHORITY;
    sb += "/";
    sb += LauncherProvider::TABLE_FAVORITES;
    sb += "/";
    sb += id;
    sb += "?";
    sb += LauncherProvider::PARAMETER_NOTIFY;
    sb += "=";
    sb += notify;
    return Uri::Parse(sb.ToString(), uri);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos