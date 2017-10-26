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

#include "elastos/droid/systemui/keyguard/CKeyguardHostView.h"
#include "elastos/droid/systemui/keyguard/KeyguardViewStateManager.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/systemui/keyguard/CameraWidgetFrame.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/os/Looper.h"
#include "Elastos.Droid.Provider.h"
#include "R.h"

using Elastos::Droid::App::IActivityOptions;
using Elastos::Droid::App::IActivityOptionsHelper;
using Elastos::Droid::App::CActivityOptionsHelper;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::AppWidget::CAppWidgetHost;
using Elastos::Droid::AppWidget::IAppWidgetProviderInfo;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::ILockPatternUtilsHelper;
using Elastos::Droid::Internal::Widget::CLockPatternUtilsHelper;
using Elastos::Droid::Media::IRemoteControlClient;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::Widget::EIID_IRemoteViewsOnClickHandler;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

ECode CKeyguardHostView::MyRunnable::Run()
{
    mHost->UpdateAndAddWidgets();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyOnDismissAction, Object,
        IKeyguardHostViewOnDismissAction)

ECode CKeyguardHostView::MyOnDismissAction::OnDismiss(
    /* [out] */ Boolean* succeed)
{
    VALIDATE_NOT_NULL(succeed)

    //try
    {
        // TODO: Unregister this handler if PendingIntent.FLAG_ONE_SHOT?
        AutoPtr<IContext> context;
        FAIL_GOTO(mView->GetContext((IContext**)&context), EXIT)

        Int32 width;
        FAIL_GOTO(mView->GetMeasuredWidth(&width), EXIT)
        Int32 height;
        FAIL_GOTO(mView->GetMeasuredHeight(&height), EXIT)
        AutoPtr<IActivityOptionsHelper> helper;
        CActivityOptionsHelper::AcquireSingleton((IActivityOptionsHelper**)&helper);
        AutoPtr<IActivityOptions> opts;
        FAIL_GOTO(helper->MakeScaleUpAnimation(mView, 0, 0, width, height, (IActivityOptions**)&opts), EXIT)

        AutoPtr<IIntentSender> intentSender;
        FAIL_GOTO(mPendingIntent->GetIntentSender((IIntentSender**)&intentSender), EXIT)
        AutoPtr<IBundle> b;
        FAIL_GOTO(opts->ToBundle((IBundle**)&b), EXIT)
        FAIL_GOTO(context->StartIntentSender(intentSender, mFillInIntent,
                IIntent::FLAG_ACTIVITY_NEW_TASK,
                IIntent::FLAG_ACTIVITY_NEW_TASK, 0, b), EXIT)
    }
    // } catch (IntentSender.SendIntentException e) {
    //     android.util.Log.e(TAG, "Cannot send pending intent: ", e);
    // } catch (Exception e) {
    //     android.util.Log.e(TAG, "Cannot send pending intent due to " +
    //             "unknown exception: ", e);
    // }
EXIT:
    *succeed = FALSE;
    return NOERROR;
}

CKeyguardHostView::MyOnClickHandler::MyOnClickHandler(
    /* [in] */ CKeyguardHostView* host)
    : mHost(host)
{
    IWeakReferenceSource::Probe((IKeyguardHostView*)host)->GetWeakReference((IWeakReference**)&mKeyguardHostView);
}

ECode CKeyguardHostView::MyOnClickHandler::OnClickHandler(
    /* [in] */ IView* view,
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* fillInIntent,
    /* [out] */ Boolean* succeed)
{
    AutoPtr<IInterface> obj;
    mKeyguardHostView->Resolve(EIID_IInterface, (IInterface**)&obj);
    AutoPtr<CKeyguardHostView> hostView = (CKeyguardHostView*)IKeyguardHostView::Probe(obj);
    if (hostView == NULL) {
        *succeed = FALSE;
        return NOERROR;
    }
    Boolean res;
    if (pendingIntent->IsActivity(&res), res) {
        AutoPtr<IKeyguardHostViewOnDismissAction> action = new MyOnDismissAction(view, pendingIntent, fillInIntent);
        IKeyguardViewBase::Probe(hostView)->SetOnDismissAction(action);

        if (hostView->mViewStateManager->IsChallengeShowing(&res), res) {
            hostView->mViewStateManager->ShowBouncer(TRUE);
        }
        else {
            IKeyguardViewBase::Probe(hostView)->Dismiss(&res);
        }
        *succeed = TRUE;
        return NOERROR;
    }
    else {
        return RemoteViews::RemoteViewsOnClickHandler::OnClickHandler(view,
                pendingIntent, fillInIntent, &res);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyOnClickListener, Object, IViewOnClickListener)

ECode CKeyguardHostView::MyOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    // Pass in an invalid widget id... the picker will allocate an ID for us
    AutoPtr<IKeyguardActivityLauncher> launcher;
    mHost->GetActivityLauncher((IKeyguardActivityLauncher**)&launcher);
    return launcher->LaunchWidgetPicker(IAppWidgetManager::INVALID_APPWIDGET_ID);
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyControlCallback, Object, IKeyguardHostViewTransportControlCallback)

ECode CKeyguardHostView::MyControlCallback::UserActivity()
{
    return mHost->mViewMediatorCallback->UserActivity();
}

CAR_INTERFACE_IMPL(CKeyguardHostView::SavedState, View::BaseSavedState,
        IKeyguardHostViewSavedState)

CKeyguardHostView::SavedState::SavedState()
    : mTransportState(0)
    , mAppWidgetToShow(IAppWidgetManager::INVALID_APPWIDGET_ID)
{
    CRect::New((IRect**)&mInsets);
}

ECode CKeyguardHostView::SavedState::constructor(
    /* [in] */ IParcelable* superState)
{
    return BaseSavedState::constructor(superState);
}

ECode CKeyguardHostView::SavedState::WriteToParcel(
    /* [in] */ IParcel* _out)
{
    assert(0);
    //BaseSavedState::WriteToParcel(out, flags);
    _out->WriteInt32(mTransportState);
    _out->WriteInt32(mAppWidgetToShow);
    assert(0);
    return NOERROR;
    //return _out->WriteParcelable(insets, 0);
}

ECode CKeyguardHostView::SavedState::constructor(
    /* [in] */ IParcel* in)
{
    assert(0);
    //BaseSavedState::constructor(in);
    in->ReadInt32(&mTransportState);
    in->ReadInt32(&mAppWidgetToShow);
    assert(0);
    //return in->ReadParcelable(NULL, mInsets);
    return NOERROR;
}

