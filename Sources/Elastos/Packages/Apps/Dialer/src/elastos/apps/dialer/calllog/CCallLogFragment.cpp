
#include "elastos/apps/dialer/calllog/CCallLogFragment.h"
#include "elastos/apps/dialer/calllog/CallLogAdapter.h"
#include "elastos/apps/dialer/calllog/CallLogListItemViews.h"
#include "elastos/apps/dialer/calllog/CallLogNotificationsHelper.h"
#include "elastos/apps/dialer/calllog/CCallLogQueryHandler.h"
#include "elastos/apps/dialer/calllog/CContactInfoHelper.h"
#include "elastos/apps/dialer/voicemail/CVoicemailStatusHelperImpl.h"
#include "elastos/apps/dialer/voicemail/VoicemailStatusHelperStatusMessage.h"
#include "elastos/apps/dialer/util/EmptyLoader.h"
#include "elastos/apps/dialer/util/DialerUtils.h"
#include "elastos/apps/dialerbind/CObjectFactory.h"
#include "R.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/core/Math.h"
#include "elastos/core/CoreUtils.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Provider::ICalls;
using Elastos::Droid::Provider::ICallLog;
using Elastos::Droid::Provider::CCallLog;
using Elastos::Droid::Provider::IContactsContractContacts;
using Elastos::Droid::Provider::CContactsContractContacts;
using Elastos::Droid::Provider::IVoicemailContractStatus;
using Elastos::Droid::Provider::CVoicemailContractStatus;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::IAbsListView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Core::EIID_IRunnable;
using Elastos::Apps::Dialer::List::IListsFragmentHostInterface;
using Elastos::Apps::Dialer::Util::EmptyLoader;
using Elastos::Apps::Dialer::Util::DialerUtils;
using Elastos::Apps::Dialer::Voicemail::IVoicemailStatusHelperStatusMessage;
using Elastos::Apps::Dialer::Voicemail::VoicemailStatusHelperStatusMessage;
using Elastos::Apps::Dialer::Voicemail::IVoicemailStatusHelperImpl;
using Elastos::Apps::Dialer::Voicemail::CVoicemailStatusHelperImpl;
using Elastos::Apps::DialerBind::CObjectFactory;

namespace Elastos {
namespace Apps {
namespace Dialer {
namespace CallLog {

//=================================================================
// CCallLogFragment::CustomContentObserver
//=================================================================
CCallLogFragment::CustomContentObserver::CustomContentObserver(
    /* [in] */ CCallLogFragment* host)
    : mHost(host)
{}

ECode CCallLogFragment::CustomContentObserver::constructor()
{
    return ContentObserver::constructor();
}

ECode CCallLogFragment::CustomContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->mRefreshDataRequired = TRUE;
    return NOERROR;
}

//=================================================================
// CCallLogFragment::FooterViewClickListener
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::FooterViewClickListener, Object, IViewOnClickListener)

CCallLogFragment::FooterViewClickListener::FooterViewClickListener(
    /* [in] */ CCallLogFragment* host)
    : mHost(host)
{}

ECode CCallLogFragment::FooterViewClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // mHost->GetActivity((IActivity**)&activity);
    IListsFragmentHostInterface::Probe(activity)->ShowCallHistory();
    return NOERROR;
}

//=================================================================
// CCallLogFragment::ItemExPandedOnPreDrawListener
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::ItemExPandedOnPreDrawListener, Object, IOnPreDrawListener)

CCallLogFragment::ItemExPandedOnPreDrawListener::ItemExPandedOnPreDrawListener(
    /* [in] */ ICallLogListItemView* view,
    /* [in] */ ICallLogListItemViews* viewHolder,
    /* [in] */ IViewTreeObserver* observer,
    /* [in] */ Int32 startingHeight,
    /* [in] */ CCallLogFragment* host)
    : mView(view)
    , mViewHolder(viewHolder)
    , mObserver(observer)
    , mStartingHeight(startingHeight)
    , mHost(host)
{}

ECode CCallLogFragment::ItemExPandedOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // We don't want to continue getting called for every draw.
    Boolean alive = FALSE;
    if (mObserver->IsAlive(&alive), alive) {
        mObserver->RemoveOnPreDrawListener(this);
    }
    // Calculate some values to help with the animation.
    Int32 endingHeight;
    IView::Probe(mView)->GetHeight(&endingHeight);
    Int32 distance = Elastos::Core::Math::Abs(endingHeight - mStartingHeight);
    Int32 baseHeight = Elastos::Core::Math::Min(endingHeight, mStartingHeight);
    Boolean isExpand = endingHeight > mStartingHeight;

    CallLogListItemViews* viewHolder = (CallLogListItemViews*)mViewHolder.Get();
    // Set the views back to the start state of the animation
    AutoPtr<IViewGroupLayoutParams> params;
    IView::Probe(mView)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(mStartingHeight);
    if (!isExpand) {
        viewHolder->mActionsView->SetVisibility(IView::VISIBLE);
    }
    CallLogAdapter::ExpandVoicemailTranscriptionView(mViewHolder, !isExpand);

    // Set up the fade effect for the action buttons.
    if (isExpand) {
        // Start the fade in after the expansion has partly completed, otherwise it
        // will be mostly over before the expansion completes.
        viewHolder->mActionsView->SetAlpha(0);
        AutoPtr<IViewPropertyAnimator> animator;
        viewHolder->mActionsView->Animate(
                (IViewPropertyAnimator**)&animator);
        animator->Alpha(1);
        animator->SetStartDelay(mHost->mFadeInStartDelay);
        animator->SetDuration(mHost->mFadeInDuration);
        animator->Start();
    }
    else {
        viewHolder->mActionsView->SetAlpha(1);
        AutoPtr<IViewPropertyAnimator> animator;
        viewHolder->mActionsView->Animate(
                (IViewPropertyAnimator**)&animator);
        animator->Alpha(0);
        animator->SetDuration(mHost->mFadeOutDuration);
        animator->Start();
    }
    IView::Probe(mView)->RequestLayout();

    // Set up the animator to animate the expansion and shadow depth.
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    AutoPtr<ArrayOf<Float> > args = ArrayOf<Float>::Alloc(2);
    if (isExpand) {
        args->Set(0, 0);
        args->Set(1, 1);
        helper->OfFloat(args, (IValueAnimator**)&animator);
    }
    else {
        args->Set(0, 1);
        args->Set(1, 0);
        helper->OfFloat(args, (IValueAnimator**)&animator);
    }

    // Figure out how much scrolling is needed to make the view fully visible.
    AutoPtr<IRect> localVisibleRect;
    CRect::New((IRect**)&localVisibleRect);
    Boolean succeeded;
    IView::Probe(mView)->GetLocalVisibleRect(localVisibleRect, &succeeded);
    Int32 top;
    localVisibleRect->GetTop(&top);
    Int32 scrollingNeeded;
    if (top > 0) {
        scrollingNeeded = -top;
    }
    else {
        Int32 measuredHeight, height;
        IView::Probe(mView)->GetMeasuredHeight(&measuredHeight);
        localVisibleRect->GetHeight(&height);
        scrollingNeeded = measuredHeight - height;
    }

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // mHost->GetListView((IListView**)&listView);
    AutoPtr<ItemExPandedAnimatorUpdateListener> listener = new ItemExPandedAnimatorUpdateListener(
            distance, baseHeight, scrollingNeeded, isExpand, mView, mViewHolder, listView, mHost);
    animator->AddUpdateListener((IAnimatorUpdateListener*)listener);

    // Set everything to their final values when the animation's done.
    AutoPtr<ItemExPandedAnimatorListenerAdapter> adapter = new ItemExPandedAnimatorListenerAdapter(
            isExpand, mView, mViewHolder);
    IAnimator::Probe(animator)->AddListener((IAnimatorListener*)adapter);
    animator->SetDuration(mHost->mExpandCollapseDuration);
    IAnimator::Probe(animator)->Start();

    // Return false so this draw does not occur to prevent the final frame from
    // being drawn for the single frame before the animations start.
    *result = FALSE;
    return NOERROR;
}

//=================================================================
// CCallLogFragment::ItemExPandedAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::ItemExPandedAnimatorUpdateListener, Object, IAnimatorUpdateListener)

CCallLogFragment::ItemExPandedAnimatorUpdateListener::ItemExPandedAnimatorUpdateListener(
    /* [in] */ Int32 distance,
    /* [in] */ Int32 baseHeight,
    /* [in] */ Int32 scrollingNeeded,
    /* [in] */ Boolean isExpand,
    /* [in] */ ICallLogListItemView* view,
    /* [in] */ ICallLogListItemViews* viewHolder,
    /* [in] */ IListView* listView,
    /* [in] */ CCallLogFragment* host)
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

ECode CCallLogFragment::ItemExPandedAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animator)
{
    AutoPtr<IInterface> value;
    animator->GetAnimatedValue((IInterface**)&value);
    Float fvalue;
    IFloat::Probe(value)->GetValue(&fvalue);

    // For each value from 0 to 1, animate the various parts of the layout.
    AutoPtr<IViewGroupLayoutParams> params;
    IView::Probe(mView)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(fvalue * mDistance + mBaseHeight);

    Float z = mHost->mExpandedItemTranslationZ * fvalue;
    ((CallLogListItemViews*)mViewHolder.Get())->mCallLogEntryView->SetTranslationZ(z);
    IView::Probe(mView)->SetTranslationZ(z); // WAR
    IView::Probe(mView)->RequestLayout();

    if (mIsExpand) {
        if (mListView != NULL) {
            Int32 scrollBy = (Int32) (fvalue * mScrollingNeeded) - mCurrentScroll;
            IAbsListView::Probe(mListView)->SmoothScrollBy(scrollBy, /* duration = */ 0);
            mCurrentScroll += scrollBy;
        }
    }

    return NOERROR;
}

//=================================================================
// CCallLogFragment::ItemExPandedAnimatorListenerAdapter
//=================================================================

CCallLogFragment::ItemExPandedAnimatorListenerAdapter::ItemExPandedAnimatorListenerAdapter(
    /* [in] */ Boolean isExpand,
    /* [in] */ ICallLogListItemView* view,
    /* [in] */ ICallLogListItemViews* viewHolder)
    : mIsExpand(isExpand)
    , mView(view)
    , mViewHolder(viewHolder)
{}

ECode CCallLogFragment::ItemExPandedAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<IViewGroupLayoutParams> params;
    IView::Probe(mView)->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);

    if (!mIsExpand) {
        ((CallLogListItemViews*)mViewHolder.Get())->mActionsView->SetVisibility(IView::GONE);
    }
    else {
        // This seems like it should be unnecessary, but without this, after
        // navigating out of the activity and then back, the action view alpha
        // is defaulting to the value (0) at the start of the expand animation.
        ((CallLogListItemViews*)mViewHolder.Get())->mActionsView->SetAlpha(1);
    }
    CallLogAdapter::ExpandVoicemailTranscriptionView(mViewHolder, mIsExpand);

    return NOERROR;
}

//=================================================================
// CCallLogFragment::CallsFetchedRunnable
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::CallsFetchedRunnable, Object, IRunnable)

CCallLogFragment::CallsFetchedRunnable::CallsFetchedRunnable(
    /* [in] */ IListView* listView,
    /* [in] */ CCallLogFragment* host)
    : mListView(listView)
    , mHost(host)
{}

ECode CCallLogFragment::CallsFetchedRunnable::Run()
{
    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // mHost->GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }
    IAbsListView::Probe(mListView)->SmoothScrollToPosition(0);

    return NOERROR;
}

//=================================================================
// CCallLogFragment::StatusMessageActionClickListener
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::StatusMessageActionClickListener, Object, IViewOnClickListener)

CCallLogFragment::StatusMessageActionClickListener::StatusMessageActionClickListener(
    /* [in] */ IUri* uri,
    /* [in] */ CCallLogFragment* host)
    : mUri(uri)
    , mHost(host)
{}

ECode CCallLogFragment::StatusMessageActionClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, mUri, (IIntent**)&intent);
    IContext::Probe(activity)->StartActivity(intent);

    return NOERROR;
}

//=================================================================
// CCallLogFragment
//=================================================================

const String CCallLogFragment::TAG("CallLogFragment");

const String CCallLogFragment::REPORT_DIALOG_TAG("report_dialog");
const Int32 CCallLogFragment::EMPTY_LOADER_ID = 0;

const String CCallLogFragment::KEY_FILTER_TYPE("filter_type");
const String CCallLogFragment::KEY_LOG_LIMIT("log_limit");
const String CCallLogFragment::KEY_DATE_LIMIT("date_limit");
const String CCallLogFragment::KEY_SHOW_FOOTER("show_footer");
const String CCallLogFragment::KEY_IS_REPORT_DIALOG_SHOWING("is_report_dialog_showing");
const String CCallLogFragment::KEY_REPORT_DIALOG_NUMBER("report_dialog_number");

// TODO:
// CAR_INTERFACE_IMPL_4(CCallLogFragment, AnalyticsListFragment, ICallLogFragment, ICallLogQueryHandlerListener,
//         ICallLogAdapterOnReportButtonClickListener, ICallLogAdapterCallItemExpandedListener)
CAR_INTERFACE_IMPL_4(CCallLogFragment, Object, ICallLogFragment, ICallLogQueryHandlerListener,
        ICallLogAdapterOnReportButtonClickListener, ICallLogAdapterCallItemExpandedListener)

CAR_OBJECT_IMPL(CCallLogFragment)

CCallLogFragment::CCallLogFragment()
    : mVoicemailSourcesAvailable(FALSE)
    , mRefreshDataRequired(TRUE)
    , mMenuVisible(TRUE)
    , mCallTypeFilter(ICallLogQueryHandler::CALL_TYPE_ALL)
    , mLogLimit(-1)
    , mDateLimit(0)
    , mHasFooterView(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mCallLogObserver = new CustomContentObserver(this);
    mCallLogObserver->constructor();
    mContactsObserver = new CustomContentObserver(this);
    mContactsObserver->constructor();
    mVoicemailStatusObserver = new CustomContentObserver(this);
    mVoicemailStatusObserver->constructor();
}

ECode CCallLogFragment::constructor()
{
    return constructor(ICallLogQueryHandler::CALL_TYPE_ALL, -1);
}

ECode CCallLogFragment::constructor(
    /* [in] */ Int32 filterType)
{
    return constructor(filterType, -1);
}

ECode CCallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int32 logLimit)
{
    // AnalyticsListFragment();
    mCallTypeFilter = filterType;
    mLogLimit = logLimit;
    return NOERROR;
}

ECode CCallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int64 dateLimit)
{
    return constructor(filterType, -1, dateLimit);
}

/**
 * Creates a call log fragment, filtering to include only calls of the desired type, occurring
 * after the specified date.  Also provides a means to limit the number of results returned.
 * @param filterType type of calls to include.
 * @param logLimit limits the number of results to return.
 * @param dateLimit limits results to calls occurring on or after the specified date.
 */
ECode CCallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int32 logLimit,
    /* [in] */ Int64 dateLimit)
{
    constructor(filterType, logLimit);
    mDateLimit = dateLimit;
    return NOERROR;
}

ECode CCallLogFragment::OnCreate(
    /* [in] */ IBundle* state)
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnCreate(state);
    if (state != NULL) {
        state->GetInt32(KEY_FILTER_TYPE, mCallTypeFilter, &mCallTypeFilter);
        state->GetInt32(KEY_LOG_LIMIT, mLogLimit, &mLogLimit);
        state->GetInt64(KEY_DATE_LIMIT, mDateLimit, &mDateLimit);
        state->GetBoolean(KEY_SHOW_FOOTER, mHasFooterView, &mHasFooterView);
        state->GetBoolean(KEY_IS_REPORT_DIALOG_SHOWING,
                mIsReportDialogShowing, &mIsReportDialogShowing);
        state->GetString(KEY_REPORT_DIALOG_NUMBER, mReportDialogNumber, &mReportDialogNumber);
    }

    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // GetActivity((IActivity**)&activity);
    // AutoPtr<IGeoUtil> geoUtil;
    // CGeoUtil::AcquireSingleton((IGeoUtil**)&geoUtil);
    String currentCountryIso;
    // geoUtil->GetCurrentCountryIso(IContext::Probe(activity), &currentCountryIso);
    AutoPtr<IContactInfoHelper> contactinfoHelper;
    CContactInfoHelper::New(IContext::Probe(activity),
            currentCountryIso, (IContactInfoHelper**)&contactinfoHelper);
    mAdapter = CObjectFactory::NewCallLogAdapter(IContext::Probe(activity),
            (ICallLogAdapterCallFetcher*)this, contactinfoHelper,
            (ICallLogAdapterCallItemExpandedListener*)this,
            (ICallLogAdapterOnReportButtonClickListener*)this, TRUE);
    assert(0 && "TODO");
    // SetListAdapter(mAdapter);

    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
    CCallLogQueryHandler::New(resolver,
            this, mLogLimit, (ICallLogQueryHandler**)&mCallLogQueryHandler);

    AutoPtr<IInterface> service;
    IContext::Probe(activity)->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
    mKeyguardManager = IKeyguardManager::Probe(service);
    AutoPtr<ICallLog> calllog;
    CCallLog::AcquireSingleton((ICallLog**)&calllog);
    AutoPtr<IUri> uri;
    calllog->GetCONTENT_URI((IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, mCallLogObserver);

    AutoPtr<IContactsContractContacts> contacts;
    CContactsContractContacts::AcquireSingleton((IContactsContractContacts**)&contacts);
    AutoPtr<IUri> contentUri;
    contacts->GetCONTENT_URI((IUri**)&contentUri);
    resolver->RegisterContentObserver(contentUri, TRUE, mContactsObserver);

    AutoPtr<IVoicemailContractStatus> status;
    CVoicemailContractStatus::AcquireSingleton((IVoicemailContractStatus**)&status);
    contentUri = NULL;
    status->GetCONTENT_URI((IUri**)&contentUri);
    resolver->RegisterContentObserver(contentUri, TRUE, mVoicemailStatusObserver);
    assert(0 && "TODO");
    // SetHasOptionsMenu(TRUE);
    UpdateCallList(mCallTypeFilter, mDateLimit);

    AutoPtr<IResources> resources;
    assert(0 && "TODO");
    // GetResources((IResources**)&resources);
    resources->GetDimension(R::dimen::call_log_expanded_translation_z,
            &mExpandedItemTranslationZ);
    resources->GetInteger(R::integer::call_log_actions_fade_in_duration, &mFadeInDuration);
    resources->GetInteger(R::integer::call_log_actions_fade_start, &mFadeInStartDelay);
    resources->GetInteger(R::integer::call_log_actions_fade_out_duration, &mFadeOutDuration);
    resources->GetInteger(
            R::integer::call_log_expand_collapse_duration, &mExpandCollapseDuration);

    if (mIsReportDialogShowing) {
        AutoPtr<IDialogFragment> df = CObjectFactory::GetReportDialogFragment(mReportDialogNumber);
        if (df != NULL) {
            IFragment::Probe(df)->SetTargetFragment((IFragment*)this, 0);
            AutoPtr<IFragmentManager> manager;
            activity->GetFragmentManager((IFragmentManager**)&manager);
            df->Show(manager, REPORT_DIALOG_TAG);
        }
    }

    return NOERROR;
}

ECode CCallLogFragment::OnCallsFetched(
    /* [in] */ ICursor* cursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        // Return false; we did not take ownership of the cursor
        *result = FALSE;
        return NOERROR;
    }

    mAdapter->SetLoading(FALSE);
    assert(0 && "TODO");
    // mAdapter->ChangeCursor(cursor);
    // This will update the state of the "Clear call log" menu item.
    activity->InvalidateOptionsMenu();
    if (mScrollToTop) {
        AutoPtr<IListView> listView;
        assert(0 && "TODO");
        // GetListView((IListView**)&listView);
        // The smooth-scroll animation happens over a fixed time period.
        // As a result, if it scrolls through a large portion of the list,
        // each frame will jump so far from the previous one that the user
        // will not experience the illusion of downward motion.  Instead,
        // if we're not already near the top of the list, we instantly jump
        // near the top, and animate from there.
        Int32 position;
        IAdapterView::Probe(listView)->GetFirstVisiblePosition(&position);
        if (position > 5) {
            IAdapterView::Probe(listView)->SetSelection(5);
        }
        // Workaround for framework issue: the smooth-scroll doesn't
        // occur if setSelection() is called immediately before.
        AutoPtr<CallsFetchedRunnable> runnable = new CallsFetchedRunnable(listView, this);
        Boolean res;
        mHandler->Post((IRunnable*)runnable, &res);

        mScrollToTop = FALSE;
    }
    mCallLogFetched = TRUE;
    DestroyEmptyLoaderIfAllDataFetched();
    *result = TRUE;
    return NOERROR;
}

ECode CCallLogFragment::OnVoicemailStatusFetched(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }
    UpdateVoicemailStatusMessage(statusCursor);

    Int32 activeSources;
    mVoicemailStatusHelper->GetNumberActivityVoicemailSources(statusCursor, &activeSources);
    SetVoicemailSourcesAvailable(activeSources != 0);
    mVoicemailStatusFetched = TRUE;
    DestroyEmptyLoaderIfAllDataFetched();

    return NOERROR;
}

void CCallLogFragment::DestroyEmptyLoaderIfAllDataFetched()
{
    if (mCallLogFetched && mVoicemailStatusFetched && mEmptyLoaderRunning) {
        mEmptyLoaderRunning = FALSE;
        AutoPtr<ILoaderManager> manager;
        assert(0 && "TODO");
        // GetLoaderManager((ILoaderManager**)&manager);
        manager->DestroyLoader(EMPTY_LOADER_ID);
    }
}

void CCallLogFragment::SetVoicemailSourcesAvailable(
    /* [in] */ Boolean voicemailSourcesAvailable)
{
    if (mVoicemailSourcesAvailable == voicemailSourcesAvailable) return;
    mVoicemailSourcesAvailable = voicemailSourcesAvailable;

    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // GetActivity((IActivity**)&activity);
    if (activity != NULL) {
        // This is so that the options menu content is updated.
        activity->InvalidateOptionsMenu();
    }
}

ECode CCallLogFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::call_log_fragment, container, FALSE, (IView**)&view);
    CVoicemailStatusHelperImpl::New((IVoicemailStatusHelperImpl**)&mVoicemailStatusHelper);
    view->FindViewById(R::id::voicemail_status, (IView**)&mStatusMessageView);

    AutoPtr<IView> temp;
    view->FindViewById(R::id::voicemail_status_message, (IView**)&temp);
    mStatusMessageText = ITextView::Probe(temp);
    temp = NULL;
    view->FindViewById(R::id::voicemail_status_action, (IView**)&temp);
    mStatusMessageAction = ITextView::Probe(temp);
    *result = view;
    REFCOUNT_ADD(*result);

    return NOERROR;
}

