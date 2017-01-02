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

#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"

using Elastos::Droid::AppWidget::AppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetHostView;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Specific {@link AppWidgetHost} that creates our {@link LauncherAppWidgetHostView}
 * which correctly captures all long-press events. This ensures that users can
 * always pick up and move widgets.
 */
class LauncherAppWidgetHost
    : public AppWidgetHost
    , public ILauncherAppWidgetHost
{
public:
    CAR_INTERFACE_DECL();

    LauncherAppWidgetHost(
        /* [in] */ ILauncher* launcher,
        /* [in] */ Int32 hostId);

    //@Override
    CARAPI StopListening();

protected:
    //@Override
    CARAPI_(AutoPtr<IAppWidgetHostView>) OnCreateView(
        /* [in] */ IContext* context,
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidget);

    CARAPI_(void) OnProvidersChanged();

public:
    ILauncher* mLauncher;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOST_H__