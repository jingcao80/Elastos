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

#ifndef __ELASTOS_DROID_WIDGET_DATETIMEVIEW_H__
#define __ELASTOS_DROID_WIDGET_DATETIMEVIEW_H__

#include <Elastos.CoreLibrary.h>
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TextView.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::TextView;

using Elastos::Text::IDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::IDate;

namespace Elastos {
namespace Droid {
namespace Widget {

class DateTimeView
    : public TextView
    , public IDateTimeView
{
private:
    class DateTimeViewReceiver
        : public BroadcastReceiver
    {
    public:
        DateTimeViewReceiver(
            /* [in] */ DateTimeView* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("DateTimeView::DateTimeViewReceiver: ")

    private:
        DateTimeView* mHost;
    };

    class DateTimeViewObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("DateTimeView::DateTimeViewObserver")

        DateTimeViewObserver(
            /* [in] */ DateTimeView* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        DateTimeView* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DateTimeView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetTime(
        /* [in] */ Int64 time);

    CARAPI Update();

protected:
    CARAPI OnAttachedToWindow();

    CARAPI OnDetachedFromWindow();

private:
    CARAPI_(AutoPtr<IDateFormat>) GetTimeFormat();

    CARAPI_(AutoPtr<IDateFormat>) GetDateFormat();

    CARAPI RegisterReceivers();

    CARAPI UnRegisterReceivers();

private:
    const static Int64 TWELVE_HOURS_IN_MINUTES;
    const static Int64 TWENTY_FOUR_HOURS_IN_MILLIS;

    const static Int32 SHOW_TIME;
    const static Int32 SHOW_MONTH_DAY_YEAR;

    Boolean mAttachedToWindow;
    Int64 mUpdateTimeMillis;

    AutoPtr<IDate> mTime;
    Int64 mTimeMillis;

    Int32 mLastDisplay;
    AutoPtr<IDateFormat> mLastFormat;

    AutoPtr<BroadcastReceiver> mBroadcastReceiver;
    AutoPtr<ContentObserver> mContentObserver;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif
