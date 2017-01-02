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

#ifndef __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__
#define __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::IPendingResult;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::R;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::View;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * This widget display an analogic clock with two hands for hours and
 * minutes.
 */
class AnalogClock
    : public Elastos::Droid::View::View
    , public IAnalogClock
{
private:
    class IntentBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        IntentBroadcastReceiver(
            /* [in] */ AnalogClock* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        AnalogClock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    AnalogClock();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle,
        /* [in] */ Int32 defStyleRes);

protected:
    virtual CARAPI OnAttachedToWindow();

    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    virtual CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) OnTimeChanged();

    CARAPI_(void) UpdateContentDescription(
        /* [in] */ ITime* time);

private:
    AutoPtr<IntentBroadcastReceiver> mIntentReceiver;
    AutoPtr<ITime> mCalendar;
    AutoPtr<IDrawable> mHourHand;
    AutoPtr<IDrawable> mMinuteHand;
    AutoPtr<IDrawable> mDial;
    Int32 mDialWidth;
    Int32 mDialHeight;
    Boolean mAttached;
    AutoPtr<IHandler> mHandler;
    Float mMinutes;
    Float mHour;
    Boolean mChanged;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_ANALOGCLOCK_H__

