
#include "elastos/apps/dialer/calllog/CCallLogFragment.h"
#include "elastos/core/Math.h"

using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Core::Math;

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
    GetActivity((IActivity**)&activity);
    IHostInterface::Probe(activity)->ShowCallHistory();
    return NOERROR;
}

//=================================================================
// CCallLogFragment::ItemExPandedOnPreDrawListener
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::FooterViewClickListener, Object, IOnPreDrawListener)

CCallLogFragment::ItemExPandedOnPreDrawListener::ItemExPandedOnPreDrawListener(
    /* [in] */ ICallLogListItemView* view,
    /* [in] */ ICallLogListItemViews* viewHolder,
    /* [in] */ IViewTreeObserver* observer,
    /* [in] */ CCallLogFragment* host)
    : mView(view)
    , mViewHolder(viewHolder)
    , mObserver(observer)
    , mHost(host)
{}

ECode CCallLogFragment::ItemExPandedOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALUE_NOT_NULL(result);

    // We don't want to continue getting called for every draw.
    Boolean alive = FALSE;
    if (mObserver->IsAlive(&alive), alive) {
        mObserver->RemoveOnPreDrawListener(this);
    }
    // Calculate some values to help with the animation.
    Int32 endingHeight;
    mView->GetHeight(&endingHeight);
    Int32 distance = Math::Abs(endingHeight - startingHeight);
    Int32 baseHeight = Math::Min(endingHeight, startingHeight);
    Boolean isExpand = endingHeight > startingHeight;

    CCallLogListItemViews* viewHolder = (CCallLogListItemViews*)mViewHolder.Get();
    // Set the views back to the start state of the animation
    AutoPtr<IViewGroupLayoutParams> params;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(startingHeight);
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
        animator->Alpha(1f)
        animator->SetStartDelay(mFadeInStartDelay)
        animator->SetDuration(mFadeInDuration)
        animator->Start();
    }
    else {
        viewHolder->mActionsView->SetAlpha(1);
        AutoPtr<IViewPropertyAnimator> animator;
        viewHolder->mActionsView->Animate(
                (IViewPropertyAnimator**)&animator);
        animator->Alpha(0);
        animator->SetDuration(mFadeOutDuration);
        animator->Start();
    }
    view->requestLayout();

    // Set up the animator to animate the expansion and shadow depth.
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    if (isExpand) {
        helper->OfFloat(0, 1, (IValueAnimator**)&animator);
    }
    else {
        helper->OfFloat(1, 0, (IValueAnimator**)&animator);
    }

    // Figure out how much scrolling is needed to make the view fully visible.
    AutoPtr<IRect> localVisibleRect;
    CRect::New((IRect**)&localVisibleRect);
    Boolean succeeded;
    mView->GetLocalVisibleRect(localVisibleRect, &succeeded);
    Int32 top;
    localVisibleRect->GetTop(&top);
    Int32 scrollingNeeded;
    if (top > 0) {
        scrollingNeeded = -top;
    }
    else {
        Int32 measuredHeight, height;
        mView->GetMeasuredHeight(&measuredHeight);
        localVisibleRect->GetHeight(&height);
        scrollingNeeded = measuredHeight - height;
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<ItemExPandedAnimatorUpdateListener> listener = new ItemExPandedAnimatorUpdateListener(
            distance, baseHeight, scrollingNeeded, isExpand, mView, mViewHolder, listView, mHost);
    animator->AddUpdateListener((IAnimatorUpdateListener*)listener);

    // Set everything to their final values when the animation's done.
    AutoPtr<ItemExPandedAnimatorListenerAdapter> adapter = new ItemExPandedAnimatorListenerAdapter(mHost);
    animator->AddListener((IAnimatorListenerAdapter*)adapter);
    animator->SetDuration(mExpandCollapseDuration);
    animator->Start();

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
    , mScrollingNeeded(scrollingNeeded)
    , mBaseHeight(baseHeight)
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
    Float value = CoreUtils::Unbox(value);

    // For each value from 0 to 1, animate the various parts of the layout.
    AutoPtr<IViewGroupLayoutParams> params;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(value * distance + baseHeight);

    Float z = mHost->mExpandedItemTranslationZ * value;
    ((CCallLogListItemViews*)mViewHolder)->mCallLogEntryView.SetTranslationZ(z);
    mView->SetTranslationZ(z); // WAR
    mView->RequestLayout();

    if (isExpand) {
        if (mListView != null) {
            Int32 scrollBy = (Int32) (value * mScrollingNeeded) - mCurrentScroll;
            mListView->SmoothScrollBy(scrollBy, /* duration = */ 0);
            mCurrentScroll += scrollBy;
        }
    }

    return NOERROR;
}

