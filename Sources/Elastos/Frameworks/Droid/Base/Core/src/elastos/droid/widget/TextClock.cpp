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

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include "elastos/droid/widget/TextClock.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/text/format/DateFormat.h"

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleDataHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

TextClock::TextClockContentObserver::TextClockContentObserver(
    /* [in] */ TextClock* host)
    : mHost(host)
{
}

ECode TextClock::TextClockContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->ChooseFormat();
    mHost->OnTimeChanged();
    return NOERROR;
}

ECode TextClock::TextClockContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    mHost->ChooseFormat();
    mHost->OnTimeChanged();
    return NOERROR;
}

TextClock::TextClockBroadcastReceiver::TextClockBroadcastReceiver(
    /* [in] */ TextClock* host)
    : mHost(host)
{}

ECode TextClock::TextClockBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (mHost->mTimeZone.IsNull() && IIntent::ACTION_TIMEZONE_CHANGED.Equals((intent->GetAction(&action), action))) {
        String timeZone;
        intent->GetStringExtra(String("time-zone"), &timeZone);
        mHost->CreateTime(timeZone);
    }
    mHost->OnTimeChanged();
    return NOERROR;
}

TextClock::TextClockRunnable::TextClockRunnable(
    /* [in] */ TextClock* host)
    : mHost(host)
{}

ECode TextClock::TextClockRunnable::Run()
{
    mHost->OnTimeChanged();

    Int64 now = SystemClock::GetUptimeMillis();
    Int64 next = now + (1000 - now % 1000);

    AutoPtr<IHandler> handler;
    mHost->GetHandler((IHandler**)&handler);
    Boolean result = FALSE;
    handler->PostAtTime(mHost->mTicker, next, &result);
    return NOERROR;
}

static AutoPtr<ICharSequence> InitDEFAULT_FORMAT_12_HOUR()
{
    AutoPtr<ICharSequence> csq;
    CString::New(String("h:mm a"), (ICharSequence**)&csq);
    return csq;
}

static AutoPtr<ICharSequence> InitDEFAULT_FORMAT_24_HOUR()
{
    AutoPtr<ICharSequence> csq;
    CString::New(String("H:mm"), (ICharSequence**)&csq);
    return csq;
}

AutoPtr<ICharSequence> TextClock::DEFAULT_FORMAT_12_HOUR = InitDEFAULT_FORMAT_12_HOUR();
AutoPtr<ICharSequence> TextClock::DEFAULT_FORMAT_24_HOUR = InitDEFAULT_FORMAT_24_HOUR();

CAR_INTERFACE_IMPL(TextClock, TextView, ITextClock);
TextClock::TextClock()
    : mHasSeconds(FALSE)
    , mAttached(FALSE)
{
}

ECode TextClock::constructor(
    /* [in] */ IContext* context)
{
    TextView::constructor(context);
    Init();
    return NOERROR;
}

ECode TextClock::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode TextClock::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode TextClock::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    TextView::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<ITypedArray> a;
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TextClock);
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);
    a->GetText(R::styleable::TextClock_format12Hour, (ICharSequence**)&mFormat12);
    a->GetText(R::styleable::TextClock_format24Hour, (ICharSequence**)&mFormat24);
    a->GetString(R::styleable::TextClock_timeZone, &mTimeZone);
    a->Recycle();

    Init();
    return NOERROR;
}

void TextClock::Init()
{
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mFormatChangeObserver = new TextClockContentObserver(this);
    mFormatChangeObserver->constructor(handler);
    mIntentReceiver = new TextClockBroadcastReceiver(this);
    mTicker = new TextClockRunnable(this);

    if (mFormat12 == NULL || mFormat24 == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> res;
        context->GetResources((IResources**)&res);
        AutoPtr<IConfiguration> config;
        res->GetConfiguration((IConfiguration**)&config);
        AutoPtr<ILocale> locale;
        config->GetLocale((ILocale**)&locale);
        AutoPtr<ILocaleDataHelper> helper;
        CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
        AutoPtr<ILocaleData> ld;
        helper->Get(locale, (ILocaleData**)&ld);
        if (mFormat12 == NULL) {
            String format;
            ld->GetTimeFormat12(&format);
            CString::New(format, (ICharSequence**)&mFormat12);
        }
        if (mFormat24 == NULL) {
            String format;
            ld->GetTimeFormat24(&format);
            CString::New(format, (ICharSequence**)&mFormat24);
        }
    }

    CreateTime(mTimeZone);
    // Wait until onAttachedToWindow() to handle the ticker
    ChooseFormat(FALSE);
}

void TextClock::CreateTime(
    /* [in] */ const String& timeZone)
{
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    if (timeZone != NULL) {
        AutoPtr<ITimeZoneHelper> timeZoneHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&timeZoneHelper);
        AutoPtr<ITimeZone> zone;
        timeZoneHelper->GetTimeZone(timeZone, (ITimeZone**)&zone);
        helper->GetInstance(zone, (ICalendar**)&mTime);
    } else {
        helper->GetInstance((ICalendar**)&mTime);
    }
}

ECode TextClock::GetFormat12Hour(
    /* [out] */ ICharSequence** hour)
{
    VALIDATE_NOT_NULL(hour);
    *hour = mFormat12;
    REFCOUNT_ADD(*hour);
    return NOERROR;
}

ECode TextClock::SetFormat12Hour(
    /* [in] */ ICharSequence* format)
{
    mFormat12 = format;

    ChooseFormat();
    OnTimeChanged();
    return NOERROR;
}

ECode TextClock::GetFormat24Hour(
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);
    *seq = mFormat24;
    REFCOUNT_ADD(*seq);
    return NOERROR;
}

ECode TextClock::SetFormat24Hour(
    /* [in] */ ICharSequence* format)
{
    mFormat24 = format;

    ChooseFormat();
    OnTimeChanged();
    return NOERROR;
}

ECode TextClock::Is24HourModeEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    *result = DateFormat::Is24HourFormat(context);
    return NOERROR;
}

ECode TextClock::GetTimeZone(
    /* [out] */ String* zone)
{
    VALIDATE_NOT_NULL(zone);
    *zone = mTimeZone;
    return NOERROR;
}

ECode TextClock::SetTimeZone(
    /* [in] */ const String& timeZone)
{
    mTimeZone = timeZone;

    CreateTime(timeZone);
    OnTimeChanged();
    return NOERROR;
}

ECode TextClock::ChooseFormat()
{
    ChooseFormat(TRUE);
    return NOERROR;
}

ECode TextClock::GetFormat(
    /* [out] */ ICharSequence** format)
{
    VALIDATE_NOT_NULL(format);
    *format = mFormat;
    REFCOUNT_ADD(*format);
    return NOERROR;
}

void TextClock::ChooseFormat(
    /* [in] */ Boolean handleTicker)
{
    Boolean format24Requested = FALSE;
    Is24HourModeEnabled(&format24Requested);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);

    AutoPtr<ILocaleDataHelper> helper;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&helper);
    AutoPtr<ILocaleData> ld;
    helper->Get(locale, (ILocaleData**)&ld);

    AutoPtr<ICharSequence> format;
    if (format24Requested) {
        String timeFormat24;
        ld->GetTimeFormat24(&timeFormat24);
        CString::New(timeFormat24, (ICharSequence**)&format);
        mFormat = Abc(mFormat24, mFormat12, format);
    }
    else {
        String timeFormat12;
        ld->GetTimeFormat12(&timeFormat12);
        CString::New(timeFormat12, (ICharSequence**)&format);
        mFormat = Abc(mFormat12, mFormat24, format);
    }

    Boolean hadSeconds = mHasSeconds;
    mHasSeconds = DateFormat::HasSeconds(mFormat);

    if (handleTicker && mAttached && hadSeconds != mHasSeconds) {
        AutoPtr<IHandler> handler;
        GetHandler((IHandler**)&handler);
        if (hadSeconds) {
            handler->RemoveCallbacks(mTicker);
        }
        else {
            mTicker->Run();
        }
    }
}

AutoPtr<ICharSequence> TextClock::Abc(
    /* [in] */ ICharSequence* a,
    /* [in] */ ICharSequence* b,
    /* [in] */ ICharSequence* c)
{
    return a == NULL ? (b == NULL ? c : b) : a;
}

ECode TextClock::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();

    if (!mAttached) {
        mAttached = TRUE;

        RegisterReceiver();
        RegisterObserver();

        CreateTime(mTimeZone);

        if (mHasSeconds) {
            mTicker->Run();
        }
        else {
            OnTimeChanged();
        }
    }
    return NOERROR;
}

ECode TextClock::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();

    if (mAttached) {
        UnregisterReceiver();
        UnregisterObserver();

        AutoPtr<IHandler> handler;
        GetHandler((IHandler**)&handler);
        handler->RemoveCallbacks(mTicker);

        mAttached = FALSE;
    }
    return NOERROR;
}

void TextClock::RegisterReceiver()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);

    filter->AddAction(IIntent::ACTION_TIME_TICK);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IHandler> handler;
    GetHandler((IHandler**)&handler);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mIntentReceiver, filter, String(NULL), handler, (IIntent**)&intent);
}

void TextClock::RegisterObserver()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->RegisterContentObserver(Settings::System::CONTENT_URI, TRUE, mFormatChangeObserver);
}

void TextClock::UnregisterReceiver()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mIntentReceiver);
}

void TextClock::UnregisterObserver()
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mFormatChangeObserver);
}

void TextClock::OnTimeChanged()
{
    Int64 currentTimeMillis;
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&currentTimeMillis);
    mTime->SetTimeInMillis(currentTimeMillis);
    SetText(DateFormat::Format(mFormat, mTime));
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