ECode CKeyguardHostView::MyRunnable2::Run()
{
    return IPagedView::Probe(mHost->mAppWidgetContainer)->SetCurrentPage(mPageToShow);
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyUserSwitcherCallback, Object,
        IKeyguardHostViewUserSwitcherCallback)

ECode CKeyguardHostView::MyUserSwitcherCallback::HideSecurityView(
   /* [in] */ Int32 duration)
{
    AutoPtr<IKeyguardSecurityContainer> container;
    mHost->GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    AutoPtr<IViewPropertyAnimator> animator;
    IView::Probe(container)->Animate((IViewPropertyAnimator**)&animator);
    animator->Alpha(0.0f);
    return animator->SetDuration(duration);
}

ECode CKeyguardHostView::MyUserSwitcherCallback::ShowSecurityView()
{
    AutoPtr<IKeyguardSecurityContainer> container;
    mHost->GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    return IView::Probe(container)->SetAlpha(1.0f);
}

ECode CKeyguardHostView::MyUserSwitcherCallback::ShowUnlockHint()
{
    AutoPtr<IKeyguardSecurityContainer> container;
    mHost->GetSecurityContainer((IKeyguardSecurityContainer**)&container);

    if (container != NULL) {
        IKeyguardSecurityView::Probe(container)->ShowUsabilityHint();
    }
    return NOERROR;
}

ECode CKeyguardHostView::MyUserSwitcherCallback::UserActivity()
{
    if (mHost->mViewMediatorCallback != NULL) {
        mHost->mViewMediatorCallback->UserActivity();
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyKeyguardWidgetPagerCallbacks, Object,
        IKeyguardWidgetPagerCallbacks)

ECode CKeyguardHostView::MyKeyguardWidgetPagerCallbacks::UserActivity()
{
    return mHost->UserActivity();
}

ECode CKeyguardHostView::MyKeyguardWidgetPagerCallbacks::OnUserActivityTimeoutChanged()
{
    return mHost->OnUserActivityTimeoutChanged();
}

ECode CKeyguardHostView::MyKeyguardWidgetPagerCallbacks::OnAddView(
    /* [in] */ IView* v)
{
    if (!mHost->ShouldEnableAddWidget()) {
        mHost->mAppWidgetContainer->SetAddWidgetEnabled(FALSE);
    }
    return NOERROR;
}

ECode CKeyguardHostView::MyKeyguardWidgetPagerCallbacks::OnRemoveView(
    /* [in] */ IView* v,
    /* [in] */ Boolean deletePermanently)
{
    if (deletePermanently) {
        Int32 appWidgetId;
        IKeyguardWidgetFrame::Probe(v)->GetContentAppWidgetId(&appWidgetId);
        if (appWidgetId != IAppWidgetManager::INVALID_APPWIDGET_ID &&
                appWidgetId != ILockPatternUtils::ID_DEFAULT_STATUS_WIDGET) {
            mHost->mAppWidgetHost->DeleteAppWidgetId(appWidgetId);
        }
    }
    return NOERROR;
}

ECode CKeyguardHostView::MyKeyguardWidgetPagerCallbacks::OnRemoveViewAnimationCompleted()
{
    if (mHost->ShouldEnableAddWidget()) {
        mHost->mAppWidgetContainer->SetAddWidgetEnabled(TRUE);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardHostView::MyCameraWidgetFrameCallbacks, Object, ICameraWidgetFrameCallbacks)

ECode CKeyguardHostView::MyCameraWidgetFrameCallbacks::OnLaunchingCamera()
{
    SetSliderHandleAlpha(0.0f);
    return NOERROR;
}

ECode CKeyguardHostView::MyCameraWidgetFrameCallbacks::OnCameraLaunchedSuccessfully()
{
    Int32 page;
    IPagedView::Probe(mHost->mAppWidgetContainer)->GetCurrentPage(&page);
    Boolean res;
    if (mHost->mAppWidgetContainer->IsCameraPage(page, &res), res) {
        IPagedView::Probe(mHost->mAppWidgetContainer)->ScrollLeft();
    }
    SetSliderHandleAlpha(1.0f);
    mHost->mShowSecurityWhenReturn = TRUE;
    return NOERROR;
}

ECode CKeyguardHostView::MyCameraWidgetFrameCallbacks::OnCameraLaunchedUnsuccessfully()
{
    SetSliderHandleAlpha(1.0);
    return NOERROR;
}

void CKeyguardHostView::MyCameraWidgetFrameCallbacks::SetSliderHandleAlpha(
    /* [in] */ Float alpha)
{
    AutoPtr<IView> view;
    mHost->FindViewById(R::id::sliding_layout, (IView**)&view);
    AutoPtr<ISlidingChallengeLayout> slider = ISlidingChallengeLayout::Probe(view);
    if (slider != NULL) {
        slider->SetHandleAlpha(alpha);
    }
}

ECode CKeyguardHostView::MyRunnable3::Run()
{
    mHost->ShowAppropriateWidgetPage();
    return NOERROR;
}

ECode CKeyguardHostView::MyKeyguardUpdateMonitorCallback::OnBootCompleted()
{
    if (mHost->mPostBootCompletedRunnable != NULL) {
        mHost->mPostBootCompletedRunnable->Run();
        mHost->mPostBootCompletedRunnable = NULL;
    }
    return NOERROR;
}

ECode CKeyguardHostView::MyKeyguardUpdateMonitorCallback::OnUserSwitchComplete(
    /* [in] */ Int32 userId)
{
    if (mHost->mKeyguardMultiUserSelectorView != NULL) {
        mHost->mKeyguardMultiUserSelectorView->FinalizeActiveUserView(TRUE);
    }
    return NOERROR;
}

Boolean CKeyguardHostView::DEBUG = IKeyguardConstants::DEBUG;
Boolean CKeyguardHostView::DEBUGXPORT = TRUE; // debug music transport control

const Int32 CKeyguardHostView::TRANSPORT_GONE = 0;
const Int32 CKeyguardHostView::TRANSPORT_INVISIBLE = 1;
const Int32 CKeyguardHostView::TRANSPORT_VISIBLE = 2;

const Int32 CKeyguardHostView::APPWIDGET_HOST_ID = 0x4B455947;

const String CKeyguardHostView::TAG("KeyguardHostView");

CAR_OBJECT_IMPL(CKeyguardHostView)

CAR_INTERFACE_IMPL(CKeyguardHostView, KeyguardViewBase, IKeyguardHostView)

CKeyguardHostView::CKeyguardHostView()
    : mFailedAttempts(0)
    , mClientGeneration(0)
    , mShowSecurityWhenReturn(FALSE)
    , mTransportState(TRANSPORT_GONE)
    , MAX_WIDGETS(5)
    , mAppWidgetToShow(0)
    , mDisabledFeatures(0)
    , mCameraDisabled(FALSE)
    , mSafeModeEnabled(FALSE)
    , mUserSetupCompleted(FALSE)
    , mUserId(0)
{
    CRect::New((IRect**)&mTempRect);

    CRect::New((IRect**)&mInsets);

    mOnClickHandler = new MyOnClickHandler(this);

    mUpdateMonitorCallbacks = new MyKeyguardUpdateMonitorCallback(this);

    mWidgetCallbacks = new MyKeyguardWidgetPagerCallbacks(this);

    mCameraWidgetCallbacks = new MyCameraWidgetFrameCallbacks(this);

    mSwitchPageRunnable = new MyRunnable3(this);
}

ECode CKeyguardHostView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardHostView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    KeyguardViewBase::constructor(context, attrs);

    if (DEBUG) Logger::E(TAG, "KeyguardHostView()");

    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);

    // Note: This depends on KeyguardHostView getting reconstructed every time the
    // user switches, since mUserId will be used for the entire session.
    // Once created, keyguard should *never* re-use this instance with another user.
    // In other words, mUserId should never change - hence it's marked final.
    mLockPatternUtils->GetCurrentUser(&mUserId);

    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    AutoPtr<IDevicePolicyManager> dpm = IDevicePolicyManager::Probe(obj);
    if (dpm != NULL) {
        mDisabledFeatures = GetDisabledFeatures(dpm);
        mCameraDisabled = dpm->GetCameraDisabled(NULL, &mCameraDisabled);
    }

    AutoPtr<ILockPatternUtilsHelper> helper;
    CLockPatternUtilsHelper::AcquireSingleton((ILockPatternUtilsHelper**)&helper);
    helper->IsSafeModeEnabled(&mSafeModeEnabled);

    // These need to be created with the user context...
    AutoPtr<IContext> userContext;
    //try {
    ECode ec = NOERROR;
    String packageName("system");
    AutoPtr<IUserHandle> handle;
    CUserHandle::New(mUserId, (IUserHandle**)&handle);
    ec = mContext->CreatePackageContextAsUser(packageName, 0, handle, (IContext**)&userContext);
    //} catch (NameNotFoundException e) {
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        //e.printStackTrace();
        // This should never happen, but it's better to have no widgets than to crash.
        userContext = context;
    }

    CAppWidgetHost::New(userContext, APPWIDGET_HOST_ID, mOnClickHandler,
            Looper::GetMyLooper(), (IAppWidgetHost**)&mAppWidgetHost);

    AutoPtr<IAppWidgetManagerHelper> helper2;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&helper2);
    helper2->GetInstance(userContext, (IAppWidgetManager**)&mAppWidgetManager);

    AutoPtr<KeyguardViewStateManager> manager =
            new KeyguardViewStateManager();
    manager->constructor((IKeyguardHostView*)this);
    mViewStateManager = IKeyguardViewStateManager::Probe(manager);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> helper3;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper3);
    Int32 value;
    helper3->GetInt32ForUser(cr,
            ISettingsSecure::USER_SETUP_COMPLETE, 0, IUserHandle::USER_CURRENT, &value);
    mUserSetupCompleted = value != 0;

    // Ensure we have the current state *before* we call showAppropriateWidgetPage()
    GetInitialTransportState();

    if (mSafeModeEnabled) {
        Logger::V(TAG, "Keyguard widgets disabled by safe mode");
    }
    if ((mDisabledFeatures & IDevicePolicyManager::KEYGUARD_DISABLE_WIDGETS_ALL) != 0) {
        Logger::V(TAG, "Keyguard widgets disabled by DPM");
    }
    if ((mDisabledFeatures & IDevicePolicyManager::KEYGUARD_DISABLE_SECURE_CAMERA) != 0) {
        Logger::V(TAG, "Keyguard secure camera disabled by DPM");
    }
    return NOERROR;
}

void CKeyguardHostView::GetInitialTransportState()
{
    AutoPtr<IKeyguardUpdateMonitorDisplayClientState> dcs;
    KeyguardUpdateMonitor::GetInstance(mContext)->GetCachedDisplayClientState(
            (IKeyguardUpdateMonitorDisplayClientState**)&dcs);
    AutoPtr<KeyguardUpdateMonitor::DisplayClientState> _dcs =
            (KeyguardUpdateMonitor::DisplayClientState*)dcs.Get();
    mTransportState = (_dcs->mClearing ? TRANSPORT_GONE :
        (IsMusicPlaying(_dcs->mPlaybackState) ? TRANSPORT_VISIBLE : TRANSPORT_INVISIBLE));

    if (DEBUGXPORT) Logger::V(TAG, "Initial transport state: %d, pbstate= %d",
            mTransportState, _dcs->mPlaybackState);
}

void CKeyguardHostView::CleanupAppWidgetIds()
{
    if (mSafeModeEnabled || WidgetsDisabled()) return;

    // Clean up appWidgetIds that are bound to lockscreen, but not actually used
    // This is only to clean up after another bug: we used to not call
    // deleteAppWidgetId when a user manually deleted a widget in keyguard. This code
    // shouldn't have to run more than once per user. AppWidgetProviders rely on callbacks
    // that are triggered by deleteAppWidgetId, which is why we're doing this
    AutoPtr<ArrayOf<Int32> > appWidgetIdsInKeyguardSettings;
    mLockPatternUtils->GetAppWidgets((ArrayOf<Int32>**)&appWidgetIdsInKeyguardSettings);
    AutoPtr<ArrayOf<Int32> > appWidgetIdsBoundToHost;
    mAppWidgetHost->GetAppWidgetIds((ArrayOf<Int32>**)&appWidgetIdsBoundToHost);
    for (Int32 i = 0; i < appWidgetIdsBoundToHost->GetLength(); i++) {
        Int32 appWidgetId = (*appWidgetIdsBoundToHost)[i];
        if (!Contains(appWidgetIdsInKeyguardSettings, appWidgetId)) {
            Logger::D(TAG, "Found a appWidgetId that's not being used by keyguard, deleting id %d"
                    , appWidgetId);
            mAppWidgetHost->DeleteAppWidgetId(appWidgetId);
        }
    }
}

Boolean CKeyguardHostView::Contains(
    /* [in] */ ArrayOf<Int32>* array,
    /* [in] */ Int32 target)
{
    for (Int32 i = 0; i < array->GetLength(); i++) {
        Int32 value = (*array)[i];

        if (value == target) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CKeyguardHostView::IsMusicPlaying(
    /* [in] */ Int32 playbackState)
{
    // This should agree with the list in AudioService.isPlaystateActive()
    switch (playbackState) {
        case IRemoteControlClient::PLAYSTATE_PLAYING:
        case IRemoteControlClient::PLAYSTATE_BUFFERING:
        case IRemoteControlClient::PLAYSTATE_FAST_FORWARDING:
        case IRemoteControlClient::PLAYSTATE_REWINDING:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_BACKWARDS:
        case IRemoteControlClient::PLAYSTATE_SKIPPING_FORWARDS:
            return TRUE;
        default:
            return FALSE;
    }
}

ECode CKeyguardHostView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Boolean result;
    KeyguardViewBase::OnTouchEvent(ev, &result);
    mTempRect->Set(0, 0, 0, 0);
    AutoPtr<IKeyguardSecurityContainer> container;
    GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    OffsetRectIntoDescendantCoords(IView::Probe(container), mTempRect);

    Int32 left;
    mTempRect->GetLeft(&left);
    Int32 top;
    mTempRect->GetTop(&top);
    ev->OffsetLocation(left, top);
    Boolean tmp;
    IView::Probe(container)->DispatchTouchEvent(ev, &tmp);
    result = tmp || result;
    ev->OffsetLocation(-left, -top);
    *res = result;
    return NOERROR;
}

Int32 CKeyguardHostView::GetWidgetPosition(
    /* [in] */ Int32 id)
{
    AutoPtr<IKeyguardWidgetPager> appWidgetContainer = mAppWidgetContainer;
    Int32 children;
    IViewGroup::Probe(appWidgetContainer)->GetChildCount(&children);
    for (Int32 i = 0; i < children; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        appWidgetContainer->GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        AutoPtr<IView> content;
        frame->GetContent((IView**)&content);
        Int32 _id;
        if (content != NULL && (content->GetId(&_id), _id) == id) {
            return i;
        }
        else if (content == NULL) {
            // Attempt to track down bug #8886916
            Logger::W(TAG, "*** Null content at i=%d,id=%d,N=%d", i, id, children);
        }
    }
    return -1;
}

ECode CKeyguardHostView::OnFinishInflate()
{
    KeyguardViewBase::OnFinishInflate();

    // Grab instances of and make any necessary changes to the main layouts. Create
    // view state manager and wire up necessary listeners / callbacks.
    AutoPtr<IView> deleteDropTarget;
    FindViewById(R::id::keyguard_widget_pager_delete_target, (IView**)&deleteDropTarget);

    AutoPtr<IView> view;
    FindViewById(R::id::app_widget_container, (IView**)&view);
    mAppWidgetContainer = IKeyguardWidgetPager::Probe(view);
    IView::Probe(mAppWidgetContainer)->SetVisibility(VISIBLE);
    mAppWidgetContainer->SetCallbacks(mWidgetCallbacks);
    IPagedView::Probe(mAppWidgetContainer)->SetDeleteDropTarget(deleteDropTarget);
    IPagedView::Probe(mAppWidgetContainer)->SetMinScale(0.5f);

    AutoPtr<IView> view2;
    FindViewById(R::id::sliding_layout, (IView**)&view2);
    mSlidingChallengeLayout = ISlidingChallengeLayout::Probe(view2);
    if (mSlidingChallengeLayout != NULL) {
        mSlidingChallengeLayout->SetOnChallengeScrolledListener(
                ISlidingChallengeLayoutOnChallengeScrolledListener::Probe(mViewStateManager));
    }
    mAppWidgetContainer->SetViewStateManager(mViewStateManager);
    mAppWidgetContainer->SetLockPatternUtils(mLockPatternUtils);

    AutoPtr<IView> view3;
    FindViewById(R::id::multi_pane_challenge, (IView**)&view3);
    mMultiPaneChallengeLayout = IMultiPaneChallengeLayout::Probe(view3);
    AutoPtr<IChallengeLayout> challenge;
    if (mSlidingChallengeLayout != NULL) {
        challenge = IChallengeLayout::Probe(mSlidingChallengeLayout);
    }
    else {
        challenge = IChallengeLayout::Probe(mMultiPaneChallengeLayout);
    }
    challenge->SetOnBouncerStateChangedListener(
            IChallengeLayoutOnBouncerStateChangedListener::Probe(mViewStateManager));
    Int32 duration;
    IChallengeLayout::Probe(challenge)->GetBouncerAnimationDuration(&duration);
    mAppWidgetContainer->SetBouncerAnimationDuration(duration);
    mViewStateManager->SetPagedView(mAppWidgetContainer);
    mViewStateManager->SetChallengeLayout(challenge);

    AutoPtr<IKeyguardSecurityContainer> container;
    GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    mViewStateManager->SetSecurityViewContainer(IKeyguardSecurityView::Probe(container));

    Boolean res;
    if (KeyguardUpdateMonitor::GetInstance(mContext)->HasBootCompleted(&res), res) {
        UpdateAndAddWidgets();
    }
    else {
        // We can't add widgets until after boot completes because AppWidgetHost may try
        // to contact the providers.  Do it later.
        mPostBootCompletedRunnable = new MyRunnable(this);
    }

    mViewStateManager->IsBouncing(&res);
    container->UpdateSecurityViews(res);
    EnableUserSelectorIfNecessary();
    return NOERROR;
}

void CKeyguardHostView::UpdateAndAddWidgets()
{
    CleanupAppWidgetIds();
    AddDefaultWidgets();
    AddWidgetsFromSettings();
    MaybeEnableAddButton();
    CheckAppWidgetConsistency();

    // Don't let the user drag the challenge down if widgets are disabled.
    if (mSlidingChallengeLayout != NULL) {
        mSlidingChallengeLayout->SetEnableChallengeDragging(!WidgetsDisabled());
    }

    // Select the appropriate page
    mSwitchPageRunnable->Run();

    // This needs to be called after the pages are all added.
    mViewStateManager->ShowUsabilityHints();
}

void CKeyguardHostView::MaybeEnableAddButton()
{
    if (!ShouldEnableAddWidget()) {
        mAppWidgetContainer->SetAddWidgetEnabled(FALSE);
    }
}

Boolean CKeyguardHostView::ShouldEnableAddWidget()
{
    return NumWidgets() < MAX_WIDGETS && mUserSetupCompleted;
}

ECode CKeyguardHostView::Dismiss(
    /* [in] */ Boolean authenticated,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean finished;
    KeyguardViewBase::Dismiss(authenticated, &finished);
    if (!finished) {
        mViewStateManager->ShowBouncer(TRUE);

        // Enter full screen mode if we're in SIM or Account screen
        AutoPtr<IKeyguardSecurityContainer> container;
        GetSecurityContainer((IKeyguardSecurityContainer**)&container);
        SecurityMode securityMode;
        container->GetSecurityMode(&securityMode);

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        Boolean isFullScreen;
        resources->GetBoolean(R::bool_::kg_sim_puk_account_full_screen, &isFullScreen);
        Boolean isSimOrAccount = securityMode == SecurityMode_SimPin
                || securityMode == SecurityMode_SimPuk
                || securityMode == SecurityMode_Account;
        IView::Probe(mAppWidgetContainer)->SetVisibility(
                isSimOrAccount && isFullScreen ? IView::GONE : IView::VISIBLE);

        // Don't show camera or search in navbar when SIM or Account screen is showing
        Int32 visibility;
        GetSystemUiVisibility(&visibility);
        SetSystemUiVisibility(isSimOrAccount ?
                (visibility | IView::STATUS_BAR_DISABLE_SEARCH)
                : (visibility & ~IView::STATUS_BAR_DISABLE_SEARCH));

        if (mSlidingChallengeLayout != NULL) {
            mSlidingChallengeLayout->SetChallengeInteractive(!isFullScreen);
        }
    }
    *result = finished;
    return NOERROR;
}

Int32 CKeyguardHostView::GetDisabledFeatures(
    /* [in] */ IDevicePolicyManager* dpm)
{
    Int32 disabledFeatures = IDevicePolicyManager::KEYGUARD_DISABLE_FEATURES_NONE;
    if (dpm != NULL) {
        Int32 currentUser;
        mLockPatternUtils->GetCurrentUser(&currentUser);
        dpm->GetKeyguardDisabledFeatures(NULL, currentUser, &disabledFeatures);
    }
    return disabledFeatures;
}

Boolean CKeyguardHostView::WidgetsDisabled()
{
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Boolean disabledByLowRamDevice;
    helper->IsLowRamDeviceStatic(&disabledByLowRamDevice);

    Boolean disabledByDpm =
            (mDisabledFeatures & IDevicePolicyManager::KEYGUARD_DISABLE_WIDGETS_ALL) != 0;
    Boolean res;
    Boolean disabledByUser = (mLockPatternUtils->GetWidgetsEnabled(&res), !res);
    return disabledByLowRamDevice || disabledByDpm || disabledByUser;
}

Boolean CKeyguardHostView::CameraDisabledByDpm()
{
    return mCameraDisabled
            || (mDisabledFeatures & IDevicePolicyManager::KEYGUARD_DISABLE_SECURE_CAMERA) != 0;
}

ECode CKeyguardHostView::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    KeyguardViewBase::SetLockPatternUtils(utils);

    AutoPtr<IKeyguardSecurityContainer> container;
    GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    Boolean res;
    mViewStateManager->IsBouncing(&res);
    return container->UpdateSecurityViews(res);
}

ECode CKeyguardHostView::OnAttachedToWindow()
{
    KeyguardViewBase::OnAttachedToWindow();
    mAppWidgetHost->StartListening();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RegisterCallback(mUpdateMonitorCallbacks);
}

ECode CKeyguardHostView::OnDetachedFromWindow()
{
    KeyguardViewBase::OnDetachedFromWindow();
    mAppWidgetHost->StopListening();
    return KeyguardUpdateMonitor::GetInstance(mContext)->RemoveCallback(mUpdateMonitorCallbacks);
}

ECode CKeyguardHostView::AddWidget(
    /* [in] */ IAppWidgetHostView* view,
    /* [in] */ int pageIndex)
{
    return mAppWidgetContainer->AddWidget(IView::Probe(view), pageIndex);
}

ECode CKeyguardHostView::OnUserSwitching(
    /* [in] */ Boolean switching)
{
    if (!switching && mKeyguardMultiUserSelectorView != NULL) {
        mKeyguardMultiUserSelectorView->FinalizeActiveUserView(FALSE);
    }
    return NOERROR;
}

ECode CKeyguardHostView::UserActivity()
{
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->UserActivity();
    }
    return NOERROR;
}

ECode CKeyguardHostView::OnUserActivityTimeoutChanged()
{
    if (mViewMediatorCallback != NULL) {
        mViewMediatorCallback->OnUserActivityTimeoutChanged();
    }
    return NOERROR;
}

ECode CKeyguardHostView::GetUserActivityTimeout(
    /* [in] */ Int64* time)
{
    // Currently only considering user activity timeouts needed by widgets.
    // Could also take into account longer timeouts for certain security views.
    if (mAppWidgetContainer != NULL) {
        return mAppWidgetContainer->GetUserActivityTimeout(time);
    }
    *time = -1;
    return NOERROR;
}

ECode CKeyguardHostView::OnResume()
{
    KeyguardViewBase::OnResume();
    if (mViewStateManager != NULL) {
        mViewStateManager->ShowUsabilityHints();
    }
    return NOERROR;
}

ECode CKeyguardHostView::OnPause()
{
    KeyguardViewBase::OnPause();
    // We use mAppWidgetToShow to show a particular widget after you add it-- once the screen
    // turns off we reset that behavior
    ClearAppWidgetToShow();
    Boolean res;
    if (KeyguardUpdateMonitor::GetInstance(mContext)->HasBootCompleted(&res), res) {
        CheckAppWidgetConsistency();
    }
    AutoPtr<ICameraWidgetFrame> cameraPage = FindCameraPage();
    if (cameraPage != NULL) {
        cameraPage->OnScreenTurnedOff();
    }
    return NOERROR;
}

ECode CKeyguardHostView::ClearAppWidgetToShow()
{
    mAppWidgetToShow = IAppWidgetManager::INVALID_APPWIDGET_ID;
    return NOERROR;
}

Boolean CKeyguardHostView::AddWidget(
    /* [in] */ Int32 appId,
    /* [in] */ Int32 pageIndex,
    /* [in] */ Boolean updateDbIfFailed)
{
    AutoPtr<IAppWidgetProviderInfo> appWidgetInfo;
    mAppWidgetManager->GetAppWidgetInfo(appId, (IAppWidgetProviderInfo**)&appWidgetInfo);
    if (appWidgetInfo != NULL) {
        AutoPtr<IAppWidgetHostView> view;
        mAppWidgetHost->CreateView(mContext, appId, appWidgetInfo, (IAppWidgetHostView**)&view);
        AddWidget(view, pageIndex);
        return TRUE;
    }
    else {
        if (updateDbIfFailed) {
            Logger::W(TAG, "*** AppWidgetInfo for app widget id %d  was null for user %d, deleting", appId, mUserId);
            mAppWidgetHost->DeleteAppWidgetId(appId);
            Boolean res;
            mLockPatternUtils->RemoveAppWidget(appId, &res);
        }
        return FALSE;
    }
}

Int32 CKeyguardHostView::NumWidgets()
{
    Int32 childCount;
    IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&childCount);
    Int32 widgetCount = 0;
    for (Int32 i = 0; i < childCount; i++) {
        Boolean res;
        if (mAppWidgetContainer->IsWidgetPage(i, &res), res) {
            widgetCount++;
        }
    }
    return widgetCount;
}

void CKeyguardHostView::AddDefaultWidgets()
{
    if (!mSafeModeEnabled && !WidgetsDisabled()) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
        AutoPtr<IView> addWidget;
        inflater->Inflate(R::layout::keyguard_add_widget, this, FALSE, (IView**)&addWidget);
        mAppWidgetContainer->AddWidget(addWidget, 0);
        AutoPtr<IView> addWidgetButton;
        addWidget->FindViewById(R::id::keyguard_add_widget_view, (IView**)&addWidgetButton);

        AutoPtr<IViewOnClickListener> lis = new MyOnClickListener(this);
        addWidgetButton->SetOnClickListener(lis);
    }

    // We currently disable cameras in safe mode because we support loading 3rd party
    // cameras we can't trust.  TODO: plumb safe mode into camera creation code and only
    // inflate system-provided camera?
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    Boolean res;
    resources->GetBoolean(R::bool_::kg_enable_camera_default_widget, &res);
    if (!mSafeModeEnabled && !CameraDisabledByDpm() && mUserSetupCompleted && res) {
        AutoPtr<IKeyguardActivityLauncher> launcher;
        GetActivityLauncher((IKeyguardActivityLauncher**)&launcher);
        AutoPtr<IView> cameraWidget = CameraWidgetFrame::Create(mContext,
                mCameraWidgetCallbacks, launcher);
        if (cameraWidget != NULL) {
            mAppWidgetContainer->AddWidget(cameraWidget);
        }
    }
}

AutoPtr<IKeyguardTransportControlView> CKeyguardHostView::GetOrCreateTransportControl()
{
    if (mTransportControl == NULL) {
        AutoPtr<ILayoutInflater> inflater;
        LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);

        AutoPtr<IView> view;
        inflater->Inflate(R::layout::keyguard_transport_control_view, this, FALSE, (IView**)&view);
        mTransportControl = IKeyguardTransportControlView::Probe(view);

        AutoPtr<IKeyguardHostViewTransportControlCallback> cb = new MyControlCallback(this);
        mTransportControl->SetTransportControlCallback(cb);
    }
    return mTransportControl;
}

Int32 CKeyguardHostView::GetInsertPageIndex()
{
    AutoPtr<IView> addWidget;
    IView::Probe(mAppWidgetContainer)->FindViewById(R::id::keyguard_add_widget,
            (IView**)&addWidget);
    Int32 insertionIndex;
    IViewGroup::Probe(mAppWidgetContainer)->IndexOfChild(addWidget, &insertionIndex);
    if (insertionIndex < 0) {
        insertionIndex = 0; // no add widget page found
    }
    else {
        insertionIndex++; // place after add widget
    }
    return insertionIndex;
}

void CKeyguardHostView::AddDefaultStatusWidget(
    /* [in] */ Int32 index)
{
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(mContext, (ILayoutInflater**)&inflater);
    AutoPtr<IView> statusWidget;
    inflater->Inflate(R::layout::keyguard_status_view, NULL, TRUE, (IView**)&statusWidget);
    mAppWidgetContainer->AddWidget(statusWidget, index);
}

void CKeyguardHostView::AddWidgetsFromSettings()
{
    if (mSafeModeEnabled || WidgetsDisabled()) {
        AddDefaultStatusWidget(0);
        return;
    }

    Int32 insertionIndex = GetInsertPageIndex();

    // Add user-selected widget
    AutoPtr<ArrayOf<Int32> > widgets;
    mLockPatternUtils->GetAppWidgets((ArrayOf<Int32>**)&widgets);

    if (widgets == NULL) {
        Logger::D(TAG, "Problem reading widgets");
    }
    else {
        for (Int32 i = widgets->GetLength() -1; i >= 0; i--) {
            if ((*widgets)[i] == ILockPatternUtils::ID_DEFAULT_STATUS_WIDGET) {
                AddDefaultStatusWidget(insertionIndex);
            }
            else {
                // We add the widgets from left to right, starting after the first page after
                // the add page. We count down, since the order will be persisted from right
                // to left, starting after camera.
                AddWidget((*widgets)[i], insertionIndex, TRUE);
            }
        }
    }
}

Int32 CKeyguardHostView::AllocateIdForDefaultAppWidget()
{
    Int32 appWidgetId;
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    String str1, str2;
    res->GetString(R::string::widget_default_package_name, &str1);
    res->GetString(R::string::widget_default_class_name, &str2);
    AutoPtr<IComponentName> defaultAppWidget;
    CComponentName::New(str1, str2, (IComponentName**)&defaultAppWidget);

    // Note: we don't support configuring the widget
    mAppWidgetHost->AllocateAppWidgetId(&appWidgetId);

    //try {
    if((ECode)E_ILLEGAL_ARGUMENT_EXCEPTION == mAppWidgetManager->BindAppWidgetId(appWidgetId, defaultAppWidget)) {
    //} catch (IllegalArgumentException e) {
        Logger::E(TAG, "Error when trying to bind default AppWidget: %d", E_ILLEGAL_ARGUMENT_EXCEPTION);
        mAppWidgetHost->DeleteAppWidgetId(appWidgetId);
        appWidgetId = IAppWidgetManager::INVALID_APPWIDGET_ID;
    }
    return appWidgetId;
}

ECode CKeyguardHostView::CheckAppWidgetConsistency()
{
    Int32 childCount;
    IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&childCount);
    Boolean widgetPageExists = FALSE;
    for (Int32 i = 0; i < childCount; i++) {
        Boolean res;
        if (mAppWidgetContainer->IsWidgetPage(i, &res), res) {
            widgetPageExists = TRUE;
            break;
        }
    }
    if (!widgetPageExists) {
        Int32 insertPageIndex = GetInsertPageIndex();

        Boolean userAddedWidgetsEnabled = !WidgetsDisabled();

        Boolean addedDefaultAppWidget = FALSE;

        if (!mSafeModeEnabled) {
            if (userAddedWidgetsEnabled) {
                Int32 appWidgetId = AllocateIdForDefaultAppWidget();
                if (appWidgetId != IAppWidgetManager::INVALID_APPWIDGET_ID) {
                    addedDefaultAppWidget = AddWidget(appWidgetId, insertPageIndex, TRUE);
                }
            }
            else {
                // note: even if widgetsDisabledByDpm() returns true, we still bind/create
                // the default appwidget if possible
                Int32 appWidgetId;
                mLockPatternUtils->GetFallbackAppWidgetId(&appWidgetId);
                if (appWidgetId == IAppWidgetManager::INVALID_APPWIDGET_ID) {
                    appWidgetId = AllocateIdForDefaultAppWidget();
                    if (appWidgetId != IAppWidgetManager::INVALID_APPWIDGET_ID) {
                        mLockPatternUtils->WriteFallbackAppWidgetId(appWidgetId);
                    }
                }
                if (appWidgetId != IAppWidgetManager::INVALID_APPWIDGET_ID) {
                    addedDefaultAppWidget = AddWidget(appWidgetId, insertPageIndex, FALSE);
                    if (!addedDefaultAppWidget) {
                        mAppWidgetHost->DeleteAppWidgetId(appWidgetId);
                        mLockPatternUtils->WriteFallbackAppWidgetId(
                                IAppWidgetManager::INVALID_APPWIDGET_ID);
                    }
                }
            }
        }

        // Use the built-in status/clock view if we can't inflate the default widget
        if (!addedDefaultAppWidget) {
            AddDefaultStatusWidget(insertPageIndex);
        }

        // trigger DB updates only if user-added widgets are enabled
        if (!mSafeModeEnabled && userAddedWidgetsEnabled) {
            AutoPtr<IView> view;
            IViewGroup::Probe(mAppWidgetContainer)->GetChildAt(insertPageIndex,
                    (IView**)&view);
            mAppWidgetContainer->OnAddView(view, insertPageIndex);
        }
    }
    return NOERROR;
}

AutoPtr<IParcelable> CKeyguardHostView::OnSaveInstanceState()
{
    if (DEBUG) Logger::D(TAG, "onSaveInstanceState, tstate= %d", mTransportState);
    AutoPtr<IParcelable> superState;
    KeyguardViewBase::OnSaveInstanceState();
    AutoPtr<SavedState> ss = new SavedState();
    ss->constructor(superState);
    // If the transport is showing, force it to show it on restore.
    Int32 index;
    Boolean showing = mTransportControl != NULL
            && (mAppWidgetContainer->GetWidgetPageIndex(IView::Probe(mTransportControl),
            &index), index) >= 0;
    ss->mTransportState = showing ? TRANSPORT_VISIBLE : mTransportState;
    ss->mAppWidgetToShow = mAppWidgetToShow;
    ss->mInsets->Set(mInsets);
    return IParcelable::Probe(ss);
}

ECode CKeyguardHostView::OnRestoreInstanceState(
    /* [in] */ IParcelable* state)
{
    if (IKeyguardHostViewSavedState::Probe(state) == NULL) {
        KeyguardViewBase::OnRestoreInstanceState(state);
        return NOERROR;
    }
    AutoPtr<SavedState> ss = (SavedState*)IKeyguardHostViewSavedState::Probe(state);
    AutoPtr<IParcelable> p;
    ss->GetSuperState((IParcelable**)&p);
    KeyguardViewBase::OnRestoreInstanceState(p);
    mTransportState = ss->mTransportState;
    mAppWidgetToShow = ss->mAppWidgetToShow;
    SetInsets(ss->mInsets);
    if (DEBUG) Logger::D(TAG, "onRestoreInstanceState, transport= %d", mTransportState);
    return mSwitchPageRunnable->Run();
}

ECode CKeyguardHostView::FitSystemWindows(
    /* [in] */ IRect* insets,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    SetInsets(insets);
    *result = TRUE;
    return NOERROR;
}

void CKeyguardHostView::SetInsets(
    /* [in] */ IRect* insets)
{
    mInsets->Set(insets);
    if (mSlidingChallengeLayout != NULL) mSlidingChallengeLayout->SetInsets(mInsets);
    if (mMultiPaneChallengeLayout != NULL) mMultiPaneChallengeLayout->SetInsets(mInsets);

    AutoPtr<ICameraWidgetFrame> cameraWidget = FindCameraPage();
    if (cameraWidget != NULL) cameraWidget->SetInsets(mInsets);
}

ECode CKeyguardHostView::OnWindowFocusChanged(
    /* [in] */ Boolean hasWindowFocus)
{
    KeyguardViewBase::OnWindowFocusChanged(hasWindowFocus);
    //if (DEBUG) Logger::D(TAG, "Window is " + (hasWindowFocus ? "focused" : "unfocused"));
    if (hasWindowFocus && mShowSecurityWhenReturn) {
        AutoPtr<IView> view;
        FindViewById(R::id::sliding_layout, (IView**)&view);
        AutoPtr<ISlidingChallengeLayout> slider = ISlidingChallengeLayout::Probe(view);
        if (slider != NULL) {
            slider->SetHandleAlpha(1);
            slider->ShowChallenge(TRUE);
        }
        mShowSecurityWhenReturn = FALSE;
    }
    return NOERROR;
}

void CKeyguardHostView::ShowAppropriateWidgetPage()
{
    Int32 state = mTransportState;
    Boolean transportAdded = EnsureTransportPresentOrRemoved(state);
    Int32 pageToShow = GetAppropriateWidgetPage(state);
    if (!transportAdded) {
        IPagedView::Probe(mAppWidgetContainer)->SetCurrentPage(pageToShow);
    }
    else if (state == TRANSPORT_VISIBLE) {
        // If the transport was just added, we need to wait for layout to happen before
        // we can set the current page.
        AutoPtr<IRunnable> r = new MyRunnable2(this, pageToShow);
        Boolean res;
        Post(r, &res);
    }
}

