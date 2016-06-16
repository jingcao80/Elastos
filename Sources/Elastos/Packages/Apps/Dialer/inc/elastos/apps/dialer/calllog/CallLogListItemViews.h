#ifndef __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMVIEWS_H__
#define __ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMVIEWS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Telecomm.h"

using Elastos::Droid::Widget::IQuickContactBadge;
using Elastos::Droid::Telecomm::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

class CallLogListItemViews
    : Object
    , ICallLogListItemViews
{
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<ICallLogListItemViews>) FromView(
        /* [in] */ IView* view);

    static CARAPI_(AutoPtr<ICallLogListItemViews>) CreateForTest(
        /* [in] */ IContext* context);

private:
    CallLogListItemViews(
        /* [in] */ IQuickContactBadge* quickContactView,
        /* [in] */ IView* primaryActionView,
        /* [in] */ IPhoneCallDetailsViews* phoneCallDetailsViews,
        /* [in] */ IView* callLogEntryView,
        /* [in] */ ITextView* dayGroupHeader);

public:
    /** The quick contact badge for the contact. */
    AutoPtr<IQuickContactBadge> mQuickContactView;
    /** The primary action view of the entry. */
    AutoPtr<IView> mPrimaryActionView;
    /** The details of the phone call. */
    AutoPtr<IPhoneCallDetailsViews> mPhoneCallDetailsViews;
    /** The text of the header for a day grouping. */
    AutoPtr<ITextView> mDayGroupHeader;
    /** The view containing the details for the call log row, including the action buttons. */
    AutoPtr<IView> mCallLogEntryView;
    /** The view containing call log item actions.  Null until the ViewStub is inflated. */
    AutoPtr<IView> mActionsView;
    /** The "call back" action button - assigned only when the action section is expanded. */
    AutoPtr<ITextView> mCallBackButtonView;
    /** The "video call" action button - assigned only when the action section is expanded. */
    AutoPtr<ITextView> mVideoCallButtonView;
    /** The "voicemail" action button - assigned only when the action section is expanded. */
    AutoPtr<ITextView> mVoicemailButtonView;
    /** The "details" action button - assigned only when the action section is expanded. */
    AutoPtr<ITextView> mDetailsButtonView;
    /** The "report" action button. */
    AutoPtr<ITextView> mReportButtonView;

    /**
     * The row Id for the first call associated with the call log entry.  Used as a key for the
     * map used to track which call log entries have the action button section expanded.
     */
    Int64 mRowId;

    /**
     * The call Ids for the calls represented by the current call log entry.  Used when the user
     * deletes a call log entry.
     */
    AutoPtr<ArrayOf<Int64> > mCallIds;

    /**
     * The callable phone number for the current call log entry.  Cached here as the call back
     * intent is set only when the actions ViewStub is inflated.
     */
    String mNumber;

    /**
     * The phone number presentation for the current call log entry.  Cached here as the call back
     * intent is set only when the actions ViewStub is inflated.
     */
    Int32 mNumberPresentation;

    /**
     * The type of call for the current call log entry.  Cached here as the call back
     * intent is set only when the actions ViewStub is inflated.
     */
    Int32 mCallType;

    /**
     * The account for the current call log entry.  Cached here as the call back
     * intent is set only when the actions ViewStub is inflated.
     */
    AutoPtr<IPhoneAccountHandle> mAccountHandle;

    /**
     * If the call has an associated voicemail message, the URI of the voicemail message for
     * playback.  Cached here as the voicemail intent is only set when the actions ViewStub is
     * inflated.
     */
    String mVoicemailUri;

    /**
     * The name or number associated with the call.  Cached here for use when setting content
     * descriptions on buttons in the actions ViewStub when it is inflated.
     */
    AutoPtr<ICharSequence> mNameOrNumber;

    /**
     * Whether or not the item has been reported by user as incorrect.
     */
    Boolean mReported;

    /**
     * Whether or not the contact info can be marked as invalid from the source where
     * it was obtained.
     */
    Boolean mCanBeReportedAsInvalid;

};

} // CallLog
} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CALLLOG_CALLLOGLISTITEMVIEWS_H__
