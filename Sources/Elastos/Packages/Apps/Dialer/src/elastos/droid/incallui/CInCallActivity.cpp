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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/incallui/CInCallActivity.h"
#include "elastos/droid/contacts/common/interactions/TouchPointManager.h"
#include "elastos/droid/incallui/InCallPresenter.h"
#include "elastos/droid/incallui/PostCharDialogFragment.h"
#include "elastos/droid/incallui/TelecomAdapter.h"
#include "elastos/droid/phone/common/animation/AnimUtils.h"
#include "R.h"
#include <elastos/droid/text/TextUtils.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/R.h>
#include <elastos/utility/logging/Logger.h>
#include "R.h"

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnCancelListener;
using Elastos::Droid::Contacts::Common::Interactions::TouchPointManager;
using Elastos::Droid::Contacts::Common::Interactions::ITouchPointManager;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::Phone::Common::Animation::AnimUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//=============================================================
// CInCallActivity::MyAnimationListenerAdapter
//=============================================================
ECode CInCallActivity::MyAnimationListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimation* animation)
{
    mHost->ShowDialpad(FALSE);
    return NOERROR;
}


//=============================================================
// CInCallActivity::MyDialogOnClickListener
//=============================================================
CAR_INTERFACE_IMPL(CInCallActivity::MyDialogOnClickListener, Object, IDialogInterfaceOnClickListener)

ECode CInCallActivity::MyDialogOnClickListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    mHost->OnDialogDismissed();
    return NOERROR;
}


//=============================================================
// CInCallActivity::MyDialogOnCancelListener
//=============================================================
CAR_INTERFACE_IMPL(CInCallActivity::MyDialogOnCancelListener, Object, IDialogInterfaceOnCancelListener)

ECode CInCallActivity::MyDialogOnCancelListener::OnCancel(
    /* [in] */ IDialogInterface* dialog)
{
    mHost->OnDialogDismissed();
    return NOERROR;
}


//=============================================================
// CInCallActivity
//=============================================================
const String CInCallActivity::SHOW_DIALPAD_EXTRA("InCallActivity.show_dialpad");
const String CInCallActivity::DIALPAD_TEXT_EXTRA("InCallActivity.dialpad_text");
const String CInCallActivity::NEW_OUTGOING_CALL("InCallActivity.new_outgoing_call");

CAR_INTERFACE_IMPL(CInCallActivity, Activity, IInCallActivity)

CAR_OBJECT_IMPL(CInCallActivity)

CInCallActivity::CInCallActivity()
    : mIsForegroundActivity(FALSE)
    , mShowDialpadRequested(FALSE)
    , mAnimateDialpadOnShow(FALSE)
    , mShowPostCharWaitDialogOnResume(FALSE)
    , mIsLandscape(FALSE)
    , mCurrentOrientation(0)
{
    mSlideOutListener = new MyAnimationListenerAdapter(this);
}

ECode CInCallActivity::OnCreate(
    /* [in] */ IBundle* icicle)
{
    Logger::D("CInCallActivity", "onCreate()...  this = %s", TO_CSTR(this));

    FAIL_RETURN(Activity::OnCreate(icicle));

    // set this flag so this activity will stay in front of the keyguard
    // Have the WindowManager filter out touch events that are "too fat".
    Int32 flags = IWindowManagerLayoutParams::FLAG_SHOW_WHEN_LOCKED
            | IWindowManagerLayoutParams::FLAG_TURN_SCREEN_ON
            | IWindowManagerLayoutParams::FLAG_IGNORE_CHEEK_PRESSES;

    AutoPtr<IWindow> window;
    GetWindow((IWindow**)&window);
    window->AddFlags(flags);

    // Setup action bar for the conference call manager.
    Boolean result;
    RequestWindowFeature(IWindow::FEATURE_ACTION_BAR_OVERLAY, &result);
    AutoPtr<IActionBar> actionBar;
    GetActionBar((IActionBar**)&actionBar);
    if (actionBar != NULL) {
        actionBar->SetDisplayHomeAsUpEnabled(TRUE);
        actionBar->SetDisplayShowTitleEnabled(TRUE);
        actionBar->Hide();
    }

    // TODO(klp): Do we need to add this back when prox sensor is not available?
    // lp.inputFeatures |= WindowManager.LayoutParams.INPUT_FEATURE_DISABLE_USER_ACTIVITY;

    // Inflate everything in incall_screen.xml and add it to the screen.
    SetContentView(Elastos::Droid::Dialer::R::layout::incall_screen);

    InitializeInCall();

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    InternalResolveIntent(intent);

    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    config->GetOrientation(&mCurrentOrientation);
    mIsLandscape = mCurrentOrientation == IConfiguration::ORIENTATION_LANDSCAPE;

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    Boolean isRtl = TextUtils::GetLayoutDirectionFromLocale(locale) ==
            IView::LAYOUT_DIRECTION_RTL;

    if (mIsLandscape) {
        AnimationUtils::LoadAnimation(this,
                isRtl ? Elastos::Droid::Dialer::R::anim::dialpad_slide_in_left :
                        Elastos::Droid::Dialer::R::anim::dialpad_slide_in_right,
                (IAnimation**)&mSlideIn);
        AnimationUtils::LoadAnimation(this,
                isRtl ? Elastos::Droid::Dialer::R::anim::dialpad_slide_out_left :
                        Elastos::Droid::Dialer::R::anim::dialpad_slide_out_right,
                (IAnimation**)&mSlideOut);
    }
    else {
        AnimationUtils::LoadAnimation(this,
                Elastos::Droid::Dialer::R::anim::dialpad_slide_in_bottom,
                (IAnimation**)&mSlideIn);
        AnimationUtils::LoadAnimation(this,
                Elastos::Droid::Dialer::R::anim::dialpad_slide_out_bottom,
                (IAnimation**)&mSlideOut);
    }

    mSlideIn->SetInterpolator(AnimUtils::EASE_IN);
    mSlideOut->SetInterpolator(AnimUtils::EASE_OUT);

    mSlideOut->SetAnimationListener(mSlideOutListener);

    if (icicle != NULL) {
        // If the dialpad was shown before, set variables indicating it should be shown and
        // populated with the previous DTMF text.  The dialpad is actually shown and populated
        // in onResume() to ensure the hosting CallCardFragment has been inflated and is ready
        // to receive it.
        icicle->GetBoolean(SHOW_DIALPAD_EXTRA, &mShowDialpadRequested);
        mAnimateDialpadOnShow = FALSE;
        icicle->GetString(DIALPAD_TEXT_EXTRA, &mDtmfText);
    }
    Logger::D("CInCallActivity", "onCreate(): exit");
    return NOERROR;
}

ECode CInCallActivity::OnSaveInstanceState(
    /* [in] */ IBundle* out)
{
    Boolean isVisible;
    mCallButtonFragment->IsDialpadVisible(&isVisible);
    out->PutBoolean(SHOW_DIALPAD_EXTRA, isVisible);
    if (mDialpadFragment != NULL) {
        out->PutString(DIALPAD_TEXT_EXTRA, mDialpadFragment->GetDtmfText());
    }
    return NOERROR;
}

ECode CInCallActivity::OnStart()
{
    Logger::D("CInCallActivity", "onStart()...");
    FAIL_RETURN(Activity::OnStart());

    // setting activity should be last thing in setup process
    InCallPresenter::GetInstance()->SetActivity(this);
    return NOERROR;
}

ECode CInCallActivity::OnResume()
{
    Logger::I("CInCallActivity", "onResume()...");
    FAIL_RETURN(Activity::OnResume());

    mIsForegroundActivity = TRUE;
    InCallPresenter::GetInstance()->OnUiShowing(TRUE);

    if (mShowDialpadRequested) {
        mCallButtonFragment->DisplayDialpad(TRUE /* show */,
                mAnimateDialpadOnShow /* animate */);
        mShowDialpadRequested = FALSE;
        mAnimateDialpadOnShow = FALSE;

        if (mDialpadFragment != NULL) {
            mDialpadFragment->SetDtmfText(mDtmfText);
            mDtmfText = NULL;
        }
    }

    if (mShowPostCharWaitDialogOnResume) {
        ShowPostCharWaitDialog(mShowPostCharWaitDialogCallId, mShowPostCharWaitDialogChars);
    }
    return NOERROR;
}

ECode CInCallActivity::OnPause()
{
    Logger::D("CInCallActivity", "onPause()...");
    FAIL_RETURN(Activity::OnPause());

    mIsForegroundActivity = FALSE;

    if (mDialpadFragment != NULL) {
        mDialpadFragment->OnDialerKeyUp(NULL);
    }

    InCallPresenter::GetInstance()->OnUiShowing(FALSE);
    return NOERROR;
}

ECode CInCallActivity::OnStop()
{
    Logger::D("CInCallActivity", "onStop()...");
    return Activity::OnStop();
}

ECode CInCallActivity::OnDestroy()
{
    Logger::D("CInCallActivity", "onDestroy()...  this = %s", TO_CSTR(this));

    InCallPresenter::GetInstance()->SetActivity(NULL);

    return Activity::OnDestroy();
}

Boolean CInCallActivity::IsForegroundActivity()
{
    return mIsForegroundActivity;
}

Boolean CInCallActivity::HasPendingErrorDialog()
{
    return mDialog != NULL;
}

ECode CInCallActivity::Finish()
{
    Logger::I("CInCallActivity", "finish().  Dialog showing: %d", (mDialog != NULL));

    // skip finish if we are still showing a dialog.
    if (!HasPendingErrorDialog() && !mAnswerFragment->HasPendingDialogs()) {
        return Activity::Finish();
    }
    return NOERROR;
}

ECode CInCallActivity::OnNewIntent(
    /* [in] */ IIntent* intent)
{
    Logger::D("CInCallActivity", "onNewIntent: intent = %s", TO_CSTR(intent));

    // We're being re-launched with a new Intent.  Since it's possible for a
    // single InCallActivity instance to persist indefinitely (even if we
    // finish() ourselves), this sequence can potentially happen any time
    // the InCallActivity needs to be displayed.

    // Stash away the new intent so that we can get it in the future
    // by calling getIntent().  (Otherwise getIntent() will return the
    // original Intent from when we first got created!)
    SetIntent(intent);

    // Activities are always paused before receiving a new intent, so
    // we can count on our onResume() method being called next.

    // Just like in onCreate(), handle the intent.
    InternalResolveIntent(intent);
    return NOERROR;
}

ECode CInCallActivity::OnBackPressed()
{
    Logger::D("CInCallActivity", "onBackPressed()...");

    // BACK is also used to exit out of any "special modes" of the
    // in-call UI:

    Boolean isVisible;
    if (mCallCardFragment->IsVisible(&isVisible), !isVisible) {
        return NOERROR;
    }

    if (mDialpadFragment != NULL && (mDialpadFragment->IsVisible(&isVisible), isVisible)) {
        mCallButtonFragment->DisplayDialpad(FALSE /* show */, TRUE /* animate */);
        return NOERROR;
    }
    else if (mConferenceManagerFragment->IsVisible(&isVisible), isVisible) {
        mConferenceManagerFragment->SetVisible(false);
        return NOERROR;
    }

    // Always disable the Back key while an incoming call is ringing
    AutoPtr<Call> call = CallList::GetInstance()->GetIncomingCall();
    if (call != NULL) {
        Logger::D("CInCallActivity", "Consume Back press for an incoming call");
        return NOERROR;
    }

    // Nothing special to do.  Fall back to the default behavior.
    return Activity::OnBackPressed();
}

ECode CInCallActivity::OnOptionsItemSelected(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 itemId;
    item->GetItemId(&itemId);
    if (itemId == Elastos::Droid::R::id::home) {
        OnBackPressed();
        *result = TRUE;
        return NOERROR;
    }
    return Activity::OnOptionsItemSelected(item, result);
}

ECode CInCallActivity::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // push input to the dialer.
    if (mDialpadFragment != NULL && (mDialpadFragment->IsVisible(result), *result) &&
            (mDialpadFragment->OnDialerKeyUp(event))) {
        *result = TRUE;
        return NOERROR;
    }
    else if (keyCode == IKeyEvent::KEYCODE_CALL) {
        // Always consume CALL to be sure the PhoneWindow won't do anything with it
        *result = TRUE;
        return NOERROR;
    }
    return Activity::OnKeyUp(keyCode, event, result);
}

ECode CInCallActivity::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_CALL: {
            Boolean handled = InCallPresenter::GetInstance()->HandleCallKey();
            if (!handled) {
                Logger::W("CInCallActivity", "InCallActivity should always handle KEYCODE_CALL in onKeyDown");
            }
            // Always consume CALL to be sure the PhoneWindow won't do anything with it
            *result = TRUE;
            return NOERROR;
        }

        // Note there's no KeyEvent.KEYCODE_ENDCALL case here.
        // The standard system-wide handling of the ENDCALL key
        // (see PhoneWindowManager's handling of KEYCODE_ENDCALL)
        // already implements exactly what the UI spec wants,
        // namely (1) "hang up" if there's a current active call,
        // or (2) "don't answer" if there's a current ringing call.

        case IKeyEvent::KEYCODE_CAMERA:
            // Disable the CAMERA button while in-call since it's too
            // easy to press accidentally.
            *result = TRUE;
            return NOERROR;

        case IKeyEvent::KEYCODE_VOLUME_UP:
        case IKeyEvent::KEYCODE_VOLUME_DOWN:
        case IKeyEvent::KEYCODE_VOLUME_MUTE:
            // Ringer silencing handled by PhoneWindowManager.
            break;

        case IKeyEvent::KEYCODE_MUTE:
            // toggle mute
            TelecomAdapter::GetInstance()->Mute(!AudioModeProvider::GetInstance()->GetMute());
            *result = TRUE;
            return NOERROR;

        // Various testing/debugging features, enabled ONLY when VERBOSE == true.
        case IKeyEvent::KEYCODE_SLASH:
            if (Logger::VERBOSE) {
                Logger::V("CInCallActivity", "----------- InCallActivity View dump --------------");
                // Dump starting from the top-level view of the entire activity:
                AutoPtr<IWindow> w;
                GetWindow((IWindow**)&w);
                AutoPtr<IView> decorView;
                w->GetDecorView((IView**)&decorView);
                Logger::D("CInCallActivity", "View dump:%s", TO_CSTR(decorView));
                *result = TRUE;
                return NOERROR;
            }
            break;
        case IKeyEvent::KEYCODE_EQUALS:
            // TODO: Dump phone state?
            break;
    }

    Int32 count;
    if ((event->GetRepeatCount(&count), count == 0) && HandleDialerKeyDown(keyCode, event)) {
        *result = TRUE;
        return NOERROR;
    }

    return Activity::OnKeyDown(keyCode, event, result);
}

Boolean CInCallActivity::HandleDialerKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event)
{
    Logger::V("CInCallActivity", "handleDialerKeyDown: keyCode %d, event %s ...", keyCode, TO_CSTR(event));

    // As soon as the user starts typing valid dialable keys on the
    // keyboard (presumably to type DTMF tones) we start passing the
    // key events to the DTMFDialer's onDialerKeyDown.
    Boolean isVisible;
    if (mDialpadFragment != NULL && (mDialpadFragment->IsVisible(&isVisible), isVisible)) {
        return mDialpadFragment->OnDialerKeyDown(event);

        // TODO: If the dialpad isn't currently visible, maybe
        // consider automatically bringing it up right now?
        // (Just to make sure the user sees the digits widget...)
        // But this probably isn't too critical since it's awkward to
        // use the hard keyboard while in-call in the first place,
        // especially now that the in-call UI is portrait-only...
    }

    return FALSE;
}

ECode CInCallActivity::OnConfigurationChanged(
    /* [in] */ IConfiguration* config)
{
    InCallPresenter::GetInstance()->GetProximitySensor()->OnConfigurationChanged(config);
    Int32 orientation;
    config->GetOrientation(&orientation);
    Logger::D("CInCallActivity", "onConfigurationChanged %d", orientation);

    // Check to see if the orientation changed to prevent triggering orientation change events
    // for other configuration changes.
    if (orientation != mCurrentOrientation) {
        mCurrentOrientation = orientation;
        AutoPtr<IWindowManager> wm;
        GetWindowManager((IWindowManager**)&wm);
        AutoPtr<IDisplay> display;
        wm->GetDefaultDisplay((IDisplay**)&display);
        Int32 rotation;
        display->GetRotation(&rotation);
        InCallPresenter::GetInstance()->OnDeviceRotationChange(rotation);
        InCallPresenter::GetInstance()->OnDeviceOrientationChange(mCurrentOrientation);
    }
    return Activity::OnConfigurationChanged(config);
}

AutoPtr<CCallButtonFragment> CInCallActivity::GetCallButtonFragment()
{
    return mCallButtonFragment;
}

AutoPtr<CCallCardFragment> CInCallActivity::GetCallCardFragment()
{
    return mCallCardFragment;
}

void CInCallActivity::InternalResolveIntent(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (action.Equals(IIntent::ACTION_MAIN)) {
        // This action is the normal way to bring up the in-call UI.
        //
        // But we do check here for one extra that can come along with the
        // ACTION_MAIN intent:
        Boolean result;
        if (intent->HasExtra(SHOW_DIALPAD_EXTRA, &result), result) {
            // SHOW_DIALPAD_EXTRA can be used here to specify whether the DTMF
            // dialpad should be initially visible.  If the extra isn't
            // present at all, we just leave the dialpad in its previous state.

            Boolean showDialpad;
            intent->GetBooleanExtra(SHOW_DIALPAD_EXTRA, FALSE, &showDialpad);
            Logger::D("CInCallActivity", "- internalResolveIntent: SHOW_DIALPAD_EXTRA: %d", showDialpad);

            RelaunchedFromDialer(showDialpad);
        }

        Boolean newOutgoingCall;
        if (intent->GetBooleanExtra(NEW_OUTGOING_CALL, FALSE, &newOutgoingCall), newOutgoingCall) {
            intent->RemoveExtra(NEW_OUTGOING_CALL);
            AutoPtr<Call> call = CallList::GetInstance()->GetOutgoingCall();
            if (call == NULL) {
                call = CallList::GetInstance()->GetPendingOutgoingCall();
            }

            AutoPtr<IBundle> extras;
            if (call != NULL) {
                AutoPtr<ICallDetails> details;
                call->GetTelecommCall()->GetDetails((ICallDetails**)&details);
                details->GetExtras((IBundle**)&extras);
            }
            if (extras == NULL) {
                // Initialize the extras bundle to avoid NPE
                CBundle::New((IBundle**)&extras);
            }

            AutoPtr<IPoint> touchPoint;
            AutoPtr<ITouchPointManager> manager = TouchPointManager::GetInstance();
            Boolean bValidPoint = FALSE;
            manager->HasValidPoint(&bValidPoint);
            if (bValidPoint) {
                // Use the most immediate touch point in the InCallUi if available
                manager->GetPoint((IPoint**)&touchPoint);
            }
            else {
                // Otherwise retrieve the touch point from the call intent
                if (call != NULL) {
                    AutoPtr<IParcelable> parcel;
                    extras->GetParcelable(ITouchPointManager::TOUCH_POINT, (IParcelable**)&parcel);
                    touchPoint = IPoint::Probe(parcel);
                }
            }
            mCallCardFragment->AnimateForNewOutgoingCall(touchPoint);

            /*
             * If both a phone account handle and a list of phone accounts to choose from are
             * missing, then disconnect the call because there is no way to place an outgoing
             * call.
             * The exception is emergency calls, which may be waiting for the ConnectionService
             * to set the PhoneAccount during the PENDING_OUTGOING state.
             */
            if (call != NULL && !IsEmergencyCall(call)) {
                AutoPtr<IArrayList> phoneAccountHandles;
                extras->GetParcelableArrayList(Elastos::Droid::Telecom::ICall::AVAILABLE_PHONE_ACCOUNTS,
                        (IArrayList**)&phoneAccountHandles);
                Boolean isEmpty;
                if (call->GetAccountHandle() == NULL &&
                        (phoneAccountHandles == NULL || (phoneAccountHandles->IsEmpty(&isEmpty), isEmpty))) {
                    TelecomAdapter::GetInstance()->DisconnectCall(call->GetId());
                }
            }
        }

        AutoPtr<Call> pendingAccountSelectionCall = CallList::GetInstance()->GetWaitingForAccountCall();
        if (pendingAccountSelectionCall != NULL) {
            mCallCardFragment->SetVisible(FALSE);
            AutoPtr<ICallDetails> details;
            pendingAccountSelectionCall->GetTelecommCall()->GetDetails((ICallDetails**)&details);
            AutoPtr<IBundle> extras;
            details->GetExtras((IBundle**)&extras);

            AutoPtr<IArrayList> phoneAccountHandles;
            if (extras != NULL) {
                extras->GetParcelableArrayList(
                        Elastos::Droid::Telecom::ICall::AVAILABLE_PHONE_ACCOUNTS, (IArrayList**)&phoneAccountHandles);
            }
            else {
                CArrayList::New((IArrayList**)&phoneAccountHandles);
            }

            AutoPtr<IFragmentManager> fm;
            GetFragmentManager((IFragmentManager**)&fm);
            assert(0 && "TODO");
            // SelectPhoneAccountDialogFragment::ShowAccountDialog(fm, phoneAccountHandles);
        }
        else {
            mCallCardFragment->SetVisible(TRUE);
        }

        return;
    }
}

Boolean CInCallActivity::IsEmergencyCall(
    /* [in] */ Call* call)
{
    AutoPtr<IUri> handle = call->GetHandle();
    if (handle == NULL) {
        return FALSE;
    }
    String number;
    handle->GetSchemeSpecificPart(&number);
    AutoPtr<IPhoneNumberUtils> utils;
    CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&utils);
    Boolean result;
    utils->IsEmergencyNumber(number, &result);
    return result;
}

void CInCallActivity::RelaunchedFromDialer(
    /* [in] */ Boolean showDialpad)
{
    mShowDialpadRequested = showDialpad;
    mAnimateDialpadOnShow = TRUE;

    if (mShowDialpadRequested) {
        // If there's only one line in use, AND it's on hold, then we're sure the user
        // wants to use the dialpad toward the exact line, so un-hold the holding line.
        AutoPtr<Call> call = CallList::GetInstance()->GetActiveOrBackgroundCall();
        if (call != NULL && call->GetState() == Call::State::ONHOLD) {
            TelecomAdapter::GetInstance()->UnholdCall(call->GetId());
        }
    }
}

void CInCallActivity::InitializeInCall()
{
    if (mCallCardFragment == NULL) {
        AutoPtr<IFragmentManager> fm;
        GetFragmentManager((IFragmentManager**)&fm);
        AutoPtr<IFragment> fg;
        fm->FindFragmentById(Elastos::Droid::Dialer::R::id::callCardFragment, (IFragment**)&fg);
        mCallCardFragment = (CCallCardFragment*)ICallCardFragment::Probe(fg);
    }

    mChildFragmentManager = NULL;
    mCallCardFragment->GetChildFragmentManager((IFragmentManager**)&mChildFragmentManager);

    if (mCallButtonFragment == NULL) {
        AutoPtr<IFragment> fg;
        mChildFragmentManager->FindFragmentById(Elastos::Droid::Dialer::R::id::callButtonFragment, (IFragment**)&fg);
        mCallButtonFragment = (CCallButtonFragment*)ICallButtonFragment::Probe(fg);
        AutoPtr<IView> v;
        mCallButtonFragment->GetView((IView**)&v);
        v->SetVisibility(IView::INVISIBLE);
    }

    if (mAnswerFragment == NULL) {
        AutoPtr<IFragment> fg;
        mChildFragmentManager->FindFragmentById(Elastos::Droid::Dialer::R::id::answerFragment, (IFragment**)&fg);
        mAnswerFragment = (CAnswerFragment*)IAnswerFragment::Probe(fg);
    }

    if (mConferenceManagerFragment == NULL) {
        AutoPtr<IFragmentManager> fm;
        GetFragmentManager((IFragmentManager**)&fm);
        AutoPtr<IFragment> fg;
        fm->FindFragmentById(Elastos::Droid::Dialer::R::id::conferenceManagerFragment, (IFragment**)&fg);
        mConferenceManagerFragment = (CConferenceManagerFragment*)IConferenceManagerFragment::Probe(fg);
        AutoPtr<IView> v;
        mConferenceManagerFragment->GetView((IView**)&v);
        v->SetVisibility(IView::INVISIBLE);
    }
}

