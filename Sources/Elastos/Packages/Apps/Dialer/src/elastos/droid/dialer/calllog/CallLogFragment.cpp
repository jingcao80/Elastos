//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/contacts/common/GeoUtil.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "elastos/droid/dialer/calllog/CallLogFragment.h"
#include "elastos/droid/dialer/calllog/CallLogNotificationsHelper.h"
#include "elastos/droid/dialer/calllog/ContactInfoHelper.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperImpl.h"
#include "elastos/droid/dialer/voicemail/VoicemailStatusHelperStatusMessage.h"
#include "elastos/droid/dialer/util/EmptyLoader.h"
#include "elastos/droid/dialer/util/DialerUtils.h"
#include "elastos/droid/dialerbind/ObjectFactory.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::ILoaderManager;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Contacts::Common::GeoUtil;
using Elastos::Droid::Contacts::Common::Util::ViewUtil;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Dialer::List::IListsFragmentHostInterface;
using Elastos::Droid::Dialer::Util::EmptyLoader;
using Elastos::Droid::Dialer::Util::DialerUtils;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperStatusMessage;
using Elastos::Droid::Dialer::Voicemail::VoicemailStatusHelperImpl;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogFragment;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogQueryHandlerListener;
using Elastos::Droid::Dialer::CallLog::EIID_ICallLogAdapterOnReportButtonClickListener;
using Elastos::Droid::Dialer::CallLog::EIID_ICallFetcher;
using Elastos::Droid::Dialer::CallLog::EIID_ICallItemExpandedListener;
using Elastos::Droid::DialerBind::ObjectFactory;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Net::IUri;
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
using Elastos::Core::IFloat;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

//=================================================================
// CallLogFragment::CustomContentObserver
//=================================================================
CallLogFragment::CustomContentObserver::CustomContentObserver(
    /* [in] */ CallLogFragment* host)
    : mHost(host)
{
    ContentObserver::constructor(mHost->mHandler);
}

ECode CallLogFragment::CustomContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->mRefreshDataRequired = TRUE;
    return NOERROR;
}


//=================================================================
// CallLogFragment::CallsFetchedRunnable
//=================================================================
ECode CallLogFragment::CallsFetchedRunnable::Run()
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    Boolean isFinishing;
    if (activity == NULL || activity->IsFinishing(&isFinishing), isFinishing) {
        return NOERROR;
    }
    IAbsListView::Probe(mListView)->SmoothScrollToPosition(0);

    return NOERROR;
}


//=================================================================
// CallLogFragment::StatusMessageActionOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogFragment::StatusMessageActionOnClickListener, Object, IViewOnClickListener)

ECode CallLogFragment::StatusMessageActionOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_VIEW, mUri, (IIntent**)&intent);
    return IContext::Probe(activity)->StartActivity(intent);
}


//=================================================================
// CallLogFragment::FooterViewOnClickListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogFragment::FooterViewOnClickListener, Object, IViewOnClickListener)

ECode CallLogFragment::FooterViewOnClickListener::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IActivity> activity;
    mHost->GetActivity((IActivity**)&activity);
    IListsFragmentHostInterface::Probe(activity)->ShowCallHistory();
    return NOERROR;
}


//=================================================================
// CallLogFragment::ItemExpandedOnPreDrawListener::ItemExpandedAnimatorUpdateListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogFragment::ItemExpandedOnPreDrawListener::ItemExpandedAnimatorUpdateListener, Object, IAnimatorUpdateListener)

