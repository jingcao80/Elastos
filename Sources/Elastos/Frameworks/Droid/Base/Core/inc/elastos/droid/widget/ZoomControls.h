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

#ifndef __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__
#define __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__

#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * The {@code ZoomControls} class displays a simple set of controls used for zooming and
 * provides callbacks to register for events. */
class ZoomControls
    : public LinearLayout
    , public IZoomControls
{
public:
    CAR_INTERFACE_DECL()

    ZoomControls();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetOnZoomInClickListener(
        /* [in] */ IViewOnClickListener* listener);

    CARAPI SetOnZoomOutClickListener(
        /* [in] */ IViewOnClickListener* listener);

    /*
     * Sets how fast you get zoom events when the user holds down the
     * zoom in/out buttons.
     */
    CARAPI SetZoomSpeed(
        /* [in] */ Int64 speed);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI Show();

    CARAPI Hide();

    CARAPI SetIsZoomInEnabled(
        /* [in] */ Boolean isEnabled);

    CARAPI SetIsZoomOutEnabled(
        /* [in] */ Boolean isEnabled);

    // @Override
    CARAPI HasFocus(
        /* [out] */ Boolean* has);

    // @Override
    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    // @Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    CARAPI Fade(
        /* [in] */ Int32 visibility,
        /* [in] */ Float startAlpha,
        /* [in] */ Float endAlpha);

    AutoPtr<IZoomButton> mZoomIn;
    AutoPtr<IZoomButton> mZoomOut;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ZOOMCONTROLS_H__
