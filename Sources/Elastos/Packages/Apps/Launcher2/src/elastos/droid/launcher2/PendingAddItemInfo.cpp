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

#include "elastos/droid/launcher2/PendingAddItemInfo.h"
#include "elastos/droid/launcher2/LauncherSettings.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICloneable;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(PendingAddItemInfo, ItemInfo, IPendingAddItemInfo);

CAR_INTERFACE_IMPL(PendingAddShortcutInfo, PendingAddItemInfo,
        IPendingAddShortcutInfo);

ECode PendingAddShortcutInfo::constructor(
    /* [in] */ IActivityInfo* activityInfo)
{
    ItemInfo::constructor();
    mShortcutActivityInfo = activityInfo;
    return NOERROR;
}

ECode PendingAddShortcutInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String name;
    IPackageItemInfo::Probe(mShortcutActivityInfo)->GetPackageName(&name);
    StringBuilder sb;
    sb += "Shortcut: ";
    sb += name;
    return sb.ToString(str);
}

CAR_INTERFACE_IMPL(PendingAddWidgetInfo, PendingAddItemInfo,
        IPendingAddWidgetInfo);

PendingAddWidgetInfo::PendingAddWidgetInfo()
    : mMimeType(NULL)
{
}

ECode PendingAddWidgetInfo::constructor(
    /* [in] */ IAppWidgetProviderInfo* i,
    /* [in] */ const String& dataMimeType,
    /* [in] */ IParcelable* data)
{
    ItemInfo::constructor();
    mItemType = LauncherSettings::Favorites::ITEM_TYPE_APPWIDGET;
    mInfo = i;
    i->GetProvider((IComponentName**)&mComponentName);
    if (!dataMimeType.IsNull() && data != NULL) {
        mMimeType = dataMimeType;
        mConfigurationData = data;
    }
    return NOERROR;
}

ECode PendingAddWidgetInfo::constructor(
    /* [in] */ PendingAddWidgetInfo* copy)
{
    ItemInfo::constructor();
    mInfo = copy->mInfo;
    mBoundWidget = copy->mBoundWidget;
    mMimeType = copy->mMimeType;
    mConfigurationData = copy->mConfigurationData;
    mComponentName = copy->mComponentName;
    mItemType = copy->mItemType;
    mSpanX = copy->mSpanX;
    mSpanY = copy->mSpanY;
    mMinSpanX = copy->mMinSpanX;
    mMinSpanY = copy->mMinSpanY;
    if (copy->mBindOptions == NULL) {
        mBindOptions = NULL;
    }
    else{
        AutoPtr<IInterface> obj;
        ICloneable::Probe(copy->mBindOptions)->Clone((IInterface**)&obj);
        mBindOptions = IBundle::Probe(obj);
    }
    return NOERROR;
}

ECode PendingAddWidgetInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    String name;
    mComponentName->ToShortString(&name);
    *str = String("Widget: ") + name;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos