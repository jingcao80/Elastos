
#ifndef __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__
#define __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Dialog.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "impl/EnableAccessibilityController.h"

#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::Dialog;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Internal::App::IAlertController;
using Elastos::Droid::Internal::App::IAlertControllerAlertParams;
using Elastos::Droid::Internal::Policy::Impl::EnableAccessibilityController;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Service::Dreams::IDreamManager;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IWindowManagerPolicyWindowManagerFuncs;
using Elastos::Droid::Widget::BaseAdapter;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemLongClickListener;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IListView;


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Policy {
namespace Impl {

extern "C" const InterfaceID EIID_SilentModeTriStateAction;

/**
 * Helper to show the global actions dialog.  Each item is an {@link Action} that
 * may show depending on whether the keyguard is showing, and whether the device
 * is provisioned.
 */
class GlobalActions
    : public IDialogInterfaceOnDismissListener
    , public IDialogInterfaceOnClickListener
    , public ElRefBase
{
private:
    /**
     * The adapter used for the list within the global actions dialog, taking
     * into account whether the keyguard is showing via
     * {@link GlobalActions#mKeyguardShowing} and whether the device is provisioned
     * via {@link GlobalActions#mDeviceProvisioned}.
     */
    class MyAdapter
        : public BaseAdapter
        , public IBaseAdapter
        , public ElRefBase
    {
    public:
        MyAdapter(
            /* [in] */ GlobalActions* host);

        CAR_INTERFACE_DECL();

        IBASEADAPTER_METHODS_DECL();

        // @Override
        CARAPI IsEnabled(
            /* [in] */ Int32 position,
            /* [out] */ Boolean* enabled);

        // @Override
        CARAPI AreAllItemsEnabled(
            /* [out] */ Boolean* enabled);

        CARAPI RegisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI UnregisterDataSetObserver(
            /* [in] */ IDataSetObserver* observer);

        CARAPI_(Int32) GetCount();

        CARAPI GetCount(
            /* [out] */ Int32* count);

        CARAPI_(AutoPtr<IInterface>) GetItem(
            /* [in] */ Int32 position);

        CARAPI GetItem(
            /* [in] */ Int32 position,
            /* [out] */ IInterface** item);

        CARAPI_(Int64) GetItemId(
        /* [in] */ Int32 position);

        CARAPI GetItemId(
            /* [in] */ Int32 position,
            /* [out] */ Int64* itemId);

        CARAPI HasStableIds(
            /* [out] */ Boolean* hasStableIds);

        CARAPI_(AutoPtr<IView>) GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI GetItemViewType(
            /* [in] */ Int32 position,
            /* [out] */ Int32* viewType);

        CARAPI GetViewTypeCount(
            /* [out] */ Int32* count);

        CARAPI IsEmpty(
            /* [out] */ Boolean* isEmpty);

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
        : public IInterface
        , public ElRefBase
    {
    public:
        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        virtual CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        virtual CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater) = 0;

        virtual CARAPI_(void) OnPress() = 0;

        virtual CARAPI_(Boolean) OnLongPress() = 0;

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

    /**
     * A single press action maintains no state, just responds to a press
     * and takes an action.
     */
    class SinglePressAction
        : public Action
    {
    public:
        SinglePressAction(
            /* [in] */ Int32 iconResId,
            /* [in] */ Int32 messageResId);

        SinglePressAction(
            /* [in] */ Int32 iconResId,
            /* [in] */ AutoPtr<IDrawable> icon,
            /* [in] */ AutoPtr<ICharSequence> message);

        SinglePressAction(
            /* [in] */ Int32 iconResId,
            /* [in] */ AutoPtr<ICharSequence> message);

        CARAPI_(Boolean) IsEnabled();

        virtual CARAPI_(void) OnPress() = 0;

        virtual CARAPI_(Boolean) OnLongPress();

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

    private:
        const Int32 mIconResId;
        const AutoPtr<IDrawable> mIcon;
        const Int32 mMessageResId;
        const AutoPtr<ICharSequence> mMessage;
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
            : public ElRefBase
        {
        public:
            static const AutoPtr<State> Off;
            static const AutoPtr<State> TurningOn;
            static const AutoPtr<State> TurningOff;
            static const AutoPtr<State> On;

        public:
            State(
                /* [in] */ Boolean intermediate);

            CARAPI_(Boolean) InTransition();

        private:
            const Boolean inTransition;
        };

    public:
        /**
         * @param enabledIconResId The icon for when this action is on.
         * @param disabledIconResid The icon for when this action is off.
         * @param essage The general information message, e.g 'Silent Mode'
         * @param enabledStatusMessageResId The on status message, e.g 'sound disabled'
         * @param disabledStatusMessageResId The off status message, e.g. 'sound enabled'
         */
        ToggleAction(
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

        CARAPI_(void) OnPress();

        CARAPI_(Boolean) OnLongPress();

        CARAPI_(Boolean) IsEnabled();

        virtual CARAPI_(void) OnToggle(
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

    protected:
        AutoPtr<State> mState;

        // prefs
        Int32 mEnabledIconResId;
        Int32 mDisabledIconResid;
        Int32 mMessageResId;
        Int32 mEnabledStatusMessageResId;
        Int32 mDisabledStatusMessageResId;

    };

    class SilentModeToggleAction
        : public ToggleAction
    {
    public:
        SilentModeToggleAction(
            /* [in] */ GlobalActions* host);

        CARAPI_(void) OnToggle(
            /* [in] */ Boolean on);

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        GlobalActions* mHost;
    };

    class SilentModeTriStateAction
        : public IViewOnClickListener
        , public Action
    {
    public:
        SilentModeTriStateAction(
            /* [in] */ IContext* context,
            /* [in] */ IAudioManager* audioManager,
            /* [in] */ IHandler* handler);

        CARAPI_(UInt32) AddRef();

        CARAPI_(UInt32) Release();

        CARAPI GetInterfaceID(
            /* [in] */ IInterface *pObject,
            /* [out] */ InterfaceID *pIID);

        CARAPI_(PInterface) Probe(
            /* [in]  */ REIID riid);

        CARAPI_(AutoPtr<IView>) Create(
            /* [in] */ IContext* context,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [in] */ ILayoutInflater* inflater);

        CARAPI_(void) OnPress();

        CARAPI_(Boolean) OnLongPress();

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
        , public IDialogInterface
        , public ElRefBase
    {
    public:
        GlobalActionsDialog(
            /* [in] */ IContext* context,
            /* [in] */ IAlertControllerAlertParams* params);

        CAR_INTERFACE_DECL();

        CARAPI Cancel();

        CARAPI Dismiss();

        // @Override
        CARAPI_(Boolean) DispatchTouchEvent(
            /* [in] */ IMotionEvent* event);

        CARAPI_(AutoPtr<IListView>) GetListView();

        // @Override
        CARAPI_(Boolean) OnKeyDown(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

        // @Override
        CARAPI_(Boolean) OnKeyUp(
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event);

    protected:
        // @Override
        CARAPI_(void) OnStart();

        // @Override
        CARAPI_(void) OnStop();

        // @Override
        CARAPI_(void) OnCreate(
            /* [in] */ IBundle* savedInstanceState);

    private:
        static CARAPI_(Int32) GetDialogTheme(
            /* [in] */ IContext* context);

    private:
        const AutoPtr<IContext> mContext;
        Int32 mWindowTouchSlop;
        AutoPtr<IAlertController> mAlert;

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

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("GlobalActions::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        GlobalActions* mHost;
    };

    // TODO: PhoneStateListener is not implement.
    // class PhoneStateListener() {
    //     @Override
    //     public void onServiceStateChanged(ServiceState serviceState) {
    //         if (!mHasTelephony) return;
    //         final boolean inAirplaneMode = serviceState.getState() == ServiceState.STATE_POWER_OFF;
    //         mAirplaneState = inAirplaneMode ? ToggleAction.State.On : ToggleAction.State.Off;
    //         mAirplaneModeOn.updateState(mAirplaneState);
    //         mAdapter.notifyDataSetChanged();
    //     }
    // };

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

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("GlobalActions::RingerModeReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        GlobalActions* mHost;
    };

    class AirplaneModeObserver
        : public ContentObserver
    {
    public:
        AirplaneModeObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ GlobalActions* host);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        GlobalActions* mHost;
    };

    class DialogHandler
        : public HandlerBase
    {
    public:
        DialogHandler(
            /* [in] */ GlobalActions* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GlobalActions* mHost;
    };

// Inner class of local variable.
private:
    class AirplaneModeOn
        : public ToggleAction
    {
    public:
        AirplaneModeOn(
            /* [in] */ GlobalActions* host);

        CARAPI_(void) OnToggle(
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

    class PowerSinglePressAction
        : public SinglePressAction
    {
    public:
        PowerSinglePressAction(
            /* [in] */ GlobalActions* host);

        CARAPI_(void) OnPress();

        CARAPI_(Boolean) OnLongPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        GlobalActions* mHost;
    };

    class BugRunnable
        : public Runnable
    {
    public:
        // @Override
        CARAPI Run();
    };

    class DialogOnClickListener
        : public IDialogInterfaceOnClickListener
        , public ElRefBase
    {
    public:
        DialogOnClickListener(
            /* [in] */ GlobalActions* host);

        CAR_INTERFACE_DECL();

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        GlobalActions* mHost;
    };

    class BugSinglePressAction
        : public SinglePressAction
    {
    public:
        BugSinglePressAction(
            /* [in] */ GlobalActions* host);

        CARAPI_(void) OnPress();

        CARAPI_(Boolean) OnLongPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        GlobalActions* mHost;
    };

    class MyAdapterViewOnItemLongClickListener
        : public IAdapterViewOnItemLongClickListener
        , public ElRefBase
    {
    public:
        MyAdapterViewOnItemLongClickListener(
            /* [in] */ GlobalActions* host);

        CAR_INTERFACE_DECL();

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
            /* [in] */ IDrawable* icon,
            /* [in] */ ICharSequence* message,
            /* [in] */ Int32 userId);

        CARAPI_(void) OnPress();

        CARAPI_(Boolean) ShowDuringKeyguard();

        CARAPI_(Boolean) ShowBeforeProvisioning();

    private:
        Int32 mUserId;
    };

public:
    /**
     * @param context everything needs a context :(
     */
    GlobalActions(
        /* [in] */ IContext* context,
        /* [in] */ IWindowManagerPolicyWindowManagerFuncs* windowManagerFuncs);

    CAR_INTERFACE_DECL();

    /**
     * Show the global actions dialog (creating if necessary)
     * @param keyguardShowing True if keyguard is showing
     */
    CARAPI_(void) ShowDialog(
        /* [in] */ Boolean keyguardShowing,
        /* [in] */ Boolean isDeviceProvisioned);

    /** {@inheritDoc} */
    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    /** {@inheritDoc} */
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

private:
    CARAPI_(void) AwakenIfNecessary();

    CARAPI_(void) HandleShow();

    /**
     * Create the global actions dialog.
     * @return A new dialog.
     */
    CARAPI_(AutoPtr<GlobalActionsDialog>) CreateDialog();

    CARAPI_(void) AddUsersToMenu(
        /* [in] */ List< AutoPtr<Action> >* items);

    CARAPI_(void) PrepareDialog();

    CARAPI_(void) RefreshSilentMode();

    CARAPI_(void) OnAirplaneModeChanged();

    /**
     * Change the airplane mode system setting
     */
    CARAPI_(void) ChangeAirplaneModeSystemSetting(
        /* [in] */ Boolean on);

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
    AutoPtr<IAudioManager> mAudioManager;
    AutoPtr<IDreamManager> mDreamManager;

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

private:
    AutoPtr<MyBroadcastReceiver> mBroadcastReceiver;
    // TODO: PhoneStateListener is not implement.
    // PhoneStateListener mPhoneStateListener;
    AutoPtr<RingerModeReceiver> mRingerModeReceiver;
    AutoPtr<AirplaneModeObserver> mAirplaneModeObserver;
    AutoPtr<IHandler> mHandler;
};

} // namespace Elastos
} // namespace Droid
} // namespace Internal
} // namespace Policy
} // namespace Impl

#endif // __ELASTOS_DROID_INTERNAL_POLICY_IMPL_GLOBALACTIONS_H__
