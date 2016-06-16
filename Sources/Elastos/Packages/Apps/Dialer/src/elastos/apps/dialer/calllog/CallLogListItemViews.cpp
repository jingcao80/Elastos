
#include "elastos/apps/dialer/calllog/CallLogListItemViews.h"

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CallLogListItemViews, Object, ICallLogListItemViews);

CallLogListItemViews::CallLogListItemViews(
    /* [in] */ IQuickContactBadge* quickContactView,
    /* [in] */ IView* primaryActionView,
    /* [in] */ IPhoneCallDetailsViews* phoneCallDetailsViews,
    /* [in] */ IView* callLogEntryView,
    /* [in] */ ITextView* dayGroupHeader)
    : mQuickContactView(quickContactView)
    , mPrimaryActionView(primaryActionView)
    , mPhoneCallDetailsViews(phoneCallDetailsViews)
    , mDayGroupHeader(dayGroupHeader)
    , mCallLogEntryView(callLogEntryView)
{}

AutoPtr<ICallLogListItemViews> CallLogListItemViews::FromView(
    /* [in] */ IView* view)
{
    AutoPtr<IView> contactPhoto;
    view->FindViewById(R::id::quick_contact_photo, (IView**)&contactPhoto);
    AutoPtr<IView> actionView;
    view->FindViewById(R::id::primary_action_view, (IView**)&actionView);
    AutoPtr<IView> logRow;
    view->FindViewById(R::id::call_log_row, (IView**)&logRow);
    AutoPtr<IView> groupLable;
    view->FindViewById(R::id::call_log_day_group_label, (IView**)&groupLable);
    AutoPtr<CallLogListItemViews> views = new CallLogListItemViews(
                IQuickContactBadge::Probe(contactPhoto),
                actionView,
                PhoneCallDetailsViews::FromView(view),
                logRow,
                ITextView::Probe(groupLable));

    return (ICallLogListItemViews*)views;
}

AutoPtr<ICallLogListItemViews> CallLogListItemViews::CreateForTest(
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

    return (ICallLogListItemViews*)views;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
