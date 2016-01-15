
#ifndef __ELASTOS_DROID_SERVER_CBLUETOOTHMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CBLUETOOTHMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CBluetoothManagerService.h"
#include <elastos/core/Thread.h>
#include "elastos/droid/os/HandlerBase.h"

using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Droid::Os::HandlerBase;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBluetoothManagerService)
{
private:
    class BluetoothServiceConnection// implements ServiceConnection
    {
    public:
        BluetoothServiceConnection(
            /* [in] */ CBluetoothManagerService* owner);

        void SetGetNameAddressOnly(Boolean getOnly);

        Boolean IsGetNameAddressOnly();

        void OnServiceConnected(ComponentName className, IBinder service);

        void OnServiceDisconnected(ComponentName className);

    private:
        CBluetoothManagerService* mOwner;
        Boolean mGetNameAddressOnly;
    };

    class BluetoothHandler : public HandlerBase
    {
    friend class CBluetoothManagerService;
    public:
        BluetoothHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CBluetoothManagerService* owner);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CBluetoothManagerService* mOwner;
         AutoPtr<IHandler> mHandler;
    };

    class MyRunnable : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CBluetoothManagerService* owner);

        CARAPI Run();

    private:
        CBluetoothManagerService* mOwner;
    };

    class MyIBluetoothCallback : public IBluetoothCallback
    {
    public:
        MyIBluetoothCallback(
            /* [in] */ CBluetoothManagerService* owner);

        CARAPI OnBluetoothStateChange();

    private:
        CBluetoothManagerService* mOwner;
    };

    class MyRunnableEx : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CBluetoothManagerService* owner);

        CARAPI Run();

    private:
        CBluetoothManagerService* mOwner;
    };

    class MyBroadcastReceiver : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CBluetoothManagerService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CBluetoothManagerService::MyBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CBluetoothManagerService* mOwner;
    };

    class BTtimerRunnable : public Runnable {
    public:
        BTtimerRunnable(
            /* [in] */ CBluetoothManagerService* owner);

        virtual CARAPI Run()
        {
            if (mState == BluetoothAdapter.STATE_ON) {
                if (DBG) Log.d(TAG, "10s is passed , get rx count again");
                mHandler.sendMessage(mHandler.obtainMessage(MESSAGE_GET_RX_COUNT,
                               0, 0));
                mDelayedHandler.postDelayed(this, 10 * 1000);
            }
        }

    private:
        CBluetoothManagerService* mOwner;
    }
public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI RegisterAdapter(
        /* [in] */ IIBluetoothManagerCallback* callback,
        /* [out] */ IIBluetooth** result);

    CARAPI UnregisterAdapter(
        /* [in] */ IIBluetoothManagerCallback callback);

    CARAPI RegisterStateChangeCallback(
        /* [in] */ IIBluetoothStateChangeCallback callback);

    CARAPI UnregisterStateChangeCallback(
        /* [in] */ IIBluetoothStateChangeCallback callback);

    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetNameAndAddress();

    CARAPI EnableNoAutoConnect(
        /* [out] */ Boolean* result);

    CARAPI Enable(
        /* [out] */ Boolean* result);

    CARAPI Disable(
        /* [in] */ Boolean persist,
        /* [out] */ Boolean* result);

    CARAPI UnbindAndFinish();

    CARAPI GetAddress(
        /* [out] */ String* result);

    CARAPI GetName(
        /* [out] */ String* result);

private:
    CARAPI_(void) RegisterForAirplaneMode(
        /* [in] */ IIntentFilter* filter);

    CARAPI_(Boolean) IsAirplaneModeOn();

    CARAPI_(Boolean) IsBluetoothPersistedStateOn();

    CARAPI_(Boolean) IsBluetoothPersistedStateOnBluetooth();

    CARAPI_(void) PersistBluetoothSetting(
        /* [in] */ Int32 setOn);

    CARAPI_(Boolean) IsNameAndAddressSet();

    CARAPI_(void) LoadStoredNameAndAddress();

    CARAPI_(void) StoreNameAndAddress(
        /* [in] */ const String& name,
        /* [in] */ const String& address);

    CARAPI_(void) SendBluetoothStateCallback(
        /* [in] */ Boolean isUp);

    CARAPI_(void) SendBluetoothServiceUpCallback();

    CARAPI_(void) SendBluetoothServiceDownCallback();

    CARAPI_(void) HandleEnable(
        /* [in] */ Boolean quietMode);

    CARAPI_(void) HandleDisable();

    CARAPI_(Boolean) CheckIfCallerIsForegroundUser();

    CARAPI_(void) BluetoothStateChangeHandler(
        /* [in] */ Int32 prevState,
        /* [in] */ Int32 newState);

    CARAPI_(Boolean) WaitForOnOff(
        /* [in] */ Boolean on,
        /* [in] */ Boolean off);

    CARAPI_(void) SendDisableMsg();

    CARAPI_(void) SendEnableMsg(
        /* [in] */ Boolean quietMode);

    CARAPI_(Boolean) CanUnbindBluetoothService();

private:
    static const String BLUETOOTH_ADMIN_PERM;
    static const String BLUETOOTH_PERM;
    static const String ACTION_SERVICE_STATE_CHANGED;
    static const String EXTRA_ACTION;
    static const String SECURE_SETTINGS_BLUETOOTH_ADDR_VALID;
    static const String SECURE_SETTINGS_BLUETOOTH_ADDRESS;
    static const String SECURE_SETTINGS_BLUETOOTH_NAME;
    static const Int32 TIMEOUT_BIND_MS;// = 3000; //Maximum msec to wait for a bind
    static const Int32 TIMEOUT_SAVE_MS;// = 500; //Maximum msec to wait for a save
    //Maximum msec to wait for service restart
    static const Int32 SERVICE_RESTART_TIME_MS;// = 200;
    //Maximum msec to delay MESSAGE_USER_SWITCHED
    static const Int32 USER_SWITCHED_TIME_MS;// = 200;

    static const Int32 MESSAGE_ENABLE;// = 1;
    static const Int32 MESSAGE_DISABLE;// = 2;
    static const Int32 MESSAGE_REGISTER_ADAPTER;// = 20;
    static const Int32 MESSAGE_UNREGISTER_ADAPTER;// = 21;
    static const Int32 MESSAGE_REGISTER_STATE_CHANGE_CALLBACK;// = 30;
    static const Int32 MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK;// = 31;
    static const Int32 MESSAGE_BLUETOOTH_SERVICE_CONNECTED;// = 40;
    static const Int32 MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED;// = 41;
    static const Int32 MESSAGE_RESTART_BLUETOOTH_SERVICE;// = 42;
    static const Int32 MESSAGE_BLUETOOTH_STATE_CHANGE;//=60;
    static const Int32 MESSAGE_TIMEOUT_BIND;// =100;
    static const Int32 MESSAGE_TIMEOUT_UNBIND;// =101;
    static const Int32 MESSAGE_GET_NAME_AND_ADDRESS;//=200;
    static const Int32 MESSAGE_SAVE_NAME_AND_ADDRESS;//=201;
    static const Int32 MESSAGE_USER_SWITCHED;// = 300;
    static const Int32 MAX_SAVE_RETRIES;//=3;
    // Bluetooth persisted setting is off
    static const Int32 BLUETOOTH_OFF;//=0;
    // Bluetooth persisted setting is on
    // and Airplane mode won't affect Bluetooth state at start up
    static const Int32 BLUETOOTH_ON_BLUETOOTH;//=1;
    // Bluetooth persisted setting is on
    // but Airplane mode will affect Bluetooth state at start up
    // and Airplane mode will have higher priority.
    static const Int32 BLUETOOTH_ON_AIRPLANE;//=2;

    AutoPtr<IContext> mContext;

    // Locks are not provided for mName and mAddress.
    // They are accessed in handler or broadcast receiver, same thread context.
    String mAddress;
    String mName;
    AutoPtr<IContentResolver> mContentResolver;
    RemoteCallbackList<IBluetoothManagerCallback> mCallbacks;
    RemoteCallbackList<IBluetoothStateChangeCallback> mStateChangeCallbacks;
    AutoPtr<IIBluetooth> mBluetooth;
    Boolean mBinding;
    Boolean mUnbinding;
    // used inside handler thread
    Boolean mQuietEnable;// = FALSE;
    // configuarion from external IBinder call which is used to
    // synchronize with broadcast receiver.
    Boolean mQuietEnableExternal;
    // configuarion from external IBinder call which is used to
    // synchronize with broadcast receiver.
    Boolean mEnableExternal;
    // used inside handler thread
    Boolean mEnable;
    Int32 mState;
    AutoPtr<IHandlerThread> mThread;
    BluetoothHandler* mHandler;
    BluetoothServiceConnection* mConnection;// = new BluetoothServiceConnection();
    MyRunnable* mBTShutDown;
    MyIBluetoothCallback* mBluetoothCallback;
    MyRunnableEx* mBTtimer;
    MyBroadcastReceiver* mReceiver;
    Object mReceiverLock;

    Object mConnectionLock;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBLUETOOTHMANAGERSERVICE_H__
