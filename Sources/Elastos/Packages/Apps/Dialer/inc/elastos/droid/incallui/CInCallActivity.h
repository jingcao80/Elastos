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

#ifndef __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
#define __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecom.h"
#include "_Elastos_Droid_InCallUI_CInCallActivity.h"
#include "elastos/droid/incallui/Call.h"
#include "elastos/droid/incallui/CAnswerFragment.h"
#include "elastos/droid/incallui/CCallButtonFragment.h"
#include "elastos/droid/incallui/CCallCardFragment.h"
#include "elastos/droid/incallui/CConferenceManagerFragment.h"
#include "elastos/droid/incallui/CDialpadFragment.h"
#include "elastos/droid/phone/common/animation/AnimationListenerAdapter.h"
#include "R.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Telecom::CDisconnectCause;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Dialer::R;
using Elastos::Droid::Phone::Common::Animation::AnimationListenerAdapter;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CInCallActivity)
    , public Activity
    , public IInCallActivity
{
private:
    class MyAnimationListenerAdapter
        : public AnimationListenerAdapter
    {
    public:
        MyAnimationListenerAdapter(
            /* [in] */ CInCallActivity* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimation* animation);

    private:
        CInCallActivity* mHost;
    };

    class MyDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        MyDialogOnClickListener(
            /* [in] */ CInCallActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CInCallActivity* mHost;
    };

    class MyDialogOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        MyDialogOnCancelListener(
            /* [in] */ CInCallActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CInCallActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL()

    CInCallActivity();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    /**
     * Returns true when theActivity is in foreground (between onResume and onPause).
     */
    CARAPI_(Boolean) IsForegroundActivity();

    /**
     * Dismisses the in-call screen.
     *
     * We never *really* finish() the InCallActivity, since we don't want to get destroyed and then
     * have to be re-created from scratch for the next call.  Instead, we just move ourselves to the
     * back of the activity stack.
     *
     * This also means that we'll no longer be reachable via the BACK button (since moveTaskToBack()
     * puts us behind the Home app, but the home app doesn't allow the BACK key to move you any
     * farther down in the history stack.)
     *
     * (Since the Phone app itself is never killed, this basically means that we'll keep a single
     * InCallActivity instance around for the entire uptime of the device.  This noticeably improves
     * the UI responsiveness for incoming calls.)
     */
    // @Override
    CARAPI Finish();

    // @Override
    CARAPI OnBackPressed();

    // @Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* config);

    CARAPI_(AutoPtr<CCallButtonFragment>) GetCallButtonFragment();

    CARAPI_(AutoPtr<CCallCardFragment>) GetCallCardFragment();

    /**
     * Simulates a user click to hide the dialpad. This will update the UI to show the call card,
     * update the checked state of the dialpad button, and update the proximity sensor state.
     */
    CARAPI_(void) HideDialpadForDisconnect();

    CARAPI_(void) DismissKeyguard(
        /* [in] */ Boolean dismiss);

    CARAPI_(void) DisplayDialpad(
        /* [in] */ Boolean showDialpad,
        /* [in] */ Boolean animate);

    CARAPI_(Boolean) IsDialpadVisible();

    CARAPI_(void) ShowConferenceCallManager();

    CARAPI_(void) ShowPostCharWaitDialog(
        /* [in] */ const String& callId,
        /* [in] */ const String& chars);

    // @Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* isConsumed);

    CARAPI_(void) MaybeShowErrorDialogOnDisconnect(
        /* [in] */ IDisconnectCause* disconnectCause);

    CARAPI_(void) DismissPendingDialogs();

protected:
    // @Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* out);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnResume();

    // onPause is guaranteed to be called when the InCallActivity goes
    // in the background.
    // @Override
    CARAPI OnPause();

    // @Override
    CARAPI OnStop();

    // @Override
    CARAPI OnDestroy();

    // @Override
    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

private:
    CARAPI_(Boolean) HasPendingErrorDialog();

    CARAPI_(Boolean) HandleDialerKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) InternalResolveIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(Boolean) IsEmergencyCall(
        /* [in] */ Call* call);

    CARAPI_(void) RelaunchedFromDialer(
        /* [in] */ Boolean showDialpad);

    CARAPI_(void) InitializeInCall();

    CARAPI_(void) ShowDialpad(
        /* [in] */ Boolean showDialpad);

    /**
     * Utility function to bring up a generic "error" dialog.
     */
    CARAPI_(void) ShowErrorDialog(
        /* [in] */ ICharSequence* msg);

    CARAPI_(void) OnDialogDismissed();

public:
    static const String SHOW_DIALPAD_EXTRA;
    static const String DIALPAD_TEXT_EXTRA;
    static const String NEW_OUTGOING_CALL;

private:
    AutoPtr<CCallButtonFragment> mCallButtonFragment;
    AutoPtr<CCallCardFragment> mCallCardFragment;
    AutoPtr<CAnswerFragment> mAnswerFragment;
    AutoPtr<CDialpadFragment> mDialpadFragment;
    AutoPtr<CConferenceManagerFragment> mConferenceManagerFragment;
    AutoPtr<IFragmentManager> mChildFragmentManager;

    Boolean mIsForegroundActivity;
    AutoPtr<IAlertDialog> mDialog;

    /** Use to pass 'showDialpad' from {@link #onNewIntent} to {@link #onResume} */
    Boolean mShowDialpadRequested;

    /** Use to determine if the dialpad should be animated on show. */
    Boolean mAnimateDialpadOnShow;

    /** Use to determine the DTMF Text which should be pre-populated in the dialpad. */
    String mDtmfText;

    /** Use to pass parameters for showing the PostCharDialog to {@link #onResume} */
    Boolean mShowPostCharWaitDialogOnResume;
    String mShowPostCharWaitDialogCallId;
    String mShowPostCharWaitDialogChars;

    Boolean mIsLandscape;
    AutoPtr<IAnimation> mSlideIn;
    AutoPtr<IAnimation> mSlideOut;
    AutoPtr<AnimationListenerAdapter> mSlideOutListener;

    /**
     * Stores the current orientation of the activity.  Used to determine if a change in orientation
     * has occurred.
     */
    Int32 mCurrentOrientation;

    friend class MyDialogOnClickListener;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
