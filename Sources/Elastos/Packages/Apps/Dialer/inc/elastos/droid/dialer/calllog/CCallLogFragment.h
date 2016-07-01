
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGFRAGMENT_H__

#include "_Elastos_Droid_Dialer_CallLog_CCallLogFragment.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Dialer::Voicemail::IVoicemailStatusHelper;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CarClass(CCallLogFragment)
    // TODO:
    // , public AnalyticsListFragment
    , public Object
    , public ICallLogFragment
    , public ICallLogQueryHandlerListener
    , public ICallLogAdapterOnReportButtonClickListener
    , public ICallLogAdapterCallItemExpandedListener
{
private:
    class CustomContentObserver
        : public ContentObserver
    {
    public:
        CustomContentObserver(
            /* [in] */ CCallLogFragment* host);

        CARAPI constructor();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        CCallLogFragment* mHost;
    };

    class FooterViewClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        FooterViewClickListener(
            /* [in] */ CCallLogFragment* host);

        CARAPI OnClick(
            /* [in] */ IView* view);
    private:
        CCallLogFragment* mHost;
    };

    class ItemExPandedOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        ItemExPandedOnPreDrawListener(
            /* [in] */ ICallLogListItemView* view,
            /* [in] */ ICallLogListItemViews* viewHolder,
            /* [in] */ IViewTreeObserver* observer,
            /* [in] */ Int32 startingHeight,
            /* [in] */ CCallLogFragment* host);

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);
    private:
        AutoPtr<ICallLogListItemView> mView;
        AutoPtr<ICallLogListItemViews> mViewHolder;
        AutoPtr<IViewTreeObserver> mObserver;
        Int32 mStartingHeight;
        CCallLogFragment* mHost;
    };

    class ItemExPandedAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL()

        ItemExPandedAnimatorUpdateListener(
            /* [in] */ Int32 distance,
            /* [in] */ Int32 baseHeight,
            /* [in] */ Int32 scrollingNeeded,
            /* [in] */ Boolean isExpand,
            /* [in] */ ICallLogListItemView* view,
            /* [in] */ ICallLogListItemViews* viewHolder,
            /* [in] */ IListView* listView,
            /* [in] */ CCallLogFragment* host);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animator);
    private:
        Int32 mCurrentScroll;
        Int32 mDistance;
        Int32 mBaseHeight;
        Int32 mScrollingNeeded;
        Boolean mIsExpand;
        AutoPtr<ICallLogListItemView> mView;
        AutoPtr<ICallLogListItemViews> mViewHolder;
        AutoPtr<IListView> mListView;
        CCallLogFragment* mHost;
    };

    class ItemExPandedAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        ItemExPandedAnimatorListenerAdapter(
            /* [in] */ Boolean isExpand,
            /* [in] */ ICallLogListItemView* view,
            /* [in] */ ICallLogListItemViews* viewHolder);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);
    private:
        Boolean mIsExpand;
        AutoPtr<ICallLogListItemView> mView;
        AutoPtr<ICallLogListItemViews> mViewHolder;
    };

    class CallsFetchedRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        CallsFetchedRunnable(
            /* [in] */ IListView* listView,
            /* [in] */ CCallLogFragment* host);

        CARAPI Run();

    private:
        AutoPtr<IListView> mListView;
        CCallLogFragment* mHost;
    };

    class StatusMessageActionClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        StatusMessageActionClickListener(
            /* [in] */ IUri* uri,
            /* [in] */ CCallLogFragment* host);

        CARAPI OnClick(
            /* [in] */ IView* view);
    private:
        AutoPtr<IUri> mUri;
        CCallLogFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CCallLogFragment();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 filterType);

    CARAPI constructor(
        /* [in] */ Int32 filterType,
        /* [in] */ Int32 logLimit);

    /**
     * Creates a call log fragment, filtering to include only calls of the desired type, occurring
     * after the specified date.
     * @param filterType type of calls to include.
     * @param dateLimit limits results to calls occurring on or after the specified date.
     */
    CARAPI constructor(
        /* [in] */ Int32 filterType,
        /* [in] */ Int64 dateLimit);

    /**
     * Creates a call log fragment, filtering to include only calls of the desired type, occurring
     * after the specified date.  Also provides a means to limit the number of results returned.
     * @param filterType type of calls to include.
     * @param logLimit limits the number of results to return.
     * @param dateLimit limits results to calls occurring on or after the specified date.
     */
    CARAPI constructor(
        /* [in] */ Int32 filterType,
        /* [in] */ Int32 logLimit,
        /* [in] */ Int64 dateLimit);

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* state);

    /** Called by the CallLogQueryHandler when the list of calls has been fetched or updated. */
    // @Override
    CARAPI OnCallsFetched(
        /* [in] */ ICursor* cursor,
        /* [out] */ Boolean* result);

    /**
     * Called by {@link CallLogQueryHandler} after a successful query to voicemail status provider.
     */
    // @Override
    CARAPI OnVoicemailStatusFetched(
        /* [in] */ ICursor* statusCursor);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnViewCreated(
        /* [in] */ IView* view,
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Based on the new intent, decide whether the list should be configured
     * to scroll up to display the first item.
     */
    CARAPI ConfigureScreenFromIntent(
        /* [in] */ IIntent* newIntent);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnResume();

    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    // @Override
    CARAPI FetchCalls();

    // @Override
    CARAPI StartCallsQuery();

    CARAPI_(AutoPtr<ICallLogAdapter>) GetAdapter();

    // @Override
    CARAPI SetMenuVisibility(
        /* [in] */ Boolean menuVisible);

     /**
     * Enables/disables the showing of the view full call history footer
     *
     * @param hasFooterView Whether or not to show the footer
     */
    CARAPI SetHasFooterView(
        /* [in] */ Boolean hasFooterView);

    // @Override
    CARAPI OnItemExpanded(
        /* [in] */ const ICallLogListItemView* view);

    /**
     * Retrieves the call log view for the specified call Id.  If the view is not currently
     * visible, returns null.
     *
     * @param callId The call Id.
     * @return The call log view.
     */
    // @Override
    CARAPI GetViewForCallId(
        /* [in] */ Int64 callId,
        /* [out] */ ICallLogListItemView** view);

    CARAPI OnBadDataReported(
        /* [in] */ const String& number);

    CARAPI OnReportButtonClick(
        /* [in] */ const String& number);

private:
    CARAPI_(void) DestroyEmptyLoaderIfAllDataFetched();

    /** Sets whether there are any voicemail sources available in the platform. */
    CARAPI_(void) SetVoicemailSourcesAvailable(
        /* [in] */ Boolean voicemailSourcesAvailable);

    CARAPI_(void) UpdateVoicemailStatusMessage(
        /* [in] */ ICursor* statusCursor);

    CARAPI_(void) StartVoicemailStatusQuery();

    CARAPI_(void) UpdateCallList(
        /* [in] */ Int32 filterType,
        /* [in] */ Int64 dateLimit);

    CARAPI UpdateEmptyMessage(
        /* [in] */ Int32 filterType);

    /** Requests updates to the data to be shown. */
    CARAPI_(void) RefreshData();

    /** Updates call data and notification state while leaving the call log tab. */
    CARAPI_(void) UpdateOnExit();

    /** Updates call data and notification state while entering the call log tab. */
    CARAPI_(void) UpdateOnEntry();

    // TODO: Move to CallLogActivity
    CARAPI_(void) UpdateOnTransition(
        /* [in] */ Boolean onEntry);

    /**
     * Determine whether or not the footer view should be added to the listview. If getView()
     * is null, which means onCreateView hasn't been called yet, defer the addition of the footer
     * until onViewCreated has been called.
     */
    CARAPI_(void) MaybeAddFooterView();

private:
    static const String TAG; // = "CallLogFragment";

    static const String REPORT_DIALOG_TAG; // = "report_dialog";
    String mReportDialogNumber;
    Boolean mIsReportDialogShowing;

    /**
     * ID of the empty loader to defer other fragments.
     */
    static const Int32 EMPTY_LOADER_ID; // = 0;

    static const String KEY_FILTER_TYPE; // = "filter_type";
    static const String KEY_LOG_LIMIT; // = "log_limit";
    static const String KEY_DATE_LIMIT; // = "date_limit";
    static const String KEY_SHOW_FOOTER; // = "show_footer";
    static const String KEY_IS_REPORT_DIALOG_SHOWING; // = "is_report_dialog_showing";
    static const String KEY_REPORT_DIALOG_NUMBER; // = "report_dialog_number";

    AutoPtr<ICallLogAdapter> mAdapter;
    AutoPtr<ICallLogQueryHandler> mCallLogQueryHandler;
    Boolean mScrollToTop;

    /** Whether there is at least one voicemail source installed. */
    Boolean mVoicemailSourcesAvailable; // = false;

    AutoPtr<IVoicemailStatusHelper> mVoicemailStatusHelper;
    AutoPtr<IView> mStatusMessageView;
    AutoPtr<ITextView> mStatusMessageText;
    AutoPtr<ITextView> mStatusMessageAction;
    AutoPtr<IKeyguardManager> mKeyguardManager;
    AutoPtr<IView> mFooterView;

    Boolean mEmptyLoaderRunning;
    Boolean mCallLogFetched;
    Boolean mVoicemailStatusFetched;

    Float mExpandedItemTranslationZ;
    Int32 mFadeInDuration;
    Int32 mFadeInStartDelay;
    Int32 mFadeOutDuration;
    Int32 mExpandCollapseDuration;

    AutoPtr<IHandler> mHandler; // = new Handler();

    // See issue 6363009
    AutoPtr<ContentObserver> mCallLogObserver; // = new CustomContentObserver();
    AutoPtr<ContentObserver> mContactsObserver; // = new CustomContentObserver();
    AutoPtr<ContentObserver> mVoicemailStatusObserver; // = new CustomContentObserver();
    Boolean mRefreshDataRequired; // = true;

    // Exactly same variable is in Fragment as a package private.
    Boolean mMenuVisible; // = true;

    // Default to all calls.
    Int32 mCallTypeFilter; // = CallLogQueryHandler.CALL_TYPE_ALL;

    // Log limit - if no limit is specified, then the default in {@link CallLogQueryHandler}
    // will be used.
    Int32 mLogLimit; // = -1;

    // Date limit (in millis since epoch) - when non-zero, only calls which occurred on or after
    // the date filter are included.  If zero, no date-based filtering occurs.
    Int64 mDateLimit; // = 0;

    // Whether or not to show the Show call history footer view
    Boolean mHasFooterView; // = false;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CCALLLOGFRAGMENT_H__
