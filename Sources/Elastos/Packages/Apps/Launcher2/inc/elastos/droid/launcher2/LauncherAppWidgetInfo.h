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

#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETINFO_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETINFO_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Represents a widget (either instantiated or about to be) in the Launcher.
 */
class LauncherAppWidgetInfo
    : public ItemInfo
    , public ILauncherAppWidgetInfo
{
public:
    CAR_INTERFACE_DECL();

    LauncherAppWidgetInfo();

    CARAPI constructor(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IComponentName* providerName);

    //@Override
    CARAPI OnAddToDatabase(
        /* [in] */ IContext* context,
        /* [in] */ IContentValues* values);

    /**
     * When we bind the widget, we should notify the widget that the size has changed if we have not
     * done so already (only really for default workspace widgets).
     */
    CARAPI OnBindAppWidget(
        /* [in] */ ILauncher* launcher);

    /**
     * Trigger an update callback to the widget to notify it that its size has changed.
     */
    CARAPI NotifyWidgetSizeChanged(
        /* [in] */ ILauncher* launcher);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    //@Override
    CARAPI Unbind();

public:
    /**
     * Indicates that the widget hasn't been instantiated yet.
     */
    static const Int32 NO_ID;

    /**
     * Identifier for this widget when talking with
     * {@link android.appwidget.AppWidgetManager} for updates.
     */
    Int32 mAppWidgetId;

    AutoPtr<IComponentName> mProviderName;

    // TODO: Are these necessary here?
    Int32 mMinWidth;
    Int32 mMinHeight;

    /**
     * View that holds this widget after it's been created.  This view isn't created
     * until Launcher knows it's needed.
     */
    AutoPtr<IAppWidgetHostView> mHostView;

private:
    Boolean mHasNotifiedInitialWidgetSizeChanged;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETINFO_H__