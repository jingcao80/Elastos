
#include "CBluetoothManagerService.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;

namespace Elastos {
namespace Droid {
namespace Server {

const Boolean DBG = TRUE;
const char* TAG = "CBluetoothManagerService";

const String CBluetoothManagerService::BLUETOOTH_ADMIN_PERM("android.permission.BLUETOOTH_ADMIN");
const String CBluetoothManagerService::BLUETOOTH_PERM("android.permission.BLUETOOTH");
const String CBluetoothManagerService::ACTION_SERVICE_STATE_CHANGED;//="com.android.bluetooth.btservice.action.STATE_CHANGED";
const String CBluetoothManagerService::EXTRA_ACTION=String("action");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_ADDR_VALID=String("bluetooth_addr_valid");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_ADDRESS=String("bluetooth_address");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_NAME=String("bluetooth_name");

const Int32 CBluetoothManagerService::TIMEOUT_BIND_MS = 3000; //Maximum msec to wait for a bind
const Int32 CBluetoothManagerService::TIMEOUT_SAVE_MS = 500; //Maximum msec to wait for a save
//Maximum msec to wait for service restart
const Int32 CBluetoothManagerService::SERVICE_RESTART_TIME_MS = 200;
//Maximum msec to delay MESSAGE_USER_SWITCHED
const Int32 CBluetoothManagerService::USER_SWITCHED_TIME_MS = 200;

const Int32 CBluetoothManagerService::MESSAGE_ENABLE = 1;
const Int32 CBluetoothManagerService::MESSAGE_DISABLE = 2;
const Int32 CBluetoothManagerService::MESSAGE_REGISTER_ADAPTER = 20;
const Int32 CBluetoothManagerService::MESSAGE_UNREGISTER_ADAPTER = 21;
const Int32 CBluetoothManagerService::MESSAGE_REGISTER_STATE_CHANGE_CALLBACK = 30;
const Int32 CBluetoothManagerService::MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK = 31;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_CONNECTED = 40;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED = 41;
const Int32 CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE = 42;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_STATE_CHANGE=60;
const Int32 CBluetoothManagerService::MESSAGE_TIMEOUT_BIND =100;
const Int32 CBluetoothManagerService::MESSAGE_TIMEOUT_UNBIND =101;
const Int32 CBluetoothManagerService::MESSAGE_GET_NAME_AND_ADDRESS=200;
const Int32 CBluetoothManagerService::MESSAGE_SAVE_NAME_AND_ADDRESS=201;
const Int32 CBluetoothManagerService::MESSAGE_USER_SWITCHED = 300;
const Int32 CBluetoothManagerService::MAX_SAVE_RETRIES=3;
// Bluetooth persisted setting is off
const Int32 CBluetoothManagerService::BLUETOOTH_OFF=0;
// Bluetooth persisted setting is on
// and Airplane mode won't affect Bluetooth state at start up
const Int32 CBluetoothManagerService::BLUETOOTH_ON_BLUETOOTH=1;
// Bluetooth persisted setting is on
// but Airplane mode will affect Bluetooth state at start up
// and Airplane mode will have higher priority.
const Int32 CBluetoothManagerService::BLUETOOTH_ON_AIRPLANE=2;

CBluetoothManagerService::BluetoothServiceConnection::BluetoothServiceConnection(
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{ }

void CBluetoothManagerService::BluetoothServiceConnection::SetGetNameAddressOnly(
    /* [in] */ Boolean getOnly)
{
    mGetNameAddressOnly = getOnly;
}

Boolean CBluetoothManagerService::BluetoothServiceConnection::IsGetNameAddressOnly()
{
    return mGetNameAddressOnly;
}

void CBluetoothManagerService::BluetoothServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder service)
{
    if (DBG) Logger::D(TAG, "BluetoothServiceConnection: connected to AdapterService");
    mOwner->mHandler->SendMessage(MESSAGE_BLUETOOTH_SERVICE_CONNECTED, 0, 0, service);
}

void CBluetoothManagerService::BluetoothServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName className)
{
    // Called if we unexpected disconnected.
    if (DBG) Logger::D(TAG, "BluetoothServiceConnection: disconnected from AdapterService");
    mOwner->mHandler->SendMessage(MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED, 0, 0, NULL);
}

CBluetoothManagerService::BluetoothHandler::BluetoothHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{
}

void CBluetoothManagerService::BluetoothHandler::HandleMessage(
    /* [in] */ IMessage * msg)
{
    Int32 what, arg1,  arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    if (DBG) Logger::D (TAG, "Message: " + what);
    switch (what) {
        case MESSAGE_GET_NAME_AND_ADDRESS:
            if (DBG) Logger::D(TAG,"MESSAGE_GET_NAME_AND_ADDRESS");
            {
                AutoLock lock(mOwner->mConnectionLock);
                //Start bind request
                if ((mOwner->mBluetooth == NULL) && (!mOwner->mBinding)) {
                    if (DBG) Logger::D(TAG, "Binding to service to get name and address");
                    mOwner->mConnection->SetGetNameAddressOnly(TRUE);
                    //Start bind timeout and bind
                    this->SendMessageDelayed(MESSAGE_TIMEOUT_BIND, 0, 0, NULL, TIMEOUT_BIND_MS);
                    AutoPtr<IIntent> i;
                    CIntent::New(IBluetooth.class.getName(), (IIntent**)&i);
                    if (!mOwner->mContext->BindService(i, mConnection,
                          IContext::BIND_AUTO_CREATE, UserHandle::USER_CURRENT)) {
                        mHandler->RemoveMessages(MESSAGE_TIMEOUT_BIND);
                        Logger::E(TAG, "fail to bind to: " + IBluetooth.class.getName());
                    } else {
                        mOwner->mBinding = TRUE;
                    }
                } else {
                    if (mOwner->mBluetooth != NULL) {
                        this->SendMessage(MESSAGE_SAVE_NAME_AND_ADDRESS, 0, 0, NULL);
                    } else {
                        // if enable is also called to bind the service
                        // wait for MESSAGE_BLUETOOTH_SERVICE_CONNECTED
                        this->SendMessageDelayed(MESSAGE_SAVE_NAME_AND_ADDRESS, 0, 0, NULL, TIMEOUT_SAVE_MS);
                    }
                }
            }
            break;

        case MESSAGE_SAVE_NAME_AND_ADDRESS:
            Boolean unbind = FALSE;
            if (DBG) Logger::D(TAG,"MESSAGE_SAVE_NAME_AND_ADDRESS");
            {
                AutoLock lock(mOwner->mConnectionLock);
                if (!mOwner->mEnable && mOwner->mBluetooth != NULL)
                    mOwner->mBluetooth->Enable();
            }
            if (mOwner->mBluetooth != NULL)
                mOwner->WaitForOnOff(TRUE, FALSE);

            {
                AutoLock lock(mOwner->mConnectionLock);
                if (mOwner->mBluetooth != NULL) {
                    String name =  NULL;
                    String address = NULL;
                    mOwner->mBluetooth->GetName(&name);
                    mOwner->mBluetooth->GetAddress(&address);

                    if (name != NULL && address != NULL) {
                        mOwner->StoreNameAndAddress(name,address);
                        if (mOwner->mConnection->IsGetNameAddressOnly()) {
                            unbind = TRUE;
                        }
                    } else {
                        if (arg1 < MAX_SAVE_RETRIES) {
                            //if (DBG) Logger::D(TAG,"Retrying name/address remote retrieval and save.....Retry count =" + retryMsg.arg1);
                            this->SendMessageDelayed(MESSAGE_SAVE_NAME_AND_ADDRESS, 1+arg1, 0, NULL, TIMEOUT_SAVE_MS);
                        } else {
                            Logger::W(TAG,"Maximum name/address remote retrieval retry exceeded");
                            if (mOwner->mConnection->IsGetNameAddressOnly())
                                unbind = TRUE;
                        }
                    }

                    if (!mOwner->mEnable)
                        mOwner->mBluetooth->Disable();
                } else {
                    // rebind service by Request GET NAME AND ADDRESS
                    // if service is unbinded by disable or
                    // MESSAGE_BLUETOOTH_SERVICE_CONNECTED is not received
                    this->SendMessage(MESSAGE_GET_NAME_AND_ADDRESS, 0, 0, NULL);
                }
            }
            if (!mOwner->mEnable && mOwner->mBluetooth != NULL)
                mOwner->WaitForOnOff(FALSE, TRUE);
            if (unbind)
                mOwner->UnbindAndFinish();
            break;

        case MESSAGE_ENABLE:
            if (DBG) {
                Logger::D(TAG, "MESSAGE_ENABLE: mBluetooth = " + mBluetooth);
            }
            mOwner->mHandler->RemoveMessages(MESSAGE_RESTART_BLUETOOTH_SERVICE);
            mOwner->mEnable = TRUE;
            mOwner->HandleEnable(arg1 == 1);
            break;

        case MESSAGE_DISABLE:
            mHandler->RemoveMessages(MESSAGE_RESTART_BLUETOOTH_SERVICE);
            if (mEnable && mBluetooth != NULL) {
                mOwner->WaitForOnOff(TRUE, FALSE);
                 mOwner->mEnable = FALSE;
                mOwner->HandleDisable();
                mOwner->WaitForOnOff(FALSE, FALSE);
            } else {
                 mOwner->mEnable = FALSE;
                mOwner->HandleDisable();
            }
            break;

        case MESSAGE_REGISTER_ADAPTER:
            IBluetoothManagerCallback callback = (IBluetoothManagerCallback) msg.obj;
            Boolean added = mCallbacks.register(callback);
            Logger::D(TAG,"Added callback: " +  (callback == NULL? "NULL": callback)  +":" +added );
            break;

        case MESSAGE_UNREGISTER_ADAPTER:
            IBluetoothManagerCallback callback = (IBluetoothManagerCallback) msg.obj;
            Boolean removed = mCallbacks.unregister(callback);
            Logger::D(TAG,"Removed callback: " +  (callback == NULL? "NULL": callback)  +":" + removed);
            break;

        case MESSAGE_REGISTER_STATE_CHANGE_CALLBACK:
            IBluetoothStateChangeCallback callback = (IBluetoothStateChangeCallback) msg.obj;
            mStateChangeCallbacks.register(callback);
            break;

        case MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK:
            IBluetoothStateChangeCallback callback = (IBluetoothStateChangeCallback) msg.obj;
            mOwner->mStateChangeCallbacks.unregister(callback);
            break;

        case MESSAGE_BLUETOOTH_SERVICE_CONNECTED:
            if (DBG) Logger::D(TAG,"MESSAGE_BLUETOOTH_SERVICE_CONNECTED");

            //Remove timeout
            mOwner->mHandler->RemoveMessages(MESSAGE_TIMEOUT_BIND);

            IBinder service = (IBinder) msg.obj;
            {
                AutoLock lock(mConnectionLock);
                mOwner->mBinding = FALSE;
                mOwner->mBluetooth = IBluetooth.Stub.asInterface(service);

                if (mOwner->mConnection->IsGetNameAddressOnly()) {
                    //Request GET NAME AND ADDRESS
                    this->SendMessage(MESSAGE_GET_NAME_AND_ADDRESS, 0, 0, NULL);
                    if (!mOwner->mEnable) return;
                }

                mOwner->mConnection->SetGetNameAddressOnly(FALSE);
                //Register callback object
                mOwner->mBluetooth->RegisterCallback(mBluetoothCallback);
                //Inform BluetoothAdapter instances that service is up
                mOwner->SendBluetoothServiceUpCallback();

                //Do enable request
                if (mOwner->mQuietEnable == FALSE) {
                    if(!mOwner->mBluetooth->Enable())
                        Logger::E(TAG,"IBluetooth.enable() returned FALSE");
                } else {
                    if(!mOwner->mBluetooth->EnableNoAutoConnect())
                        Logger::E(TAG,"IBluetooth.enableNoAutoConnect() returned FALSE");
                }
            }

            if (!mOwner->mEnable) {
                mOwner->WaitForOnOff(TRUE, FALSE);
                mOwner->HandleDisable();
                mOwner->WaitForOnOff(FALSE, FALSE);
            }
            break;

        case MESSAGE_TIMEOUT_BIND:
            Logger::E(TAG, "MESSAGE_TIMEOUT_BIND");
            {
                AutoLock lock(mConnectionLock);
                mOwner->mBinding = FALSE;
            }
            break;

        case MESSAGE_BLUETOOTH_STATE_CHANGE:
            Int32 prevState = arg1;
            Int32 newState = arg2;
            if (DBG) Logger::D(TAG, "MESSAGE_BLUETOOTH_STATE_CHANGE: prevState = " + prevState + ", newState=" + newState);
            mOwner->mState = newState;
            mOwner->BluetoothStateChangeHandler(prevState, newState);
            break;

        case MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED:
            Logger::E(TAG, "MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED");
            {
                AutoLock lock(mConnectionLock);
                // if service is unbinded already, do nothing and return
                if (mOwner->mBluetooth == NULL) return;
                mOwner->mBluetooth = NULL;
            }

            if (mOwner->mEnable) {
                mOwner->mEnable = FALSE;
                // Send a Bluetooth Restart message
                this->SendMessageDelayed(MESSAGE_RESTART_BLUETOOTH_SERVICE, 0, 0, NULL,
                    SERVICE_RESTART_TIME_MS);
            }

            if (!mOwner->mConnection->IsGetNameAddressOnly()) {
                mOwner->SendBluetoothServiceDownCallback();

                // Send BT state broadcast to update
                // the BT icon correctly
                if ((mState == BluetoothAdapter::STATE_TURNING_ON) ||
                    (mState == BluetoothAdapter::STATE_ON)) {
                    bluetoothStateChangeHandler(BluetoothAdapter::STATE_ON,
                                                BluetoothAdapter::STATE_TURNING_OFF);
                    mState = BluetoothAdapter::STATE_TURNING_OFF;
                }
                if (mState == BluetoothAdapter::STATE_TURNING_OFF) {
                    bluetoothStateChangeHandler(BluetoothAdapter::STATE_TURNING_OFF,
                                                BluetoothAdapter::STATE_OFF);
                }

                mHandler.removeMessages(MESSAGE_BLUETOOTH_STATE_CHANGE);

                mOwner->BluetoothStateChangeHandler(BluetoothAdapter::STATE_ON,
                                            BluetoothAdapter::STATE_TURNING_OFF);
                mOwner->mState = BluetoothAdapter::STATE_OFF;
            }
            break;

        case MESSAGE_RESTART_BLUETOOTH_SERVICE:
            Logger::D(TAG, "MESSAGE_RESTART_BLUETOOTH_SERVICE:"
                +" Restart IBluetooth service");
            /* Enable without persisting the setting as
             it doesnt change when IBluetooth
             service restarts */
            mOwner->mEnable = TRUE;
            mOwner->HandleEnable(mOwner->mQuietEnable);
            break;

        case MESSAGE_TIMEOUT_UNBIND:
            Logger::E(TAG, "MESSAGE_TIMEOUT_UNBIND");
            {
                AutoLock lock(mOwner->mConnectionLock);
                mOwner->mUnbinding = FALSE;
            }
            break;

        case MESSAGE_USER_SWITCHED:
            if (DBG) {
                Logger::D(TAG, "MESSAGE_USER_SWITCHED");
            }
            mHandler->RemoveMessages(MESSAGE_USER_SWITCHED);
            /* disable and enable BT when detect a user switch */
            if (mOwner->mEnable && mOwner->mBluetooth != NULL) {
                {
                    AutoLock lock(mOwner->mConnectionLock);
                    if (mOwner->mBluetooth != NULL) {
                        //Unregister callback object
                        mOwner->mBluetooth->UnregisterCallback(mBluetoothCallback);
                    }
                }

                if (mState == BluetoothAdapter::STATE_TURNING_OFF) {
                    // MESSAGE_USER_SWITCHED happened right after MESSAGE_ENABLE
                    bluetoothStateChangeHandler(mState, BluetoothAdapter::STATE_OFF);
                    mState = BluetoothAdapter::STATE_OFF;
                }
                if (mState == BluetoothAdapter::STATE_OFF) {
                    bluetoothStateChangeHandler(mState, BluetoothAdapter::STATE_TURNING_ON);
                    mState = BluetoothAdapter::STATE_TURNING_ON;
                }

                mOwner->WaitForOnOff(TRUE, FALSE);

                if (mState == BluetoothAdapter::STATE_TURNING_ON) {
                    mOwner->BluetoothStateChangeHandler(mOwner->mState, BluetoothAdapter::STATE_ON);
                }

                // disable
                mOwner->HandleDisable();

                // Pbap service need receive STATE_TURNING_OFF intent to close
                mOwner->BluetoothStateChangeHandler(
                    BluetoothAdapter::STATE_ON, BluetoothAdapter::STATE_TURNING_OFF);

                mOwner->WaitForOnOff(FALSE, TRUE);

                mOwner->BluetoothStateChangeHandler(
                    BluetoothAdapter::STATE_TURNING_OFF, BluetoothAdapter::STATE_OFF);

                mOwner->SendBluetoothServiceDownCallback();
                {
                    AutoLock lock(mConnectionLock);
                    if (mOwner->mBluetooth != NULL) {
                        mOwner->mBluetooth = NULL;
                        //Unbind
                        mOwner->mContext->UnbindService(mConnection);
                    }
                }
                SystemClock.sleep(100);

                // enable
                mHandler->RemoveMessages(MESSAGE_BLUETOOTH_STATE_CHANGE);
                mState = BluetoothAdapter::STATE_OFF;
                // enable
                mOwner->HandleEnable(mQuietEnable);
            }
            else if (mOwner->mBinding || mOwner->mBluetooth != NULL) {
                // if user is switched when service is being binding
                // delay sending MESSAGE_USER_SWITCHED
                this->SendMessageDelayed(MESSAGE_USER_SWITCHED, 0, 1+arg2, NULL, USER_SWITCHED_TIME_MS);
                if (DBG)
                    Logger::D(TAG, "delay MESSAGE_USER_SWITCHED " + userMsg.arg2);
            }
            break;

    }
}

CBluetoothManagerService::MyRunnable::MyRunnable(
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{ }

ECode CBluetoothManagerService::MyRunnable::Run()
{
    mOwner->ReleaseWakeLock();
    return NOERROR;
}

CBluetoothManagerService::MyIBluetoothCallback::MyIBluetoothCallback(
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{ }

ECode CBluetoothManagerService::MyIBluetoothCallback::OnBluetoothStateChange(
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState)
{
    mOwner->mHandler->SendMessage(MESSAGE_BLUETOOTH_STATE_CHANGE, prevState, newState, NULL);
    return NOERROR;
}

CBluetoothManagerService::MyRunnableEx::MyRunnable(
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{ }

ECode CBluetoothManagerService::MyRunnableEx::Run()
{
    if (mOwner->mState == BluetoothAdapter::STATE_ON) {
        if (DBG) Log.d(TAG, "10s is passed , get rx count again");
        mOwner->mHandler->SendMessage(MESSAGE_GET_RX_COUNT, 0, 0, NULL);
        mOwner->mDelayedHandler.postDelayed(this, 10 * 1000);
    }
    return NOERROR;
}

CBluetoothManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CBluetoothManagerService* owner)
    : mOwner(owner)
{ }

ECode CBluetoothManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (BluetoothAdapter::ACTION_LOCAL_NAME_CHANGED.Equals(action)) {
        String newName = intent.getStringExtra(BluetoothAdapter::EXTRA_LOCAL_NAME);
        if (DBG) Log.d(TAG, "Bluetooth Adapter name changed to " + newName);
        if (newName != NULL) {
            mOwner->StoreNameAndAddress(newName, NULL);
        }
    }
    else if (IIntent::ACTION_AIRPLANE_MODE_CHANGED.Equals(action)) {
        AutoLock lock(mReceiverLock);
        if (mOwner->IsBluetoothPersistedStateOn()) {
            if (mOwner->IsAirplaneModeOn()) {
                mOwner->PersistBluetoothSetting(BLUETOOTH_ON_AIRPLANE);
            } else {
                mOwner->PersistBluetoothSetting(BLUETOOTH_ON_BLUETOOTH);
            }
        }
        if (mOwner->IsAirplaneModeOn()) {
            // disable without persisting the setting
            mOwner->SendDisableMsg();
        } else if (mEnableExternal) {
            // enable without persisting the setting
            mOwner->SendEnableMsg(mQuietEnableExternal);
        }
    }
    else if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 num;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &num)
        mOwner->mHandler->SendMessage(MESSAGE_USER_SWITCHED, num, 0, NULL);
    }
    else if (Intent.ACTION_BOOT_COMPLETED.equals(action)) {
        {
            AutoLock lock(mReceiverLock);
            if (mOwner->mEnableExternal && mOwner->IsBluetoothPersistedStateOnBluetooth()) {
                //Enable
                // if (DBG) Log.d(TAG, "Auto-enabling Bluetooth.");
                mOwner->SendEnableMsg(mQuietEnableExternal);
            }
        }

        if (!mOwner->IsNameAndAddressSet()) {
            //Sync the Bluetooth name and address from the Bluetooth Adapter
            // if (DBG) Log.d(TAG,"Retrieving Bluetooth Adapter name and address...");
            mOwner->GetNameAndAddress();
        }
    }
    return NOERROR;
}


void CBluetoothManagerService::RegisterForAirplaneMode(
    /* [in] */ IIntentFilter* filter)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String airplaneModeRadios;
    Settings::Global::GetString(resolver, Settings::Global::AIRPLANE_MODE_RADIOS, &airplaneModeRadios);
    String toggleableRadios;
    Settings::Global::GetString(resolver, Settings::Global::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleableRadios);
    Boolean mIsAirplaneSensitive = airplaneModeRadios == NULL ? TRUE :
            airplaneModeRadios.Contains(Settings::Global::RADIO_BLUETOOTH);
    if (mIsAirplaneSensitive) {
        filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    }
}

ECode CBluetoothManagerService::constructor(
    /* [in] */ IContext* context)
{
    CHandlerThread::New("BluetoothManager", (*IHandlerThread*)&mThread);
    mThread->Start();
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    mHandler = new BluetoothHandler(looper, this);

    mContext = context;
    mBluetooth = NULL;
    mBinding = FALSE;
    mUnbinding = FALSE;
    mEnable = FALSE;
    mState = BluetoothAdapter::STATE_OFF;
    mQuietEnableExternal = FALSE;
    mEnableExternal = FALSE;
    mAddress = NULL;
    mName = NULL;
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    mCallbacks = new RemoteCallbackList<IBluetoothManagerCallback>();
    mStateChangeCallbacks = new RemoteCallbackList<IBluetoothStateChangeCallback>();
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETE, (IIntentFilter**)&filter);
    filter->AddAction(BluetoothAdapter::ACTION_LOCAL_NAME_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    RegisterForAirplaneMode(filter);

    mContext->RegisterReceiver(mReceiver, filter);
    Boolean airplaneModeOn = IsAirplaneModeOn();
    Boolean bluetoothOn = IsBluetoothPersistedStateOn();
    LoadStoredNameAndAddress();

    String value;
    if (IsBluetoothPersistedStateOn()) {
        mEnableExternal = true;
    }
    else {
        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        sysProp->Get(String("ro.property.bluetooth.rtk8723a"), &value);
        if (String("true").Equals(value)) {
            //Realtek add start
            //Enable
            AutoPtr<IMessage> msg1;
            mHandler->ObtainMessage(MESSAGE_ENABLE, (IMessage**)&msg1);
            msg1->SetArg1(0); //No persist
            msg1->SetArg2(1); //Quiet mode
            Boolean rst;
            mHandler->SendMessage(msg1, &rst);
            Thread::Sleep(2000);
            // try {
            //     Thread.sleep(2000); //add delay to make sure BT is enabled
            // } catch (Exception e) {}

            //Disable
            AutoPtr<IMessage> msg2;
            mHandler->ObtainMessage(MESSAGE_DISABLE, (IMessage**)&msg2);
            msg2->SetArg1(0);
            mHandler->SendMessage(msg2, &rst);
        }
   }
   //Realtek add end

    return NOERROR;
}

/**
 *  Returns TRUE if airplane mode is currently on
 */
Boolean CBluetoothManagerService::IsAirplaneModeOn()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 val;
    Settings::Global::GetInt32(resolver, Settings::Global::AIRPLANE_MODE_ON, 0, &val);
    return (val == 1);
}

/**
 *  Returns TRUE if the Bluetooth saved state is "on"
 */
Boolean CBluetoothManagerService::IsBluetoothPersistedStateOn()
{
    Int32 val;
    Settings::Global::GetInt32(mContentResolver, Settings::Global::BLUETOOTH_ON, 0, &val);
    return (val == BLUETOOTH_OFF);
}

/**
 *  Returns true if the Bluetooth saved state is BLUETOOTH_ON_BLUETOOTH
 */
Boolean CBluetoothManagerService::IsBluetoothPersistedStateOn()
{
    Int32 val;
    Settings::Global::GetInt32(mContentResolver, Settings::Global::BLUETOOTH_ON, 0, &val);
    return (val == BLUETOOTH_ON_BLUETOOTH);
}

/**
 *  Save the Bluetooth on/off state
 *
 */
void CBluetoothManagerService::PersistBluetoothSetting(
    /* [in] */ Int32 setOn)
{
    Boolean bol;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Settings::Global::PutInt32(resolver, Settings::Global::BLUETOOTH_ON, setOn, &bol);
}

/**
 * Returns TRUE if the Bluetooth Adapter's name and address is
 * locally cached
 * @return
 */
Boolean CBluetoothManagerService::IsNameAndAddressSet()
{
    return mName !=NULL && mAddress!= NULL && mName.GetLength()>0 && mAddress.GetLength()>0;
}

/**
 * Retrieve the Bluetooth Adapter's name and address and save it in
 * in the local cache
 */
void CBluetoothManagerService::LoadStoredNameAndAddress()
{
    if (DBG) Logger::D(TAG, "Loading stored name and address");
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean bol;
    res->GetBoolean(R::bool_::config_bluetooth_address_validation, &bol);
    Int32 val;
    Settings::Secure::GetInt32(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDR_VALID, 0, &val)
    if (bol && (val == 0)) {
        // if the valid flag is not set, don't load the address and name
        if (DBG) Logger::D(TAG, "invalid bluetooth name and address stored");
        return;
    }
    Settings::Secure::GetString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_NAME, &mName);
    Settings::Secure::GetString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDRESS, &mAddress);
    if (DBG) Logger::D(TAG, "Stored bluetooth Name=" + mName + ",Address=" + mAddress);
}

/**
 * Save the Bluetooth name and address in the persistent store.
 * Only non-NULL values will be saved.
 * @param name
 * @param address
 */
void CBluetoothManagerService::StoreNameAndAddress(
    /* [in] */ const String& name,
    /* [in] */ const String& address)
{
    Boolean bol;
    String str;

    if (name != NULL) {
        Settings::Secure::PutString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_NAME, name, &bol);
        mName = name;
        Settings::Secure::GetString(mContentResolver,SECURE_SETTINGS_BLUETOOTH_NAME, &str);
        if (DBG)
            Logger::D(TAG,"Stored Bluetooth name: " + str);
    }

    if (address != NULL) {
        Settings::Secure::PutString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDRESS, address, &bol);
        mAddress=address;
        Settings::Secure::GetString(mContentResolver,SECURE_SETTINGS_BLUETOOTH_ADDRESS, &str)
        if (DBG)  Logger::D(TAG,"Stored Bluetoothaddress: " + str);
    }

    if ((name != NULL) && (address != NULL)) {
        Settings::Secure::PutInt32(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDR_VALID, 1, &bol);
    }
}

ECode CBluetoothManagerService::RegisterAdapter(
    /* [in] */ IIBluetoothManagerCallback* callback,
    /* [out] */ IIBluetooth** result)
{
    VALIDATE_NOT_NULL(result);

    mHandler->SendMessage(MESSAGE_REGISTER_ADAPTER, 0, 0, callback);
    {
        AutoLock lock(mConnectionLock);
        *result = mBluetooth;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode CBluetoothManagerService::UnregisterAdapter(
    /* [in] */ IIBluetoothManagerCallback* callback)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
                                            "Need BLUETOOTH permission"));
    mHandler->SendMessage(MESSAGE_UNREGISTER_ADAPTER, 0, 0, callback);
    return NOERROR;
}

ECode CBluetoothManagerService::RegisterStateChangeCallback(
    /* [in] */ IIBluetoothStateChangeCallback callback)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
                                            "Need BLUETOOTH permission"));
    mHandler->SendMessage(MESSAGE_REGISTER_STATE_CHANGE_CALLBACK, 0, 0, callback);
    return NOERROR;
}

ECode CBluetoothManagerService::UnregisterStateChangeCallback(
    /* [in] */ IIBluetoothStateChangeCallback callback)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
                                            "Need BLUETOOTH permission"));
    mHandler->SendMessage(MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK, 0, 0, callback);
    return NOERROR;
}

ECode CBluetoothManagerService::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        !CheckIfCallerIsForegroundUser()) {
        Logger::W(TAG,"isEnabled(): not allowed for non-active and non system user");
        *result = FALSE;
        return NOERROR;
    }

    {
        AutoLock lock(mConnectionLock);
        Boolean bol;
        mBluetooth->IsEnabled(&bol);
        *result = (mBluetooth != NULL && bol);
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothManagerService::GetNameAndAddress()
{
    if (DBG) {
//        Logger::D(TAG,"getNameAndAddress(): mBluetooth = " + mBluetooth +
//              " mBinding = " + mBinding);
    }
    mHandler->SendMessage(MESSAGE_GET_NAME_AND_ADDRESS, 0, 0, NULL);
    return NOERROR;
}

ECode CBluetoothManagerService::EnableNoAutoConnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_ADMIN_PERM,
                                            "Need BLUETOOTH ADMIN permission"));

    if (DBG) {
//        Logger::D(TAG,"enableNoAutoConnect():  mBluetooth =" + mBluetooth +
//                " mBinding = " + mBinding);
    }

    Int32 callingAppId = UserHandle::GetAppId(Binder::GetCallingUid());
    if (callingAppId != IProcess::NFC_UID) {
        // throw new SecurityException("no permission to enable Bluetooth quietly");
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(mReceiverLock);
        mQuietEnableExternal = TRUE;
        mEnableExternal = TRUE;
        SendEnableMsg(TRUE);
    }

    *result = TRUE;
    return NOERROR;

}

ECode CBluetoothManagerService::Enable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        (!CheckIfCallerIsForegroundUser())) {
        Logger::W(TAG,"enable(): not allowed for non-active and non system user");
        *result = FALSE;
        return NOERROR;
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        BLUETOOTH_ADMIN_PERM,
        String("Need BLUETOOTH ADMIN permission")));
    // if (DBG) {
    //     Log.d(TAG,"enable():  mBluetooth =" + mBluetooth +
    //             " mBinding = " + mBinding);
    // }

    {
        AutoLock lock(mReceiverLock);
        mQuietEnableExternal = FALSE;
        mEnableExternal = TRUE;
        // waive WRITE_SECURE_SETTINGS permission check
        Int64 callingIdentity = Binder::ClearCallingIdentity();
        PersistBluetoothSetting(BLUETOOTH_ON_BLUETOOTH);
        Binder::RestoreCallingIdentity(callingIdentity);
        SendEnableMsg(FALSE);
    }

    *result = FALSE;
    return NOERROR;
}

ECode CBluetoothManagerService::Disable(
    /* [in] */ Boolean persist,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_ADMIN_PERM,
                                            "Need BLUETOOTH ADMIN permissicacheNameAndAddresson"));

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        (!CheckIfCallerIsForegroundUser())) {
        Logger::W(TAG,"disable(): not allowed for non-active and non system user");
        *result = FALSE;
        return NOERROR;
    }

    // if (DBG) {
    //     Log.d(TAG,"disable(): mBluetooth = " + mBluetooth +
    //         " mBinding = " + mBinding);
    // }

    {
        AutoLock lock(mReceiverLock);
        if (persist) {
            // waive WRITE_SECURE_SETTINGS permission check
            Int64 callingIdentity = Binder::ClearCallingIdentity();
            PersistBluetoothSetting(BLUETOOTH_OFF);
            Binder::RestoreCallingIdentity(callingIdentity);
        }
        mEnableExternal = FALSE;
        SendDisableMsg();
    }

    *result = TRUE;
    return NOERROR;
}

ECode CBluetoothManagerService::UnbindAndFinish()
{
    if (DBG) {
//        Logger::D(TAG,"UnbindAndFinish(): " + mBluetooth +
//            " mBinding = " + mBinding);
    }

    {
        AutoLock lock(mConnectionLock);
        if (mUnbinding)
            return NOERROR;

        mUnbinding = TRUE;
        if (mBluetooth != NULL) {
            if (!mConnection->IsGetNameAddressOnly()) {
                //Unregister callback object
                mBluetooth->UnregisterCallback(mBluetoothCallback);
            }
            if (DBG) Logger::D(TAG, "Sending unbind request.");
            mBluetooth = NULL;
            //Unbind
            mContext->UnbindService(mConnection);
            mUnbinding = FALSE;
            mBinding = FALSE;
        } else {
            mUnbinding=FALSE;
        }
    }
    return NOERROR;
}

void CBluetoothManagerService::SendBluetoothStateCallback(
    /* [in] */ Boolean isUp)
{
    Int32 n;
    mStateChangeCallbacks->BeginBroadcast(&n);
    if (DBG) Logger::D(TAG,"Broadcasting onBluetoothStateChange("+isUp+") to " + n + " receivers.");
    for (Int32 i = 0; i < n; i++) {
         mStateChangeCallbacks->GetBroadcastItem(i).onBluetoothStateChange(isUp);
    }
    mStateChangeCallbacks->FinishBroadcast();
}

/**
 * Inform BluetoothAdapter instances that Adapter service is up
 */
void CBluetoothManagerService::SendBluetoothServiceUpCallback()
{
    if (!mConnection->IsGetNameAddressOnly()) {
        if (DBG) Logger::D(TAG,"Calling onBluetoothServiceUp callbacks");
        Int32 n;
        mCallbacks->BeginBroadcast(&n);
        Logger::D(TAG,"Broadcasting onBluetoothServiceUp() to " + n + " receivers.");
        for (Int32 i = 0; i < n; i++) {
            mCallbacks->GetBroadcastItem(i).onBluetoothServiceUp(mBluetooth);
        }
        mCallbacks->FinishBroadcast();
    }
}
/**
 * Inform BluetoothAdapter instances that Adapter service is down
 */
void CBluetoothManagerService::SendBluetoothServiceDownCallback()
{
    if (!mConnection->IsGetNameAddressOnly()) {
        if (DBG) Logger::D(TAG,"Calling onBluetoothServiceDown callbacks");
        Int32 n;
        mCallbacks->BeginBroadcast(&n);
        Logger::D(TAG,"Broadcasting onBluetoothServiceDown() to " + n + " receivers.");
        for (Int32 i = 0; i < n; i++) {
             mCallbacks->GetBroadcastItem(i).onBluetoothServiceDown();
        }
        mCallbacks->FinishBroadcast();
    }
}
ECode CBluetoothManagerService::GetAddress(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        BLUETOOTH_PERM, String("Need BLUETOOTH permission")));

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
            (!CheckIfCallerIsForegroundUser())) {
        Logger::W(TAG,"getAddress(): not allowed for non-active user");
        return NOERROR;
    }

    {
        AutoLock lock(mConnectionLock);
        if (mBluetooth != NULL) {
             return mBluetooth->GetAddress(result);
        }
    }
    // mAddress is accessed from outside.
    // It is alright without a lock. Here, bluetooth is off, no other thread is
    // changing mAddress
    *result = mAddress;
    return NOERROR;
}

ECode CBluetoothManagerService::GetName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        BLUETOOTH_PERM, String("Need BLUETOOTH permission")));

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
            (!CheckIfCallerIsForegroundUser())) {
        Logger::W(TAG,"getName(): not allowed for non-active user");
        return NOERROR;
    }

    {
        AutoLock lock(mConnectionLock);
        if (mBluetooth != NULL) {
            return mBluetooth->GetName(result);
        }
    }
    // mName is accessed from outside.
    // It alright without a lock. Here, bluetooth is off, no other thread is
    // changing mName
    *result = mName;
    return NOERROR;
}

void CBluetoothManagerService::HandleEnable(
    /* [in] */ Boolean quietMode)
{
    mQuietEnable = quietMode;
    {
        AutoLock lock(mConnectionLock);
        if ((mBluetooth == NULL) && (!mBinding)) {
            //Start bind timeout and bind
            mHandler->SendMessageDelayed(MESSAGE_TIMEOUT_BIND, 0, 0, NULL, TIMEOUT_BIND_MS);
            mConnection->SetGetNameAddressOnly(FALSE);
            AutoPtr<IIntent> i;
            CIntent::New(IBluetooth.class.getName(), (IIntent**)&i);
            if (!mContext->BindService(i, mConnection, IContext::BIND_AUTO_CREATE,
                                      UserHandle::USER_CURRENT)) {
                mHandler->RemoveMessages(MESSAGE_TIMEOUT_BIND);
                Logger::E(TAG, "Fail to bind to: " + IBluetooth.class.getName());
            } else {
                mBinding = TRUE;
            }
        } else if (mBluetooth != NULL) {
            if (mConnection->IsGetNameAddressOnly()) {
                // if GetNameAddressOnly is set, we can clear this flag,
                // so the service won't be unbind
                // after name and address are saved
                mConnection->SetGetNameAddressOnly(FALSE);
                //Register callback object
                mBluetooth->RegisterCallback(mBluetoothCallback);
                //Inform BluetoothAdapter instances that service is up
                SendBluetoothServiceUpCallback();
            }
            //Enable bluetooth

            Boolean bol;
            if (!mQuietEnable) {
                mBluetooth->Enable(&bol);
                if(!bol)
                    Logger::E(TAG,"IBluetooth.enable() returned FALSE");
            } else {
                mBluetooth->EnableNoAutoConnect(&bol);
                if(!bol)
                    Logger::E(TAG,"IBluetooth.enableNoAutoConnect() returned FALSE");
            }
        }
    }
}

void CBluetoothManagerService::HandleDisable()
{
    {
        AutoLock lock(mConnectionLock);
        // don't need to disable if GetNameAddressOnly is set,
        // service will be unbinded after Name and Address are saved
        if ((mBluetooth != NULL) && (!mConnection->IsGetNameAddressOnly())) {
            if (DBG)
                Logger::D(TAG,"Sending off request.");
            Boolean bol;
            mBluetooth->Disable(&bol);
            if(!bol)
                Logger::E(TAG,"IBluetooth.disable() returned FALSE");
        }
    }
}

Boolean CBluetoothManagerService::CheckIfCallerIsForegroundUser()
{
    Int32 foregroundUser;
    Int32 callingUser = UserHandle::GetCallingUserId();
    Int32 callingUid = Binder::GetCallingUid();
    Int64 callingIdentity = Binder::ClearCallingIdentity();
    Int32 callingAppId = UserHandle::GetAppId(callingUid);
    Boolean valid = FALSE;
//    try {
        foregroundUser = ActivityManager::GetCurrentUser();
        valid = (callingUser == foregroundUser) ||
                callingAppId == IProcess::NFC_UID;
        if (DBG) {
//            Logger::D(TAG, "checkIfCallerIsForegroundUser: valid=" + valid
//                + " callingUser=" + callingUser
//                + " foregroundUser=" + foregroundUser);
        }
//    } finally {
    Binder::RestoreCallingIdentity(callingIdentity);
//    }
    return valid;
}

void CBluetoothManagerService::BluetoothStateChangeHandler(
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState)
{
    if (prevState != newState) {
        //Notify all proxy objects first of adapter state change
        if (newState == BluetoothAdapter::STATE_ON || newState == BluetoothAdapter::STATE_OFF) {
            Boolean isUp = (newState==BluetoothAdapter::STATE_ON);
            SendBluetoothStateCallback(isUp);

            //If Bluetooth is off, send service down event to proxy objects, and unbind
            if (!isUp && CanUnbindBluetoothService()) {
                SendBluetoothServiceDownCallback();
                UnbindAndFinish();
            }
        }

        //Send broadcast message to everyone else
        AutoPtr<IIntent> intent;
        CIntent::New(BluetoothAdapter::ACTION_STATE_CHANGED, (IIntent**)&intent);
        intent->PutExtra(BluetoothAdapter::EXTRA_PREVIOUS_STATE, prevState);
        intent->PutExtra(BluetoothAdapter::EXTRA_STATE, newState);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        if (DBG)
            Logger::D(TAG,"Bluetooth State Change Intent: " + prevState + " -> " + newState);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL, BLUETOOTH_PERM);
    }
}

/**
 *  if on is TRUE, wait for state become ON
 *  if off is TRUE, wait for state become OFF
 *  if both on and off are FALSE, wait for state not ON
 */
Boolean CBluetoothManagerService::WaitForOnOff(
    /* [in] */ Boolean on,
    /* [in] */ Boolean off)
{
    Int32 i = 0;
    while (i < 10) {
        {
            AutoLock lock(mConnectionLock);
                if (mBluetooth == NULL)
                    break;
                Int32 val;
                mBluetooth->GetState(&val);
                if (on) {
                    if (val == BluetoothAdapter::STATE_ON) return TRUE;
                } else if (off) {
                    if (val == BluetoothAdapter::STATE_OFF) return TRUE;
                } else {
                    if (val != BluetoothAdapter::STATE_ON) return TRUE;
                }
        }

        if (on || off) {
            SystemClock::Sleep(300);
        } else {
            SystemClock::Sleep(50);
        }
        i++;
    }
    Logger::E(TAG,"WaitForOnOff time out");
    return FALSE;
}

void CBluetoothManagerService::SendDisableMsg()
{
    mHandler->SendMessage(mHandler.obtainMessage(MESSAGE_DISABLE));
}

void CBluetoothManagerService::SendEnableMsg(
    /* [in] */ Boolean quietMode)
{
    mHandler->SendMessage(mHandler.obtainMessage(MESSAGE_ENABLE,
                         quietMode ? 1 : 0, 0));
}

Boolean CBluetoothManagerService::CanUnbindBluetoothService()
{
    assert(0 && "TODO");
    // synchronized(mConnection) {
    //     //Only unbind with mEnable flag not set
    //     //For race condition: disable and enable back-to-back
    //     //Avoid unbind right after enable due to callback from disable
    //     //Only unbind with Bluetooth at OFF state
    //     //Only unbind without any MESSAGE_BLUETOOTH_STATE_CHANGE message
    //     try {
    //         if (mEnable || (mBluetooth == null)) return false;
    //         if (mHandler.hasMessages(MESSAGE_BLUETOOTH_STATE_CHANGE)) return false;
    //         return (mBluetooth.getState() == BluetoothAdapter.STATE_OFF);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, "getState()", e);
    //     }
    // }
    return false;
}


} // namespace Server
} // namespace Droid
} // namespace Elastos
