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

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/DigitalClock.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/net/CUriHelper.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;

using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CString;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

///////////////////////////////////////////////////////////////////////////////
//                     DigitalClock::DigitalClockRunnable
///////////////////////////////////////////////////////////////////////////////

DigitalClock::DigitalClockRunnable::DigitalClockRunnable(
    /* [in] */ DigitalClock* host)
    :mHost(host)
{
}

ECode DigitalClock::DigitalClockRunnable::Run()
{
    if (mHost->mTickerStopped) return NOERROR;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now = 0;
    system->GetCurrentTimeMillis(&now);
    mHost->mCalendar->SetTimeInMillis(now);
    AutoPtr<ICharSequence> csq;
    CString::New(mHost->mFormat, (ICharSequence**)&csq);
    mHost->SetText(DateFormat::Format(csq, mHost->mCalendar));
    mHost->Invalidate();
    now = SystemClock::GetUptimeMillis();
    Int64 next = now + (1000 - now % 1000);
    Boolean res;
    mHost->mHandler->PostAtTime(mHost->mTicker, next, &res);
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
//                     DigitalClock::FormatChangeObserver
///////////////////////////////////////////////////////////////////////////////
DigitalClock::FormatChangeObserver::FormatChangeObserver(
    /* [in] */ DigitalClock* host)
    : mHost(host)
{
}

ECode DigitalClock::FormatChangeObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->SetFormat();
    return NOERROR;
}

///////////////////////////////////////////////////////////////////////////////
//                     DigitalClock::
///////////////////////////////////////////////////////////////////////////////
CAR_INTERFACE_IMPL(DigitalClock, TextView, IDigitalClock)

DigitalClock::DigitalClock()
    : mTickerStopped(FALSE)
{
}

ECode DigitalClock::constructor(
        /* [in] */ IContext* context)
{
    TextView::constructor(context);
    InitClock();
    return NOERROR;
}

ECode DigitalClock::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    TextView::constructor(context, attrs);
    InitClock();
    return NOERROR;
}

void DigitalClock::InitClock()
{
    if (mCalendar == NULL) {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        helper->GetInstance((ICalendar**)&mCalendar);
    }

    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mFormatChangeObserver = new FormatChangeObserver(this);
    mFormatChangeObserver->constructor(handler);

    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    AutoPtr<IContentResolver> resolver;
    cxt->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(String("content://settings/system"), (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, mFormatChangeObserver);
    SetFormat();
}

ECode DigitalClock::OnAttachedToWindow()
{
    mTickerStopped = FALSE;
    TextView::OnAttachedToWindow();
    CHandler::New((IHandler**)&mHandler);
    mTicker = new DigitalClockRunnable(this);
    mTicker->Run();
    return NOERROR;
}

ECode DigitalClock::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    mTickerStopped = TRUE;
    return NOERROR;
}

void DigitalClock::SetFormat()
{
    AutoPtr<IContext> cxt;
    GetContext((IContext**)&cxt);
    using Elastos::Droid::Text::Format::IDateFormat;
    AutoPtr<IDateFormat> df;
    CDateFormat::AcquireSingleton((IDateFormat**)&df);
    df->GetTimeFormatString(cxt, &mFormat);
}

ECode DigitalClock::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    TextView::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("DigitalClock"), (ICharSequence**)&seq));
    IAccessibilityRecord::Probe(event)->SetClassName(seq);
    return NOERROR;
}

ECode DigitalClock::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    TextView::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    FAIL_RETURN(CString::New(String("DigitalClock"), (ICharSequence**)&seq));
    info->SetClassName(seq);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
