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

#ifndef __ELASTOS_DROID_WIDGET_VIEWFLIPPER_H__
#define __ELASTOS_DROID_WIDGET_VIEWFLIPPER_H__

#include "elastos/droid/widget/ViewAnimator.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Widget {

class ECO_PUBLIC ViewFlipper
    : public ViewAnimator
    , public IViewFlipper
{
private:
    class ECO_LOCAL MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ViewFlipper::MyHandler")

        MyHandler(
            /* [in] */ ViewFlipper* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        ViewFlipper* mHost;
    };

    class ECO_LOCAL FlipperReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("ViewFlipper::FlipperReceiver")

        FlipperReceiver(
            /* [in] */ ViewFlipper* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        ViewFlipper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ViewFlipper();

    CARAPI constructor(
        /* [in] */ IContext* ctx);

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    /**
     * How long to wait before flipping to the next view
     *
     * @param milliseconds
     *            time in milliseconds
     */
    CARAPI SetFlipInterval(
        /* [in] */ Int32 milliseconds);

    /**
     * Start a timer to cycle through child views
     */
    CARAPI StartFlipping();

    /**
     * No more flips
     */
    CARAPI StopFlipping();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Returns TRUE if the child views are flipping.
     */
    CARAPI IsFlipping(
        /* [out] */ Boolean* flipping);

    /**
     * Set if this view automatically calls {@link #startFlipping()} when it
     * becomes attached to a window.
     */
    CARAPI SetAutoStart(
        /* [in] */ Boolean autoStart);

    /**
     * Returns TRUE if this view automatically calls {@link #startFlipping()}
     * when it becomes attached to a window.
     */
    CARAPI IsAutoStart(
        /* [out] */ Boolean* start);

protected:
    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI OnWindowVisibilityChanged(
        /* [in] */ Int32 visibility);
    /**
     * Internal method to start or stop dispatching flip {@link Message} based
     * on {@link #mRunning} and {@link #mVisible} state.
     */
    CARAPI_(void) UpdateRunning();

    /**
     * Internal method to start or stop dispatching flip {@link Message} based
     * on {@link #mRunning} and {@link #mVisible} state.
     *
     * @param flipNow Determines whether or not to execute the animation now, in
     *            addition to queuing future flips. If omitted, defaults to
     *            TRUE.
     */
    CARAPI_(void) UpdateRunning(
        /* [in] */ Boolean flipNow);

protected:
    static String TAG;
    static const Boolean LOGD;

    static const Int32 DEFAULT_INTERVAL;

    AutoPtr<BroadcastReceiver> mReceiver;
    Int32 mFlipInterval;
    Boolean mAutoStart;

    Boolean mRunning;
    Boolean mStarted;
    Boolean mVisible;
    Boolean mUserPresent;
    static const Int32 FLIP_MSG;
    AutoPtr<IHandler> mHandler;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_VIEWFLIPPER_H__
