#include "elastos/droid/systemui/statusbar/policy/DateView.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/text/format/DateFormat.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IDate;
using Elastos::Utility::CDate;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::SystemUI::SystemUIR;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

DateView::DateViewBroadcastReceiver::DateViewBroadcastReceiver(
    /* [in] */ DateView* host)
    : mHost(host)
{}

ECode DateView::DateViewBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Slogger::V("DateViewBroadcastReceiver", " >><< DateView::DateViewBroadcastReceiver::OnReceive");

    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_TIME_TICK.Equals(action)
            || IIntent::ACTION_TIME_CHANGED.Equals(action)
            || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)) {
        mHost->UpdateClock();
    }
    return NOERROR;
}

const String DateView::TAG("DateView");
const Int32 DateView::DATE_TEXTVIEW_SCALER = 18;

DateView::DateView()
    : mAttachedToWindow(FALSE)
    , mWindowVisible(FALSE)
    , mUpdating(FALSE)
{
}

DateView::DateView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : TextView(context, attrs)
    , mAttachedToWindow(FALSE)
    , mWindowVisible(FALSE)
    , mUpdating(FALSE)
{
    mIntentReceiver = new DateViewBroadcastReceiver(this);
    SetTextSize(DATE_TEXTVIEW_SCALER);
}

ECode DateView::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    TextView::Init(context, attrs);
    return SetTextSize(DATE_TEXTVIEW_SCALER);
}

ECode DateView::OnAttachedToWindow()
{
    TextView::OnAttachedToWindow();
    mAttachedToWindow = TRUE;
    SetUpdates();
    return NOERROR;
}

ECode DateView::OnDetachedFromWindow()
{
    TextView::OnDetachedFromWindow();
    mAttachedToWindow = FALSE;
    SetUpdates();
    return NOERROR;
}

void DateView::OnWindowVisibilityChanged(
    /* [in] */ Int32 visibility)
{
    TextView::OnWindowVisibilityChanged(visibility);
    mWindowVisible = visibility == IView::VISIBLE;
    SetUpdates();
}

ECode DateView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    TextView::OnVisibilityChanged(changedView, visibility);
    SetUpdates();
    return NOERROR;
}

Int32 DateView::GetSuggestedMinimumWidth()
{
    // makes the large background bitmap not force us to full width
    return 0;
}

void DateView::UpdateClock()
{
    AutoPtr<IContext> context = GetContext();
    String dateFormat;
    context->GetString(SystemUIR::string::abbrev_wday_month_day_no_year, &dateFormat);

    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    AutoPtr<ICharSequence> text = DateFormat::Format(dateFormat, date);
    SetText(text);
}

Boolean DateView::IsVisible()
{
    AutoPtr<IView> v = THIS_PROBE(IView);
    Int32 visibility;
    while (TRUE) {
        v->GetVisibility(&visibility);
        if (visibility != IView::VISIBLE) {
            return FALSE;
        }

        AutoPtr<IViewParent> parent;
        v->GetParent((IViewParent**)&parent);
        v = IView::Probe(parent.Get());
        if (!v) {
            return TRUE;
        }
    }
}

void DateView::SetUpdates()
{
    Boolean update = mAttachedToWindow && mWindowVisible && IsVisible();
    if (update != mUpdating) {
        mUpdating = update;
        if (update) {
            // Register for Intent broadcasts for the clock and battery
            AutoPtr<IIntentFilter> filter;
            CIntentFilter::New((IIntentFilter**)&filter);
            filter->AddAction(IIntent::ACTION_TIME_TICK);
            filter->AddAction(IIntent::ACTION_TIME_CHANGED);
            filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
            AutoPtr<IIntent> stickyIntent;
            String nullStr;
            mContext->RegisterReceiver(mIntentReceiver, filter, nullStr, NULL,
                (IIntent**)&stickyIntent);
            UpdateClock();
        }
        else {
            mContext->UnregisterReceiver(mIntentReceiver);
        }
    }
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
