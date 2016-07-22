
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/policy/impl/EnableAccessibilityController.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/widget/BaseAdapter.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Os::IBinder;
using Elastos::Droid::App::Dialog;
using Elastos::Droid::App::IProfile;
using Elastos::Droid::App::IProfileManager;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Internal::Policy::Impl::EnableAccessibilityController;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindowManagerPolicyWindowManagerFuncs;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

/**
 * Helper to show the global actions dialog.  Each item is an {@link Action} that
 * may show depending on whether the keyguard is showing, and whether the device
 * is provisioned.
 */
class GlobalActions
    : public Object
{
private:
    class DialogListener
        : public Object
        , public IDialogInterfaceOnDismissListener
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogListener(
            /* [in] */ GlobalActions* host);

        /** {@inheritDoc} */
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

        /** {@inheritDoc} */
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        GlobalActions* mHost;
    };

    /**
     * The adapter used for the list within the global actions dialog, taking
     * into account whether the keyguard is showing via
     * {@link GlobalActions#mKeyguardShowing} and whether the device is provisioned
     * via {@link GlobalActions#mDeviceProvisioned}.
     */
    class MyAdapter
        : public BaseAdapter
    {
    public:
        MyAdapter(
            /* [in] */ GlobalActions* host);

        // @Override
        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        // @Override
        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

    private:
        GlobalActions* mHost;
    };

    // note: the scheme below made more sense when we were planning on having
    // 8 different things in the global actions dialog.  seems overkill with
    // only 3 items now, but may as well keep this flexible approach so it will
    // be easy should someone decide at the last minute to include something
    // else, such as 'enable wifi', or 'enable bluetooth'

    /**
     * What each item in the global actions dialog must be able to support.
     */
    class Action
        : public Object
    {
    public:
        /**
          * @return Text that will be announced when dialog is created.  null
          *     for none.
          */
        virtual CARAPI_(AutoPtr<ICharSequence>) GetLabelForAccessibility(
            /* [in] */ IContext* context) = 0;

        virtual CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater) = 0;

        virtual CARAPI OnPress() = 0;

        /**
         * @return whether this action should appear in the dialog when the keygaurd
         *    is showing.
         */
        virtual CARAPI_(Boolean) ShowDuringKeyguard() = 0;

        /**
         * @return whether this action should appear in the dialog before the
         *   device is provisioned.
         */
        virtual CARAPI_(Boolean) ShowBeforeProvisioning() = 0;

        virtual CARAPI_(Boolean) IsEnabled() = 0;
    };

    class LongPressAction
        : public Action
        , public IGlobalActionsLongPressAction
    {
    public:
        virtual CARAPI_(Boolean) OnLongPress() = 0;

    };

    /**
     * A single press action maintains no state, just responds to a press
     * and takes an action.
     */
    class SinglePressAction
        : public Action
        , public IGlobalActionsSinglePressAction
    {
    public:
        CAR_INTERFACE_DECL()

        SinglePressAction(
            /* [in] */ GlobalActions* host,
            /* [in] */ Int32 iconResId,
            /* [in] */ Int32 messageResId);

        SinglePressAction(
            /* [in] */ GlobalActions* host,
            /* [in] */ Int32 iconResId,
            /* [in] */ IDrawable* icon,
            /* [in] */ ICharSequence* message);

        SinglePressAction(
            /* [in] */ GlobalActions* host,
            /* [in] */ Int32 iconResId,
            /* [in] */ ICharSequence* message);

        CARAPI_(Boolean) IsEnabled();

        virtual CARAPI_(AutoPtr<ICharSequence>) GetStatus();

        virtual CARAPI SetStatus(
            /* [in] */ ICharSequence* status);

        virtual CARAPI OnPress() = 0;

        virtual CARAPI_(AutoPtr<ICharSequence>) GetLabelForAccessibility(
            /* [in] */ IContext* context);

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

    protected:
        GlobalActions* mHost;

    private:
        const Int32 mIconResId;
        const AutoPtr<IDrawable> mIcon;
        const Int32 mMessageResId;
        const AutoPtr<ICharSequence> mMessage;
        AutoPtr<ICharSequence> mStatusMessage;
    };

    class PowerAction
        : public SinglePressAction
        , public IGlobalActionsLongPressAction
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("GlobalActions::PowerAction")

        PowerAction(
            /* [in] */ GlobalActions* host);

        virtual CARAPI_(Boolean) OnLongPress();

        virtual CARAPI_(Boolean) ShowDuringKeyguard();

        virtual CARAPI_(Boolean) ShowBeforeProvisioning();

        virtual CARAPI OnPress();
    };

    class RebootAction
        : public SinglePressAction
    {
    public:
        TO_STRING_IMPL("GlobalActions::RebootAction")

        RebootAction(
            /* [in] */ GlobalActions* host);

        virtual CARAPI_(Boolean) ShowDuringKeyguard();

        virtual CARAPI_(Boolean) ShowBeforeProvisioning();

        virtual CARAPI OnPress();
    };

    /**
     * A toggle action knows whether it is on or off, and displays an icon
     * and status message accordingly.
     */
    class ToggleAction
        : public Action
    {
    public:
        class State
            : public Object
        {
        public:
            State(
                /* [in] */ Boolean intermediate);

            CARAPI_(Boolean) InTransition();

        private:
            const Boolean mInTransition;
        };

    public:
        TO_STRING_IMPL("GlobalActions::ToggleAction")

        /**
         * @param enabledIconResId The icon for when this action is on.
         * @param disabledIconResid The icon for when this action is off.
         * @param essage The general information message, e.g 'Silent Mode'
         * @param enabledStatusMessageResId The on status message, e.g 'sound disabled'
         * @param disabledStatusMessageResId The off status message, e.g. 'sound enabled'
         */
        ToggleAction(
            /* [in] */ GlobalActions* host,
            /* [in] */ Int32 enabledIconResId,
            /* [in] */ Int32 disabledIconResid,
            /* [in] */ Int32 message,
            /* [in] */ Int32 enabledStatusMessageResId,
            /* [in] */ Int32 disabledStatusMessageResId);

        /**
         * Override to make changes to resource IDs just before creating the
         * View.
         */
        virtual CARAPI_(void) WillCreate();

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

        CARAPI OnPress();

        CARAPI_(AutoPtr<ICharSequence>) GetLabelForAccessibility(
            /* [in] */ IContext* context);

        CARAPI_(Boolean) IsEnabled();

        virtual CARAPI OnToggle(
            /* [in] */ Boolean on) = 0;

        CARAPI_(void) UpdateState(
            /* [in] */ State* state);

    protected:
        /**
         * Implementations may override this if their state can be in on of the intermediate
         * states until some notification is received (e.g airplane mode is 'turning off' until
         * we know the wireless connections are back online
         * @param buttonOn Whether the button was turned on or off
         */
        virtual CARAPI_(void) ChangeStateFromPress(
            /* [in] */ Boolean buttonOn);

    public:
        static const AutoPtr<State> Off;
        static const AutoPtr<State> TurningOn;
        static const AutoPtr<State> TurningOff;
        static const AutoPtr<State> On;
    protected:
        AutoPtr<State> mState;

        // prefs
        Int32 mEnabledIconResId;
        Int32 mDisabledIconResid;
        Int32 mMessageResId;
        Int32 mEnabledStatusMessageResId;
        Int32 mDisabledStatusMessageResId;
        GlobalActions* mHost;
    };

    class SilentModeToggleAction
        : public ToggleAction
    {
    public:
        TO_STRING_IMPL("GlobalActions::SilentModeToggleAction")

        SilentModeToggleAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnToggle(
            /* [in] */ Boolean on);

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    };

    class SilentModeTriStateAction
        : public Action
        , public IViewOnClickListener
        , public IGlobalActionsSilentModeTriStateAction
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("GlobalActions::SilentModeToggleAction")

        SilentModeTriStateAction(
            /* [in] */ IContext* context,
            /* [in] */ IAudioManager* audioManager,
            /* [in] */ IHandler* handler);

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

        CARAPI OnPress();

        CARAPI_(AutoPtr<ICharSequence>) GetLabelForAccessibility(
            /* [in] */ IContext* context);

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

        CARAPI_(Boolean) IsEnabled();

        CARAPI_(void) WillCreate();

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CARAPI_(Int32) RingerModeToIndex(
            /* [in] */ Int32 ringerMode);

        CARAPI_(Int32) IndexToRingerMode(
            /*  */ Int32 index);

    private:
        AutoPtr< ArrayOf<Int32> > ITEM_IDS;

        AutoPtr<IAudioManager> mAudioManager;
        AutoPtr<IHandler> mHandler;
        AutoPtr<IContext> mContext;
    };

    class GlobalActionsDialog
        : public Dialog
    {
    public:
        class EnableAccessibilityControllerRunnable
            : public Runnable
        {
        public:
            EnableAccessibilityControllerRunnable(
                /* [in] */ GlobalActionsDialog* host);

            // @Override
            CARAPI Run();
        private:
            GlobalActionsDialog* mHost;
        };

    public:
        GlobalActionsDialog();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IAlertControllerAlertParams* params);

        // @Override
        CARAPI DispatchTouchEvent(
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

        CARAPI_(AutoPtr<IListView>) GetListView();

        // @Override
        CARAPI DispatchPopulateAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnKeyDown(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI OnKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

    protected:
        // @Override
        CARAPI OnStart();

        // @Override
        CARAPI OnStop();

        // @Override
        CARAPI OnCreate(
            /* [in] */ IBundle* savedInstanceState);

    private:
        static CARAPI_(Int32) GetDialogTheme(
            /* [in] */ IContext* context);

    private:
        AutoPtr<IContext> mContext;
        Int32 mWindowTouchSlop;
        AutoPtr<IAlertController> mAlert;
        AutoPtr<MyAdapter> mAdapter;

        AutoPtr<EnableAccessibilityController> mEnableAccessibilityController;

        Boolean mIntercepted;
        Boolean mCancelOnUp;
    };

// Inner class of global variable.
private:
    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ GlobalActions* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("GlobalActions::MyBroadcastReceiver")
    private:
        GlobalActions* mHost;
    };

    class ThemeChangeReceiver
        : public BroadcastReceiver
    {
    public:
        ThemeChangeReceiver(
            /* [in] */ GlobalActions* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("GlobalActions::ThemeChangeReceiver")
    private:
        GlobalActions* mHost;
    };

    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        TO_STRING_IMPL("GlobalActions::MyPhoneStateListener")

        MyPhoneStateListener(
            /* [in] */ GlobalActions* host);

        CARAPI OnServiceStateChanged(
            /* [in] */ IServiceState* serviceState);

    private:
        GlobalActions* mHost;
    };

    class RingerModeReceiver
        : public BroadcastReceiver
    {
    public:
        RingerModeReceiver(
            /* [in] */ GlobalActions* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("GlobalActions::RingerModeReceiver: ")
    private:
        GlobalActions* mHost;
    };

    class AirplaneModeObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("GlobalActions::AirplaneModeObserver")

        AirplaneModeObserver(
            /* [in] */ GlobalActions* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        GlobalActions* mHost;
    };

    class DialogHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("GlobalActions::DialogHandler")

        DialogHandler(
            /* [in] */ GlobalActions* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GlobalActions* mHost;
    };

// Inner class of local variable.
private:
    class AirplaneModeOnAction
        : public ToggleAction
    {
    public:
        AirplaneModeOnAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnToggle(
            /* [in] */ Boolean on);

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    protected:
        // @Override
        CARAPI_(void) ChangeStateFromPress(
            /* [in] */ Boolean buttonOn);
    private:
        GlobalActions* mHost;
    };

     /**
     * A single press action maintains no state, just responds to a press
     * and takes an action.
     */
    class ProfileChooseAction
        : public Action
    {
    public:
        ProfileChooseAction(
            /* [in] */ GlobalActions* host);

        CARAPI_(Boolean) IsEnabled();

        virtual CARAPI OnPress() = 0;

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

    protected:
        AutoPtr<IProfileManager> mProfileManager;
        GlobalActions* mHost;
    };

    class ProfileAction
        : public ProfileChooseAction
    {
    public:
        ProfileAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnPress();

        CARAPI_(Boolean) OnLongPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

        CARAPI_(AutoPtr<ICharSequence>) GetLabelForAccessibility(
            /* [in] */ IContext* context);
    };

    class BugRunnable
        : public Runnable
    {
    public:
        // @Override
        CARAPI Run();
    };

    class BugReportDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        BugReportDialogOnClickListener(
            /* [in] */ GlobalActions* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        GlobalActions* mHost;
    };

    class ProfileDialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ProfileDialogOnClickListener(
            /* [in] */ GlobalActions* host,
            /* [in] */ ArrayOf<IProfile*>* profiles);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        GlobalActions* mHost;
        AutoPtr<ArrayOf<IProfile*> > mProfiles;
    };

    class BugSinglePressAction
        : public SinglePressAction
    {
    public:
        BugSinglePressAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnPress();

        //CARAPI_(Boolean) OnLongPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

        virtual CARAPI_(AutoPtr<ICharSequence>) GetStatus();
    };

    class SettingsSinglePressAction
        : public SinglePressAction
    {
    public:
        SettingsSinglePressAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();
    };

    class LockdownSinglePressAction
        : public SinglePressAction
    {
    public:
        LockdownSinglePressAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();
    };

    class MyAdapterViewOnItemLongClickListener
        : public Object
        , public IAdapterViewOnItemLongClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyAdapterViewOnItemLongClickListener(
            /* [in] */ GlobalActions* host);

        // @Override
        CARAPI OnItemLongClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id,
            /* [out] */ Boolean* result);

    private:
        GlobalActions* mHost;
    };

    class UserSinglePressAction
        : public SinglePressAction
    {
    public:
        UserSinglePressAction(
            /* [in] */ GlobalActions* host,
            /* [in] */ Int32 iconId,
            /* [in] */ IDrawable* icon,
            /* [in] */ ICharSequence* message,
            /* [in] */ Int32 userId);

        CARAPI OnPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        Int32 mUserId;
    };

    class ScreenshotAction
        : public SinglePressAction
    {
    public:
        ScreenshotAction(
            /* [in] */ GlobalActions* host);

        CARAPI OnPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        Int32 mUserId;
    };

    class  ScreenshotTimeoutRunnable
        : public Runnable
    {
    public:
        ScreenshotTimeoutRunnable(
            /* [in] */ GlobalActions* host);

        CARAPI Run();

    private:
        GlobalActions* mHost;
    };

    class TakeScreenshotServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        TakeScreenshotServiceConnection(
            /* [in] */ GlobalActions* host);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    private:
        GlobalActions* mHost;
    };

    class TakeScreenshotHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("GlobalActions::TakeScreenshotHandler")

        TakeScreenshotHandler(
            /* [in] */ GlobalActions* host,
            /* [in] */ IServiceConnection* conn);

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        GlobalActions* mHost;
        AutoPtr<IServiceConnection> mConnection;
    };

public:
    GlobalActions();

    /**
     * @param context everything needs a context :(
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs);

    /**
     * Show the global actions dialog (creating if necessary)
     * @param keyguardShowing True if keyguard is showing
     */
    CARAPI_(void) ShowDialog(
        /* [in] */ Boolean keyguardShowing,
        /* [in] */ Boolean isDeviceProvisioned);

private:
    CARAPI_(void) AwakenIfNecessary();

    CARAPI_(void) HandleShow();

    CARAPI_(AutoPtr<IContext>) GetUiContext();

    /**
     * Create the global actions dialog.
     * @return A new dialog.
     */
    CARAPI_(AutoPtr<GlobalActionsDialog>) CreateDialog();

    CARAPI_(void) CreateProfileDialog();

    CARAPI_(AutoPtr<Action>) GetScreenshotAction();

    CARAPI_(AutoPtr<Action>) GetBugReportAction();

    CARAPI_(AutoPtr<Action>) GetSettingsAction();

    CARAPI_(AutoPtr<Action>) GetLockdownAction();

    CARAPI_(AutoPtr<IUserInfo>) GetCurrentUser();

    CARAPI_(Boolean) IsCurrentUserOwner();

    CARAPI_(void) AddUsersToMenu(
        /* [in] */ List< AutoPtr<Action> >* items);

    CARAPI_(void) TakeScreenshot();

    CARAPI_(void) PrepareDialog();

    CARAPI_(void) RefreshSilentMode();

    CARAPI_(void) UpdatePowerMenuActions();

    CARAPI_(void) OnAirplaneModeChanged();

    /**
     * Change the airplane mode system setting
     */
    CARAPI_(void) ChangeAirplaneModeSystemSetting(
        /* [in] */ Boolean on);

    CARAPI_(void) StartQuickBoot();


    /**
     * Generate a new bitmap (width x height pixels, ARGB_8888) with the input bitmap scaled
     * to fit and clipped to an inscribed circle.
     * @param input Bitmap to resize and clip
     * @param width Width of output bitmap (and diameter of circle)
     * @param height Height of output bitmap
     * @return A shiny new bitmap for you to use
     */
    CARAPI_(AutoPtr<IBitmap>) CreateCircularClip(
        /* [in] */ IBitmap* bmp,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    static const String TAG;
    static const Boolean SHOW_SILENT_TOGGLE;

    static const Int32 MESSAGE_DISMISS;
    static const Int32 MESSAGE_REFRESH;
    static const Int32 MESSAGE_SHOW;
    static const Int32 DIALOG_DISMISS_DELAY; // ms

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManagerPolicyWindowManagerFuncs> mWindowManagerFuncs;
    AutoPtr<IContext> mUiContext;
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IIDreamManager> mDreamManager;

    AutoPtr< List< AutoPtr<Action> > > mItems;
    AutoPtr<GlobalActionsDialog> mDialog;

    AutoPtr<Action> mSilentModeAction;
    AutoPtr<ToggleAction> mAirplaneModeOn;

    AutoPtr<MyAdapter> mAdapter;

    Boolean mKeyguardShowing;
    Boolean mDeviceProvisioned;
    AutoPtr<ToggleAction::State> mAirplaneState;
    Boolean mIsWaitingForEcmExit;
    Boolean mHasTelephony;
    Boolean mHasVibrator;
    Boolean mShowSilentToggle;
    AutoPtr<IProfile> mChosenProfile;

    // Power menu customizations
    String mActions;
    Boolean mProfilesEnabled;

    /**
     * functions needed for taking screenhots.
     * This leverages the built in ICS screenshot functionality
     */
    Object mScreenshotLock;
    AutoPtr<IServiceConnection> mScreenshotConnection;

    AutoPtr<IRunnable> mScreenshotTimeout;

    AutoPtr<MyBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<IBroadcastReceiver> mThemeChangeReceiver;
    AutoPtr<IPhoneStateListener> mPhoneStateListener;
    AutoPtr<RingerModeReceiver> mRingerModeReceiver;
    AutoPtr<AirplaneModeObserver> mAirplaneModeObserver;
    AutoPtr<DialogHandler> mHandler;
};

} // namespace Impl
} // namespace Policy
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__
