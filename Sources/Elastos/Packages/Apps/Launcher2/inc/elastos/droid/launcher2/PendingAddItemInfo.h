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

#ifndef  __ELASTOS_DROID_LAUNCHER2_PENDINGADDITEMINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_PENDINGADDITEMINFO_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * We pass this object with a drag from the customization tray
 */
class PendingAddItemInfo
    : public ItemInfo
    , public IPendingAddItemInfo
{
public:
    CAR_INTERFACE_DECL();

    PendingAddItemInfo() {}

    /**
     * The component that will be created.
     */
    AutoPtr<IComponentName> mComponentName;
};

class PendingAddShortcutInfo
    : public PendingAddItemInfo
    , public IPendingAddShortcutInfo
{
public:
    CAR_INTERFACE_DECL();

    PendingAddShortcutInfo() {}

    CARAPI constructor(
        /* [in] */ IActivityInfo* activityInfo);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IActivityInfo> mShortcutActivityInfo;
};

class PendingAddWidgetInfo
    : public PendingAddItemInfo
    , public IPendingAddWidgetInfo
{
public:
    CAR_INTERFACE_DECL();

    PendingAddWidgetInfo();

    CARAPI constructor(
        /* [in] */ IAppWidgetProviderInfo* i,
        /* [in] */ const String& dataMimeType,
        /* [in] */ IParcelable* data);

    // Copy constructor
    CARAPI constructor(
        /* [in] */ PendingAddWidgetInfo* copy);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IAppWidgetProviderInfo> mInfo;
    AutoPtr<IAppWidgetHostView> mBoundWidget;
    AutoPtr<IBundle> mBindOptions;

    // Any configuration data that we want to pass to a configuration activity when
    // starting up a widget
    String mMimeType;
    AutoPtr<IParcelable> mConfigurationData;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_PENDINGADDITEMINFO_H__