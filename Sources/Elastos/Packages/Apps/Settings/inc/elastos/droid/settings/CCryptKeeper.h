#ifndef __ELASTOS_DROID_SETTINGS_CCRYPTKEEPER_H__
#define __ELASTOS_DROID_SETTINGS_CCRYPTKEEPER_H__

#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Widget.h"
#include "_Elastos_Droid_Settings_CCryptKeeper.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::Internal::Widget::ILockPatternView;
using Elastos::Droid::Internal::Widget::IOnPatternListener;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Storage::IIMountService;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::InputMethod::IInputMethodManager;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IOnEditorActionListener;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Settings screens to show the UI flows for encrypting/decrypting the device.
 *
 * This may be started via adb for debugging the UI layout, without having to go through
 * encryption flows everytime. It should be noted that starting the activity in this manner
 * is only useful for verifying UI-correctness - the behavior will not be identical.
 * <pre>
 * $ adb shell pm enable com.android.settings/.CryptKeeper
 * $ adb shell am start \
 *     -e "com.android.settings.CryptKeeper.DEBUG_FORCE_VIEW" "progress" \
 *     -n com.android.settings/.CryptKeeper
 * </pre>
 */
CarClass(CCryptKeeper)
    , public Activity
{
private:
    class InnerListener
        : public Object
        , public IOnEditorActionListener
        , public IViewOnKeyListener
        , public IViewOnTouchListener
        , public ITextWatcher
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::InnerListener")

        CAR_INTERFACE_DECL();

        InnerListener(
            /* [in] */ CCryptKeeper* host);

        //@Override
        CARAPI OnEditorAction(
            /* [in] */ ITextView* v,
            /* [in] */ Int32 actionId,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        //@Override
        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        //@Override
        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        CCryptKeeper* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::MyRunnable")

        MyRunnable(
            /* [in] */ CCryptKeeper* host,
            /* [in] */ Int32 id,
            /* [in] */ IInputMethodManager* imm);

        CARAPI Run();

    private:
        CCryptKeeper* mHost;
        Int32 mId;
        AutoPtr<IInputMethodManager> mImm;
    };

    /**
     * Used to propagate state through configuration changes (e.g. screen rotation)
     */
    class NonConfigurationInstanceState
        : public Object
        , public INonConfigurationInstanceState
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::NonConfigurationInstanceState")

        CAR_INTERFACE_DECL()

        NonConfigurationInstanceState(
            /* [in] */ IPowerManagerWakeLock* _wakelock);

    public:
        AutoPtr<IPowerManagerWakeLock> mWakelock;
    };

    class DecryptTask
        //AsyncTask<String, Void, Integer>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::DecryptTask")

        DecryptTask(
            /* [in] */ CCryptKeeper* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params, //String...
            /* [out] */ IInterface** result); //Integer

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* failedAttempts);//Integer

    private:
        CARAPI_(void) Hide(
            /* [in] */ Int32 id);

    private:
        CCryptKeeper* mHost;
    };

    class ValidationTask
        // AsyncTask<Void, Void, Boolean>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::ValidationTask")

        ValidationTask(
            /* [in] */ CCryptKeeper* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params, //Void...
            /* [out] */ IInterface** result); //Boolean

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);//Boolean

    public:
        Int32 mState;

    private:
        CCryptKeeper* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::MyHandler")

        MyHandler(
            /* [in] */ CCryptKeeper* host);

        CARAPI constructor();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CCryptKeeper* mHost;
    };

    class LockPatternViewOnPatternListener
        : public Object
        , public IOnPatternListener
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::LockPatternViewOnPatternListener")

        CAR_INTERFACE_DECL()

        LockPatternViewOnPatternListener(
            /* [in] */ CCryptKeeper* host);

        //@Override
        CARAPI OnPatternStart();

        //@Override
        CARAPI OnPatternCleared();

        //@Override
        CARAPI OnPatternDetected(
            /* [in] */ IList* pattern); //List<LockPatternView.Cell>

        //@Override
        CARAPI OnPatternCellAdded(
            /* [in] */ IList* pattern);//List<Cell>

    private:
        CCryptKeeper* mHost;
    };

    class SetupUiAsyncTask
        // AsyncTask<Void, Void, Void>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::SetupUiAsyncTask")

        SetupUiAsyncTask(
            /* [in] */ CCryptKeeper* host);

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* v, //void...
            /* [out] */ IInterface** result); //void

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* v);//java.lang.Void

    protected:
        Int32 mType;
        String mOwnerInfo;
        Boolean mPatternVisible;

    private:
        CCryptKeeper* mHost;
    };

    class MyOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CCryptKeeper::MyOnClickListener")

        CAR_INTERFACE_DECL()

        MyOnClickListener(
            /* [in] */ CCryptKeeper* host,
            /* [in] */ Int32 id,
            /* [in] */ Boolean corrupt,
            /* [in] */ IInputMethodManager* imm);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CCryptKeeper* mHost;
        Int32 mId;
        Boolean mCorrupt;
        AutoPtr<IInputMethodManager> mImm;
    };

public:
    CAR_OBJECT_DECL()

    CCryptKeeper();

    virtual ~CCryptKeeper();

    CARAPI constructor();

    /**
     * Ignore back events after the user has entered the decrypt screen and while the device is
     * encrypting.
     */
    //@Override
    CARAPI OnBackPressed();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    /**
     * Note, we defer the state check and screen setup to OnStart() because this will be
     * re-run if the user clicks the power button (sleeping/waking the screen), and this is
     * especially important if we were to lose the wakelock for any reason.
     */
    //@Override
    CARAPI OnStart();

    //@Override
    CARAPI OnStop();

    /**
     * Reconfiguring, so propagate the wakelock to the next instance.  This runs between OnStop()
     * and OnDestroy() and only if we are changing configuration (e.g. rotation).  Also clears
     * mWakeLock so the subsequent call to onDestroy does not release it.
     */
    //@Override
    CARAPI OnRetainNonConfigurationInstance(
        /* [out] */ IInterface** instance);

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI OnEditorAction(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 actionId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI BeforeTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 count,
        /* [in] */ Int32 after);

    //@Override
    CARAPI OnTextChanged(
        /* [in] */ ICharSequence* s,
        /* [in] */ Int32 start,
        /* [in] */ Int32 before,
        /* [in] */ Int32 count);

    //@Override
    CARAPI AfterTextChanged(
        /* [in] */ IEditable* s);

private:
    CARAPI_(void) HandleBadAttempt(
        /* [in] */ IInteger32* failedAttempts);

    /** @return whether or not this Activity was started for debugging the UI only. */
    CARAPI_(Boolean) IsDebugView();

    /** @return whether or not this Activity was started for debugging the specific UI view only. */
    CARAPI_(Boolean) IsDebugView(
        /* [in] */ const String& viewType /* non-nullable */);

    /**
     * Notify the user that we are awaiting input. Currently this sends an audio alert.
     */
    CARAPI_(void) NotifyUser();

    /**
     * Initializes the UI based on the current state of encryption.
     * This is idempotent - calling repeatedly will simply re-initialize the UI.
     */
    CARAPI_(void) SetupUi();

    /**
     * Start encrypting the device.
     */
    CARAPI_(void) EncryptionProgressInit();

    /**
     * Show factory reset screen allowing the user to reset their phone when
     * there is nothing else we can do
     * @param corrupt TRUE if userdata is corrupt, FALSE if encryption failed
     *        partway through
     */
    CARAPI_(void) ShowFactoryReset(
        /* [in] */ Boolean corrupt);

    CARAPI_(void) UpdateProgress();

    /** Disable password input for a while to force the user to waste time between retries */
    CARAPI_(void) Cooldown();

    /**
     * Sets the back status: enabled or disabled according to the parameter.
     * @param isEnabled TRUE if back is enabled, FALSE otherwise.
     */
    CARAPI_(void) SetBackFunctionality(
        /* [in] */ Boolean isEnabled);

    CARAPI_(void) FakeUnlockAttempt(
        /* [in] */ IView* postingView);

    CARAPI_(void) PasswordEntryInit();

    /**
     * Method adapted from com.android.inputmethod.latin.Utils
     *
     * @param imm The input method manager
     * @param shouldIncludeAuxiliarySubtypes
     * @return TRUE if we have multiple IMEs to choose from
     */
    CARAPI_(Boolean) HasMultipleEnabledIMEsOrSubtypes(
        /* [in] */ IInputMethodManager* imm,
        /* [in] */ Boolean shouldIncludeAuxiliarySubtypes);

    CARAPI_(AutoPtr<IIMountService>) GetMountService();

    /**
     * Set airplane mode on the device if it isn't an LTE device.
     * Full story: In minimal boot mode, we cannot save any state. In particular, we cannot save
     * any incoming SMS's. So SMSs that are received here will be silently dropped to the floor.
     * That is bad. Also, we cannot receive any telephone calls in this state. So to avoid
     * both these problems, we turn the radio off. However, on certain networks turning on and
     * off the radio takes a Int64 time. In such cases, we are better off leaving the radio
     * running so the latency of an E911 call is short.
     * The behavior after this is:
     * 1. Emergency dialing: the emergency dialer has logic to force the device out of
     *    airplane mode and restart the radio.
     * 2. Full boot: we read the persistent settings from the previous boot and restore the
     *    radio to whatever it was before it restarted. This also happens when rebooting a
     *    phone that has no encryption.
     */
    CARAPI_(void) SetAirplaneModeIfNecessary();

    /**
     * Code to update the state of, and handle clicks from, the "Emergency call" button.
     *
     * This code is mostly duplicated from the corresponding code in
     * LockPatternUtils and LockPatternKeyguardView under frameworks/base.
     */
    CARAPI_(void) UpdateEmergencyCallButtonState();

    CARAPI_(Boolean) IsEmergencyCallCapable();

    CARAPI_(void) TakeEmergencyCallAction();

    CARAPI_(void) LaunchEmergencyDialer();

    CARAPI_(AutoPtr<ITelephonyManager>) GetTelephonyManager();

    CARAPI_(AutoPtr<ITelecomManager>) GetTelecomManager();

    /**
     * Listen to key events so we can disable sounds when we get a keyinput in EditText.
     */
    CARAPI_(void) DelayAudioNotification();

public:
    // Intent action for launching the Emergency Dialer activity.
    static const String ACTION_EMERGENCY_DIAL;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;

protected:
    static const Int32 MIN_LENGTH_BEFORE_REPORT;

    AutoPtr<LockPatternViewOnPatternListener> mChooseNewLockPatternListener;

private:
    static const String TAG;

    static const String DECRYPT_STATE;
    /** Message sent to us to indicate encryption update progress. */
    static const Int32 MESSAGE_UPDATE_PROGRESS = 1;
    /** Message sent to us to cool-down (waste user's time between password attempts) */
    static const Int32 MESSAGE_COOLDOWN = 2;
    /** Message sent to us to indicate alerting the user that we are waiting for password entry */
    static const Int32 MESSAGE_NOTIFY = 3;

    // Constants used to control policy.
    static const Int32 MAX_FAILED_ATTEMPTS;
    static const Int32 COOL_DOWN_ATTEMPTS;
    static const Int32 COOL_DOWN_INTERVAL; // 30 seconds

    // Debug Intent extras so that this Activity may be started via adb for debugging UI layouts
    static const String EXTRA_FORCE_VIEW;
    static const String FORCE_VIEW_PROGRESS;
    static const String FORCE_VIEW_ERROR;
    static const String FORCE_VIEW_PASSWORD;

    /** When encryption is detected, this flag indicates whether or not we've checked for errors. */
    Boolean mValidationComplete;
    Boolean mValidationRequested;
    /** A flag to indicate that the volume is in a bad state (e.g. partially encrypted). */
    Boolean mEncryptionGoneBad;
    /** If gone bad, should we show encryption failed (FALSE) or corrupt (TRUE)*/
    Boolean mCorrupt;
    /** A flag to indicate when the back event should be ignored */
    Boolean mIgnoreBack;
    Int32 mCooldown;
    AutoPtr<IEditText> mPasswordEntry;
    AutoPtr<ILockPatternView> mLockPatternView;
    /** Number of calls to {@link #NotifyUser()} to ignore before notifying. */
    Int32 mNotificationCountdown;
    /** Number of calls to {@link #NotifyUser()} before we release the wakelock */
    Int32 mReleaseWakeLockCountdown;
    Int32 mStatusString;

    // how Int64 we wait to clear a wrong pattern
    static const Int32 WRONG_PATTERN_CLEAR_TIMEOUT_MS;

    // how Int64 we wait to clear a right pattern
    static const Int32 RIGHT_PATTERN_CLEAR_TIMEOUT_MS;

    // When the user enters a short pin/password, run this to show an error,
    // but don't count it against attempts.
    AutoPtr<MyRunnable> mFakeUnlockAttemptRunnable;;

    // TODO: this should be tuned to match minimum decryption timeout
    static const Int32 FAKE_ATTEMPT_DELAY;

    AutoPtr<MyRunnable> mClearPatternRunnable;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<IAudioManager> mAudioManager;
    /** The status bar where back/home/recent buttons are shown. */
    AutoPtr<IStatusBarManager> mStatusBar;

    /** All the widgets to disable in the status bar */
    static Int32 sWidgetsToDisable;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CCRYPTKEEPER_H__