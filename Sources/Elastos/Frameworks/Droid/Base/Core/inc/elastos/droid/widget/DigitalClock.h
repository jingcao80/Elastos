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

#ifndef __ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__
#define __ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

class DigitalClock
    : public TextView
    , public IDigitalClock
{
private:
    class DigitalClockRunnable
        : public Runnable
    {
    public:
        DigitalClockRunnable(
            /* [in] */ DigitalClock* host);

        CARAPI Run();

    private:
        DigitalClock* mHost;
    };

    class FormatChangeObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("DigitalClock::DigitalClock")

        FormatChangeObserver(
            /* [in] */ DigitalClock* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        DigitalClock* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DigitalClock();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    CARAPI_(void) InitClock();

    CARAPI_(void) SetFormat();

public:
    AutoPtr<ICalendar> mCalendar;
    String mFormat;

private:
    AutoPtr<FormatChangeObserver> mFormatChangeObserver;
    AutoPtr<DigitalClockRunnable> mTicker;
    AutoPtr<IHandler> mHandler;

    Boolean mTickerStopped;
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_DIGITALCLOCK_H__
