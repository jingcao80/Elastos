
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGADAPTER_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGADAPTER_H__

#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/dialer/PhoneCallDetails.h"
#include "elastos/droid/dialer/calllog/ContactInfo.h"
#include "elastos/droid/dialer/calllog/ContactInfoHelper.h"
#include "elastos/droid/dialer/calllog/CallLogListItemHelper.h"
#include "elastos/droid/dialer/calllog/PhoneNumberDisplayHelper.h"
#include "elastos/droid/dialer/calllog/CallLogGroupBuilder.h"
#include "elastos/droid/dialer/calllog/CallLogListItemViews.h"
#include "elastos/droid/common/widget/GroupingListAdapter.h"
#include "elastos/droid/view/View.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/core/Thread.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Contacts::Common::IContactPhotoManager;
using Elastos::Droid::Common::Widget::GroupingListAdapter;
using Elastos::Droid::Dialer::PhoneCallDetails;
using Elastos::Droid::Dialer::Util::IExpirableCache;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::Thread;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ILinkedList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

class CallLogAdapter
    : public GroupingListAdapter
    , public ICallLogAdapter
    , public IOnPreDrawListener
    , public ICallLogGroupBuilderGroupCreator
{
public:
    TO_STRING_IMPL("CallLogAdapter")

private:
    /**
     * Stores a phone number of a call with the country code where it originally occurred.
     * <p>
     * Note the country does not necessarily specifies the country of the phone number itself, but
     * it is the country in which the user was in when the call was placed or received.
     */
    class NumberWithCountryIso
        : public Object
        , public INumberWithCountryIso
    {
    public:
        NumberWithCountryIso(
            /* [in] */ const String& number,
            /* [in] */ const String& countryIso)
            : mNumber(number)
            , mCountryIso(countryIso)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    public:
        String mNumber;
        String mCountryIso;
    };

    /**
     * A request for contact details for the given number.
     */
    class ContactInfoRequest
        : public Object
        , public IContactInfoRequest
    {
    public:
        ContactInfoRequest(
            /* [in] */ const String& number,
            /* [in] */ const String& countryIso,
            /* [in] */ ContactInfo* callLogInfo)
            : mNumber(number)
            , mCountryIso(countryIso)
            , mCallLogInfo(callLogInfo)
        {}

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    public:
        /** The number to look-up. */
        String mNumber;
        /** The country in which a call to or from this number was placed or received. */
        String mCountryIso;
        /** The cached contact information stored in the call log. */
        AutoPtr<ContactInfo> mCallLogInfo;
    };

    /** Listener for the primary or secondary actions in the list.
     *  Primary opens the call details.
     *  Secondary calls or plays.
     **/
    class ActionListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActionListener(
            /* [in] */ CallLogAdapter* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        CallLogAdapter* mHost;
    };

    /**
     * The onClickListener used to expand or collapse the action buttons section for a call log
     * entry.
     */
    class ExpandCollapseListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ExpandCollapseListener(
            /* [in] */ CallLogAdapter* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CallLogAdapter* mHost;
    };

    class AccessibilityDelegate
        : public Elastos::Droid::View::View::AccessibilityDelegate
    {
    public:
        AccessibilityDelegate(
            /* [in] */ CallLogAdapter* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnRequestSendAccessibilityEvent(
            /* [in] */ IViewGroup* host,
            /* [in] */ IView* child,
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* res);

    private:
        CallLogAdapter* mHost;
    };

    class CallLogAdapterHandler
        : public Handler
    {
    public:
        CallLogAdapterHandler(
            /* [in] */ CallLogAdapter* host);

        TO_STRING_IMPL("CallLogAdapter::CallLogAdapterHandler")

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CallLogAdapter* mHost;
    };

    /*
     * Handles requests for contact name and number type.
     */
    class QueryThread
        : public Thread
    {
    public:
        QueryThread(
            /* [in] */ CallLogAdapter* host);

        CARAPI_(void) StopProcessing();

        // @Override
        CARAPI Run();

    private:
        /* volatile*/ Boolean mDone;
        CallLogAdapter* mHost;
    };

    class ReportButtonClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ReportButtonClickListener(
            /* [in] */ CallLogListItemViews* views,
            /* [in] */ CallLogAdapter* host)
            : mViews(views)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        AutoPtr<CallLogListItemViews> mViews;
        CallLogAdapter* mHost;
    };

    class BadgeContainerClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        BadgeContainerClickListener(
            /* [in] */ PhoneCallDetails* details,
            /* [in] */ CallLogAdapter* host)
            : mDetails(details)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        AutoPtr<PhoneCallDetails> mDetails;
        CallLogAdapter* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CallLogAdapter();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ ICallFetcher* callFetcher,
        /* [in] */ ContactInfoHelper* contactInfoHelper,
        /* [in] */ ICallItemExpandedListener* callItemExpandedListener,
        /* [in] */ ICallLogAdapterOnReportButtonClickListener* onReportButtonClickListener,
        /* [in] */ Boolean isCallLog);

    // @Override
    CARAPI OnPreDraw(
        /* [out] */ Boolean* result);

    CARAPI SetLoading(
        /* [in] */ Boolean loading);

    // @Override
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

    /**
     * Stops the background thread that processes updates and cancels any
     * pending requests to start it.
     */
    CARAPI StopRequestProcessing();

    CARAPI InvalidateCache();

    static CARAPI_(void) ExpandVoicemailTranscriptionView(
        /* [in] */ CallLogListItemViews* views,
        /* [in] */ Boolean isExpanded);

    /**
     * Bind a call log entry view for testing purposes.  Also inflates the action view stub so
     * unit tests can access the buttons contained within.
     *
     * @param view The current call log row.
     * @param context The current context.
     * @param cursor The cursor to bind from.
     */
    // @VisibleForTesting
    CARAPI_(void) BindViewForTest(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    /**
     * Sets whether processing of requests for contact details should be enabled.
     * <p>
     * This method should be called in tests to disable such processing of requests when not
     * needed.
     */
    // @VisibleForTesting
    CARAPI_(void) DisableRequestProcessingForTest();

    // @VisibleForTesting
    CARAPI_(void) InjectContactInfoForTest(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [in] */ ContactInfo* contactInfo);

    // @Override
    CARAPI AddGroup(
        /* [in] */ Int32 cursorPosition,
        /* [in] */ Int32 size,
        /* [in] */ Boolean expanded);

    /**
     * Stores the day group associated with a call in the call log.
     *
     * @param rowId The row Id of the current call.
     * @param dayGroup The day group the call belongs in.
     */
    // @Override
    CARAPI SetDayGroup(
        /* [in] */ Int64 rowId,
        /* [in] */ Int32 dayGroup);

    /**
     * Clears the day group associations on re-bind of the call log.
     */
    // @Override
    CARAPI ClearDayGroups();

    /*
     * Get the number from the Contacts, if available, since sometimes
     * the number provided by caller id may not be formatted properly
     * depending on the carrier (roaming) in use at the time of the
     * incoming call.
     * Logic : If the caller-id number starts with a "+", use it
     *         Else if the number in the contacts starts with a "+", use that one
     *         Else if the number in the contacts is longer, use that one
     */
    CARAPI GetBetterNumberFromContacts(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [out] */ String* result);

    CARAPI OnBadDataReported(
        /* [in] */ const String& number);

protected:
    /**
     * Requery on background thread when {@link Cursor} changes.
     */
    // @Override
    CARAPI OnContentChanged();

    /**
     * Enqueues a request to look up the contact details for the given phone number.
     * <p>
     * It also provides the current contact info stored in the call log for this number.
     * <p>
     * If the {@code immediate} parameter is true, it will start immediately the thread that looks
     * up the contact information (if it has not been already started). Otherwise, it will be
     * started with a delay. See {@link #START_PROCESSING_REQUESTS_DELAY_MILLIS}.
     */
    CARAPI_(void) EnqueueRequest(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [in] */ ContactInfo* callLogInfo,
        /* [in] */ Boolean immediate);

    // @Override
    CARAPI_(void) AddGroups(
        /* [in] */ ICursor* cursor);

    // @Override
    CARAPI_(AutoPtr<IView>) NewStandAloneView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(AutoPtr<IView>) NewGroupView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(AutoPtr<IView>) NewChildView(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent);

    // @Override
    CARAPI_(void) BindStandAloneView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    // @Override
    CARAPI_(void) BindChildView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    // @Override
    CARAPI_(void) BindGroupView(
        /* [in] */ IView* view,
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 groupSize,
        /* [in] */ Boolean expanded);

    CARAPI_(void) BindBadge(
        /* [in] */ IView* view,
        /* [in] */ ContactInfo* info,
        /* [in] */ PhoneCallDetails* details,
        /* [in] */ Int32 callType);

private:
    CARAPI_(void) StartActivityForAction(
        /* [in] */ IView* view);

    /**
     * Starts a background thread to process contact-lookup requests, unless one
     * has already been started.
     */
    CARAPI_(void) StartRequestProcessing();

    /**
     * Stop receiving onPreDraw() notifications.
     */
    CARAPI_(void) UnregisterPreDrawListener();

    /**
     * Queries the appropriate content provider for the contact associated with the number.
     * <p>
     * Upon completion it also updates the cache in the call log, if it is different from
     * {@code callLogInfo}.
     * <p>
     * The number might be either a SIP address or a phone number.
     * <p>
     * It returns true if it updated the content of the cache and we should therefore tell the
     * view to update its content.
     */
    CARAPI_(Boolean) QueryContactInfo(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [in] */ ContactInfo* callLogInfo);

    CARAPI_(void) FindAndCacheViews(
        /* [in] */ IView* view);

    /**
     * Binds the views in the entry to the data in the call log.
     *
     * @param view the view corresponding to this entry
     * @param c the cursor pointing to the entry in the call log
     * @param count the number of entries in the current item, greater than 1 if it is a group
     */
    CARAPI_(void) BindView(
        /* [in] */ IView* view,
        /* [in] */ ICursor* c,
        /* [in] */ Int32 count);

    /**
     * Retrieves the day group of the previous call in the call log.  Used to determine if the day
     * group has changed and to trigger display of the day group text.
     *
     * @param cursor The call log cursor.
     * @return The previous day group, or DAY_GROUP_NONE if this is the first call.
     */
    CARAPI_(Int32) GetPreviousDayGroup(
        /* [in] */ ICursor* cursor);

    /**
     * Given a call Id, look up the day group that the call belongs to.  The day group data is
     * populated in {@link com.android.dialer.calllog.CallLogGroupBuilder}.
     *
     * @param callId The call to retrieve the day group for.
     * @return The day group for the call.
     */
    CARAPI_(Int32) GetDayGroupForCall(
        /* [in] */ Int64 callId);

    /**
     * Determines if a call log row with the given Id is expanded.
     * @param rowId The row Id of the call.
     * @return True if the row should be expanded.
     */
    CARAPI_(Boolean) IsExpanded(
        /* [in] */ Int64 rowId);

    /**
     * Toggles the expansion state tracked for the call log row identified by rowId and returns
     * the new expansion state.  Assumes that only a single call log row will be expanded at any
     * one point and tracks the current and previous expanded item.
     *
     * @param rowId The row Id associated with the call log row to expand/collapse.
     * @return True where the row is now expanded, false otherwise.
     */
    CARAPI_(Boolean) ToggleExpansion(
        /* [in] */ Int64 rowId);

    /**
     * Expands or collapses the view containing the CALLBACK, VOICEMAIL and DETAILS action buttons.
     *
     * @param callLogItem The call log entry parent view.
     * @param isExpanded The new expansion state of the view.
     */
    CARAPI_(void) ExpandOrCollapseActions(
        /* [in] */ ICallLogListItemView* callLogItem,
        /* [in] */ Boolean isExpanded);

    /**
     * Configures the action buttons in the expandable actions ViewStub.  The ViewStub is not
     * inflated during initial binding, so click handlers, tags and accessibility text must be set
     * here, if necessary.
     *
     * @param callLogItem The call log list item view.
     */
    CARAPI_(void) InflateActionViewStub(
        /* [in] */ IView* callLogItem);

    /***
     * Binds click handlers and intents to the voicemail, details and callback action buttons.
     *
     * @param views  The call log item views.
     */
    CARAPI_(void) BindActionButtons(
        /* [in] */ CallLogListItemViews* views);

    /** Checks whether the contact info from the call log matches the one from the contacts db. */
    CARAPI_(Boolean) CallLogInfoMatches(
        /* [in] */ ContactInfo* callLogInfo,
        /* [in] */ ContactInfo* info);

    /** Stores the updated contact info in the call log if it is different from the current one. */
    CARAPI_(void) UpdateCallLogContactInfoCache(
        /* [in] */ const String& number,
        /* [in] */ const String& countryIso,
        /* [in] */ ContactInfo* updatedInfo,
        /* [in] */ ContactInfo* callLogInfo);

    /** Returns the contact information as stored in the call log. */
    CARAPI_(AutoPtr<ContactInfo>) GetContactInfoFromCallLog(
        /* [in] */ ICursor* c);

    /**
     * Returns the call types for the given number of items in the cursor.
     * <p>
     * It uses the next {@code count} rows in the cursor to extract the types.
     * <p>
     * It position in the cursor is unchanged by this function.
     */
    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetCallTypes(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 count);

    /**
     * Determine the features which were enabled for any of the calls that make up a call log
     * entry.
     *
     * @param cursor The cursor.
     * @param count The number of calls for the current call log entry.
     * @return The features.
     */
    CARAPI_(Int32) GetCallFeatures(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 count);

    CARAPI_(void) SetPhoto(
        /* [in] */ CallLogListItemViews* views,
        /* [in] */ Int64 photoId,
        /* [in] */ IUri* contactUri,
        /* [in] */ const String& displayName,
        /* [in] */ const String& identifier,
        /* [in] */ Int32 contactType);

    CARAPI_(void) SetPhoto(
        /* [in] */ CallLogListItemViews* views,
        /* [in] */ IUri* photoUri,
        /* [in] */ IUri* contactUri,
        /* [in] */ const String& displayName,
        /* [in] */ const String& identifier,
        /* [in] */ Int32 contactType);

    /**
     * Retrieves the call Ids represented by the current call log row.
     *
     * @param cursor Call log cursor to retrieve call Ids from.
     * @param groupSize Number of calls associated with the current call log row.
     * @return Array of call Ids.
     */
    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetCallIds(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 groupSize);

    /**
     * Determines the description for a day group.
     *
     * @param group The day group to retrieve the description for.
     * @return The day group description.
     */
    CARAPI_(AutoPtr<ICharSequence>) GetGroupDescription(
        /* [in] */ Int32 group);

    /**
     * Manages the state changes for the UI interaction where a call log row is expanded.
     *
     * @param view The view that was tapped
     * @param animate Whether or not to animate the expansion/collapse
     * @param forceExpand Whether or not to force the call log row into an expanded state regardless
     *        of its previous state
     */
    CARAPI_(void) HandleRowExpanded(
        /* [in] */ ICallLogListItemView* view,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean forceExpand);

protected:
    AutoPtr<IContext> mContext;

private:
    static const Int32 VOICEMAIL_TRANSCRIPTION_MAX_LINES = 10;

    /** The time in millis to delay starting the thread processing requests. */
    static const Int32 START_PROCESSING_REQUESTS_DELAY_MILLIS = 1000;

    /** The size of the cache of contact info. */
    static const Int32 CONTACT_INFO_CACHE_SIZE = 100;

    /** Constant used to indicate no row is expanded. */
    static const Int64 NONE_EXPANDED = -1;

    static const Int32 REDRAW = 1;
    static const Int32 START_THREAD = 2;

    AutoPtr<ContactInfoHelper> mContactInfoHelper;
    AutoPtr<ICallFetcher> mCallFetcher;
    AutoPtr<IToast> mReportedToast;
    AutoPtr<ICallLogAdapterOnReportButtonClickListener> mOnReportButtonClickListener;
    AutoPtr<IViewTreeObserver> mViewTreeObserver;

    /**
     * A cache of the contact details for the phone numbers in the call log.
     * <p>
     * The content of the cache is expired (but not purged) whenever the application comes to
     * the foreground.
     * <p>
     * The key is number with the country in which the call was placed or received.
     */
    AutoPtr<IExpirableCache> mContactInfoCache;

    /**
     * Tracks the call log row which was previously expanded.  Used so that the closure of a
     * previously expanded call log entry can be animated on rebind.
     */
    Int64 mPreviouslyExpanded; // = NONE_EXPANDED;

    /**
     * Tracks the currently expanded call log row.
     */
    Int64 mCurrentlyExpanded; // = NONE_EXPANDED;

    /**
     *  Hashmap, keyed by call Id, used to track the day group for a call.  As call log entries are
     *  put into the primary call groups in {@link com.android.dialer.calllog.CallLogGroupBuilder},
     *  they are also assigned a secondary "day group".  This hashmap tracks the day group assigned
     *  to all calls in the call log.  This information is used to trigger the display of a day
     *  group header above the call log entry at the start of a day group.
     *  Note: Multiple calls are grouped into a single primary "call group" in the call log, and
     *  the cursor used to bind rows includes all of these calls.  When determining if a day group
     *  change has occurred it is necessary to look at the last entry in the call log to determine
     *  its day group.  This hashmap provides a means of determining the previous day group without
     *  having to reverse the cursor to the start of the previous day call log entry.
     */
    HashMap<Int64, AutoPtr<IInteger32> > mDayGroups;

    /**
     * List of requests to update contact details.
     * <p>
     * Each request is made of a phone number to look up, and the contact info currently stored in
     * the call log for this number.
     * <p>
     * The requests are added when displaying the contacts and are processed by a background
     * thread.
     */
    AutoPtr<ILinkedList> mRequests;

    Boolean mLoading;

    AutoPtr<QueryThread> mCallerIdThread;

    /** Instance of helper class for managing views. */
    AutoPtr<CallLogListItemHelper> mCallLogViewsHelper;

    // /** Helper to set up contact photos. *//
    AutoPtr<IContactPhotoManager> mContactPhotoManager;
    /** Helper to parse and process phone numbers. */
    AutoPtr<PhoneNumberDisplayHelper> mPhoneNumberHelper;
    /** Helper to group call log entries. */
    AutoPtr<CallLogGroupBuilder> mCallLogGroupBuilder;

    AutoPtr<ICallItemExpandedListener> mCallItemExpandedListener;

    /** Can be set to true by tests to disable processing of requests. */
    /*volatile*/ Boolean mRequestProcessingDisabled;

    Boolean mIsCallLog;

    AutoPtr<IView> mBadgeContainer;
    AutoPtr<IImageView> mBadgeImageView;
    AutoPtr<ITextView> mBadgeText;

    Int32 mCallLogBackgroundColor;
    Int32 mExpandedBackgroundColor;
    Float mExpandedTranslationZ;

    /** Listener for the primary or secondary actions in the list.
     *  Primary opens the call details.
     *  Secondary calls or plays.
     **/
    AutoPtr<IViewOnClickListener> mActionListener;

    /**
     * The onClickListener used to expand or collapse the action buttons section for a call log
     * entry.
     */
    AutoPtr<IViewOnClickListener> mExpandCollapseListener;

    AutoPtr<AccessibilityDelegate> mAccessibilityDelegate;

    AutoPtr<IHandler> mHandler;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CALLLOGADAPTER_H__