Boolean CKeyguardHostView::EnsureTransportPresentOrRemoved(
    /* [in] */ Int32 state)
{
    Boolean showing = GetWidgetPosition(R::id::keyguard_transport_control) != -1;
    Boolean visible = state == TRANSPORT_VISIBLE;
    Boolean shouldBeVisible = state == TRANSPORT_INVISIBLE && IsMusicPlaying(state);
    if (!showing && (visible || shouldBeVisible)) {
        // insert to left of camera if it exists, otherwise after right-most widget
        Int32 count;
        IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&count);
        Int32 lastWidget = count - 1;
        Int32 position = 0; // handle no widget case
        if (lastWidget >= 0) {
            Boolean res;
            position = (mAppWidgetContainer->IsCameraPage(lastWidget, &res), res) ?
                    lastWidget : lastWidget + 1;
        }
        if (DEBUGXPORT) Logger::V(TAG, "add transport at %d", position);
        AutoPtr<IKeyguardTransportControlView> cv = GetOrCreateTransportControl();
        mAppWidgetContainer->AddWidget(IView::Probe(cv), position);
        return TRUE;
    }
    else if (showing && state == TRANSPORT_GONE) {
        if (DEBUGXPORT) Logger::V(TAG, "remove transport");
        AutoPtr<IKeyguardTransportControlView> cv = GetOrCreateTransportControl();
        mAppWidgetContainer->RemoveWidget(IView::Probe(cv));
        mTransportControl = NULL;
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        KeyguardUpdateMonitor::GetInstance(context)->DispatchSetBackground(NULL);
    }
    return FALSE;
}

AutoPtr<ICameraWidgetFrame> CKeyguardHostView::FindCameraPage()
{
    Int32 count;
    IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&count);
    for (Int32 i = count - 1; i >= 0; i--) {
        Boolean res;
        if (mAppWidgetContainer->IsCameraPage(i, &res), res) {
            AutoPtr<IView> view;
            IViewGroup::Probe(mAppWidgetContainer)->GetChildAt(i, (IView**)&view);
            AutoPtr<ICameraWidgetFrame> tmp = ICameraWidgetFrame::Probe(view);
            return tmp;
        }
    }
    return NULL;
}

ECode CKeyguardHostView::IsMusicPage(
    /* [in] */ Int32 pageIndex,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = pageIndex >= 0 && pageIndex == GetWidgetPosition(R::id::keyguard_transport_control);
    return NOERROR;
}

Int32 CKeyguardHostView::GetAppropriateWidgetPage(
    /* [in] */ Int32 musicTransportState)
{
    // assumes at least one widget (besides camera + add)
    if (mAppWidgetToShow != IAppWidgetManager::INVALID_APPWIDGET_ID) {
        Int32 childCount;
        IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IKeyguardWidgetFrame> frame;
            mAppWidgetContainer->GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
            Int32 id;
            frame->GetContentAppWidgetId(&id);
            if (id == mAppWidgetToShow) {
                return i;
            }
        }
        mAppWidgetToShow = IAppWidgetManager::INVALID_APPWIDGET_ID;
    }
    // if music playing, show transport
    if (musicTransportState == TRANSPORT_VISIBLE) {
        if (DEBUG) Logger::D(TAG, "Music playing, show transport");
        AutoPtr<IKeyguardTransportControlView> cv = GetOrCreateTransportControl();
        Int32 index;
        mAppWidgetContainer->GetWidgetPageIndex(IView::Probe(cv), &index);
        return index;
    }

    // else show the right-most widget (except for camera)
    Int32 count;
    IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&count);
    Int32 rightMost = count - 1;
    Boolean res;
    if (mAppWidgetContainer->IsCameraPage(rightMost, &res), res) {
        rightMost--;
    }
    if (DEBUG) Logger::D(TAG, "Show right-most page %d", rightMost);
    return rightMost;
}

void CKeyguardHostView::EnableUserSelectorIfNecessary()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    AutoPtr<IUserManager> um = IUserManager::Probe(obj);
    if (um == NULL) {
        // Throwable t = new Throwable();
        // t.fillInStackTrace();
        // Log.e(TAG, "user service is null.", t);
        return;
    }

    // if there are multiple users, we need to enable to multi-user switcher
    Boolean res;
    if (um->IsUserSwitcherEnabled(&res), !res) {
        return;
    }

    AutoPtr<IView> multiUserView;
    FindViewById(R::id::keyguard_user_selector, (IView**)&multiUserView);
    if (multiUserView == NULL) {
        if (DEBUG) Logger::D(TAG, "can't find user_selector in layout.");
        return;
    }

    if (IKeyguardMultiUserSelectorView::Probe(multiUserView) != NULL) {
        mKeyguardMultiUserSelectorView = IKeyguardMultiUserSelectorView::Probe(multiUserView);
        IView::Probe(mKeyguardMultiUserSelectorView)->SetVisibility(IView::VISIBLE);
        AutoPtr<IList> users;
        um->GetUsers(TRUE, (IList**)&users);
        mKeyguardMultiUserSelectorView->AddUsers(ICollection::Probe(users));
        AutoPtr<IKeyguardHostViewUserSwitcherCallback> callback = new MyUserSwitcherCallback(this);
        mKeyguardMultiUserSelectorView->SetCallback(callback);
    }
    else {
        // Throwable t = new Throwable();
        // t.fillInStackTrace();
        if (multiUserView == NULL) {
            Logger::E(TAG, "could not find the user_selector.");
        }
        else {
            Logger::E(TAG, "user_selector is the wrong type.");
        }
    }
}

ECode CKeyguardHostView::CleanUp()
{
    // Make sure we let go of all widgets and their package contexts promptly. If we don't do
    // this, and the associated application is uninstalled, it can cause a soft reboot.
    Int32 count;
    IViewGroup::Probe(mAppWidgetContainer)->GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IKeyguardWidgetFrame> frame;
        mAppWidgetContainer->GetWidgetPageAt(i, (IKeyguardWidgetFrame**)&frame);
        IViewGroup::Probe(frame)->RemoveAllViews();
    }
    AutoPtr<IKeyguardSecurityContainer> container;
    GetSecurityContainer((IKeyguardSecurityContainer**)&container);
    return IKeyguardSecurityView::Probe(container)->OnPause(); // clean up any actions in progress
}

ECode CKeyguardHostView::GoToWidget(
    /* [in] */ Int32 appWidgetId)
{
    mAppWidgetToShow = appWidgetId;
    return mSwitchPageRunnable->Run();
}

ECode CKeyguardHostView::ShowBouncer(
    /* [in] */ Boolean show)
{
    KeyguardViewBase::ShowBouncer(show);
    return mViewStateManager->ShowBouncer(show);
}

ECode CKeyguardHostView::OnExternalMotionEvent(
    /* [in] */ IMotionEvent* event)
{
    return mAppWidgetContainer->HandleExternalCameraEvent(event);
}

ECode CKeyguardHostView::OnCreateOptions(
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        Int32 widgetToShow;
        options->GetInt32(ILockPatternUtils::KEYGUARD_SHOW_APPWIDGET,
                IAppWidgetManager::INVALID_APPWIDGET_ID, &widgetToShow);
        if (widgetToShow != IAppWidgetManager::INVALID_APPWIDGET_ID) {
            GoToWidget(widgetToShow);
        }
    }
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
