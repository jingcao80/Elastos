//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "elastos/droid/launcher2/LauncherSettings.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Provider::EIID_IBaseColumns;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(LauncherSettings::Favorites, Object, ILauncherSettingsBaseLauncherColumns,
        IBaseColumns);

static AutoPtr<IUri> InitCONTENT_URI()
{
    StringBuilder sb;
    sb += "content://";
    sb += ILauncherProvider::AUTHORITY;
    sb += "/";
    sb += ILauncherProvider::TABLE_FAVORITES;
    sb += "?";
    sb += ILauncherProvider::PARAMETER_NOTIFY;
    sb += "=true";

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
    sb += "content://";
    sb += ILauncherProvider::AUTHORITY;
    sb += "/";
    sb += ILauncherProvider::TABLE_FAVORITES;
    sb += "?";
    sb += ILauncherProvider::PARAMETER_NOTIFY;
    sb += "=false";

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

ECode LauncherSettings::Favorites::GetContentUri(
    /* [in] */ Int64 id,
    /* [in] */ Boolean notify,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = NULL;

    StringBuilder sb;
    sb += "content://";
    sb += ILauncherProvider::AUTHORITY;
    sb += "/";
    sb += ILauncherProvider::TABLE_FAVORITES;
    sb += "/";
    sb += id;
    sb += "?";
    sb += ILauncherProvider::PARAMETER_NOTIFY;
    sb += "=";
    sb += notify;
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> tmp;
    return uriHelper->Parse(sb.ToString(), uri);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos