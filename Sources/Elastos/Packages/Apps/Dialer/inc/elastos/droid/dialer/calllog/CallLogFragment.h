
#ifndef __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGFRAGMENT_H__
#define __ELASTOS_DROID_DIALER_CALLLOG_CALLLOGFRAGMENT_H__

#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/dialer/calllog/CallLogAdapter.h"
#include "elastos/droid/dialer/calllog/CallLogQueryHandler.h"
#include "elastos/droid/dialer/calllog/CallLogListItemViews.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsListFragment.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::App::IKeyguardManager;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;
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

class CallLogFragment
    : public AnalyticsListFragment
    , public ICallLogFragment
    , public ICallLogQueryHandlerListener
    , public ICallLogAdapterOnReportButtonClickListener
    , public ICallLogAdapterCallFetcher
    , public ICallLogAdapterCallItemExpandedListener
{
private:
    class CustomContentObserver
        : public ContentObserver
    {
    public:
        CustomContentObserver(
            /* [in] */ CallLogFragment* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CallLogFragment* mHost;
    };

    class CallsFetchedRunnable
        : public Runnable
    {
    public:
        CallsFetchedRunnable(
            /* [in] */ IListView* listView,
            /* [in] */ CallLogFragment* host)
            : mListView(listView)
            , mHost(host)

        CARAPI Run();

    private:
        AutoPtr<IListView> mListView;
        CallLogFragment* mHost;
    };

    class StatusMessageActionOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        StatusMessageActionOnClickListener(
            /* [in] */ IUri* uri,
            /* [in] */ CallLogFragment* host)
            : mUri(uri)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        AutoPtr<IUri> mUri;
        CallLogFragment* mHost;
    };

    class FooterViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        FooterViewOnClickListener(
            /* [in] */ CallLogFragment* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        CallLogFragment* mHost;
    };

    class ItemExpandedOnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    private:
        class ItemExpandedAnimatorUpdateListener
            : public Object
            , public IAnimatorUpdateListener
        {
        public:
            CAR_INTERFACE_DECL()

            ItemExpandedAnimatorUpdateListener(
                /* [in] */ Int32 distance,
                /* [in] */ Int32 baseHeight,
                /* [in] */ Int32 scrollingNeeded,
                /* [in] */ Boolean isExpand,
                /* [in] */ IView* view,
                /* [in] */ CallLogListItemViews* viewHolder,
                /* [in] */ IListView* listView,
                /* [in] */ CallLogFragment* host)
                : mCurrentScroll(0)
                , mDistance(distance)
                , mBaseHeight(baseHeight)
                , mScrollingNeeded(scrollingNeeded)
                , mIsExpand(isExpand)
                , mView(view)
                , mViewHolder(viewHolder)
                , mListView(listView)
                , mHost(host)
            {}

            CARAPI OnAnimationUpdate(
                /* [in] */ IValueAnimator* animator);

        private:
            Int32 mCurrentScroll;
            Int32 mDistance;
            Int32 mBaseHeight;
            Int32 mScrollingNeeded;
            Boolean mIsExpand;
            AutoPtr<IView> mView;
            AutoPtr<CallLogListItemViews> mViewHolder;
            AutoPtr<IListView> mListView;
            CallLogFragment* mHost;
        };

        class ItemExpandedAnimatorListenerAdapter
            : public AnimatorListenerAdapter
        {
        public:
            ItemExpandedAnimatorListenerAdapter(
                /* [in] */ Boolean isExpand,
                /* [in] */ IView* view,
                /* [in] */ CallLogListItemViews* viewHolder)
                : mIsExpand(isExpand)
                , mView(view)
                , mViewHolder(viewHolder)
            {}

            CARAPI OnAnimationEnd(
                /* [in] */ IAnimator* animator);

        private:
            Boolean mIsExpand;
            AutoPtr<IView> mView;
            AutoPtr<CallLogListItemViews> mViewHolder;
        };

    public:
        CAR_INTERFACE_DECL()

        ItemExpandedOnPreDrawListener(
            /* [in] */ ICallLogListItemView* view,
            /* [in] */ CallLogListItemViews* viewHolder,
            /* [in] */ IViewTreeObserver* observer,
            /* [in] */ Int32 startingHeight,
            /* [in] */ CallLogFragment* host)
            : mView(view)
            , mViewHolder(viewHolder)
            , mObserver(observer)
            , mStartingHeight(startingHeight)
            , mHost(host)
        {}

        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        AutoPtr<ICallLogListItemView> mView;
        AutoPtr<CallLogListItemViews> mViewHolder;
        AutoPtr<IViewTreeObserver> mObserver;
        Int32 mStartingHeight;
        CallLogFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CallLogFragment();

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

    CARAPI_(AutoPtr<CallLogAdapter>) GetAdapter();

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
        /* [in] */ ICallLogListItemView* view);

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
    static const String TAG;

    static const String REPORT_DIALOG_TAG;

    /**
     * ID of the empty loader to defer other fragments.
     */
    static const Int32 EMPTY_LOADER_ID = 0;

    static const String KEY_FILTER_TYPE;
    static const String KEY_LOG_LIMIT;
    static const String KEY_DATE_LIMIT;
    static const String KEY_SHOW_FOOTER;
    static const String KEY_IS_REPORT_DIALOG_SHOWING;
    static const String KEY_REPORT_DIALOG_NUMBER;

    String mReportDialogNumber;
    Boolean mIsReportDialogShowing;

    AutoPtr<CallLogAdapter> mAdapter;
    AutoPtr<CallLogQueryHandler> mCallLogQueryHandler;
    Boolean mScrollToTop;

    /** Whether there is at least one voicemail source installed. */
    Boolean mVoicemailSourcesAvailable;

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

    AutoPtr<IHandler> mHandler;

    // See issue 6363009
    AutoPtr<IContentObserver> mCallLogObserver;
    AutoPtr<IContentObserver> mContactsObserver;
    AutoPtr<IContentObserver> mVoicemailStatusObserver;
    Boolean mRefreshDataRequired;

    // Exactly same variable is in Fragment as a package private.
    Boolean mMenuVisible;

    // Default to all calls.
    Int32 mCallTypeFilter; // = CallLogQueryHandler.CALL_TYPE_ALL;

    // Log limit - if no limit is specified, then the default in {@link CallLogQueryHandler}
    // will be used.
    Int32 mLogLimit;

    // Date limit (in millis since epoch) - when non-zero, only calls which occurred on or after
    // the date filter are included.  If zero, no date-based filtering occurs.
    Int64 mDateLimit;

    // Whether or not to show the Show call history footer view
    Boolean mHasFooterView;

    friend class CustomContentObserver;
    friend class ItemExpandedOnPreDrawListener;
};

} // CallLog
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_CALLLOG_CallLogFragment_H__
