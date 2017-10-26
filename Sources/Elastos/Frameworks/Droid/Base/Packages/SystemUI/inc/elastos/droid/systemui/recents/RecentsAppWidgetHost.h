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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.AppWidget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/appwidget/AppWidgetHost.h"
#include "elastos/droid/systemui/recents/RecentsConfiguration.h"

using Elastos::Droid::AppWidget::AppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::SystemUI::Recents::IRecentsAppWidgetHostCallbacks;
using Elastos::Droid::SystemUI::Recents::RecentsConfiguration;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {

class RecentsAppWidgetHost
    : public AppWidgetHost
{
public:
    RecentsAppWidgetHost(
        /* [in] */ IContext* context,
        /* [in] */ Int32 hostId);

    using AppWidgetHost::StartListening;

    CARAPI StartListening(
        /* [in] */ IRecentsAppWidgetHostCallbacks* cb);

    // @Override
    CARAPI StopListening();

    CARAPI IsListening(
        /* [out] */ Boolean* result);
protected:
    // @Override
    CARAPI OnProviderChanged(
        /* [in] */ Int32 appWidgetId,
        /* [in] */ IAppWidgetProviderInfo* appWidgetInfo);

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IRecentsAppWidgetHostCallbacks> mCb;
    AutoPtr<RecentsConfiguration> mConfig;
    Boolean mIsListening;
};

} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_RECENTSAPPWIDGETHOST_H__