void CInCallActivity::HideDialpadForDisconnect()
{
    mCallButtonFragment->DisplayDialpad(FALSE /* show */, TRUE /* animate */);
}

void CInCallActivity::DismissKeyguard(
    /* [in] */ Boolean dismiss)
{
    AutoPtr<IWindow> win;
    GetWindow((IWindow**)&win);
    if (dismiss) {
        win->AddFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD);
    }
    else {
        win->ClearFlags(IWindowManagerLayoutParams::FLAG_DISMISS_KEYGUARD);
    }
}

void CInCallActivity::ShowDialpad(
    /* [in] */ Boolean showDialpad)
{
    // If the dialpad is being shown and it has not already been loaded, replace the dialpad
    // placeholder with the actual fragment before continuing.
    if (mDialpadFragment == NULL && showDialpad) {
        AutoPtr<IFragmentTransaction> loadTransaction;
        mChildFragmentManager->BeginTransaction((IFragmentTransaction**)&loadTransaction);
        AutoPtr<IView> fragmentContainer = FindViewById(Elastos::Droid::Dialer::R::id::dialpadFragmentContainer);
        AutoPtr<CDialpadFragment> temp;
        CDialpadFragment::NewByFriend((CDialpadFragment**)&temp);
        mDialpadFragment = temp;
        Int32 id;
        fragmentContainer->GetId(&id);
        EGuid clsId;
        mDialpadFragment->GetClassID(&clsId);
        loadTransaction->Replace(id, IFragment::Probe(mDialpadFragment), String(clsId.mUunm));
        Int32 value;
        loadTransaction->CommitAllowingStateLoss(&value);
        Boolean result;
        mChildFragmentManager->ExecutePendingTransactions(&result);
    }

    AutoPtr<IFragmentTransaction> ft;
    mChildFragmentManager->BeginTransaction((IFragmentTransaction**)&ft);
    if (showDialpad) {
        ft->Show(mDialpadFragment);
    } else {
        ft->Hide(mDialpadFragment);
    }
    Int32 value;
    ft->CommitAllowingStateLoss(&value);
}

void CInCallActivity::DisplayDialpad(
    /* [in] */ Boolean showDialpad,
    /* [in] */ Boolean animate)
{
    // If the dialpad is already visible, don't animate in. If it's gone, don't animate out.
    if ((showDialpad && IsDialpadVisible()) || (!showDialpad && !IsDialpadVisible())) {
        return;
    }
    // We don't do a FragmentTransaction on the hide case because it will be dealt with when
    // the listener is fired after an animation finishes.
    if (!animate) {
        ShowDialpad(showDialpad);
    }
    else {
        if (showDialpad) {
            ShowDialpad(TRUE);
            mDialpadFragment->AnimateShowDialpad();
        }
        mCallCardFragment->OnDialpadVisiblityChange(showDialpad);
        AutoPtr<IView> v;
        mDialpadFragment->GetView((IView**)&v);
        v->StartAnimation(showDialpad ? mSlideIn : mSlideOut);
    }

    InCallPresenter::GetInstance()->GetProximitySensor()->OnDialpadVisible(showDialpad);
}

