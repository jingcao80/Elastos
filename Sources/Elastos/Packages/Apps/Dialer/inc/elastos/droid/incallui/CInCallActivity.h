
#ifndef __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
#define __ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__

#include "_Elastos_Droid_InCallUI_CInCallActivity.h"
#include "elastos/droid/dialer/dialpad/CDialpadFragment.h"
#include "elastos/droid/incallui/CAnswerFragment.h"
#include "elastos/droid/incallui/CCallButtonFragment.h"
#include "elastos/droid/incallui/CCallCardFragment.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Dialer::Dialpad::CDialpadFragment;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CInCallActivity)
    , public Activity
    , public IInCallActivity
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL()

    /**
     * Returns true when theActivity is in foreground (between onResume and onPause).
     */
    CARAPI_(Boolean) IsForegroundActivity();

    /**
     * Simulates a user click to hide the dialpad. This will update the UI to show the call card,
     * update the checked state of the dialpad button, and update the proximity sensor state.
     */
    public void hideDialpadForDisconnect()

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

    CARAPI_(void) MaybeShowErrorDialogOnDisconnect(
        /* [in] */ IDisconnectCause* disconnectCause);

public:
    static const String SHOW_DIALPAD_EXTRA;
    static const String DIALPAD_TEXT_EXTRA;
    static const String NEW_OUTGOING_CALL;

private:
    AutoPtr<CCallButtonFragment> mCallButtonFragment;
    AutoPtr<CCallCardFragment> mCallCardFragment;
    AutoPtr<CAnswerFragment> mAnswerFragment;
    AutoPtr<CDialpadFragment> mDialpadFragment;
//    private ConferenceManagerFragment mConferenceManagerFragment;
//    private FragmentManager mChildFragmentManager;
//
//    Boolean mIsForegroundActivity;
//    AutoPtr<IAlertDialog> mDialog;
//
//    /** Use to pass 'showDialpad' from {@link #onNewIntent} to {@link #onResume} */
//    Boolean mShowDialpadRequested;
//
//    /** Use to determine if the dialpad should be animated on show. */
//    Boolean mAnimateDialpadOnShow;
//
//    /** Use to determine the DTMF Text which should be pre-populated in the dialpad. */
//    String mDtmfText;
//
//    /** Use to pass parameters for showing the PostCharDialog to {@link #onResume} */
//    Boolean mShowPostCharWaitDialogOnResume;
//    String mShowPostCharWaitDialogCallId;
//    String mShowPostCharWaitDialogChars;
//
//    Boolean mIsLandscape;
//    AutoPtr<IAnimation> mSlideIn;
//    AutoPtr<IAnimation> mSlideOut;
//    AnimationListenerAdapter mSlideOutListener = new AnimationListenerAdapter() {
//        @Override
//        public void onAnimationEnd(Animation animation) {
//            showDialpad(false);
//        }
//    };
//
//    /**
//     * Stores the current orientation of the activity.  Used to determine if a change in orientation
//     * has occurred.
//     */
//    Int32 mCurrentOrientation;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INCALLUI_CINCALLACTIVITY_H__
