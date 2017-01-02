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

#ifndef  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "elastos/droid/appwidget/AppWidgetHostView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"

using Elastos::Droid::AppWidget::AppWidgetHostView;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * {@inheritDoc}
 */
class LauncherAppWidgetHostView
    : public AppWidgetHostView
    , public ILauncherAppWidgetHostView
{
public:
    CAR_INTERFACE_DECL();

    TO_STRING_IMPL("LauncherAppWidgetHostView")

    LauncherAppWidgetHostView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    //@Override
    CARAPI UpdateAppWidget(
        /* [in] */ IRemoteViews* remoteViews);

    CARAPI OrientationChangedSincedInflation(
        /* [out] */ Boolean* result);

    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CancelLongPress();

    //@Override
    CARAPI GetDescendantFocusability(
        /* [out] */ Int32* result);

protected:
    //@Override
    CARAPI GetErrorView(
        /* [out] */ IView** view);

private:
    AutoPtr<CheckLongPressHelper> mLongPressHelper;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IContext> mContext;
    Int32 mPreviousOrientation;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_LAUNCHERAPPWIDGETHOSTVIEW_H__