Boolean CInCallActivity::IsDialpadVisible()
{
    Boolean isVisible;
    return mDialpadFragment != NULL && (mDialpadFragment->IsVisible(&isVisible), isVisible);
}

void CInCallActivity::ShowConferenceCallManager()
{
    mConferenceManagerFragment->SetVisible(TRUE);
}

void CInCallActivity::ShowPostCharWaitDialog(
    /* [in] */ const String& callId,
    /* [in] */ const String& chars)
{
    if (IsForegroundActivity()) {
        AutoPtr<PostCharDialogFragment> fragment = new PostCharDialogFragment(callId,  chars);
        AutoPtr<IFragmentManager> fragmentManager;
        GetFragmentManager((IFragmentManager**)&fragmentManager);
        fragment->Show(fragmentManager, String("postCharWait"));

        mShowPostCharWaitDialogOnResume = FALSE;
        mShowPostCharWaitDialogCallId = String(NULL);
        mShowPostCharWaitDialogChars = String(NULL);
    }
    else {
        mShowPostCharWaitDialogOnResume = TRUE;
        mShowPostCharWaitDialogCallId = callId;
        mShowPostCharWaitDialogChars = chars;
    }
}

ECode CInCallActivity::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* isConsumed)
{
    if (mCallCardFragment != NULL) {
        mCallCardFragment->DispatchPopulateAccessibilityEvent(event);
    }
    return Activity::DispatchPopulateAccessibilityEvent(event, isConsumed);
}

void CInCallActivity::MaybeShowErrorDialogOnDisconnect(
    /* [in] */ IDisconnectCause* disconnectCause)
{
    Logger::D("CInCallActivity", "maybeShowErrorDialogOnDisconnect");

    Boolean isFinishing;
    AutoPtr<ICharSequence> desc;
    Int32 code;
    if ((IsFinishing(&isFinishing), !isFinishing) &&
            (disconnectCause->GetDescription((ICharSequence**)&desc), !TextUtils::IsEmpty(desc))
            && ((disconnectCause->GetCode(&code), code == IDisconnectCause::ERROR) ||
                    (disconnectCause->GetCode(&code), code == IDisconnectCause::RESTRICTED))) {
        ShowErrorDialog(desc);
    }
}

void CInCallActivity::DismissPendingDialogs()
{
    if (mDialog != NULL) {
        IDialogInterface::Probe(mDialog)->Dismiss();
        mDialog = NULL;
    }
    mAnswerFragment->DismissPendingDialogues();
}

void CInCallActivity::ShowErrorDialog(
    /* [in] */ ICharSequence* msg)
{
    Logger::I("CInCallActivity", "Show Dialog: %s", TO_CSTR(msg));

    DismissPendingDialogs();

    AutoPtr<IAlertDialogBuilder> builder;
    CAlertDialogBuilder::New(IContext::Probe(this), (IAlertDialogBuilder**)&builder);
    builder->SetMessage(msg);
    AutoPtr<IDialogInterfaceOnClickListener> listener = (IDialogInterfaceOnClickListener*)new MyDialogOnClickListener(this);
    builder->SetPositiveButton(Elastos::Droid::Dialer::R::string::ok, listener);
    AutoPtr<IDialogInterfaceOnCancelListener> cancelListener  = new MyDialogOnCancelListener(this);
    builder->SetOnCancelListener(cancelListener);
    AutoPtr<IAlertDialog> temp;
    builder->Create((IAlertDialog**)&temp);
    mDialog = temp;

    AutoPtr<IDialog> d = IDialog::Probe(mDialog);
    AutoPtr<IWindow> win;
    d->GetWindow((IWindow**)&win);
    win->AddFlags(IWindowManagerLayoutParams::FLAG_DIM_BEHIND);
    d->Show();
}

void CInCallActivity::OnDialogDismissed()
{
    mDialog = NULL;
    InCallPresenter::GetInstance()->OnDismissDialog();
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