ECode CallLogFragment::ItemExpandedOnPreDrawListener::ItemExpandedAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animator)
{
    AutoPtr<IInterface> value;
    animator->GetAnimatedValue((IInterface**)&value);
    Float fvalue;
    IFloat::Probe(value)->GetValue(&fvalue);

    // For each value from 0 to 1, animate the various parts of the layout.
    AutoPtr<IViewGroupLayoutParams> params;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(fvalue * mDistance + mBaseHeight);

    Float z = mHost->mExpandedItemTranslationZ * fvalue;
    mViewHolder->mCallLogEntryView->SetTranslationZ(z);
    mView->SetTranslationZ(z); // WAR
    mView->RequestLayout();

    if (mIsExpand) {
        if (mListView != NULL) {
            Int32 scrollBy = (Int32)(fvalue * mScrollingNeeded) - mCurrentScroll;
            IAbsListView::Probe(mListView)->SmoothScrollBy(scrollBy, /* duration = */ 0);
            mCurrentScroll += scrollBy;
        }
    }

    return NOERROR;
}


//=================================================================
// CallLogFragment::ItemExpandedOnPreDrawListener::ItemExpandedAnimatorListenerAdapter
//=================================================================

ECode CallLogFragment::ItemExpandedOnPreDrawListener::ItemExpandedAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animator)
{
    AutoPtr<IViewGroupLayoutParams> params;
    mView->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(IViewGroupLayoutParams::WRAP_CONTENT);

    if (!mIsExpand) {
        mViewHolder->mActionsView->SetVisibility(IView::GONE);
    }
    else {
        // This seems like it should be unnecessary, but without this, after
        // navigating out of the activity and then back, the action view alpha
        // is defaulting to the value (0) at the start of the expand animation.
        mViewHolder->mActionsView->SetAlpha(1);
    }
    CallLogAdapter::ExpandVoicemailTranscriptionView(mViewHolder, mIsExpand);

    return NOERROR;
}


//=================================================================
// CallLogFragment::ItemExpandedOnPreDrawListener
//=================================================================
CAR_INTERFACE_IMPL(CallLogFragment::ItemExpandedOnPreDrawListener, Object, IOnPreDrawListener)

ECode CallLogFragment::ItemExpandedOnPreDrawListener::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // We don't want to continue getting called for every draw.
    Boolean alive;
    if (mObserver->IsAlive(&alive), alive) {
        mObserver->RemoveOnPreDrawListener(this);
    }
    // Calculate some values to help with the animation.
    AutoPtr<IView> view = IView::Probe(mView);
    Int32 endingHeight;
    view->GetHeight(&endingHeight);
    Int32 distance = Elastos::Core::Math::Abs(endingHeight - mStartingHeight);
    Int32 baseHeight = Elastos::Core::Math::Min(endingHeight, mStartingHeight);
    Boolean isExpand = endingHeight > mStartingHeight;

    // Set the views back to the start state of the animation
    AutoPtr<IViewGroupLayoutParams> params;
    view->GetLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetHeight(mStartingHeight);
    if (!isExpand) {
        mViewHolder->mActionsView->SetVisibility(IView::VISIBLE);
    }
    CallLogAdapter::ExpandVoicemailTranscriptionView(mViewHolder, !isExpand);

    // Set up the fade effect for the action buttons.
    if (isExpand) {
        // Start the fade in after the expansion has partly completed, otherwise it
        // will be mostly over before the expansion completes.
        mViewHolder->mActionsView->SetAlpha(0.0);
        AutoPtr<IViewPropertyAnimator> animator;
        mViewHolder->mActionsView->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(1.0);
        animator->SetStartDelay(mHost->mFadeInStartDelay);
        animator->SetDuration(mHost->mFadeInDuration);
        animator->Start();
    }
    else {
        mViewHolder->mActionsView->SetAlpha(1.0);
        AutoPtr<IViewPropertyAnimator> animator;
        mViewHolder->mActionsView->Animate((IViewPropertyAnimator**)&animator);
        animator->Alpha(0.0);
        animator->SetDuration(mHost->mFadeOutDuration);
        animator->Start();
    }
    view->RequestLayout();

    // Set up the animator to animate the expansion and shadow depth.
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    AutoPtr<ArrayOf<Float> > args = ArrayOf<Float>::Alloc(2);
    if (isExpand) {
        (*args)[0] = 0.0;
        (*args)[1] = 1.0;
    }
    else {
        (*args)[0] = 1.0;
        (*args)[1] = 0.0;
    }
    helper->OfFloat(args, (IValueAnimator**)&animator);

    // Figure out how much scrolling is needed to make the view fully visible.
    AutoPtr<IRect> localVisibleRect;
    CRect::New((IRect**)&localVisibleRect);
    Boolean succeeded;
    view->GetLocalVisibleRect(localVisibleRect, &succeeded);
    Int32 top;
    localVisibleRect->GetTop(&top);
    Int32 scrollingNeeded;
    if (top > 0) {
        scrollingNeeded = -top;
    }
    else {
        Int32 measuredHeight, height;
        view->GetMeasuredHeight(&measuredHeight);
        localVisibleRect->GetHeight(&height);
        scrollingNeeded = measuredHeight - height;
    }

    AutoPtr<IListView> listView;
    mHost->GetListView((IListView**)&listView);
    AutoPtr<IAnimatorUpdateListener> listener = (IAnimatorUpdateListener*)new ItemExpandedAnimatorUpdateListener(
            distance, baseHeight, scrollingNeeded, isExpand, view, mViewHolder, listView, mHost);
    animator->AddUpdateListener(listener);

    // Set everything to their final values when the animation's done.
    AutoPtr<IAnimatorListener> adapter = (IAnimatorListener*)new ItemExpandedAnimatorListenerAdapter(
            isExpand, view, mViewHolder);
    AutoPtr<IAnimator> a = IAnimator::Probe(animator);
    a->AddListener(adapter);
    animator->SetDuration(mHost->mExpandCollapseDuration);
    a->Start();

    // Return false so this draw does not occur to prevent the final frame from
    // being drawn for the single frame before the animations start.
    *result = FALSE;
    return NOERROR;
}


//=================================================================
// CallLogFragment
//=================================================================
const String CallLogFragment::TAG("CallLogFragment");
const String CallLogFragment::REPORT_DIALOG_TAG("report_dialog");
const Int32 CallLogFragment::EMPTY_LOADER_ID;
const String CallLogFragment::KEY_FILTER_TYPE("filter_type");
const String CallLogFragment::KEY_LOG_LIMIT("log_limit");
const String CallLogFragment::KEY_DATE_LIMIT("date_limit");
const String CallLogFragment::KEY_SHOW_FOOTER("show_footer");
const String CallLogFragment::KEY_IS_REPORT_DIALOG_SHOWING("is_report_dialog_showing");
const String CallLogFragment::KEY_REPORT_DIALOG_NUMBER("report_dialog_number");

CAR_INTERFACE_IMPL_5(CallLogFragment, AnalyticsListFragment, ICallLogFragment, ICallLogQueryHandlerListener,
        ICallLogAdapterOnReportButtonClickListener, ICallFetcher, ICallItemExpandedListener)

CallLogFragment::CallLogFragment()
    : mIsReportDialogShowing(FALSE)
    , mScrollToTop(FALSE)
    , mVoicemailSourcesAvailable(FALSE)
    , mEmptyLoaderRunning(FALSE)
    , mCallLogFetched(FALSE)
    , mVoicemailStatusFetched(FALSE)
    , mExpandedItemTranslationZ(0.0)
    , mFadeInDuration(0)
    , mFadeInStartDelay(0)
    , mFadeOutDuration(0)
    , mExpandCollapseDuration(0)
    , mRefreshDataRequired(TRUE)
    , mMenuVisible(TRUE)
    , mCallTypeFilter(CallLogQueryHandler::CALL_TYPE_ALL)
    , mLogLimit(-1)
    , mDateLimit(0)
    , mHasFooterView(FALSE)
{
    CHandler::New((IHandler**)&mHandler);
    mCallLogObserver = (IContentObserver*)new CustomContentObserver(this);
    mContactsObserver = (IContentObserver*)new CustomContentObserver(this);
    mVoicemailStatusObserver = (IContentObserver*)new CustomContentObserver(this);
}

ECode CallLogFragment::constructor()
{
    return constructor(CallLogQueryHandler::CALL_TYPE_ALL, -1);
}

ECode CallLogFragment::constructor(
    /* [in] */ Int32 filterType)
{
    return constructor(filterType, -1);
}

ECode CallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int32 logLimit)
{
    AnalyticsListFragment::constructor();
    mCallTypeFilter = filterType;
    mLogLimit = logLimit;
    return NOERROR;
}

ECode CallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int64 dateLimit)
{
    return constructor(filterType, -1, dateLimit);
}

ECode CallLogFragment::constructor(
    /* [in] */ Int32 filterType,
    /* [in] */ Int32 logLimit,
    /* [in] */ Int64 dateLimit)
{
    constructor(filterType, logLimit);
    mDateLimit = dateLimit;
    return NOERROR;
}

ECode CallLogFragment::OnCreate(
    /* [in] */ IBundle* state)
{
    FAIL_RETURN(AnalyticsListFragment::OnCreate(state))
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
    AutoPtr<IContext> context = IContext::Probe(activity);
    String currentCountryIso = GeoUtil::GetCurrentCountryIso(context);
    AutoPtr<ContactInfoHelper> ciHelper  = new ContactInfoHelper(context, currentCountryIso);
    mAdapter = ObjectFactory::NewCallLogAdapter(context, ICallFetcher::Probe(this),
            ciHelper, ICallItemExpandedListener::Probe(this),
            ICallLogAdapterOnReportButtonClickListener::Probe(this), TRUE);
    SetListAdapter(IListAdapter::Probe(mAdapter));

    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    mCallLogQueryHandler = new CallLogQueryHandler();
    mCallLogQueryHandler->constructor(resolver, ICallLogQueryHandlerListener::Probe(this), mLogLimit);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::KEYGUARD_SERVICE, (IInterface**)&service);
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
    SetHasOptionsMenu(TRUE);
    UpdateCallList(mCallTypeFilter, mDateLimit);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    resources->GetDimension(Elastos::Droid::Dialer::R::dimen::call_log_expanded_translation_z, &mExpandedItemTranslationZ);
    resources->GetInteger(Elastos::Droid::Dialer::R::integer::call_log_actions_fade_in_duration, &mFadeInDuration);
    resources->GetInteger(Elastos::Droid::Dialer::R::integer::call_log_actions_fade_start, &mFadeInStartDelay);
    resources->GetInteger(Elastos::Droid::Dialer::R::integer::call_log_actions_fade_out_duration, &mFadeOutDuration);
    resources->GetInteger(
            Elastos::Droid::Dialer::R::integer::call_log_expand_collapse_duration, &mExpandCollapseDuration);

    if (mIsReportDialogShowing) {
        AutoPtr<IDialogFragment> df = ObjectFactory::GetReportDialogFragment(mReportDialogNumber);
        if (df != NULL) {
            IFragment::Probe(df)->SetTargetFragment(IFragment::Probe(this), 0);
            AutoPtr<IFragmentManager> manager;
            activity->GetFragmentManager((IFragmentManager**)&manager);
            df->Show(manager, REPORT_DIALOG_TAG);
        }
    }

    return NOERROR;
}

ECode CallLogFragment::OnCallsFetched(
    /* [in] */ ICursor* cursor,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Boolean isFinishing;
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
        AutoPtr<IAdapterView> av = IAdapterView::Probe(listView);
        Int32 position;
        av->GetFirstVisiblePosition(&position);
        if (position > 5) {
            av->SetSelection(5);
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

ECode CallLogFragment::OnVoicemailStatusFetched(
    /* [in] */ ICursor* statusCursor)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    Boolean isFinishing;
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

void CallLogFragment::DestroyEmptyLoaderIfAllDataFetched()
{
    if (mCallLogFetched && mVoicemailStatusFetched && mEmptyLoaderRunning) {
        mEmptyLoaderRunning = FALSE;
        AutoPtr<ILoaderManager> manager;
        GetLoaderManager((ILoaderManager**)&manager);
        manager->DestroyLoader(EMPTY_LOADER_ID);
    }
}

void CallLogFragment::SetVoicemailSourcesAvailable(
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

ECode CallLogFragment::OnCreateView(
    /* [in] */ ILayoutInflater* inflater,
    /* [in] */ IViewGroup* container,
    /* [in] */ IBundle* savedState,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Dialer::R::layout::call_log_fragment, container, FALSE, (IView**)&view);
    mVoicemailStatusHelper = (IVoicemailStatusHelper*)new VoicemailStatusHelperImpl();
    mStatusMessageView = NULL;
    view->FindViewById(Elastos::Droid::Dialer::R::id::voicemail_status, (IView**)&mStatusMessageView);

    AutoPtr<IView> temp;
    view->FindViewById(Elastos::Droid::Dialer::R::id::voicemail_status_message, (IView**)&temp);
    mStatusMessageText = ITextView::Probe(temp);
    temp = NULL;
    view->FindViewById(Elastos::Droid::Dialer::R::id::voicemail_status_action, (IView**)&temp);
    mStatusMessageAction = ITextView::Probe(temp);

    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CallLogFragment::OnViewCreated(
    /* [in] */ IView* view,
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(AnalyticsListFragment::OnViewCreated(view, savedInstanceState))

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IView> emptyView;
    view->FindViewById(Elastos::Droid::Dialer::R::id::empty_list_view, (IView**)&emptyView);
    IAdapterView::Probe(listView)->SetEmptyView(emptyView);
    listView->SetItemsCanFocus(TRUE);
    MaybeAddFooterView();

    return UpdateEmptyMessage(mCallTypeFilter);
}

ECode CallLogFragment::ConfigureScreenFromIntent(
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

ECode CallLogFragment::OnStart()
{
    // TODO:
    // Start the empty loader now to defer other fragments.  We destroy it when both calllog
    // and the voicemail status are fetched.
    // AutoPtr<ILoaderManager> manager;
    // GetLoaderManager((ILoaderManager**)&manager);
    // AutoPtr<IActivity> activity;
    // GetActivity((IActivity**)&activity);
    // AutoPtr<EmptyLoader::Callback> callback = new EmptyLoader::Callback(IContext::Probe(activity));
    // AutoPtr<ILoader> loader;
    // manager->InitLoader(EMPTY_LOADER_ID, NULL,
    //         (ILoaderManagerLoaderCallbacks*)callback, (ILoader**)&loader);
    // mEmptyLoaderRunning = TRUE;
    return AnalyticsListFragment::OnStart();
}

ECode CallLogFragment::OnResume()
{
    AnalyticsListFragment::OnResume();
    RefreshData();
    return NOERROR;
}

void CallLogFragment::UpdateVoicemailStatusMessage(
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
        AutoPtr<VoicemailStatusHelperStatusMessage> message = (VoicemailStatusHelperStatusMessage*)(IObject*)item.Get();
        if (message->ShowInCallLog()) {
            mStatusMessageText->SetText(message->mCallLogMessageId);
        }
        if (message->mActionMessageId != -1) {
            mStatusMessageAction->SetText(message->mActionMessageId);
        }
        AutoPtr<IView> statusMessageAction = IView::Probe(mStatusMessageAction);
        if (message->mActionUri != NULL) {
            statusMessageAction->SetVisibility(IView::VISIBLE);
            AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new StatusMessageActionOnClickListener(
                    message->mActionUri, this);
            statusMessageAction->SetOnClickListener(listener);
        }
        else {
            statusMessageAction->SetVisibility(IView::GONE);
        }
    }
}

ECode CallLogFragment::OnPause()
{
    AnalyticsListFragment::OnPause();
    // Kill the requests thread
    mAdapter->StopRequestProcessing();
    return NOERROR;
}

ECode CallLogFragment::OnStop()
{
    AnalyticsListFragment::OnStop();
    UpdateOnExit();
    return NOERROR;
}

ECode CallLogFragment::OnDestroy()
{
    AnalyticsListFragment::OnDestroy();
    mAdapter->StopRequestProcessing();
    mAdapter->ChangeCursor(NULL);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);
    resolver->UnregisterContentObserver(mCallLogObserver);
    resolver->UnregisterContentObserver(mContactsObserver);
    resolver->UnregisterContentObserver(mVoicemailStatusObserver);

    return NOERROR;
}

ECode CallLogFragment::OnSaveInstanceState(
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

ECode CallLogFragment::FetchCalls()
{
    mCallLogQueryHandler->FetchCalls(mCallTypeFilter, mDateLimit);

    return NOERROR;
}

ECode CallLogFragment::StartCallsQuery()
{
    mAdapter->SetLoading(TRUE);
    mCallLogQueryHandler->FetchCalls(mCallTypeFilter, mDateLimit);

    return NOERROR;
}

void CallLogFragment::StartVoicemailStatusQuery()
{
    mCallLogQueryHandler->FetchVoicemailStatus();
}

void CallLogFragment::UpdateCallList(
    /* [in] */ Int32 filterType,
    /* [in] */ Int64 dateLimit)
{
    mCallLogQueryHandler->FetchCalls(filterType, dateLimit);
}

ECode CallLogFragment::UpdateEmptyMessage(
    /* [in] */ Int32 filterType)
{
    Int32 messageId;
    switch (filterType) {
        case ICalls::MISSED_TYPE:
            messageId = Elastos::Droid::Dialer::R::string::recentMissed_empty;
            break;
        case ICalls::VOICEMAIL_TYPE:
            messageId = Elastos::Droid::Dialer::R::string::recentVoicemails_empty;
            break;
        case CallLogQueryHandler::CALL_TYPE_ALL:
            messageId = Elastos::Droid::Dialer::R::string::recentCalls_empty;
            break;
        default:
            Logger::E(TAG, "Unexpected filter type in CallLogFragment: %d", filterType);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IView> view;
    IAdapterView::Probe(listView)->GetEmptyView((IView**)&view);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    DialerUtils::ConfigureEmptyListView(
            view, Elastos::Droid::Dialer::R::drawable::empty_call_log, messageId, resources);

    return NOERROR;
}

AutoPtr<CallLogAdapter> CallLogFragment::GetAdapter()
{
    return mAdapter;
}

ECode CallLogFragment::SetMenuVisibility(
    /* [in] */ Boolean menuVisible)
{
    AnalyticsListFragment::SetMenuVisibility(menuVisible);
    if (mMenuVisible != menuVisible) {
        mMenuVisible = menuVisible;
        Boolean isResumed;
        if (!menuVisible) {
            UpdateOnExit();
        }
        else if (IsResumed(&isResumed), isResumed) {
            RefreshData();
        }
    }

    return NOERROR;
}

void CallLogFragment::RefreshData()
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

void CallLogFragment::UpdateOnExit()
{
    UpdateOnTransition(FALSE);
}

void CallLogFragment::UpdateOnEntry()
{
    UpdateOnTransition(TRUE);
}

void CallLogFragment::UpdateOnTransition(
    /* [in] */ Boolean onEntry)
{
    // We don't want to update any call data when keyguard is on because the user has likely not
    // seen the new calls yet.
    // This might be called before onCreate() and thus we need to check null explicitly.
    Boolean result;
    if (mKeyguardManager != NULL &&
            (mKeyguardManager->InKeyguardRestrictedInputMode(&result), !result)) {
        // On either of the transitions we update the missed call and voicemail notifications.
        // While exiting we additionally consume all missed calls (by marking them as read).
        mCallLogQueryHandler->MarkNewCallsAsOld();
        if (!onEntry) {
            mCallLogQueryHandler->MarkMissedCallsAsRead();
        }
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        CallLogNotificationsHelper::RemoveMissedCallNotifications(IContext::Probe(activity));
        CallLogNotificationsHelper::UpdateVoicemailNotifications(IContext::Probe(activity));
    }
}

ECode CallLogFragment::SetHasFooterView(
    /* [in] */ Boolean hasFooterView)
{
    mHasFooterView = hasFooterView;
    MaybeAddFooterView();
    return NOERROR;
}

void CallLogFragment::MaybeAddFooterView()
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
        AutoPtr<IViewGroup> viewGroup = IViewGroup::Probe(view);
        inflater->Inflate(Elastos::Droid::Dialer::R::layout::recents_list_footer,
                viewGroup, FALSE, (IView**)&mFooterView);

        AutoPtr<IViewOnClickListener> listener = (IViewOnClickListener*)new FooterViewOnClickListener(this);
        mFooterView->SetOnClickListener(listener);
    }

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    Boolean result;
    listView->RemoveFooterView(mFooterView, &result);
    listView->AddFooterView(mFooterView);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    ViewUtil::AddBottomPaddingToListViewForFab(listView, resources);
}

ECode CallLogFragment::OnItemExpanded(
    /* [in] */ ICallLogListItemView* view)
{
    AutoPtr<IView> itemView = IView::Probe(view);
    Int32 startingHeight;
    itemView->GetHeight(&startingHeight);
    AutoPtr<IInterface> temp;
    itemView->GetTag((IInterface**)&temp);
    AutoPtr<CallLogListItemViews> viewHolder = (CallLogListItemViews*)(IObject*)temp.Get();

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IViewTreeObserver> observer;
    IView::Probe(listView)->GetViewTreeObserver((IViewTreeObserver**)&observer);

    AutoPtr<IOnPreDrawListener> listener = (IOnPreDrawListener*)new ItemExpandedOnPreDrawListener(
            view, viewHolder, observer, startingHeight, this);
    observer->AddOnPreDrawListener(listener);

    return NOERROR;
}

ECode CallLogFragment::GetViewForCallId(
    /* [in] */ Int64 callId,
    /* [out] */ ICallLogListItemView** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IListView> listView;
    GetListView((IListView**)&listView);
    AutoPtr<IAdapterView> av = IAdapterView::Probe(listView);
    Int32 firstPosition;
    av->GetFirstVisiblePosition(&firstPosition);
    Int32 lastPosition;
    av->GetLastVisiblePosition(&lastPosition);

    for (Int32 position = 0; position <= lastPosition - firstPosition; position++) {
        AutoPtr<IView> view;
        IViewGroup::Probe(listView)->GetChildAt(position, (IView**)&view);

        if (view != NULL) {
            AutoPtr<IInterface> tag;
            view->GetTag((IInterface**)&tag);
            if (tag != NULL) {
                AutoPtr<CallLogListItemViews> viewHolder = (CallLogListItemViews*)(IObject*)tag.Get();
                if (viewHolder->mRowId == callId) {
                    *result = ICallLogListItemView::Probe(view);
                    REFCOUNT_ADD(*result)
                    return NOERROR;
                }
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CallLogFragment::OnBadDataReported(
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

ECode CallLogFragment::OnReportButtonClick(
    /* [in] */ const String& number)
{
    AutoPtr<IDialogFragment> df = ObjectFactory::GetReportDialogFragment(number);
    if (df != NULL) {
        IFragment::Probe(df)->SetTargetFragment((IFragment*)this, 0);
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
} // Droid
} // Elastos
