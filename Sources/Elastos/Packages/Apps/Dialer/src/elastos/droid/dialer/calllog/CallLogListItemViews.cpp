
#include "elastos/droid/dialer/calllog/CallLogListItemViews.h"
#include "R.h"

using Elastos::Droid::View::CView;
using Elastos::Droid::Widget::CQuickContactBadge;
using Elastos::Droid::Widget::CTextView;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CallLogListItemViews::CallLogListItemViews(
    /* [in] */ IQuickContactBadge* quickContactView,
    /* [in] */ IView* primaryActionView,
    /* [in] */ PhoneCallDetailsViews* phoneCallDetailsViews,
    /* [in] */ IView* callLogEntryView,
    /* [in] */ ITextView* dayGroupHeader)
    : mQuickContactView(quickContactView)
    , mPrimaryActionView(primaryActionView)
    , mPhoneCallDetailsViews(phoneCallDetailsViews)
    , mDayGroupHeader(dayGroupHeader)
    , mCallLogEntryView(callLogEntryView)
    , mRowId(0)
    , mNumberPresentation(0)
    , mCallType(0)
    , mReported(FALSE)
    , mCanBeReportedAsInvalid(FALSE)
{}

AutoPtr<CallLogListItemViews> CallLogListItemViews::FromView(
    /* [in] */ IView* view)
{
    AutoPtr<IView> contactPhoto;
    view->FindViewById(Elastos::Droid::Dialer::R::id::quick_contact_photo, (IView**)&contactPhoto);
    AutoPtr<IView> actionView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::primary_action_view, (IView**)&actionView);
    AutoPtr<IView> logRow;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_log_row, (IView**)&logRow);
    AutoPtr<IView> groupLable;
    view->FindViewById(Elastos::Droid::Dialer::R::id::call_log_day_group_label, (IView**)&groupLable);
    AutoPtr<CallLogListItemViews> views = new CallLogListItemViews(
                IQuickContactBadge::Probe(contactPhoto),
                actionView,
                PhoneCallDetailsViews::FromView(view),
                logRow,
                ITextView::Probe(groupLable));

    return views;
}

AutoPtr<CallLogListItemViews> CallLogListItemViews::CreateForTest(
    /* [in] */ IContext* context)
{
    AutoPtr<IQuickContactBadge> badge;
    CQuickContactBadge::New(context, (IQuickContactBadge**)&badge);
    AutoPtr<IView> primaryActionView;
    CView::New(context, (IView**)&primaryActionView);
    AutoPtr<IView> callLogEntryView;
    CView::New(context, (IView**)&callLogEntryView);
    AutoPtr<ITextView> textView;
    CTextView::New(context, (ITextView**)&textView);
    AutoPtr<CallLogListItemViews> views = new CallLogListItemViews(
            badge, primaryActionView,
            PhoneCallDetailsViews::CreateForTest(context),
            callLogEntryView, textView);

    CTextView::New(context, (ITextView**)&views->mCallBackButtonView);
    CTextView::New(context, (ITextView**)&views->mVoicemailButtonView);
    CTextView::New(context, (ITextView**)&views->mDetailsButtonView);
    CTextView::New(context, (ITextView**)&views->mReportButtonView);
    CView::New(context, (IView**)&views->mActionsView);

    return views;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
