
#include "elastos/droid/bluetooth/BluetoothHeadsetClient.h"

using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Content::EIID_IServiceConnection;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

//=====================================================================
//    BluetoothHeadsetClient::BluetoothStateChangeCallbackStub
//=====================================================================
CAR_INTERFACE_IMPL_2(BluetoothHeadsetClient::BluetoothStateChangeCallbackStub, Object, IIBluetoothStateChangeCallback, IBinder);

BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub()
{
}

BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::BluetoothStateChangeCallbackStub(
    /* [in] */ IBluetoothHeadsetClient* owner)
{
    mOwner = (BluetoothHeadsetClient*)owner;
}

ECode BluetoothHeadsetClient::BluetoothStateChangeCallbackStub::OnBluetoothStateChange(
    /* [in] */ Boolean up)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "onBluetoothStateChange: up=" + up);
    // if (!up) {
    //     if (VDBG) Log.d(TAG,"Unbinding service...");
    //     synchronized (mConnection) {
    //         try {
    //             mService = null;
    //             mContext.unbindService(mConnection);
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // } else {
    //     synchronized (mConnection) {
    //         try {
    //             if (mService == null) {
    //                 if (VDBG) Log.d(TAG,"Binding service...");
    //                 Intent intent = new Intent(IBluetoothHeadsetClient.class.getName());
    //                 doBind();
    //             }
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//            BluetoothHeadsetClient::InnerServiceConnection
//=====================================================================
CAR_INTERFACE_IMPL(BluetoothHeadsetClient::InnerServiceConnection, Object, IServiceConnection);

BluetoothHeadsetClient::InnerServiceConnection::InnerServiceConnection(
    /* [in] */ BluetoothHeadsetClient* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode BluetoothHeadsetClient::InnerServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* className,
    /* [in] */ IBinder* service)
{
    VALIDATE_NOT_NULL(className);
    VALIDATE_NOT_NULL(service);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object connected");
    // mService = IBluetoothHeadsetClient.Stub.asInterface(service);
    //
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceConnected(BluetoothProfile.HEADSET_CLIENT,
    //             BluetoothHeadsetClient.this);
    // }
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::InnerServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* className)
{
    VALIDATE_NOT_NULL(className);
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "Proxy object disconnected");
    // mService = null;
    // if (mServiceListener != null) {
    //     mServiceListener.onServiceDisconnected(BluetoothProfile.HEADSET_CLIENT);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                        BluetoothHeadsetClient
//=====================================================================
const String BluetoothHeadsetClient::TAG("BluetoothHeadsetClient");
const Boolean BluetoothHeadsetClient::DBG = true;
const Boolean BluetoothHeadsetClient::VDBG = false;


CAR_INTERFACE_IMPL_2(BluetoothHeadsetClient, Object, IBluetoothHeadsetClient, IBluetoothProfile);

BluetoothHeadsetClient::BluetoothHeadsetClient()
{
}

BluetoothHeadsetClient::BluetoothHeadsetClient(
    /* [in] */ IContext* context,
    /* [in] */ IBluetoothProfileServiceListener* l)
{
    // ==================before translated======================
    // mContext = context;
    // mServiceListener = l;
    // mAdapter = BluetoothAdapter.getDefaultAdapter();
    //
    // IBluetoothManager mgr = mAdapter.getBluetoothManager();
    // if (mgr != null) {
    //     try {
    //         mgr.registerStateChangeCallback(mBluetoothStateChangeCallback);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // doBind();
}

ECode BluetoothHeadsetClient::DoBind(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // Intent intent = new Intent(IBluetoothHeadsetClient.class.getName());
    // ComponentName comp = intent.resolveSystemService(mContext.getPackageManager(), 0);
    // intent.setComponent(comp);
    // if (comp == null || !mContext.bindServiceAsUser(intent, mConnection, 0,
    //          android.os.Process.myUserHandle())) {
    //     Log.e(TAG, "Could not bind to Bluetooth Headset Client Service with " + intent);
    //     return false;
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::Close()
{
    // ==================before translated======================
    // if (VDBG) log("close()");
    //
    // IBluetoothManager mgr = mAdapter.getBluetoothManager();
    // if (mgr != null) {
    //     try {
    //         mgr.unregisterStateChangeCallback(mBluetoothStateChangeCallback);
    //     } catch (Exception e) {
    //         Log.e(TAG,"",e);
    //     }
    // }
    //
    // synchronized (mConnection) {
    //     if (mService != null) {
    //         try {
    //             mService = null;
    //             mContext.unbindService(mConnection);
    //         } catch (Exception re) {
    //             Log.e(TAG,"",re);
    //         }
    //     }
    // }
    // mServiceListener = null;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::Connect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("connect(" + device + ")");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.connect(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::Disconnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("disconnect(" + device + ")");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.disconnect(device);
    //     } catch (RemoteException e) {
    //       Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //       return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetConnectedDevices(
    /* [out] */ IList** devices)
{
    // ==================before translated======================
    // if (VDBG) log("getConnectedDevices()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getConnectedDevices();
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetDevicesMatchingConnectionStates(
    /* [in] */ ArrayOf<Int32>* states,
    /* [out] */ IList** devices)
{
    // ==================before translated======================
    // if (VDBG) log("getDevicesMatchingStates()");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getDevicesMatchingConnectionStates(states);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return new ArrayList<BluetoothDevice>();
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return new ArrayList<BluetoothDevice>();
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetConnectionState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* state)
{
    VALIDATE_NOT_NULL(state);
    // ==================before translated======================
    // if (VDBG) log("getConnectionState(" + device + ")");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.getConnectionState(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return BluetoothProfile.STATE_DISCONNECTED;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return BluetoothProfile.STATE_DISCONNECTED;
    assert(0);
    return 0;
}

ECode BluetoothHeadsetClient::SetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 priority,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("setPriority(" + device + ", " + priority + ")");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     if (priority != BluetoothProfile.PRIORITY_OFF &&
    //             priority != BluetoothProfile.PRIORITY_ON) {
    //       return false;
    //     }
    //     try {
    //         return mService.setPriority(device, priority);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return false;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetPriority(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getPriority(" + device + ")");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.getPriority(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, Log.getStackTraceString(new Throwable()));
    //         return PRIORITY_OFF;
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return PRIORITY_OFF;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::StartVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("startVoiceRecognition()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.startVoiceRecognition(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::StopVoiceRecognition(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("stopVoiceRecognition()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.stopVoiceRecognition(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentCalls(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getCurrentCalls()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.getCurrentCalls(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentAgEvents(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getCurrentCalls()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.getCurrentAgEvents(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return null;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::AcceptCall(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 flag,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("acceptCall()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.acceptCall(device, flag);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::HoldCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("holdCall()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.holdCall(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::RejectCall(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("rejectCall()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.rejectCall(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::TerminateCall(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("terminateCall()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.terminateCall(device, index);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::EnterPrivateMode(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 index,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("enterPrivateMode()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.enterPrivateMode(device, index);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::ExplicitCallTransfer(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("explicitCallTransfer()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.explicitCallTransfer(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::Redial(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("redial()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.redial(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::Dial(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ const String& number,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("dial()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.dial(device, number);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::DialMemory(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Int32 location,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("dialMemory()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.dialMemory(device, location);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::SendDTMF(
    /* [in] */ IBluetoothDevice* device,
    /* [in] */ Byte code,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("sendDTMF()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.sendDTMF(device, code);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetLastVoiceTagNumber(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("getLastVoiceTagNumber()");
    // if (mService != null && isEnabled() &&
    //         isValidDevice(device)) {
    //     try {
    //         return mService.getLastVoiceTagNumber(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG,  Log.getStackTraceString(new Throwable()));
    //     }
    // }
    // if (mService == null) Log.w(TAG, "Proxy not attached to service");
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::AcceptIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("acceptIncomingConnect");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.acceptIncomingConnect(device);
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::RejectIncomingConnect(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (DBG) log("rejectIncomingConnect");
    // if (mService != null) {
    //     try {
    //         return mService.rejectIncomingConnect(device);
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetAudioState(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (VDBG) log("getAudioState");
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getAudioState(device);
    //     } catch (RemoteException e) {Log.e(TAG, e.toString());}
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return BluetoothHeadsetClient.STATE_AUDIO_DISCONNECTED;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::ConnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.connectAudio();
    //     } catch (RemoteException e) {
    //         Log.e(TAG, e.toString());
    //     }
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::DisconnectAudio(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.disconnectAudio();
    //     } catch (RemoteException e) {
    //         Log.e(TAG, e.toString());
    //     }
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode BluetoothHeadsetClient::GetCurrentAgFeatures(
    /* [in] */ IBluetoothDevice* device,
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(device);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (mService != null && isEnabled()) {
    //     try {
    //         return mService.getCurrentAgFeatures(device);
    //     } catch (RemoteException e) {
    //         Log.e(TAG, e.toString());
    //     }
    // } else {
    //     Log.w(TAG, "Proxy not attached to service");
    //     if (DBG) Log.d(TAG, Log.getStackTraceString(new Throwable()));
    // }
    // return null;
    assert(0);
    return NOERROR;
}

Boolean BluetoothHeadsetClient::IsEnabled()
{
    // ==================before translated======================
    // if (mAdapter.getState() == BluetoothAdapter.STATE_ON) return true;
    // return false;
    assert(0);
    return FALSE;
}

Boolean BluetoothHeadsetClient::IsValidDevice(
    /* [in] */ IBluetoothDevice* device)
{
    // ==================before translated======================
    // if (device == null) return false;
    //
    // if (BluetoothAdapter.checkBluetoothAddress(device.getAddress())) return true;
    // return false;
    assert(0);
    return FALSE;
}

void BluetoothHeadsetClient::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Log.d(TAG, msg);
    assert(0);
}

} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
