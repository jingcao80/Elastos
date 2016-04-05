
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/StringBuilder.h>
#include "R.h"

using Elastos::Droid::Provider::EIID_IBaseColumns;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(LauncherSettings::Favorites, Object, ILauncherSettingsBaseLauncherColumns,
        IBaseColumns);

static AutoPtr<IUri> InitCONTENT_URI()
{
    StringBuilder sb;
    assert(0 && "need class LauncherProvider");
    // sb += "content://";
    // sb += LauncherProvider::AUTHORITY;
    // sb += "/";
    // sb += LauncherProvider::TABLE_FAVORITES;
    // sb += "?";
    // sb += LauncherProvider::PARAMETER_NOTIFY;
    // sb += "=true";

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> tmp;
    uriHelper->Parse(sb.ToString(), (IUri**)&tmp);
    return tmp;
}

AutoPtr<IUri> LauncherSettings::Favorites::CONTENT_URI = InitCONTENT_URI();

static AutoPtr<IUri> InitCONTENT_URI_NO_NOTIFICATION()
{
    StringBuilder sb;
    assert(0 && "need class LauncherProvider");
    // sb += "content://";
    // sb += LauncherProvider::AUTHORITY;
    // sb += "/";
    // sb += LauncherProvider::TABLE_FAVORITES;
    // sb += "?";
    // sb += LauncherProvider::PARAMETER_NOTIFY;
    // sb += "=false";

    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> tmp;
    uriHelper->Parse(sb.ToString(), (IUri**)&tmp);
    return tmp;
}

AutoPtr<IUri> LauncherSettings::Favorites::CONTENT_URI_NO_NOTIFICATION = InitCONTENT_URI_NO_NOTIFICATION();

const String LauncherSettings::Favorites::CONTAINER("container");

const String LauncherSettings::Favorites::SCREEN("screen");

const String LauncherSettings::Favorites::CELLX("cellX");

const String LauncherSettings::Favorites::CELLY("cellY");

const String LauncherSettings::Favorites::SPANX("spanX");

const String LauncherSettings::Favorites::SPANY("spanY");

const String LauncherSettings::Favorites::PROFILE_ID("profileId");

const Int32 LauncherSettings::Favorites::ITEM_TYPE_LIVE_FOLDER = 3;

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
    assert(0 && "need class LauncherProvider");
    // sb += "content://";
    // sb += LauncherProvider::AUTHORITY;
    // sb += "/";
    // sb += LauncherProvider::TABLE_FAVORITES;
    // sb += "/";
    // sb += id;
    // sb += "?";
    // sb += LauncherProvider::PARAMETER_NOTIFY;
    // sb += "=";
    // sb += notify;
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> tmp;
    return uriHelper->Parse(sb.ToString(), (IUri**)&uri);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos