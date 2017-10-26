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

#include "elastos/droid/systemui/keyguard/CKeyguardStatusView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "R.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/text/format/DateFormat.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

static const String DTAG("KeyguardStatusView");
const Boolean CKeyguardStatusView::DEBUG = FALSE;

CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::MyKeyguardUpdateMonitorCallback(
    /* [in] */ CKeyguardStatusView* host)
    : mHost(host)
{}

ECode CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::OnTimeChanged()
{
    mHost->Refresh();
    return NOERROR;
}

ECode CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::OnKeyguardVisibilityChanged(
    /* [in] */ Boolean showing)
{
    if (showing) {
        if (DEBUG) Logger::V(DTAG, "refresh statusview showing: %d", showing);
        mHost->Refresh();
        mHost->UpdateOwnerInfo();
    }
    return NOERROR;
}

ECode CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOn()
{
    mHost->SetEnableMarquee(TRUE);
    return NOERROR;
}

ECode CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::OnScreenTurnedOff(
    /* [in] */ Int32 why)
{
    mHost->SetEnableMarquee(FALSE);
    return NOERROR;
}

ECode CKeyguardStatusView::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    mHost->Refresh();
    mHost->UpdateOwnerInfo();
    return NOERROR;
}

String CKeyguardStatusView::Patterns::mDateView;
String CKeyguardStatusView::Patterns::mClockView12;
String CKeyguardStatusView::Patterns::mClockView24;
String CKeyguardStatusView::Patterns::mCacheKey;

void CKeyguardStatusView::Patterns::Update(
    /* [in] */ IContext* context,
    /* [in] */ Boolean hasAlarm)
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);

    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    String dateViewSkel;
    res->GetString(hasAlarm
            ? R::string::abbrev_wday_month_day_no_year_alarm
            : R::string::abbrev_wday_month_day_no_year, &dateViewSkel);
    String clockView12Skel;
    res->GetString(R::string::clock_12hr_format, &clockView12Skel);
    String clockView24Skel;
    res->GetString(R::string::clock_24hr_format, &clockView24Skel);
    String str;
    locale->ToString(&str);
    String key = str + dateViewSkel + clockView12Skel + clockView24Skel;
    if (key.Equals(mCacheKey)) return;

    mDateView = DateFormat::GetBestDateTimePattern(locale, dateViewSkel);

    mClockView12 = DateFormat::GetBestDateTimePattern(locale, clockView12Skel);
    // CLDR insists on adding an AM/PM indicator even though it wasn't in the skeleton
    // format.  The following code removes the AM/PM indicator if we didn't want it.
    if (!clockView12Skel.Contains("a")) {
        String v;
        StringUtils::ReplaceAll(mClockView12, String("a"), String(""), &v);
        mClockView12 = v.Trim();
    }

    mClockView24 = DateFormat::GetBestDateTimePattern(locale, clockView24Skel);

    // Use fancy colon.
    mClockView24 = mClockView24.Replace(':', 0xee01);
    mClockView12 = mClockView12.Replace(':', 0xee01);

    mCacheKey = key;
}

CAR_OBJECT_IMPL(CKeyguardStatusView)

CAR_INTERFACE_IMPL(CKeyguardStatusView, GridLayout, IKeyguardStatusView)

ECode CKeyguardStatusView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CKeyguardStatusView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CKeyguardStatusView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    mInfoCallback = new MyKeyguardUpdateMonitorCallback(this);
    return GridLayout::constructor(context, attrs, defStyle);
}

void CKeyguardStatusView::SetEnableMarquee(
    /* [in] */ Boolean enabled)
{
    if (DEBUG) Logger::V(DTAG, "%s transport text marquee", (enabled ? "Enable" : "Disable"));
    if (mAlarmStatusView != NULL) IView::Probe(mAlarmStatusView)->SetSelected(enabled);
    if (mOwnerInfo != NULL) IView::Probe(mOwnerInfo)->SetSelected(enabled);
}

ECode CKeyguardStatusView::OnFinishInflate()
{
    GridLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::alarm_status, (IView**)&view);
    mAlarmStatusView = ITextView::Probe(view);

    view = NULL;
    FindViewById(R::id::date_view, (IView**)&view);
    mDateView = ITextClock::Probe(view);

    view = NULL;
    FindViewById(R::id::clock_view, (IView**)&view);
    mClockView = ITextClock::Probe(view);

    view = NULL;
    FindViewById(R::id::owner_info, (IView**)&view);
    mOwnerInfo = ITextView::Probe(view);
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    CLockPatternUtils::New(ctx, (ILockPatternUtils**)&mLockPatternUtils);
    Boolean screenOn = TRUE;
    KeyguardUpdateMonitor::GetInstance(mContext)->IsScreenOn(&screenOn);
    SetEnableMarquee(screenOn);
    Refresh();
    UpdateOwnerInfo();

    // Disable elegant text height because our fancy colon makes the ymin value huge for no
    // reason.
    ITextView::Probe(mClockView)->SetElegantTextHeight(FALSE);
    return NOERROR;
}

ECode CKeyguardStatusView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    GridLayout::OnConfigurationChanged(newConfig);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 v = 0;
    res->GetDimensionPixelSize(R::dimen::widget_big_font_size, &v);
    ITextView::Probe(mClockView)->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
    res->GetDimensionPixelSize(R::dimen::widget_label_font_size, &v);
    ITextView::Probe(mDateView)->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
    res->GetDimensionPixelSize(R::dimen::widget_label_font_size, &v);
    mOwnerInfo->SetTextSize(ITypedValue::COMPLEX_UNIT_PX, v);
    return NOERROR;
}

ECode CKeyguardStatusView::RefreshTime()
{
    AutoPtr<ICharSequence> dv;
    CString::New(Patterns::mDateView, (ICharSequence**)&dv);
    mDateView->SetFormat24Hour(dv);
    mDateView->SetFormat12Hour(dv);

    AutoPtr<ICharSequence> cv;
    CString::New(Patterns::mClockView12, (ICharSequence**)&cv);
    mClockView->SetFormat12Hour(cv);

    cv = NULL;
    CString::New(Patterns::mClockView24, (ICharSequence**)&cv);
    mClockView->SetFormat24Hour(cv);
    return NOERROR;
}

void CKeyguardStatusView::Refresh()
{
    AutoPtr<IAlarmClockInfo> nextAlarm;
    mLockPatternUtils->GetNextAlarm((IAlarmClockInfo**)&nextAlarm);
    Patterns::Update(mContext, nextAlarm != NULL);

    RefreshTime();
    RefreshAlarmStatus(nextAlarm);
}

void CKeyguardStatusView::RefreshAlarmStatus(
    /* [in] */ IAlarmClockInfo* nextAlarm)
{
    if (nextAlarm != NULL) {
        String alarm = FormatNextAlarm(mContext, nextAlarm);
        AutoPtr<ICharSequence> cs;
        CString::New(alarm, (ICharSequence**)&cs);
        mAlarmStatusView->SetText(cs);

        AutoPtr<IResources> res;
        GetResources((IResources**)&res);

        AutoPtr<ArrayOf<IInterface*> > objs = ArrayOf<IInterface*>::Alloc(1);
        objs->Set(0, cs);
        String v;
        res->GetString(R::string::keyguard_accessibility_next_alarm, objs, &v);

        cs = NULL;
        CString::New(v, (ICharSequence**)&cs);
        IView::Probe(mAlarmStatusView)->SetContentDescription(cs);
        IView::Probe(mAlarmStatusView)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(mAlarmStatusView)->SetVisibility(IView::GONE);
    }
}

String CKeyguardStatusView::FormatNextAlarm(
    /* [in] */ IContext* context,
    /* [in] */ IAlarmClockInfo* info)
{
    if (info == NULL) {
        return String("");
    }
    String skeleton = DateFormat::Is24HourFormat(context) ? String("EHm") : String("Ehma");

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);

    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    String pattern = DateFormat::GetBestDateTimePattern(locale, skeleton);

    Int64 t = 0;
    info->GetTriggerTime(&t);
    AutoPtr<ICharSequence> in;
    CString::New(pattern, (ICharSequence**)&in);
    AutoPtr<ICharSequence> out = DateFormat::Format(in, t);
    String v;
    out->ToString(&v);
    return v;
}

void CKeyguardStatusView::UpdateOwnerInfo()
{
    if (mOwnerInfo == NULL) return;
    String ownerInfo = GetOwnerInfo();
    if (!TextUtils::IsEmpty(ownerInfo)) {
        IView::Probe(mOwnerInfo)->SetVisibility(IView::VISIBLE);
        AutoPtr<ICharSequence> cs;
        CString::New(ownerInfo, (ICharSequence**)&cs);
        mOwnerInfo->SetText(cs);
    }
    else {
        IView::Probe(mOwnerInfo)->SetVisibility(IView::GONE);
    }
}

ECode CKeyguardStatusView::OnAttachedToWindow()
{
    GridLayout::OnAttachedToWindow();
    KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mInfoCallback);
    return NOERROR;
}

ECode CKeyguardStatusView::OnDetachedFromWindow()
{
    GridLayout::OnDetachedFromWindow();
    KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mInfoCallback);
    return NOERROR;
}

ECode CKeyguardStatusView::GetAppWidgetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = ILockPatternUtils::ID_DEFAULT_STATUS_WIDGET;
    return NOERROR;
}

String CKeyguardStatusView::GetOwnerInfo()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> res;
    ctx->GetContentResolver((IContentResolver**)&res);
    String info;
    Boolean ownerInfoEnabled;
    mLockPatternUtils->IsOwnerInfoEnabled(&ownerInfoEnabled);
    if (ownerInfoEnabled) {
        Int32 user = 0;
        mLockPatternUtils->GetCurrentUser(&user);
        mLockPatternUtils->GetOwnerInfo(user, &info);
    }
    return info;
}

ECode CKeyguardStatusView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
