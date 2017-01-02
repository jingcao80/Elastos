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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHPHONESERVICE_H__
#define __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHPHONESERVICE_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/Object.h>
#include <elastos/droid/app/Service.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Handler.h>
#include <Elastos.Droid.Bluetooth.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Bluetooth::IIBluetoothHeadsetPhone;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Bluetooth::IBluetoothProfileServiceListener;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Utility::IMap;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class CallsManager;
/**
 * Bluetooth headset manager for Telecom. This class shares the call state with the bluetooth device
 * and accepts call-related commands to perform on behalf of the BT device.
 */
class BluetoothPhoneService
    : public Elastos::Droid::App::Service
    , public IBluetoothPhoneService
{
public:
    class SubIBluetoothHeadsetPhone
        : public Object
        , public IBinder
        , public IIBluetoothHeadsetPhone
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IInterface* host);

        // @Override
        CARAPI AnswerCall(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI HangupCall(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI SendDtmf(
            /* [in] */ Int32 dtmf,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetNetworkOperator(
            /* [out] */ String* result);

        // @Override
        CARAPI GetSubscriberNumber(
            /* [out] */ String* result);

        // @Override
        CARAPI ListCurrentCalls(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI QueryPhoneState(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ProcessChld(
            /* [in] */ Int32 chld,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI UpdateBtHandsfreeAfterRadioTechnologyChange();

        // @Override
        CARAPI CdmaSetSecondCallState(
            /* [in] */ Boolean state);

        // @Override
        CARAPI CdmaSwapSecondCallState();

        CARAPI ToString(
            /* [out] */ String* result);

    private:
        BluetoothPhoneService* mHost;
    };

private:
    /**
     * Request object for performing synchronous requests to the main thread.
     */
    class MainThreadRequest
        : public Object
        , public IBluetoothPhoneServiceMainThreadRequest
    {
    public:
        CAR_INTERFACE_DECL()

        MainThreadRequest(
            /* [in] */ BluetoothPhoneService* host);

        CARAPI constructor(
            /* [in] */ Int32 param);

        CARAPI SetResult(
            /* [in] */ IInterface* value);

    public:
        AutoPtr<IObject> mResult;

        Int32 mParam;

    private:
        BluetoothPhoneService* mHost;
    };

    class SubHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("BluetoothPhoneService::SubHandler")

        SubHandler(
            /* [in] */ BluetoothPhoneService* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        BluetoothPhoneService* mHost;
    };

    class SubCallsManagerListenerBase
        : public CallsManagerListenerBase
    {
    public:
        SubCallsManagerListenerBase(
            /* [in] */ BluetoothPhoneService* host);

        // @Override
        CARAPI OnCallAdded(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallRemoved(
            /* [in] */ ICall* call);

        // @Override
        CARAPI OnCallStateChanged(
            /* [in] */ ICall* call,
            /* [in] */ Int32 oldState,
            /* [in] */ Int32 newState);

        // @Override
        CARAPI OnForegroundCallChanged(
            /* [in] */ ICall* oldForegroundCall,
            /* [in] */ ICall* newForegroundCall);

        // @Override
        CARAPI OnIsConferencedChanged(
            /* [in] */ ICall* call);

    private:
        BluetoothPhoneService* mHost;
    };

    class SubBluetoothProfileServiceListener
        : public Object
        , public IBluetoothProfileServiceListener
    {
    public:
        CAR_INTERFACE_DECL()

        SubBluetoothProfileServiceListener(
            /* [in] */ BluetoothPhoneService* host);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        BluetoothPhoneService* mHost;
    };

    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("BluetoothPhoneService::SubBroadcastReceiver")

        SubBroadcastReceiver(
            /* [in] */ BluetoothPhoneService* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        BluetoothPhoneService* mHost;
    };

    friend class SubIBluetoothHeadsetPhone;
public:
    CAR_INTERFACE_DECL()

    BluetoothPhoneService();

    CARAPI constructor();

    static CARAPI Start(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** result);

    // @Override
    CARAPI OnCreate();

    // @Override
    CARAPI OnDestroy();

private:
    CARAPI ProcessChld(
        /* [in] */ Int32 chld,
        /* [out] */ Boolean* result);

    CARAPI EnforceModifyPermission();

    CARAPI SendSynchronousRequest(
        /* [in] */ Int32 message,
        /* [out] */ IInterface** result);

    CARAPI SendSynchronousRequest(
        /* [in] */ Int32 message,
        /* [in] */ Int32 param,
        /* [out] */ IInterface** result);

    CARAPI SendListOfCalls(
        /* [in] */ Boolean shouldLog);

    /**
     * Sends a single clcc (C* List Current Calls) event for the specified call.
     */
    CARAPI SendClccForCall(
        /* [in] */ ICall* call,
        /* [in] */ Boolean shouldLog);

    CARAPI SendClccEndMarker();

    /**
     * Returns the caches index for the specified call.  If no such index exists, then an index is
     * given (smallest number starting from 1 that isn't already taken).
     */
    CARAPI_(Int32) GetIndexForCall(
        /* [in] */ ICall* call);

    /**
     * Sends an update of the current call state to the current Headset.
     *
     * @param force {@code true} if the headset state should be sent regardless if no changes to the
     *      state have occurred, {@code false} if the state should only be sent if the state has
     *      changed.
     */
    CARAPI UpdateHeadsetWithCallState(
        /* [in] */ Boolean force);

    CARAPI GetBluetoothCallStateForUpdate(
        /* [out] */ Int32* result);

    CARAPI_(Int32) ConvertCallState(
        /* [in] */ Int32 callState,
        /* [in] */ Boolean isForegroundCall);

    CARAPI_(AutoPtr<CallsManager>) GetCallsManager();

    /**
     * Returns the best phone account to use for the given state of all calls.
     * First, tries to return the phone account for the foreground call, second the default
     * phone account for PhoneAccount.SCHEME_TEL.
     */
    CARAPI_(AutoPtr<IPhoneAccount>) GetBestPhoneAccount();

private:
    static const String TAG;

    static const Int32 MSG_ANSWER_CALL;

    static const Int32 MSG_HANGUP_CALL;

    static const Int32 MSG_SEND_DTMF;

    static const Int32 MSG_PROCESS_CHLD;

    static const Int32 MSG_GET_NETWORK_OPERATOR;

    static const Int32 MSG_LIST_CURRENT_CALLS;

    static const Int32 MSG_QUERY_PHONE_STATE;

    static const Int32 MSG_GET_SUBSCRIBER_NUMBER;

    // match up with bthf_call_state_t of bt_hf.h
    static const Int32 CALL_STATE_ACTIVE;

    static const Int32 CALL_STATE_HELD;

    static const Int32 CALL_STATE_DIALING;

    static const Int32 CALL_STATE_ALERTING;

    static const Int32 CALL_STATE_INCOMING;

    static const Int32 CALL_STATE_WAITING;

    static const Int32 CALL_STATE_IDLE;

    // Terminate all held or set UDUB("busy") to a waiting call
    static const Int32 CHLD_TYPE_RELEASEHELD;

    // Terminate all active calls and accepts a waiting/held call
    static const Int32 CHLD_TYPE_RELEASEACTIVE_ACCEPTHELD;

    // Hold all active calls and accepts a waiting/held call
    static const Int32 CHLD_TYPE_HOLDACTIVE_ACCEPTHELD;

    // Add all held calls to a conference
    static const Int32 CHLD_TYPE_ADDHELDTOCONF;

    Int32 mNumActiveCalls;

    Int32 mNumHeldCalls;

    Int32 mBluetoothCallState;

    String mRingingAddress;

    Int32 mRingingAddressType;

    AutoPtr<ICall> mOldHeldCall;

    /**
     * Binder implementation of IBluetoothHeadsetPhone. Implements the command interface that the
     * bluetooth headset code uses to control call.
     */
    AutoPtr<IIBluetoothHeadsetPhone> mBinder;

    /**
     * Main-thread handler for BT commands.  Since telecom logic runs on a single thread, commands
     * that are sent to it from the headset need to be moved over to the main thread before
     * executing. This handler exists for that reason.
     */
    AutoPtr<IHandler> mHandler;

    /**
     * Listens to call changes from the CallsManager and calls into methods to update the bluetooth
     * headset with the new states.
     */
    AutoPtr<ICallsManagerListener> mCallsManagerListener;

    /**
     * Listens to connections and disconnections of bluetooth headsets.  We need to save the current
     * bluetooth headset so that we know where to send call updates.
     */
    AutoPtr<IBluetoothProfileServiceListener> mProfileListener;

    /**
     * Receives events for global state changes of the bluetooth adapter.
     */
    AutoPtr<IBroadcastReceiver> mBluetoothAdapterReceiver;

    AutoPtr<IBluetoothAdapter> mBluetoothAdapter;

    AutoPtr<IBluetoothHeadset> mBluetoothHeadset;

    // A map from Calls to indexes used to identify calls for CLCC (C* List Current Calls).
    AutoPtr<IMap> mClccIndexMap;

    Boolean mHeadsetUpdatedRecently;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_BLUETOOTHPHONESERVICE_H__
