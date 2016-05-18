
#include "elastos/droid/systemui/statusbar/policy/CSplitClockView.h"
#include "../../R.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Character.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::IDateFormat;
using Elastos::Droid::View::IView;
using Elastos::Core::CString;
using Elastos::Core::Character;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CSplitClockView::IntentReceiver::IntentReceiver(
    /* [in] */ CSplitClockView* host)
    : mHost(host)
{}

ECode CSplitClockView::IntentReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_TIME_CHANGED.Equals(action)
            || IIntent::ACTION_TIMEZONE_CHANGED.Equals(action)
            || IIntent::ACTION_LOCALE_CHANGED.Equals(action)) {
        mHost->UpdatePatterns();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CSplitClockView, LinearLayout, ISplitClockView)
CSplitClockView::CSplitClockView()
{
    mIntentReceiver = new IntentReceiver(this);
}

ECode CSplitClockView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LinearLayout::constructor(context, attrs);
}

ECode CSplitClockView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::time_view, (IView**)&view);
    mTimeView = ITextClock::Probe(view);

    view = NULL;
    FindViewById(R::id::am_pm_view, (IView**)&view);
    mAmPmView = ITextClock::Probe(view);
    return NOERROR;
}

ECode CSplitClockView::OnAttachedToWindow()
{
    LinearLayout::OnAttachedToWindow();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    filter->AddAction(IIntent::ACTION_LOCALE_CHANGED);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IIntent> i;
    context->RegisterReceiver(mIntentReceiver, filter, String(NULL), NULL, (IIntent**)&i);

    UpdatePatterns();
    return NOERROR;
}

ECode CSplitClockView::OnDetachedFromWindow()
{
    LinearLayout::OnDetachedFromWindow();

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    context->UnregisterReceiver(mIntentReceiver);
    return NOERROR;
}

void CSplitClockView::UpdatePatterns()
{
    AutoPtr<IDateFormat> format;
    CDateFormat::AcquireSingleton((IDateFormat**)&format);
    String formatString;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    format->GetTimeFormatString(context, &formatString);
    Int32 index = GetAmPmPartEndIndex(formatString);
    String timeString;
    String amPmString;
    if (index == -1) {
        timeString = formatString;
        amPmString = "";
    } else {
        timeString = formatString.Substring(0, index);
        amPmString = formatString.Substring(index);
    }

    AutoPtr<ICharSequence> cs;
    CString::New(timeString, (ICharSequence**)&cs);
    mTimeView->SetFormat12Hour(cs);
    mTimeView->SetFormat24Hour(cs);

    cs = NULL;
    CString::New(amPmString, (ICharSequence**)&cs);
    mAmPmView->SetFormat12Hour(cs);
    mAmPmView->SetFormat24Hour(cs);
}

Int32 CSplitClockView::GetAmPmPartEndIndex(
    /* [in] */ const String& formatString)
{
    Boolean hasAmPm = FALSE;
    Int32 length = formatString.GetLength();
    for (Int32 i = length - 1; i >= 0; i--) {
        Char32 c = formatString.GetChar(i);
        Boolean isAmPm = c == 'a';
        Boolean isWhitespace = Character::IsWhitespace(c);
        if (isAmPm) {
            hasAmPm = TRUE;
        }
        if (isAmPm || isWhitespace) {
            continue;
        }
        if (i == length - 1) {
            // First character was not AM/PM and not whitespace, so it's not ending with AM/PM.
            return -1;
        } else {

            // If we have AM/PM at all, return last index, or -1 to indicate that it's not
            // ending with AM/PM.
            return hasAmPm ? i + 1 : -1;
        }
    }

    // Only AM/PM and whitespaces? The whole string is AM/PM. Else: Only whitespaces in the
    // string.
    return hasAmPm ? 0 : -1;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