//=================================================================
// CCallLogFragment::ItemExPandedAnimatorListenerAdapter
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::ItemExPandedAnimatorListenerAdapter, Object, IAnimatorListenerAdapter)

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
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(ILayoutParams::WRAP_CONTENT);

    if (!mIsExpand) {
        ((CCallLogListItemViews*)mViewHolder)->mActionsView->SetVisibility(IView::GONE);
    }
    else {
        // This seems like it should be unnecessary, but without this, after
        // navigating out of the activity and then back, the action view alpha
        // is defaulting to the value (0) at the start of the expand animation.
        ((CCallLogListItemViews*)mViewHolder)->mActionsView->SetAlpha(1);
    }
    CallLogAdapter::ExpandVoicemailTranscriptionView(viewHolder, isExpand);

    return NOERROR;
}

//=================================================================
// CCallLogFragment::CallsFetchedRunnable
//=================================================================
CAR_INTERFACE_IMPL(CCallLogFragment::CallsFetchedRunnable, Object, IRunnable)

CCallLogFragment::CallsFetchedRunnable::CCallLogFragment(
    /* [in] */ IListView listView,
    /* [in] */ CCallLogFragment* host)
    : mListView(listView)
    , mHost(host)
{}

ECode CCallLogFragment::CallsFetchedRunnable::Run()
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }
    mListView->SmoothScrollToPosition(0);

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
    GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, mUri, &intent);
    activity->StartActivity(intent);

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

CAR_INTERFACE_IMPL_4(CCallLogFragment, AnalyticsListFragment, ICallLogFragment, ICallLogQueryHandlerListener, \
        ICallLogAdapterOnReportButtonClickListener, ICallLogAdapterCallItemExpandedListener)

CAR_OBJECT_IMPL()

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
    AnalyticsListFragment();
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
    AnalyticsListFragment::OnCreate(state);
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
    GetActivity((IActivity**)&activity);
    String currentCountryIso = GeoUtil::GetCurrentCountryIso(activity);
    AutoPtr<IContactInfoHelper> contactinfoHelper;
    CContactInfoHelper::New(activity, currentCountryIso, (IContactInfoHelper**)&contactinfoHelper);
    mAdapter = CObjectFactory::NewCallLogAdapter(activity, this,
            contactinfoHelper, this, this, TRUE);
    SetListAdapter(mAdapter);

    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);
    CCallLogQueryHandler::New(resolver,
            this, mLogLimit, (ICallLogQueryHandler**)&mCallLogQueryHandler);

    AutoPtr<IInterface> service;
    activity->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
    mKeyguardManager = IKeyguardManager::Probe(service);
    resolver->RegisterContentObserver(ICallLog::CONTENT_URI, TRUE, mCallLogObserver);

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
    SetHasOptionsMenu(TRUE);
    UpdateCallList(mCallTypeFilter, mDateLimit);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDimension(R::dimen::call_log_expanded_translation_z,
            &mExpandedItemTranslationZ);
    resources->GetInteger(R::integer::call_log_actions_fade_in_duration, &mFadeInDuration);
    resources->GetInteger(R::integer::call_log_actions_fade_start, &mFadeInStartDelay);
    resources->GetInteger(R::integer::call_log_actions_fade_out_duration, &mFadeOutDuration);
    resources->GetInteger(
            R::integer::call_log_expand_collapse_duration, &mExpandCollapseDuration);

    if (mIsReportDialogShowing) {
        AutoPtr<IDialogFragment> df = ObjectFactory::GetReportDialogFragment(mReportDialogNumber);
        if (df != NULL) {
            df->SetTargetFragment(this, 0);
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
    VALUE_NOT_NULL(result)

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        // Return false; we did not take ownership of the cursor
        *result = FALSE;
        return NOERROR;
    }

    mAdapter->SetLoading(FALSE);
    mAdapter->ChangeCursor(cursor);
    // This will update the state of the "Clear call log" menu item.
    activity->InvalidateOptionsMenu();
    if (mScrollToTop) {
        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        // The smooth-scroll animation happens over a fixed time period.
        // As a result, if it scrolls through a large portion of the list,
        // each frame will jump so far from the previous one that the user
        // will not experience the illusion of downward motion.  Instead,
        // if we're not already near the top of the list, we instantly jump
        // near the top, and animate from there.
        Int32 position;
        listView->GetFirstVisiblePosition(&position);
        if (position > 5) {
            listView->SetSelection(5);
        }
        // Workaround for framework issue: the smooth-scroll doesn't
        // occur if setSelection() is called immediately before.
        AutoPtr<CallsFetchedRunnable> runnable = new CallsFetchedRunnable(listView, this);
        mHandler->Post((IRunnable*)runnable);

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
    GetActivity((IActivity**)&activity);
    Boolean isFinishing = FALSE;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }
    UpdateVoicemailStatusMessage(statusCursor);

    Int32 activeSources;
    mVoicemailStatusHelper->GetNumberActivityVoicemailSources(statusCursor, &activeSources);
    SetVoicemailSourcesAvailable(activeSources != 0);
    mVoicemailStatusFetched = TRUE;
    destroyEmptyLoaderIfAllDataFetched();

    return NOERROR;
}

void CCallLogFragment::DestroyEmptyLoaderIfAllDataFetched()
{
    if (mCallLogFetched && mVoicemailStatusFetched && mEmptyLoaderRunning) {
        mEmptyLoaderRunning = FALSE;
        GetLoaderManager()->DestroyLoader(EMPTY_LOADER_ID);
    }
}

void CCallLogFragment::SetVoicemailSourcesAvailable(
    /* [in] */ Boolean voicemailSourcesAvailable)
{
    if (mVoicemailSourcesAvailable == voicemailSourcesAvailable) return;
    mVoicemailSourcesAvailable = voicemailSourcesAvailable;

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
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
    VALUE_NOT_NULL(result);

    AutoPtr<IView> view;
    inflater->Inflate(R::layout::call_log_fragment, container, FALSE, &view);
    CVoicemailStatusHelperImpl::New((VoicemailStatusHelperImpl**)&mVoicemailStatusHelper);
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
    AnalyticsListFragment::OnViewCreated(view, savedInstanceState);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IView> emptyView;
    view->FindViewById(R::id::empty_list_view, (IView**)&emptyView)
    listView->SetEmptyView(emptyView);
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
    GetLoaderManager((ILoaderManager**)&manager);
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    manager->InitLoader(EMPTY_LOADER_ID, NULL,
            new EmptyLoader::Callback(activity));
    mEmptyLoaderRunning = TRUE;
    AnalyticsListFragment::OnStart();

    return NOERROR;
}

ECode CCallLogFragment::OnResume()
{
    AnalyticsListFragment::OnResume();
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
            mStatusMessageText->SetText(message.callLogMessageId);
        }
        if (message.actionMessageId != -1) {
            mStatusMessageAction->SetText(
                    ((VoicemailStatusHelper::StatusMessage*)message)->mActionMessageId);
        }
        if (((VoicemailStatusHelper::StatusMessage*)message)->mActionUri != NULL) {
            mStatusMessageAction->SetVisibility(IView::VISIBLE);
            AutoPtr<StatusMessageActionClickListener> listener =
                    new StatusMessageActionClickListener(
                    ((VoicemailStatusHelper::StatusMessage*)message)->mActionUri, this);
            mStatusMessageAction->SetOnClickListener((IViewOnClickListener*)listener);
        }
        else {
            mStatusMessageAction->SetVisibility(IView::GONE);
        }
    }
}

ECode CCallLogFragment::OnPause()
{
    AnalyticsListFragment::OnPause();
    // Kill the requests thread
    mAdapter->StopRequestProcessing();

    return NOERROR;
}

ECode CCallLogFragment::OnStop()
{
    AnalyticsListFragment::OnStop();
    UpdateOnExit();

    return NOERROR;
}

ECode CCallLogFragment::OnDestroy()
{
    AnalyticsListFragment::OnDestroy();
    mAdapter->StopRequestProcessing();
    mAdapter->ChangeCursor(NULL);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    activity->getContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mCallLogObserver);
    resolver->UnregisterContentObserver(mContactsObserver);
    resolver->UnregisterContentObserver(mVoicemailStatusObserver);

    return NOERROR;
}

ECode CCallLogFragment::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    AnalyticsListFragment::OnSaveInstanceState(outState);
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
    GetListView((IListView**)&listView);
    AutoPtr<IView> view;
    listView->getEmptyView((IView**)&view);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    DialerUtils::ConfigureEmptyListView(
            view, R::drawable::empty_call_log, messageId, resources);

    return NOERROR;
}

AutpPtr<ICallLogAdapter> CCallLogFragment::GetAdapter()
{
    return mAdapter;
}

ECode CCallLogFragment::SetMenuVisibility(
    /* [in] */ Boolean menuVisible)
{
    AnalyticsListFragment::SetMenuVisibility(menuVisible);
    if (mMenuVisible != menuVisible) {
        mMenuVisible = menuVisible;
        Boolean isResumed = FALSE;
        if (!menuVisible) {
            UpdateOnExit();
        }
        else if (IsResumed(&isResumed), isResumed) {
            RefreshData();
        }
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
        GetActivity((IActivity**)&activity);
        CallLogNotificationsHelper::RemoveMissedCallNotifications(activity);
        CallLogNotificationsHelper::UpdateVoicemailNotifications(activity);
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
    if (!mHasFooterView || GetView((IView**)&view), view == NULL) {
        return;
    }

    if (mFooterView == NULL) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<ILayoutInflater> inflater;
        activity->GetLayoutInflater((ILayoutInflater**)&inflater);

        view = NULL;
        GetView((IView**)&view);
        IViewGroup* viewGroup = IViewGroup::Probe(view);
        inflater->inflate(R::layout::recents_list_footer,
                viewGroup, FALSE, (IView**)&mFooterView);

        AutoPtr<FooterViewClickListener> listener = new FooterViewClickListener(this);
        mFooterView.setOnClickListener((IViewOnClickListener*)listener);
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    listView->RemoveFooterView(mFooterView);
    listView->AddFooterView(mFooterView);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    ViewUtil::AddBottomPaddingToListViewForFab(listView, getResources());
}

ECode CCallLogFragment::OnItemExpanded(
    /* [in] */ const ICallLogListItemView* view)
{
    Int32 startingHeight;
    view->GetHeight(&startingHeight);
    AutoPtr<IView> temp;
    view->GetTag((IView**)&temp);
    ICallLogListItemViews* viewHolder = ICallLogListItemViews::Probe(temp);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IViewTreeObserver> observer;
    listView->GetViewTreeObserver(&observer);

    AutoPtr<ItemExPandedOnPreDrawListener> listener = new ItemExPandedOnPreDrawListener(
            view, viewHolder, observer, this);
    observer->AddOnPreDrawListener((IOnPreDrawListener*)listener);

    return NOERROR;
}

ECode CCallLogFragment::GetViewForCallId(
    /* [in] */ Int64 callId,
    /* [out] */ ICallLogListItemView** result)
{
    VALUE_NOT_NULL(result);

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Int32 firstPosition;
    listView->GetFirstVisiblePosition(&firstPosition);
    Int32 lastPosition;
    listView->GetLastVisiblePosition(&lastPosition);

    for (Int32 position = 0; position <= lastPosition - firstPosition; position++) {
        AutoPtr<IView> view;
        listView->GetChildAt(position, (IView**)&view);

        if (view != NULL) {
            AutoPtr<IView> tag;
            view->GetTag((IView**)&tag);
            ICallLogListItemViews* viewHolder = ICallLogListItemViews::Probe(temp);
            if (viewHolder != NULL && ((CCallLogListItemViews*)viewHolder)->mRowId == callId) {
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
        return;
    }
    mAdapter->OnBadDataReported(number);
    mAdapter->NotifyDataSetChanged();

    return NOERROR;
}

ECode CCallLogFragment::OnReportButtonClick(
    /* [in] */ const String& number)
{
    AutoPtr<IDialogFragment> df = CObjectFactory::GetReportDialogFragment(number);
    if (df != NULL) {
        df->SetTargetFragment(this, 0);
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
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
