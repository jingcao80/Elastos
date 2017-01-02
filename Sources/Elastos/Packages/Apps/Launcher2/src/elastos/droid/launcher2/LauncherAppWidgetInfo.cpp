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

#include "elastos/droid/launcher2/LauncherAppWidgetInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "elastos/droid/launcher2/AppWidgetResizeFrame.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const Int32 LauncherAppWidgetInfo::NO_ID = -1;

CAR_INTERFACE_IMPL(LauncherAppWidgetInfo, ItemInfo, ILauncherAppWidgetInfo);

LauncherAppWidgetInfo::LauncherAppWidgetInfo()
    : mAppWidgetId(NO_ID)
    , mMinWidth(-1)
    , mMinHeight(-1)
    , mHasNotifiedInitialWidgetSizeChanged(FALSE)
{
}

ECode LauncherAppWidgetInfo::constructor(
    /* [in] */ Int32 appWidgetId,
    /* [in] */ IComponentName* providerName)
{
    ItemInfo::constructor();
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET;
    mAppWidgetId = appWidgetId;
    mProviderName = providerName;

    // Since the widget isn't instantiated yet, we don't know these values. Set them to -1
    // to indicate that they should be calculated based on the layout and minWidth/minHeight
    mSpanX = -1;
    mSpanY = -1;
    return NOERROR;
}

ECode LauncherAppWidgetInfo::OnAddToDatabase(
    /* [in] */ IContext* context,
    /* [in] */ IContentValues* values)
{
    ItemInfo::OnAddToDatabase(context, values);
    return values->Put(LauncherSettings::Favorites::APPWIDGET_ID, mAppWidgetId);
}

ECode LauncherAppWidgetInfo::OnBindAppWidget(
    /* [in] */ ILauncher* launcher)
{
    if (!mHasNotifiedInitialWidgetSizeChanged) {
        return NotifyWidgetSizeChanged(launcher);
    }
    return NOERROR;
}

ECode LauncherAppWidgetInfo::NotifyWidgetSizeChanged(
    /* [in] */ ILauncher* launcher)
{
    AppWidgetResizeFrame::UpdateWidgetSizeRanges(mHostView, launcher, mSpanX, mSpanY);
    mHasNotifiedInitialWidgetSizeChanged = TRUE;
    return NOERROR;
}

ECode LauncherAppWidgetInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "AppWidget(id=";
    sb += mAppWidgetId;
    sb += ")";
    return sb.ToString(str);
}

ECode LauncherAppWidgetInfo::Unbind()
{
    ItemInfo::Unbind();
    mHostView = NULL;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos