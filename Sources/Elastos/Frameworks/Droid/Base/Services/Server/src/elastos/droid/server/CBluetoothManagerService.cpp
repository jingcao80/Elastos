#include "elastos/droid/server/CBluetoothManagerService.h"
#include "elastos/droid/server/CBluetoothManagerServiceBluetoothCallback.h"
#include "elastos/droid/server/IoThread.h"
#include <elastos/droid/R.h>
#include <elastos/droid/os/Looper.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/UserHandle.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/provider/Settings.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.Droid.Bluetooth.h>
#include <Elastos.CoreLibrary.Utility.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::R;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::EIID_IIBluetoothCallback;
using Elastos::Droid::Bluetooth::EIID_IIBluetoothManager;
using Elastos::Droid::Bluetooth::IIBluetoothCallback;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const Boolean DBG = TRUE;
const String TAG("CBluetoothManagerService");

const String CBluetoothManagerService::BLUETOOTH_ADMIN_PERM("android.permission.BLUETOOTH_ADMIN");
const String CBluetoothManagerService::BLUETOOTH_PERM("android.permission.BLUETOOTH");
const String CBluetoothManagerService::ACTION_SERVICE_STATE_CHANGED("com.android.bluetooth.btservice.action.STATE_CHANGED");
const String CBluetoothManagerService::EXTRA_ACTION("action");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_ADDR_VALID("bluetooth_addr_valid");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_ADDRESS("bluetooth_address");
const String CBluetoothManagerService::SECURE_SETTINGS_BLUETOOTH_NAME("bluetooth_name");

const Int32 CBluetoothManagerService::TIMEOUT_BIND_MS = 3000; //Maximum msec to wait for a bind
const Int32 CBluetoothManagerService::TIMEOUT_SAVE_MS = 500; //Maximum msec to wait for a save
const Int32 CBluetoothManagerService::SERVICE_RESTART_TIME_MS = 200;
const Int32 CBluetoothManagerService::ERROR_RESTART_TIME_MS = 200;
const Int32 CBluetoothManagerService::USER_SWITCHED_TIME_MS = 200;

const Int32 CBluetoothManagerService::MESSAGE_ENABLE = 1;
const Int32 CBluetoothManagerService::MESSAGE_DISABLE = 2;
const Int32 CBluetoothManagerService::MESSAGE_REGISTER_ADAPTER = 20;
const Int32 CBluetoothManagerService::MESSAGE_UNREGISTER_ADAPTER = 21;
const Int32 CBluetoothManagerService::MESSAGE_REGISTER_Q_ADAPTER = 22;
const Int32 CBluetoothManagerService::MESSAGE_UNREGISTER_Q_ADAPTER = 23;
const Int32 CBluetoothManagerService::MESSAGE_REGISTER_STATE_CHANGE_CALLBACK = 30;
const Int32 CBluetoothManagerService::MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK = 31;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_CONNECTED = 40;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED = 41;
const Int32 CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE = 42;
const Int32 CBluetoothManagerService::MESSAGE_BLUETOOTH_STATE_CHANGE = 60;
const Int32 CBluetoothManagerService::MESSAGE_TIMEOUT_BIND = 100;
const Int32 CBluetoothManagerService::MESSAGE_TIMEOUT_UNBIND = 101;
const Int32 CBluetoothManagerService::MESSAGE_GET_NAME_AND_ADDRESS= 200;
const Int32 CBluetoothManagerService::MESSAGE_SAVE_NAME_AND_ADDRESS= 201;
const Int32 CBluetoothManagerService::MESSAGE_USER_SWITCHED = 300;
const Int32 CBluetoothManagerService::MAX_SAVE_RETRIES = 3;
const Int32 CBluetoothManagerService::MAX_ERROR_RESTART_RETRIES = 6;
const Int32 CBluetoothManagerService::WAIT_NORMAL = 10;
const Int32 CBluetoothManagerService::WAIT_USERSWITCH = 30;

const Int32 CBluetoothManagerService::BLUETOOTH_OFF = 0;
const Int32 CBluetoothManagerService::BLUETOOTH_ON_BLUETOOTH = 1;
const Int32 CBluetoothManagerService::BLUETOOTH_ON_AIRPLANE = 2;

const Int32 CBluetoothManagerService::SERVICE_IBLUETOOTH = 1;
const Int32 CBluetoothManagerService::SERVICE_IBLUETOOTHGATT = 2;
const Int32 CBluetoothManagerService::SERVICE_IBLUETOOTHQ = 3;

//========================================================================
// CBluetoothManagerService::BluetoothServiceConnection
//========================================================================

CAR_INTERFACE_IMPL(CBluetoothManagerService::BluetoothServiceConnection, Object, IServiceConnection)

CBluetoothManagerService::BluetoothServiceConnection::BluetoothServiceConnection(
    /* [in] */ CBluetoothManagerService* host)
    : mHost(host)
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

ECode CBluetoothManagerService::BluetoothServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    String clsName;
    className->GetClassName(&clsName);

    if (DBG) {
        Slogger::D(TAG, "BluetoothServiceConnection: connected to AdapterService %s",
            clsName.string());
    }

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MESSAGE_BLUETOOTH_SERVICE_CONNECTED, (IMessage**)&msg);
    // TBD if (className.getClassName().equals(IBluetooth.class.getName())) {
    if (clsName.Equals("Elastos.Droid.Bluetooth.Btservice.CAdapterService")) {
        msg->SetArg1(SERVICE_IBLUETOOTH);
        // } else if (className.getClassName().equals(IBluetoothGatt.class.getName())) {
    }
    else if (clsName.Equals("Elastos.Droid.Bluetooth.Gatt.CGattService")) {
        msg->SetArg1(SERVICE_IBLUETOOTHGATT);
    }
    else if (clsName.Equals("Elastos.Droid.Bluetooth.Btservice.CQAdapterService")) {
        msg->SetArg1(SERVICE_IBLUETOOTHQ);
        Slogger::E(TAG, "Bluetooth Q service connected: %s", clsName.string());
    }
    else {
        Slogger::E(TAG, "Unknown service connected: %s", clsName.string());
        return NOERROR;
    }

    msg->SetObj(service);
    Boolean bval;
    return mHost->mHandler->SendMessage(msg, &bval);
}

ECode CBluetoothManagerService::BluetoothServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    String clsName;
    className->GetClassName(&clsName);

    // Called if we unexpected disconnected.
    if (DBG) Slogger::D(TAG, "BluetoothServiceConnection: disconnected from AdapterService%s",
        clsName.string());

    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED, (IMessage**)&msg);
    // TBD if (className.getClassName().equals(IBluetooth.class.getName())) {
    if (clsName.Equals("Elastos.Droid.Bluetooth.Btservice.CAdapterService")) {
        msg->SetArg1(SERVICE_IBLUETOOTH);
        // } else if (className.getClassName().equals(IBluetoothGatt.class.getName())) {
    }
    else if (clsName.Equals("Elastos.Droid.Bluetooth.Btservice.CGattService")) {
        msg->SetArg1(SERVICE_IBLUETOOTHGATT);
    }
    else if (clsName.Equals("Elastos.Droid.Bluetooth.Btservice.CQAdapterService")) {
        msg->SetArg1(SERVICE_IBLUETOOTHQ);
    }
    else {
        Slogger::E(TAG, "Unknown service disconnected: %s", clsName.string());
        return NOERROR;
    }

    Boolean bval;
    return mHost->mHandler->SendMessage(msg, &bval);
}

//========================================================================
// CBluetoothManagerService::BluetoothHandler
//========================================================================
CBluetoothManagerService::BluetoothHandler::BluetoothHandler()
{}

ECode CBluetoothManagerService::BluetoothHandler::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ CBluetoothManagerService* host)
{
    mHost = host;
    return Handler::constructor(looper);
}

ECode CBluetoothManagerService::BluetoothHandler::HandleMessage(
    /* [in] */ IMessage * msg)
{
    Int32 what, arg1,  arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    Boolean bval;
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    if (DBG) Slogger::D (TAG, "Message: %d", what);
    switch (what) {
        case CBluetoothManagerService::MESSAGE_GET_NAME_AND_ADDRESS: {
            if (DBG) Slogger::D(TAG,"MESSAGE_GET_NAME_AND_ADDRESS");
            AutoLock lock(mHost->mConnection);
            //Start bind request
            if ((mHost->mBluetooth == NULL) && (!mHost->mBinding)) {
                if (DBG) Slogger::D(TAG, "Binding to service to get name and address");
                mHost->mConnection->SetGetNameAddressOnly(TRUE);
                //Start bind timeout and bind
                AutoPtr<IMessage> bindMsg;
                mHost->mHandler->ObtainMessage(CBluetoothManagerService::MESSAGE_TIMEOUT_BIND, (IMessage**)&bindMsg);
                mHost->mHandler->SendMessageDelayed(bindMsg, TIMEOUT_BIND_MS, &bval);
                AutoPtr<IIntent> i;
                CIntent::New(String("Elastos.Droid.Bluetooth.IIBluetooth"), (IIntent**)&i);

                if (!mHost->DoBind(i, mHost->mConnection,
                    IContext::BIND_AUTO_CREATE | IContext::BIND_IMPORTANT, UserHandle::CURRENT)) {
                        mHost->mHandler->RemoveMessages(MESSAGE_TIMEOUT_BIND);
                }
                else {
                    mHost->mBinding = TRUE;
                }
            }
            else {
                AutoPtr<IMessage> bindMsg;
                mHost->mHandler->ObtainMessage(
                    CBluetoothManagerService::MESSAGE_SAVE_NAME_AND_ADDRESS, (IMessage**)&bindMsg);

                if (mHost->mBluetooth != NULL) {
                    mHost->mHandler->SendMessage(bindMsg, &bval);
                }
                else {
                    // if enable is also called to bind the service
                    // wait for MESSAGE_BLUETOOTH_SERVICE_CONNECTED
                    mHost->mHandler->SendMessageDelayed(bindMsg, TIMEOUT_SAVE_MS, &bval);
                }
            }
        }
        break;

        case CBluetoothManagerService::MESSAGE_SAVE_NAME_AND_ADDRESS: {
            Boolean unbind = FALSE;
            if (DBG) Slogger::D(TAG,"MESSAGE_SAVE_NAME_AND_ADDRESS");
            {
                AutoLock lock(mHost->mConnection);
                if (!mHost->mEnable && mHost->mBluetooth != NULL)
                    mHost->mBluetooth->Enable(&bval);
            }
            if (mHost->mBluetooth != NULL)
                mHost->WaitForOnOff(TRUE, FALSE, WAIT_NORMAL);

            {
                AutoLock lock(mHost->mConnection);
                if (mHost->mBluetooth != NULL) {
                    String name, address;
                    mHost->mBluetooth->GetName(&name);
                    mHost->mBluetooth->GetAddress(&address);

                    if (name != NULL && address != NULL) {
                        mHost->StoreNameAndAddress(name, address);
                        if (mHost->mConnection->IsGetNameAddressOnly()) {
                            unbind = TRUE;
                        }
                    }
                    else {
                        if (arg1 < MAX_SAVE_RETRIES) {
                            //if (DBG) Slogger::D(TAG,"Retrying name/address remote retrieval and save.....Retry count =" + retryMsg.arg1);
                            AutoPtr<IMessage> retryMsg;
                            mHost->mHandler->ObtainMessage(
                                CBluetoothManagerService::MESSAGE_SAVE_NAME_AND_ADDRESS, (IMessage**)&retryMsg);
                            retryMsg->SetArg1(1 + arg1);
                            mHost->mHandler->SendMessageDelayed(
                                retryMsg, CBluetoothManagerService::TIMEOUT_SAVE_MS, &bval);
                        }
                        else {
                            Slogger::W(TAG,"Maximum name/address remote retrieval retry exceeded");
                            if (mHost->mConnection->IsGetNameAddressOnly())
                                unbind = TRUE;
                        }
                    }

                    if (!mHost->mEnable)
                        mHost->mBluetooth->Disable(&bval);
                }
                else {
                    // rebind service by Request GET NAME AND ADDRESS
                    // if service is unbinded by disable or
                    // MESSAGE_BLUETOOTH_SERVICE_CONNECTED is not received
                    AutoPtr<IMessage> bindMsg;
                    mHost->mHandler->ObtainMessage(
                        CBluetoothManagerService::MESSAGE_GET_NAME_AND_ADDRESS, (IMessage**)&bindMsg);
                    mHost->mHandler->SendMessage(bindMsg, &bval);
                }
            }
            if (!mHost->mEnable && mHost->mBluetooth != NULL) {
                mHost->WaitForOnOff(FALSE, TRUE, WAIT_NORMAL);
            }
            if (unbind) {
                mHost->UnbindAndFinish();
            }
            break;
        }

        case CBluetoothManagerService::MESSAGE_ENABLE: {
            if (DBG) {
                Slogger::D(TAG, "MESSAGE_ENABLE: mBluetooth = %s",
                    TO_CSTR(mHost->mBluetooth));
            }
            mHost->mHandler->RemoveMessages(CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE);
            mHost->mEnable = TRUE;
            mHost->HandleEnable(arg1 == 1);
            break;
        }

        case CBluetoothManagerService::MESSAGE_DISABLE:
            mHost->mHandler->RemoveMessages(CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE);
            if (mHost->mEnable && mHost->mBluetooth != NULL) {
                mHost->WaitForOnOff(TRUE, FALSE, WAIT_NORMAL);
                mHost->mEnable = FALSE;
                mHost->HandleDisable();
                mHost->WaitForOnOff(FALSE, FALSE, WAIT_NORMAL);
            }
            else {
                mHost->mEnable = FALSE;
                mHost->HandleDisable();
            }
            break;

        case CBluetoothManagerService::MESSAGE_REGISTER_ADAPTER: {
            IIBluetoothManagerCallback* cb = IIBluetoothManagerCallback::Probe(obj);
            Boolean added;
            mHost->mCallbacks->Register(cb, &added);
            Slogger::D(TAG,"Added callback: %s: %d", TO_CSTR(cb), added);
            break;
        }

        case CBluetoothManagerService::MESSAGE_REGISTER_Q_ADAPTER: {
            IIBluetoothManagerCallback* cb = IIBluetoothManagerCallback::Probe(obj);
            Boolean added = FALSE;
            mHost->mQCallbacks->Register(cb, &added);
            Slogger::D(TAG,"Q Added callback: %s: %d", TO_CSTR(cb), added);
            break;
        }

        case CBluetoothManagerService::MESSAGE_UNREGISTER_ADAPTER: {
            IIBluetoothManagerCallback* cb = IIBluetoothManagerCallback::Probe(obj);
            Boolean removed;
            mHost->mCallbacks->Unregister(cb, &removed);
            Slogger::D(TAG,"Removed callback: %s: %d", TO_CSTR(cb), removed );
            break;
        }

        case CBluetoothManagerService::MESSAGE_UNREGISTER_Q_ADAPTER: {
            IIBluetoothManagerCallback* cb = IIBluetoothManagerCallback::Probe(obj);
            Boolean removed;
            mHost->mQCallbacks->Unregister(cb, &removed);
            Slogger::D(TAG,"Q Removed callback: %s: %d", TO_CSTR(cb), removed);
            break;
        }

        case CBluetoothManagerService::MESSAGE_REGISTER_STATE_CHANGE_CALLBACK: {
            IIBluetoothStateChangeCallback* cb = IIBluetoothStateChangeCallback::Probe(obj);
            if (cb != NULL) {
                mHost->mStateChangeCallbacks->Register(cb, &bval);
            }
            break;
        }

        case CBluetoothManagerService::MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK: {
            IIBluetoothStateChangeCallback* cb = IIBluetoothStateChangeCallback::Probe(obj);
            if (cb != NULL) {
                mHost->mStateChangeCallbacks->Unregister(cb, &bval);
            }
            break;
        }

        case CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_CONNECTED: {
            if (DBG) Slogger::D(TAG,"MESSAGE_BLUETOOTH_SERVICE_CONNECTED %d", arg1);

            mHost->mIsBluetoothServiceConnected = TRUE;

            AutoPtr<IBinder> service = IBinder::Probe(obj);
            {
                AutoLock lock(mHost->mConnection);

                if (arg1 == CBluetoothManagerService::SERVICE_IBLUETOOTHGATT) {
                    mHost->mBluetoothGatt = IIBluetoothGatt::Probe(service);
                    break;
                } // else must be SERVICE_IBLUETOOTH

                if (arg1 == CBluetoothManagerService::SERVICE_IBLUETOOTHQ) {
                    assert(0 && "TODO");
                    // mHost->mQBluetooth = IIQBluetoothGatt::Probe(service);
                    Slogger::D(TAG,"mQBluetooth: %p", mHost->mQBluetooth.Get());
                    mHost->SendQBluetoothServiceUpCallback();
                    break;
                } // else must be SERVICE_IBLUETOOTH

                //Remove timeout
                mHandler->RemoveMessages(CBluetoothManagerService::MESSAGE_TIMEOUT_BIND);

                mHost->mBinding = FALSE;
                mHost->mBluetooth = IIBluetooth::Probe(service);

                Int32 ival;
                Settings::Secure::GetInt32(mHost->mContentResolver, ISettingsSecure::BLUETOOTH_HCI_LOG, 0, &ival);
                Boolean enableHciSnoopLog = ival == 1;
                ECode ec = mHost->mBluetooth->ConfigHciSnoopLog(enableHciSnoopLog, &bval);
                if (!bval) {
                    Slogger::E(TAG,"IBluetooth.configHciSnoopLog return FALSE");
                }
                if (ec == (ECode)E_REMOTE_EXCEPTION) {
                    Slogger::E(TAG,"Unable to call configHciSnoopLog");
                }

                if (mHost->mConnection->IsGetNameAddressOnly()) {
                    //Request GET NAME AND ADDRESS
                    AutoPtr<IMessage> restartMsg;
                    mHost->mHandler->ObtainMessage(
                        CBluetoothManagerService::MESSAGE_GET_NAME_AND_ADDRESS, (IMessage**)&restartMsg);

                    mHost->mHandler->SendMessage(restartMsg, &bval);
                    if (!mHost->mEnable) return NOERROR;
                }

                mHost->mConnection->SetGetNameAddressOnly(FALSE);
                //Register callback object
                mHost->mBluetooth->RegisterCallback(mHost->mBluetoothCallback);
                //Inform BluetoothAdapter instances that service is up
                mHost->SendBluetoothServiceUpCallback();

                //Do enable request
                if (mHost->mQuietEnable == FALSE) {
                    if(!mHost->mBluetooth->Enable(&bval))
                        Slogger::E(TAG,"IBluetooth.enable() returned FALSE");
                }
                else {
                    mHost->mBluetooth->EnableNoAutoConnect(&bval);
                    if(!bval)
                        Slogger::E(TAG,"IBluetooth.enableNoAutoConnect() returned FALSE");
                }
            }

            if (!mHost->mEnable) {
                mHost->WaitForOnOff(TRUE, FALSE, WAIT_NORMAL);
                mHost->HandleDisable();
                mHost->WaitForOnOff(FALSE, FALSE, WAIT_NORMAL);
            }
            break;
        }

        case CBluetoothManagerService::MESSAGE_TIMEOUT_BIND: {
                Slogger::E(TAG, "MESSAGE_TIMEOUT_BIND");
                AutoLock lock(mHost->mConnection);
                mHost->mBinding = FALSE;
            }
            break;

        case CBluetoothManagerService::MESSAGE_BLUETOOTH_STATE_CHANGE: {
            Int32 prevState = arg1;
            Int32 newState = arg2;
            if (DBG) Slogger::D(TAG, "MESSAGE_BLUETOOTH_STATE_CHANGE: prevState = %d, newState=%d",
                prevState, newState);
            mHost->mState = newState;
            mHost->BluetoothStateChangeHandler(prevState, newState);

            // handle error state transition case from TURNING_ON to OFF
            // unbind and rebind bluetooth service and enable bluetooth
            if ((prevState == IBluetoothAdapter::STATE_TURNING_ON) &&
                (newState == IBluetoothAdapter::STATE_OFF) &&
                (mHost->mBluetooth != NULL) && mHost->mEnable) {
                mHost->RecoverBluetoothServiceFromError();
            }
            if (newState == IBluetoothAdapter::STATE_ON) {
                // bluetooth is working, reset the counter
                if (mHost->mErrorRecoveryRetryCounter != 0) {
                    Slogger::W(TAG, "bluetooth is recovered from error");
                    mHost->mErrorRecoveryRetryCounter = 0;
                }
            }
            break;
        }

        case CBluetoothManagerService::MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED:
            Slogger::E(TAG, "MESSAGE_BLUETOOTH_SERVICE_DISCONNECTED %d", arg1);

            mHost->mIsBluetoothServiceConnected = FALSE;

            {
                AutoLock lock(mHost->mConnection);
                if (arg1 == SERVICE_IBLUETOOTH) {
                    // if service is unbinded already, do nothing and return
                    if (mHost->mBluetooth == NULL) break;
                    mHost->mBluetooth = NULL;
                }
                else if (arg1 == SERVICE_IBLUETOOTHGATT) {
                    mHost->mBluetoothGatt = NULL;
                    break;
                }
                else if (arg1 == SERVICE_IBLUETOOTHQ) {
                    mHost->mQBluetooth = NULL;
                    break;
                }
                else {
                    Slogger::E(TAG, "Bad msg.arg1: %d", arg1);
                    break;
                }
            }

            if (mHost->mEnable) {
                mHost->mEnable = FALSE;
                // Send a Bluetooth Restart message
                AutoPtr<IMessage> restartMsg;
                mHost->mHandler->ObtainMessage(
                    CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE, (IMessage**)&restartMsg);
                mHost->mHandler->SendMessageDelayed(
                    restartMsg, CBluetoothManagerService::SERVICE_RESTART_TIME_MS, &bval);
            }

            if (!mHost->mConnection->IsGetNameAddressOnly()) {
                mHost->SendBluetoothServiceDownCallback();
                mHost->SendQBluetoothServiceDownCallback();

                // Send BT state broadcast to update
                // the BT icon correctly
                if ((mHost->mState == IBluetoothAdapter::STATE_TURNING_ON) ||
                    (mHost->mState == IBluetoothAdapter::STATE_ON)) {
                    mHost->BluetoothStateChangeHandler(
                        IBluetoothAdapter::STATE_ON, IBluetoothAdapter::STATE_TURNING_OFF);
                    mHost->mState = IBluetoothAdapter::STATE_TURNING_OFF;
                }
                if (mHost->mState == IBluetoothAdapter::STATE_TURNING_OFF) {
                    mHost->BluetoothStateChangeHandler(
                        IBluetoothAdapter::STATE_TURNING_OFF, IBluetoothAdapter::STATE_OFF);
                }

                mHost->mHandler->RemoveMessages(MESSAGE_BLUETOOTH_STATE_CHANGE);

                mHost->BluetoothStateChangeHandler(
                    IBluetoothAdapter::STATE_ON, IBluetoothAdapter::STATE_TURNING_OFF);
                mHost->mState = IBluetoothAdapter::STATE_OFF;
            }
            break;

        case CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE:
            Slogger::D(TAG, "MESSAGE_RESTART_BLUETOOTH_SERVICE: Restart IBluetooth service");
            /* Enable without persisting the setting as
             it doesnt change when IBluetooth
             service restarts */
            mHost->mEnable = TRUE;
            mHost->HandleEnable(mHost->mQuietEnable);
            break;

        case CBluetoothManagerService::MESSAGE_TIMEOUT_UNBIND:
            Slogger::E(TAG, "MESSAGE_TIMEOUT_UNBIND");
            {
                AutoLock lock(mHost->mConnection);
                mHost->mUnbinding = FALSE;
            }
            break;

        case CBluetoothManagerService::MESSAGE_USER_SWITCHED: {
            if (DBG) {
                Slogger::D(TAG, "MESSAGE_USER_SWITCHED");
            }
            mHandler->RemoveMessages(MESSAGE_USER_SWITCHED);
            /* disable and enable BT when detect a user switch */
            if (mHost->mEnable && mHost->mBluetooth != NULL) {
                {
                    AutoLock lock(mHost->mConnection);
                    if (mHost->mBluetooth != NULL) {
                        //Unregister callback object
                        mHost->mBluetooth->UnregisterCallback(mHost->mBluetoothCallback);
                    }
                }

                if (mHost->mState == IBluetoothAdapter::STATE_TURNING_OFF) {
                    // MESSAGE_USER_SWITCHED happened right after MESSAGE_ENABLE
                    mHost->BluetoothStateChangeHandler(mHost->mState, IBluetoothAdapter::STATE_OFF);
                    mHost->mState = IBluetoothAdapter::STATE_OFF;
                }
                if (mHost->mState == IBluetoothAdapter::STATE_OFF) {
                    mHost->BluetoothStateChangeHandler(mHost->mState, IBluetoothAdapter::STATE_TURNING_ON);
                    mHost->mState = IBluetoothAdapter::STATE_TURNING_ON;
                }

                mHost->WaitForOnOff(TRUE, FALSE, WAIT_USERSWITCH);

                if (mHost->mState == IBluetoothAdapter::STATE_TURNING_ON) {
                    mHost->BluetoothStateChangeHandler(mHost->mState, IBluetoothAdapter::STATE_ON);
                }

                // disable
                mHost->HandleDisable();

                // Pbap service need receive STATE_TURNING_OFF intent to close
                mHost->BluetoothStateChangeHandler(
                    IBluetoothAdapter::STATE_ON, IBluetoothAdapter::STATE_TURNING_OFF);

                mHost->WaitForOnOff(FALSE, TRUE, WAIT_USERSWITCH);

                mHost->BluetoothStateChangeHandler(
                    IBluetoothAdapter::STATE_TURNING_OFF, IBluetoothAdapter::STATE_OFF);

                mHost->SendBluetoothServiceDownCallback();
                mHost->SendQBluetoothServiceDownCallback();
                {
                    AutoLock lock(mHost->mConnection);
                    if (mHost->mBluetooth != NULL) {
                        mHost->mBluetooth = NULL;
                        //Unbind
                        mHost->mContext->UnbindService(mHost->mConnection);
                    }
                }
                SystemClock::Sleep(100);

                // enable
                mHandler->RemoveMessages(CBluetoothManagerService::MESSAGE_BLUETOOTH_STATE_CHANGE);
                mHost->mState = IBluetoothAdapter::STATE_OFF;
                // enable
                mHost->HandleEnable(mHost->mQuietEnable);
            }
            else if (mHost->mBinding || mHost->mBluetooth != NULL) {
                // if user is switched when service is being binding
                // delay sending MESSAGE_USER_SWITCHED
                AutoPtr<IMessage> userMsg;
                mHost->mHandler->ObtainMessage(
                    CBluetoothManagerService::MESSAGE_RESTART_BLUETOOTH_SERVICE, (IMessage**)&userMsg);
                userMsg->SetArg2(1 + arg2);
                mHost->mHandler->SendMessageDelayed(
                    userMsg, CBluetoothManagerService::USER_SWITCHED_TIME_MS, &bval);
                if (DBG)
                    Slogger::D(TAG, "delay MESSAGE_USER_SWITCHED %d", 1 + arg2);
            }
            break;
        }
    }

    return NOERROR;
}

//========================================================================
// CBluetoothManagerService::BluetoothCallback
//========================================================================
CAR_INTERFACE_IMPL_2(CBluetoothManagerService::BluetoothCallback, Object, IIBluetoothCallback, IBinder)

CBluetoothManagerService::BluetoothCallback::BluetoothCallback()
{}

ECode CBluetoothManagerService::BluetoothCallback::constructor(
    /* [in] */ IIBluetoothManager* bm)
{
    mHost = (CBluetoothManagerService*)bm;
    return NOERROR;
}

ECode CBluetoothManagerService::BluetoothCallback::OnBluetoothStateChange(
    /* [in] */ Int32 prevState,
    /* [in] */ Int32 newState)
{
    AutoPtr<IMessage> msg;
    mHost->mHandler->ObtainMessage(
        CBluetoothManagerService::MESSAGE_BLUETOOTH_STATE_CHANGE, prevState, newState, (IMessage**)&msg);
    Boolean bval;
    mHost->mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::BluetoothCallback::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//========================================================================
// CBluetoothManagerService::MyBroadcastReceiver
//========================================================================
CBluetoothManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CBluetoothManagerService* host)
    : mHost(host)
{ }

ECode CBluetoothManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (IBluetoothAdapter::ACTION_LOCAL_NAME_CHANGED.Equals(action)) {
        String newName;
        intent->GetStringExtra(IBluetoothAdapter::EXTRA_LOCAL_NAME, &newName);
        if (DBG) Slogger::D(TAG, "Bluetooth Adapter name changed to %s", newName.string());
        if (newName != NULL) {
            mHost->StoreNameAndAddress(newName, String(NULL));
        }
    }
    else if (IIntent::ACTION_AIRPLANE_MODE_CHANGED.Equals(action)) {
        AutoLock lock(mHost->mReceiver);
        if (mHost->IsBluetoothPersistedStateOn()) {
            if (mHost->IsAirplaneModeOn()) {
                mHost->PersistBluetoothSetting(BLUETOOTH_ON_AIRPLANE);
            }
            else {
                mHost->PersistBluetoothSetting(BLUETOOTH_ON_BLUETOOTH);
            }
        }
        if (mHost->IsAirplaneModeOn()) {
            // disable without persisting the setting
            mHost->SendDisableMsg();
        }
        else if (mHost->mEnableExternal) {
            // enable without persisting the setting
            mHost->SendEnableMsg(mHost->mQuietEnableExternal);
        }
    }
    else if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 num;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &num);
        AutoPtr<IMessage> msg;
        mHost->mHandler->ObtainMessage(
            MESSAGE_BLUETOOTH_STATE_CHANGE, num, 0, (IMessage**)&msg);
        Boolean bval;
        mHost->mHandler->SendMessage(msg, &bval);
    }
    else if (IIntent::ACTION_BOOT_COMPLETED.Equals(action)) {
        {
            AutoLock lock(mHost->mReceiver);
            if (mHost->mEnableExternal && mHost->IsBluetoothPersistedStateOnBluetooth()) {
                //Enable
                if (DBG) Slogger::D(TAG, "Auto-enabling Bluetooth.");
                mHost->SendEnableMsg(mHost->mQuietEnableExternal);
            }
        }

        if (!mHost->IsNameAndAddressSet()) {
            //Sync the Bluetooth name and address from the Bluetooth Adapter
            if (DBG) Slogger::D(TAG,"Retrieving Bluetooth Adapter name and address...");
            mHost->GetNameAndAddress();
        }
    }
    return NOERROR;
}

//====================================================================
// CBluetoothManagerService
//====================================================================
CAR_INTERFACE_IMPL_2(CBluetoothManagerService, Object, IIBluetoothManager, IBinder)

CAR_OBJECT_IMPL(CBluetoothManagerService)

CBluetoothManagerService::CBluetoothManagerService()
    : mBinding(FALSE)
    , mUnbinding(FALSE)
    , mQuietEnable(FALSE)
    , mQuietEnableExternal(FALSE)
    , mEnableExternal(FALSE)
    , mEnable(FALSE)
    , mState(0)
    , mErrorRecoveryRetryCounter(0)
    , mSystemUiUid(0)
    , mIsBluetoothServiceConnected(FALSE)
{}

ECode CBluetoothManagerService::constructor(
    /* [in] */ IContext* context)
{
    mConnection = new BluetoothServiceConnection(this);

    CBluetoothManagerServiceBluetoothCallback::New(this, (IIBluetoothCallback**)&mBluetoothCallback);
    mReceiver = new MyBroadcastReceiver(this);

    AutoPtr<ILooper> looper;
    IoThread::Get()->GetLooper((ILooper**)&looper);
    mHandler = new BluetoothHandler();
    mHandler->constructor(looper, this);

    mContext = context;
    mState = IBluetoothAdapter::STATE_OFF;
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mCallbacks);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mQCallbacks);
    CRemoteCallbackList::New((IRemoteCallbackList**)&mStateChangeCallbacks);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BOOT_COMPLETED, (IIntentFilter**)&filter);
    filter->AddAction(IBluetoothAdapter::ACTION_LOCAL_NAME_CHANGED);
    filter->AddAction(IIntent::ACTION_USER_SWITCHED);
    RegisterForAirplaneMode(filter);
    filter->SetPriority(IIntentFilter::SYSTEM_HIGH_PRIORITY);
    AutoPtr<IIntent> i;
    mContext->RegisterReceiver(mReceiver, filter, (IIntent**)&i);
    LoadStoredNameAndAddress();

    if (IsBluetoothPersistedStateOn()) {
        mEnableExternal = TRUE;
    }

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    Int32 sysUiUid = -1;
    ECode ec = pm->GetPackageUid(String("Elastos.Droid.SystemUI"), UserHandle::USER_OWNER, &sysUiUid);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Slogger::E(TAG, "Unable to resolve SystemUI's UID.");
    }
    mSystemUiUid = sysUiUid;

    return NOERROR;
}

void CBluetoothManagerService::RegisterForAirplaneMode(
    /* [in] */ IIntentFilter* filter)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String airplaneModeRadios;
    Settings::Global::GetString(resolver, ISettingsGlobal::AIRPLANE_MODE_RADIOS, &airplaneModeRadios);
    String toggleableRadios;
    Settings::Global::GetString(resolver, ISettingsGlobal::AIRPLANE_MODE_TOGGLEABLE_RADIOS, &toggleableRadios);
    Boolean mIsAirplaneSensitive = airplaneModeRadios == NULL ? TRUE :
            airplaneModeRadios.Contains(ISettingsGlobal::RADIO_BLUETOOTH);
    if (mIsAirplaneSensitive) {
        filter->AddAction(IIntent::ACTION_AIRPLANE_MODE_CHANGED);
    }
}

Boolean CBluetoothManagerService::IsAirplaneModeOn()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 val;
    Settings::Global::GetInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &val);
    return (val == 1);
}

Boolean CBluetoothManagerService::IsBluetoothPersistedStateOn()
{
    Int32 val;
    Settings::Global::GetInt32(mContentResolver, ISettingsGlobal::BLUETOOTH_ON, 0, &val);
    return (val == BLUETOOTH_OFF);
}

Boolean CBluetoothManagerService::IsBluetoothPersistedStateOnBluetooth()
{
    Int32 val;
    Settings::Global::GetInt32(mContentResolver, ISettingsGlobal::BLUETOOTH_ON, 0, &val);
    return (val == BLUETOOTH_ON_BLUETOOTH);
}

void CBluetoothManagerService::PersistBluetoothSetting(
    /* [in] */ Int32 setOn)
{
    Boolean bval;
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Settings::Global::PutInt32(resolver, ISettingsGlobal::BLUETOOTH_ON, setOn, &bval);
}

Boolean CBluetoothManagerService::IsNameAndAddressSet()
{
    return !mName.IsNullOrEmpty() && !mAddress.IsNullOrEmpty();
}

void CBluetoothManagerService::LoadStoredNameAndAddress()
{
    if (DBG) Slogger::D(TAG, "Loading stored name and address");
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Boolean bval;
    res->GetBoolean(R::bool_::config_bluetooth_address_validation, &bval);
    Int32 val;
    Settings::Secure::GetInt32(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDR_VALID, 0, &val);
    if (bval && (val == 0)) {
        // if the valid flag is not set, don't load the address and name
        if (DBG) Slogger::D(TAG, "invalid bluetooth name and address stored");
        return;
    }
    Settings::Secure::GetString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_NAME, &mName);
    Settings::Secure::GetString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDRESS, &mAddress);
    if (DBG) Slogger::D(TAG, "Stored bluetooth Name=%s, Address=%s",
        mName.string(), mAddress.string());
}

void CBluetoothManagerService::StoreNameAndAddress(
    /* [in] */ const String& name,
    /* [in] */ const String& address)
{
    Boolean bval;
    String str;
    if (name != NULL) {
        Settings::Secure::PutString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_NAME, name, &bval);
        mName = name;
        Settings::Secure::GetString(mContentResolver,SECURE_SETTINGS_BLUETOOTH_NAME, &str);
        if (DBG)
            Slogger::D(TAG,"Stored Bluetooth name: %s", str.string());
    }

    if (address != NULL) {
        Settings::Secure::PutString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDRESS, address, &bval);
        mAddress = address;
        Settings::Secure::GetString(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDRESS, &str);
        if (DBG)  Slogger::D(TAG,"Stored Bluetoothaddress: %s", str.string());
    }

    if ((name != NULL) && (address != NULL)) {
        Settings::Secure::PutInt32(mContentResolver, SECURE_SETTINGS_BLUETOOTH_ADDR_VALID, 1, &bval);
    }
}

ECode CBluetoothManagerService::RegisterAdapter(
    /* [in] */ IIBluetoothManagerCallback* callback,
    /* [out] */ IIBluetooth** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (callback == NULL) {
        Slogger::W(TAG, "Callback is NULL in RegisterAdapter");
        return NOERROR;
    }

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_REGISTER_ADAPTER, (IMessage**)&msg);
    msg->SetObj(callback);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    {
        AutoLock lock(mConnection);
        *result = mBluetooth;
        REFCOUNT_ADD(*result);
    }
    return NOERROR;
}

ECode CBluetoothManagerService::UnregisterAdapter(
    /* [in] */ IIBluetoothManagerCallback* callback)
{
    if (callback == NULL) {
        Slogger::W(TAG, "Callback is NULL in UnregisterAdapter");
        return NOERROR;
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
        String("Need BLUETOOTH permission")));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_UNREGISTER_ADAPTER, (IMessage**)&msg);
    msg->SetObj(callback);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::RegisterStateChangeCallback(
    /* [in] */ IIBluetoothStateChangeCallback* callback)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
        String("Need BLUETOOTH permission")));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_REGISTER_STATE_CHANGE_CALLBACK, 0, 0, callback, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::UnregisterStateChangeCallback(
    /* [in] */ IIBluetoothStateChangeCallback* callback)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
        String("Need BLUETOOTH permission")));
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_UNREGISTER_STATE_CHANGE_CALLBACK, 0, 0, callback, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        !CheckIfCallerIsForegroundUser()) {
        Slogger::W(TAG,"isEnabled(): not allowed for non-active and non system user");
        return NOERROR;
    }

    {
        AutoLock lock(mConnection);
        Boolean bval;
        mBluetooth->IsEnabled(&bval);
        *result = (mBluetooth != NULL && bval);
    }
    return NOERROR;
}

ECode CBluetoothManagerService::GetNameAndAddress()
{
    if (DBG) {
       Slogger::D(TAG, "GetNameAndAddress(): %s mBinding=%d",
            TO_CSTR(mBluetooth), mBinding);
    }
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_GET_NAME_AND_ADDRESS, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::EnableNoAutoConnect(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_ADMIN_PERM,
        String("Need BLUETOOTH ADMIN permission")));

    if (DBG) {
       Slogger::D(TAG, "EnableNoAutoConnect(): %s mBinding=%d",
            TO_CSTR(mBluetooth), mBinding);
    }

    Int32 callingAppId = UserHandle::GetAppId(Binder::GetCallingUid());
    if (callingAppId != IProcess::NFC_UID) {
        // throw new SecurityException("no permission to enable Bluetooth quietly");
        return E_SECURITY_EXCEPTION;
    }

    {
        AutoLock lock(mReceiver);
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
    return NOERROR;
}

ECode CBluetoothManagerService::Disable(
    /* [in] */ Boolean persist,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_ADMIN_PERM,
        String("Need BLUETOOTH ADMIN permissicacheNameAndAddresson")));

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        (!CheckIfCallerIsForegroundUser())) {
        Slogger::W(TAG,"disable(): not allowed for non-active and non system user");
        return NOERROR;
    }

    if (DBG) {
       Slogger::D(TAG,"Disable()(): %s mBinding=%d",
            TO_CSTR(mBluetooth), mBinding);
    }

    {
        AutoLock lock(mReceiver);
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
       Slogger::D(TAG,"UnbindAndFinish(): %s mBinding=%d",
            TO_CSTR(mBluetooth), mBinding);
    }

    {
        AutoLock lock(mConnection);
        if (mUnbinding)
            return NOERROR;

        mUnbinding = TRUE;
        if (mBluetooth != NULL) {
            Boolean bval = mConnection->IsGetNameAddressOnly();
            if (!bval) {
                //Unregister callback object
                mBluetooth->UnregisterCallback(mBluetoothCallback);
            }
            if (DBG) Slogger::D(TAG, "Sending unbind request.");
            mBluetooth = NULL;
            mQBluetooth = NULL;
            //Unbind
            mContext->UnbindService(mConnection);
            mUnbinding = FALSE;
            mBinding = FALSE;
        }
        else {
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
    if (DBG) Slogger::D(TAG,"Broadcasting onBluetoothStateChange(%d) to %d receivers.", isUp, n);
    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> obj;
        mStateChangeCallbacks->GetBroadcastItem(i, (IInterface**)&obj);
        IIBluetoothStateChangeCallback::Probe(obj)->OnBluetoothStateChange(isUp);
    }
    mStateChangeCallbacks->FinishBroadcast();
}

void CBluetoothManagerService::SendBluetoothServiceUpCallback()
{
    if (!mConnection->IsGetNameAddressOnly()) {
        if (DBG) Slogger::D(TAG,"Calling onBluetoothServiceUp callbacks");
        Int32 n;
        mCallbacks->BeginBroadcast(&n);
        Slogger::D(TAG,"Broadcasting onBluetoothServiceUp() to %d receivers.", n);
        for (Int32 i = 0; i < n; i++) {
            AutoPtr<IInterface> obj;
            mCallbacks->GetBroadcastItem(i, (IInterface**)&obj);
            IIBluetoothManagerCallback::Probe(obj)->OnBluetoothServiceUp(mBluetooth);
        }
        mCallbacks->FinishBroadcast();
    }
}

ECode CBluetoothManagerService::GetBluetoothGatt(
    /* [out] */ IIBluetoothGatt** bg)
{
    VALIDATE_NOT_NULL(bg)
    // sync protection
    *bg = mBluetoothGatt;
    REFCOUNT_ADD(*bg)
    return NOERROR;
}

void CBluetoothManagerService::SendBluetoothServiceDownCallback()
{
    Boolean bval = mConnection->IsGetNameAddressOnly();
    if (!bval) {
        if (DBG) Slogger::D(TAG,"Calling onBluetoothServiceDown callbacks");
        Int32 n;
        mCallbacks->BeginBroadcast(&n);
        Slogger::D(TAG,"Broadcasting onBluetoothServiceDown() to %d receivers.", n);
        for (Int32 i = 0; i < n; i++) {
            AutoPtr<IInterface> obj;
            mCallbacks->GetBroadcastItem(i, (IInterface**)&obj);
            IIBluetoothManagerCallback::Probe(obj)->OnBluetoothServiceDown();
        }
        mCallbacks->FinishBroadcast();
    }
}

void CBluetoothManagerService::SendQBluetoothServiceUpCallback()
{
    if (DBG) {
        Slogger::D(TAG,"Calling onQBluetoothServiceUp callbacks");
    }
    Int32 n = 0;
    mQCallbacks->BeginBroadcast(&n);
    Slogger::D(TAG,"Broadcasting onQBluetoothServiceUp() to %d receivers.", n);

    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> p;
        mQCallbacks->GetBroadcastItem(i, (IInterface**)&p);
        IIQBluetoothManagerCallback::Probe(p)->OnQBluetoothServiceUp(mQBluetooth);
    }
    mQCallbacks->FinishBroadcast();
}

void CBluetoothManagerService::SendQBluetoothServiceDownCallback()
{
    if (DBG) {
        Slogger::D(TAG,"Calling onQBluetoothServiceDown callbacks");
    }
    Int32 n = 0;
    mQCallbacks->BeginBroadcast(&n);
    Slogger::D(TAG,"Broadcasting onQBluetoothServiceDown() to %d receivers.", n);

    for (Int32 i = 0; i < n; i++) {
        AutoPtr<IInterface> p;
        mQCallbacks->GetBroadcastItem(i, (IInterface**)&p);
        IIQBluetoothManagerCallback::Probe(p)->OnQBluetoothServiceDown();
    }
    mQCallbacks->FinishBroadcast();
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
        Slogger::W(TAG,"getAddress(): not allowed for non-active user");
        return NOERROR;
    }

    {
        AutoLock lock(mConnection);
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
        Slogger::W(TAG,"getName(): not allowed for non-active user");
        return NOERROR;
    }

    {
        AutoLock lock(mConnection);
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
        AutoLock lock(mConnection);
        if ((mBluetooth == NULL) && (!mBinding)) {
            //Start bind timeout and bind
            AutoPtr<IMessage> msg;
            mHandler->ObtainMessage(MESSAGE_TIMEOUT_BIND, (IMessage**)&msg);
            Boolean bval;
            mHandler->SendMessageDelayed(msg, TIMEOUT_BIND_MS, &bval);
            mConnection->SetGetNameAddressOnly(FALSE);
            AutoPtr<IIntent> i;
            CIntent::New(String("Elastos.Droid.Bluetooth.IIBluetooth"), (IIntent**)&i);
            if (!DoBind(i, mConnection, IContext::BIND_AUTO_CREATE | IContext::BIND_IMPORTANT,
                    UserHandle::CURRENT)) {
                mHandler->RemoveMessages(MESSAGE_TIMEOUT_BIND);
            }
            else {
                mBinding = TRUE;
            }
        }
        else if (mBluetooth != NULL) {
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

            Boolean bval;
            if (!mQuietEnable) {
                mBluetooth->Enable(&bval);
                if(!bval) {
                    Slogger::E(TAG,"IBluetooth.enable() returned FALSE");
                }
            }
            else {
                mBluetooth->EnableNoAutoConnect(&bval);
                if(!bval) {
                    Slogger::E(TAG,"IBluetooth.enableNoAutoConnect() returned FALSE");
                }
            }
        }
    }
}

Boolean CBluetoothManagerService::DoBind(
    /* [in] */ IIntent* intent,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IComponentName> comp;
    intent->ResolveSystemService(pm, 0, (IComponentName**)&comp);
    intent->SetComponent(comp);
    Boolean bval;
    if (comp == NULL || (mContext->BindServiceAsUser(intent, conn, flags, user, &bval), !bval)) {
        Slogger::E(TAG, "Fail to bind to: %s", TO_CSTR(intent));
        return FALSE;
    }
    return TRUE;
}

void CBluetoothManagerService::HandleDisable()
{
    AutoLock lock(mConnection);
    // don't need to disable if GetNameAddressOnly is set,
    // service will be unbinded after Name and Address are saved
    if ((mBluetooth != NULL) && !mConnection->IsGetNameAddressOnly()) {
        if (DBG) {
            Slogger::D(TAG,"Sending off request.");
        }
        Boolean bval;
        mBluetooth->Disable(&bval);
        if(!bval) {
            Slogger::E(TAG,"IBluetooth.disable() returned FALSE");
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
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    helper->GetCurrentUser(&foregroundUser);
    valid = (callingUser == foregroundUser) ||
            callingAppId == IProcess::NFC_UID ||
            callingAppId == mSystemUiUid;
    if (DBG) {
        Slogger::D(TAG, "checkIfCallerIsForegroundUser: valid=%d, callingUser=%d, foregroundUser=%d",
            valid, callingUser, foregroundUser);
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
        if (newState == IBluetoothAdapter::STATE_ON || newState == IBluetoothAdapter::STATE_OFF) {
            Boolean isUp = (newState == IBluetoothAdapter::STATE_ON);
            SendBluetoothStateCallback(isUp);

            if (isUp) {
                AutoPtr<IPackageManager> pm;
                mContext->GetPackageManager((IPackageManager**)&pm);
                // connect to GattService
                Boolean bval;
                if (pm->HasSystemFeature(IPackageManager::FEATURE_BLUETOOTH_LE, &bval), bval) {
                    AutoPtr<IIntent> i;
                    CIntent::New(String("Elastos.Droid.Bluetooth.IIBluetoothGatt"), (IIntent**)&i);
                    DoBind(i, mConnection, IContext::BIND_AUTO_CREATE | IContext::BIND_IMPORTANT,
                        UserHandle::CURRENT);

                    AutoPtr<IIntent> iqc;
                    CIntent::New(String("Elastos.Droid.Bluetooth.IIBluetoothGatt"), (IIntent**)&iqc);
                    DoBind(iqc, mConnection, IContext::BIND_AUTO_CREATE | IContext::BIND_IMPORTANT,
                            UserHandle::CURRENT);
                }
            }
            else {
                //If Bluetooth is off, send service down event to proxy objects, and unbind
                if (!isUp && CanUnbindBluetoothService()) {
                    SendBluetoothServiceDownCallback();
                    SendQBluetoothServiceDownCallback();
                    UnbindAndFinish();
                }
            }
        }

        //Send broadcast message to everyone else
        AutoPtr<IIntent> intent;
        CIntent::New(IBluetoothAdapter::ACTION_STATE_CHANGED, (IIntent**)&intent);
        intent->PutExtra(IBluetoothAdapter::EXTRA_PREVIOUS_STATE, prevState);
        intent->PutExtra(IBluetoothAdapter::EXTRA_STATE, newState);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
        intent->AddFlags(IIntent::FLAG_RECEIVER_FOREGROUND);
        if (DBG) {
            Slogger::D(TAG,"Bluetooth State Change Intent: %d -> %d", prevState, newState);
        }
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL, BLUETOOTH_PERM);
    }
}

Boolean CBluetoothManagerService::WaitForOnOff(
    /* [in] */ Boolean on,
    /* [in] */ Boolean off,
    /* [in] */ Int32 loop)
{
    Int32 i = 0;
    while (i < loop) {
        {
            AutoLock lock(mConnection);
            if (mBluetooth == NULL)
                break;
            Int32 val;
            mBluetooth->GetState(&val);
            if (on) {
                if (val == IBluetoothAdapter::STATE_ON) return TRUE;
            }
            else if (off) {
                if (val == IBluetoothAdapter::STATE_OFF) return TRUE;
            }
            else {
                if (val != IBluetoothAdapter::STATE_ON) return TRUE;
            }
        }

        if (on || off) {
            SystemClock::Sleep(300);
        }
        else {
            SystemClock::Sleep(50);
        }
        i++;
    }
    Slogger::E(TAG,"WaitForOnOff time out");
    return FALSE;
}

void CBluetoothManagerService::SendDisableMsg()
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MESSAGE_DISABLE, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
}

void CBluetoothManagerService::SendEnableMsg(
    /* [in] */ Boolean quietMode)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(
        MESSAGE_ENABLE, quietMode ? 1 : 0, 0, (IMessage**)&msg);
    Boolean bval;
    mHandler->SendMessage(msg, &bval);
}

Boolean CBluetoothManagerService::CanUnbindBluetoothService()
{
    assert(0 && "TODO");
    // {    AutoLock syncLock(mConnection);
    //     //Only unbind with mEnable flag not set
    //     //For race condition: disable and enable back-to-back
    //     //Avoid unbind right after enable due to callback from disable
    //     //Only unbind with Bluetooth at OFF state
    //     //Only unbind without any MESSAGE_BLUETOOTH_STATE_CHANGE message
    //     try {
    //         if (mEnable || (mBluetooth == NULL)) return FALSE;
    //         if (mHandler.hasMessages(MESSAGE_BLUETOOTH_STATE_CHANGE)) return FALSE;
    //         return (mBluetooth->getState() == IBluetoothAdapter::STATE_OFF);
    //     } catch (RemoteException e) {
    //         Slogger::E(TAG, "getState()", e);
    //     }
    // }
    return FALSE;
}

ECode CBluetoothManagerService::RegisterQAdapter(
    /* [in] */ IIQBluetoothManagerCallback* qbmcallback,
    /* [out] */ IIQBluetooth** qBluetooth)
{
    VALIDATE_NOT_NULL(qBluetooth)
    assert(0 && "TODO");
    // *qBluetooth = NULL;

    // if (callback == NULL) {
    //     Slogger::W(TAG, "Callback is NULL in RegisterAdapter");
    //     return NOERROR;
    // }

    // AutoPtr<IMessage> msg;
    // mHandler->ObtainMessage(MESSAGE_REGISTER_Q_ADAPTER, (IMessage**)&msg);
    // msg->SetObj(qbmcallback);
    // Boolean bval;
    // mHandler->SendMessage(msg, &bval);
    // {
    //     AutoLock lock(mConnection);
    //     *qBluetooth = mQBluetooth;
    //     REFCOUNT_ADD(*qBluetooth);
    // }
    return NOERROR;
}


ECode CBluetoothManagerService::UnregisterQAdapter(
    /* [in] */ IIQBluetoothManagerCallback* qbmcallback)
{
    assert(0 && "TODO");
    // FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(BLUETOOTH_PERM,
    //     String("Need BLUETOOTH permission")));
    // AutoPtr<IMessage> msg;
    // mHandler->ObtainMessage(MESSAGE_UNREGISTER_Q_ADAPTER, (IMessage**)&msg);
    // msg->SetObj(qbmcallback);
    // Boolean bval;
    // mHandler->SendMessage(msg, &bval);
    return NOERROR;
}

ECode CBluetoothManagerService::Enable(
    /* [in] */ const String& callingPackage,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if ((Binder::GetCallingUid() != IProcess::SYSTEM_UID) &&
        (!CheckIfCallerIsForegroundUser())) {
        Slogger::W(TAG,"enable(): not allowed for non-active and non system user");
        return NOERROR;
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        BLUETOOTH_ADMIN_PERM,
        String("Need BLUETOOTH ADMIN permission")));

    if (DBG) {
       Slogger::D(TAG, "Enable()(): %s mBinding=%d",
            TO_CSTR(mBluetooth), mBinding);
    }

    AutoPtr<IInterface> serv;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&serv);
    AutoPtr<IAppOpsManager> appOps = IAppOpsManager::Probe(serv);
    Int32 callingUid = Binder::GetCallingUid();
    Int32 op = 0;
    appOps->NoteOp(IAppOpsManager::OP_BLUETOOTH_CHANGE, callingUid,
            callingPackage, &op);
    if (op != IAppOpsManager::MODE_ALLOWED) {
        return NOERROR;
    }

    {
        AutoLock lock(mReceiver);
        mQuietEnableExternal = FALSE;
        mEnableExternal = TRUE;
        // waive WRITE_SECURE_SETTINGS permission check
        Int64 callingIdentity = Binder::ClearCallingIdentity();
        PersistBluetoothSetting(BLUETOOTH_ON_BLUETOOTH);
        Binder::RestoreCallingIdentity(callingIdentity);
        SendEnableMsg(FALSE);
    }

    *result = TRUE;
    return NOERROR;
}

ECode CBluetoothManagerService::GetQBluetooth(
    /* [out] */ IIQBluetooth** qBluetooth)
{
    VALIDATE_NOT_NULL(qBluetooth)
    // sync protection
    *qBluetooth = mQBluetooth;
    REFCOUNT_ADD(*qBluetooth)
    return NOERROR;
}

void CBluetoothManagerService::RecoverBluetoothServiceFromError()
{
    Slogger::E(TAG,"RecoverBluetoothServiceFromError");
    {    AutoLock syncLock(mConnection);
        if (mBluetooth != NULL) {
            //Unregister callback object
            ECode ec = mBluetooth->UnregisterCallback(mBluetoothCallback);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Unable to unregister, ec=%08x", ec);
            }
        }
    }

    SystemClock::Sleep(500);

    // disable
    HandleDisable();

    WaitForOnOff(FALSE, TRUE, WAIT_NORMAL);

    SendBluetoothServiceDownCallback();
    SendQBluetoothServiceDownCallback();
    {    AutoLock syncLock(mConnection);
        if (mBluetooth != NULL) {
            mBluetooth = NULL;
            if(mQBluetooth != NULL) {
                mQBluetooth = NULL;
            }
            //Unbind
            mContext->UnbindService(mConnection);
        }
    }

    mHandler->RemoveMessages(MESSAGE_BLUETOOTH_STATE_CHANGE);
    mState = IBluetoothAdapter::STATE_OFF;

    mEnable = FALSE;

    if (mErrorRecoveryRetryCounter++ < MAX_ERROR_RESTART_RETRIES) {
        // Send a Bluetooth Restart message to reenable bluetooth
        AutoPtr<IMessage> restartMsg;
        mHandler->ObtainMessage(MESSAGE_RESTART_BLUETOOTH_SERVICE, (IMessage**)&restartMsg);
        Boolean bval;
        mHandler->SendMessageDelayed(restartMsg, ERROR_RESTART_TIME_MS, &bval);
    }
    else {
        // todo: notify user to power down and power up phone to make bluetooth work.
    }
}

ECode CBluetoothManagerService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CBluetoothManagerService:");
    sb += "name:";
    sb += mName.string();
    *str = sb.ToString();
    return NOERROR;
}

} // namespace Server
} // namespace Droid
} // namespace Elastos