ECode CCallLogFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnViewCreated(view, savedInstanceState);

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // GetListView((IListView**)&listView);
    AutoPtr<IView> emptyView;
    view->FindViewById(R::id::empty_list_view, (IView**)&emptyView);
    IAdapterView::Probe(listView)->SetEmptyView(emptyView);
    listView->SetItemsCanFocus(TRUE);
    MaybeAddFooterView();

    return UpdateEmptyMessage(mCallTypeFilter);
}

ECode CCallLogFragment::ConfigureScreenFromIntent(
    /* [in] */ IIntent* newIntent)
{
    // Typically, when switching to the call-log we want to show the user
    // the same section of the list that they were most recently looking
    // at.  However, under some circumstances, we want to automatically
    // scroll to the top of the list to present the newest call items.
    // For example, immediately after a call is finished, we want to
    // display information about that call.
    String type;
    newIntent->GetType(&type);
    mScrollToTop = ICalls::CONTENT_TYPE.Equals(type);

    return NOERROR;
}

ECode CCallLogFragment::OnStart()
{
    // Start the empty loader now to defer other fragments.  We destroy it when both calllog
    // and the voicemail status are fetched.
    AutoPtr<ILoaderManager> manager;
    assert(0 && "TODO");
    // GetLoaderManager((ILoaderManager**)&manager);
    AutoPtr<IActivity> activity;
    // GetActivity((IActivity**)&activity);
    AutoPtr<EmptyLoader::Callback> callback = new EmptyLoader::Callback();
    callback->constructor(IContext::Probe(activity));
    AutoPtr<ILoader> loader;
    manager->InitLoader(EMPTY_LOADER_ID, NULL,
            (ILoaderManagerLoaderCallbacks*)callback, (ILoader**)&loader);
    mEmptyLoaderRunning = TRUE;
    assert(0 && "TODO");
    // AnalyticsListFragment::OnStart();

    return NOERROR;
}

ECode CCallLogFragment::OnResume()
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnResume();
    RefreshData();
    return NOERROR;
}

void CCallLogFragment::UpdateVoicemailStatusMessage(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IList> messages;
    mVoicemailStatusHelper->GetStatusMessages(statusCursor, (IList**)&messages);

    Int32 size;
    messages->GetSize(&size);
    if (size == 0) {
        mStatusMessageView->SetVisibility(IView::GONE);
    }
    else {
        mStatusMessageView->SetVisibility(IView::VISIBLE);
        // TODO: Change the code to show all messages. For now just pick the first message.
        AutoPtr<IInterface> item;
        messages->Get(0, (IInterface**)&item);
        IVoicemailStatusHelperStatusMessage* message = IVoicemailStatusHelperStatusMessage::Probe(item);
        Boolean showInCallLog = FALSE;
        if (message->ShowInCallLog(&showInCallLog), showInCallLog) {
            mStatusMessageText->SetText(
                    ((VoicemailStatusHelperStatusMessage*)message)->mCallLogMessageId);
        }
        if (((VoicemailStatusHelperStatusMessage*)message)->mActionMessageId != -1) {
            mStatusMessageAction->SetText(
                    ((VoicemailStatusHelperStatusMessage*)message)->mActionMessageId);
        }
        if (((VoicemailStatusHelperStatusMessage*)message)->mActionUri != NULL) {
            IView::Probe(mStatusMessageAction)->SetVisibility(IView::VISIBLE);
            AutoPtr<StatusMessageActionClickListener> listener =
                    new StatusMessageActionClickListener(
                    ((VoicemailStatusHelperStatusMessage*)message)->mActionUri, this);
            IView::Probe(mStatusMessageAction)->SetOnClickListener((IViewOnClickListener*)listener);
        }
        else {
            IView::Probe(mStatusMessageAction)->SetVisibility(IView::GONE);
        }
    }
}

ECode CCallLogFragment::OnPause()
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnPause();
    // Kill the requests thread
    mAdapter->StopRequestProcessing();

    return NOERROR;
}

ECode CCallLogFragment::OnStop()
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnStop();
    UpdateOnExit();

    return NOERROR;
}

ECode CCallLogFragment::OnDestroy()
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnDestroy();
    // mAdapter->StopRequestProcessing();
    // mAdapter->ChangeCursor(NULL);

    AutoPtr<IActivity> activity;
    assert(0 && "TODO");
    // GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mCallLogObserver);
    resolver->UnregisterContentObserver(mContactsObserver);
    resolver->UnregisterContentObserver(mVoicemailStatusObserver);

    return NOERROR;
}

ECode CCallLogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    assert(0 && "TODO");
    // AnalyticsListFragment::OnSaveInstanceState(outState);
    outState->PutInt32(KEY_FILTER_TYPE, mCallTypeFilter);
    outState->PutInt32(KEY_LOG_LIMIT, mLogLimit);
    outState->PutInt64(KEY_DATE_LIMIT, mDateLimit);
    outState->PutBoolean(KEY_SHOW_FOOTER, mHasFooterView);
    outState->PutBoolean(KEY_IS_REPORT_DIALOG_SHOWING, mIsReportDialogShowing);
    outState->PutString(KEY_REPORT_DIALOG_NUMBER, mReportDialogNumber);

    return NOERROR;
}

ECode CCallLogFragment::FetchCalls()
{
    mCallLogQueryHandler->FetchCalls(mCallTypeFilter, mDateLimit);

    return NOERROR;
}

ECode CCallLogFragment::StartCallsQuery()
{
    mAdapter->SetLoading(TRUE);
    mCallLogQueryHandler->FetchCalls(mCallTypeFilter, mDateLimit);

    return NOERROR;
}

void CCallLogFragment::StartVoicemailStatusQuery()
{
    mCallLogQueryHandler->FetchVoicemailStatus();
}

void CCallLogFragment::UpdateCallList(
    /* [in] */ Int32 filterType,
    /* [in] */ Int64 dateLimit)
{
    mCallLogQueryHandler->FetchCalls(filterType, dateLimit);
}

ECode CCallLogFragment::UpdateEmptyMessage(
    /* [in] */ Int32 filterType)
{
    Int32 messageId;
    switch (filterType) {
        case ICalls::MISSED_TYPE:
            messageId = R::string::recentMissed_empty;
            break;
        case ICalls::VOICEMAIL_TYPE:
            messageId = R::string::recentVoicemails_empty;
            break;
        case ICallLogQueryHandler::CALL_TYPE_ALL:
            messageId = R::string::recentCalls_empty;
            break;
        default:
            Logger::E(TAG, "Unexpected filter type in CallLogFragment: %d", filterType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unexpected filter type in CallLogFragment: "
            //         + filterType);
    }

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // GetListView((IListView**)&listView);
    AutoPtr<IView> view;
    IAdapterView::Probe(listView)->GetEmptyView((IView**)&view);
    AutoPtr<IResources> resources;
    assert(0 && "TODO");
    // GetResources((IResources**)&resources);
    DialerUtils::ConfigureEmptyListView(
            view, R::drawable::empty_call_log, messageId, resources);

    return NOERROR;
}

AutoPtr<ICallLogAdapter> CCallLogFragment::GetAdapter()
{
    return mAdapter;
}

ECode CCallLogFragment::SetMenuVisibility(
    /* [in] */ Boolean menuVisible)
{
    assert(0 && "TODO");
    // AnalyticsListFragment::SetMenuVisibility(menuVisible);
    if (mMenuVisible != menuVisible) {
        mMenuVisible = menuVisible;
        Boolean isResumed = FALSE;
        assert(0 && "TODO");
        // if (!menuVisible) {
        //     UpdateOnExit();
        // }
        // // else if (IsResumed(&isResumed), isResumed) {
        //     RefreshData();
        // }
    }

    return NOERROR;
}

void CCallLogFragment::RefreshData()
{
    // Prevent unnecessary refresh.
    if (mRefreshDataRequired) {
        // Mark all entries in the contact info cache as out of date, so they will be looked up
        // again once being shown.
        mAdapter->InvalidateCache();
        StartCallsQuery();
        StartVoicemailStatusQuery();
        UpdateOnEntry();
        mRefreshDataRequired = FALSE;
    }
}

void CCallLogFragment::UpdateOnExit()
{
    UpdateOnTransition(FALSE);
}

void CCallLogFragment::UpdateOnEntry()
{
    UpdateOnTransition(TRUE);
}

void CCallLogFragment::UpdateOnTransition(
    /* [in] */ Boolean onEntry)
{
    // We don't want to update any call data when keyguard is on because the user has likely not
    // seen the new calls yet.
    // This might be called before onCreate() and thus we need to check null explicitly.
    Boolean result = TRUE;
    if (mKeyguardManager != NULL &&
            mKeyguardManager->InKeyguardRestrictedInputMode(&result), !result) {
        // On either of the transitions we update the missed call and voicemail notifications.
        // While exiting we additionally consume all missed calls (by marking them as read).
        mCallLogQueryHandler->MarkNewCallsAsOld();
        if (!onEntry) {
            mCallLogQueryHandler->MarkMissedCallsAsRead();
        }
        AutoPtr<IActivity> activity;
        assert(0 && "TODO");
        // GetActivity((IActivity**)&activity);
        CallLogNotificationsHelper::RemoveMissedCallNotifications(IContext::Probe(activity));
        CallLogNotificationsHelper::UpdateVoicemailNotifications(IContext::Probe(activity));
    }
}

ECode CCallLogFragment::SetHasFooterView(
    /* [in] */ Boolean hasFooterView)
{
    mHasFooterView = hasFooterView;
    MaybeAddFooterView();

    return NOERROR;
}

void CCallLogFragment::MaybeAddFooterView()
{
    AutoPtr<IView> view;
    assert(0 && "TODO");
    // if (!mHasFooterView || GetView((IView**)&view), view == NULL) {
    //     return;
    // }

    if (mFooterView == NULL) {
        AutoPtr<IActivity> activity;
        assert(0 && "TODO");
        // GetActivity((IActivity**)&activity);
        AutoPtr<ILayoutInflater> inflater;
        activity->GetLayoutInflater((ILayoutInflater**)&inflater);

        view = NULL;
        assert(0 && "TODO");
        // GetView((IView**)&view);
        IViewGroup* viewGroup = IViewGroup::Probe(view);
        inflater->Inflate(R::layout::recents_list_footer,
                viewGroup, FALSE, (IView**)&mFooterView);

        AutoPtr<FooterViewClickListener> listener = new FooterViewClickListener(this);
        mFooterView->SetOnClickListener((IViewOnClickListener*)listener);
    }

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // GetListView((IListView**)&listView);
    // listView->RemoveFooterView(mFooterView);
    // listView->AddFooterView(mFooterView);

    // AutoPtr<IResources> resources;
    // GetResources((IResources**)&resources);
    // ViewUtil::AddBottomPaddingToListViewForFab(listView, resources);
}

ECode CCallLogFragment::OnItemExpanded(
    /* [in] */ const ICallLogListItemView* view)
{
    Int32 startingHeight;
    IView::Probe((ICallLogListItemView*)view)->GetHeight(&startingHeight);
    AutoPtr<IInterface> temp;
    IView::Probe((ICallLogListItemView*)view)->GetTag((IInterface**)&temp);
    ICallLogListItemViews* viewHolder = ICallLogListItemViews::Probe(temp);

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // GetListView((IListView**)&listView);
    AutoPtr<IViewTreeObserver> observer;
    IView::Probe((ICallLogListItemView*)view)->GetViewTreeObserver(
            (IViewTreeObserver**)&observer);

    AutoPtr<ItemExPandedOnPreDrawListener> listener = new ItemExPandedOnPreDrawListener(
            (ICallLogListItemView*)view, viewHolder, observer, startingHeight, this);
    observer->AddOnPreDrawListener((IOnPreDrawListener*)listener);

    return NOERROR;
}

ECode CCallLogFragment::GetViewForCallId(
    /* [in] */ Int64 callId,
    /* [out] */ ICallLogListItemView** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IListView> listView;
    assert(0 && "TODO");
    // GetListView((IListView**)&listView);
    Int32 firstPosition;
    IAdapterView::Probe(listView)->GetFirstVisiblePosition(&firstPosition);
    Int32 lastPosition;
    IAdapterView::Probe(listView)->GetLastVisiblePosition(&lastPosition);

    for (Int32 position = 0; position <= lastPosition - firstPosition; position++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(listView)->GetChildAt(position, (IView**)&view);

        if (view != NULL) {
            AutoPtr<IInterface> tag;
            view->GetTag((IInterface**)&tag);
            ICallLogListItemViews* viewHolder = ICallLogListItemViews::Probe(tag);
            if (viewHolder != NULL && ((CallLogListItemViews*)viewHolder)->mRowId == callId) {
                *result = ICallLogListItemView::Probe(view);
                REFCOUNT_ADD(*result);
                return NOERROR;
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CCallLogFragment::OnBadDataReported(
    /* [in] */ const String& number)
{
    mIsReportDialogShowing = TRUE;
    if (number.IsNull()) {
        return NOERROR;
    }
    mAdapter->OnBadDataReported(number);
    IBaseAdapter::Probe(mAdapter)->NotifyDataSetChanged();

    return NOERROR;
}

ECode CCallLogFragment::OnReportButtonClick(
    /* [in] */ const String& number)
{
    AutoPtr<IDialogFragment> df = CObjectFactory::GetReportDialogFragment(number);
    if (df != NULL) {
        IFragment::Probe(df)->SetTargetFragment((IFragment*)this, 0);
        AutoPtr<IActivity> activity;
        assert(0 && "TODO");
        // GetActivity((IActivity**)&activity);
        AutoPtr<IFragmentManager> manager;
        activity->GetFragmentManager((IFragmentManager**)&manager);
        df->Show(manager, REPORT_DIALOG_TAG);
        mReportDialogNumber = number;
        mIsReportDialogShowing = TRUE;
    }

    return NOERROR;
}

} // CallLog
} // Dialer
} // Apps
} // Elastos
