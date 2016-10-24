
#include "elastos/droid/server/telecom/BluetoothPhoneService.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManager.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/PhoneAccountRegistrar.h"
#include "elastos/droid/server/telecom/TelecomApp.h"
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telephony.h>
#include <elastos/droid/Manifest.h>
#include <elastos/droid/text/TextUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::IApplication;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::EIID_IBluetoothProfileServiceListener;
using Elastos::Droid::Bluetooth::EIID_IIBluetoothHeadsetPhone;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Telecom::ICallState;
using Elastos::Droid::Telecom::IGatewayInfo;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Droid::Telecom::IPhoneCapabilities;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::CInteger32;
using Elastos::Core::CBoolean;
using Elastos::Core::IBoolean;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// BluetoothPhoneService::MainThreadRequest
//=============================================================================
CAR_INTERFACE_IMPL(BluetoothPhoneService::MainThreadRequest, Object, IBluetoothPhoneServiceMainThreadRequest)

BluetoothPhoneService::MainThreadRequest::MainThreadRequest(
    /* [in] */ BluetoothPhoneService* host)
    : mHost(host)
{}

ECode BluetoothPhoneService::MainThreadRequest::constructor(
    /* [in] */ Int32 param)
{
    this->mParam = param;
    return NOERROR;
}

ECode BluetoothPhoneService::MainThreadRequest::SetResult(
    /* [in] */ IInterface* value)
{
    mResult = IObject::Probe(value);
    {
        AutoLock lock(this);
        NotifyAll();
    }
    return NOERROR;
}

//=============================================================================
// BluetoothPhoneService::SubIBluetoothHeadsetPhone
//=============================================================================
CAR_INTERFACE_IMPL_2(BluetoothPhoneService::SubIBluetoothHeadsetPhone, Object, IBinder, IIBluetoothHeadsetPhone)

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::constructor(
    /* [in] */ IInterface* host)
{
    mHost = (BluetoothPhoneService*) IObject::Probe(host);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::AnswerCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceModifyPermission();
    Log::I(BluetoothPhoneService::TAG, "BT - answering call");
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_ANSWER_CALL, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::HangupCall(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceModifyPermission();
    Log::I(BluetoothPhoneService::TAG, "BT - hanging up call");
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_HANGUP_CALL, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::SendDtmf(
    /* [in] */ Int32 dtmf,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    mHost->EnforceModifyPermission();
    Log::I(BluetoothPhoneService::TAG, "BT - sendDtmf %c", Log::DEBUG ? dtmf : '.');
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_SEND_DTMF, dtmf, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::GetNetworkOperator(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Log::I(BluetoothPhoneService::TAG, "getNetworkOperator");
    mHost->EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_GET_NETWORK_OPERATOR, (IInterface**)&obj);
    IObject::Probe(obj)->ToString(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::GetSubscriberNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    Log::I(BluetoothPhoneService::TAG, "getSubscriberNumber");
    mHost->EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_GET_SUBSCRIBER_NUMBER, (IInterface**)&obj);
    IObject::Probe(obj)->ToString(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::ListCurrentCalls(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // only log if it is after we recently updated the headset state or else it can clog
    // the android log since this can be queried every second.
    Boolean logQuery = mHost->mHeadsetUpdatedRecently;
    mHost->mHeadsetUpdatedRecently = FALSE;
    if (logQuery) {
        Log::I(BluetoothPhoneService::TAG, "listcurrentCalls");
    }
    mHost->EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_LIST_CURRENT_CALLS, logQuery ? 1 : 0, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::QueryPhoneState(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log::I(BluetoothPhoneService::TAG, "queryPhoneState");
    mHost->EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_QUERY_PHONE_STATE, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::ProcessChld(
    /* [in] */ Int32 chld,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Log::I(BluetoothPhoneService::TAG, "processChld %d", chld);
    mHost->EnforceModifyPermission();
    AutoPtr<IInterface> obj;
    mHost->SendSynchronousRequest(MSG_PROCESS_CHLD, chld, (IInterface**)&obj);
    IBoolean::Probe(obj)->GetValue(result);
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::UpdateBtHandsfreeAfterRadioTechnologyChange()
{
    Log::D(BluetoothPhoneService::TAG, "RAT change");
    // deprecated
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::CdmaSetSecondCallState(
    /* [in] */ Boolean state)
{
    Log::D(BluetoothPhoneService::TAG, "cdma 1");
    // deprecated
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::CdmaSwapSecondCallState()
{
    Log::D(BluetoothPhoneService::TAG, "cdma 2");
    // deprecated
    return NOERROR;
}

ECode BluetoothPhoneService::SubIBluetoothHeadsetPhone::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    return IObject::Probe(TO_IINTERFACE(this))->ToString(result);
}

//=============================================================================
// BluetoothPhoneService::SubHandler
//=============================================================================
BluetoothPhoneService::SubHandler::SubHandler(
    /* [in] */ BluetoothPhoneService* host)
    : mHost(host)
{}

ECode BluetoothPhoneService::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    AutoPtr<IBluetoothPhoneServiceMainThreadRequest> request = IBluetoothPhoneServiceMainThreadRequest::Probe(msgObj);
    AutoPtr<CallsManager> callsManager = mHost->GetCallsManager();
    AutoPtr<ICall> call;
    if (request == NULL) {
        Log::I(BluetoothPhoneService::TAG, "handleMessage: request is null");
        return NOERROR;
    }
    Log::D(BluetoothPhoneService::TAG, "handleMessage(%d) w/ param %s",
            msgWhat, request == NULL ? "NULL" : StringUtils::ToString(((MainThreadRequest*) request.Get())->mParam).string());
    if (msgWhat == MSG_ANSWER_CALL) {
        // try {
        ECode ec = NOERROR;
        callsManager->GetRingingCall((ICall**)&call);
        if (call != NULL) {
            ec = mHost->GetCallsManager()->AnswerCall(call, 0);
        }
        // } finally {
        AutoPtr<IBoolean> result;
        CBoolean::New(call != NULL, (IBoolean**)&result);
        ((MainThreadRequest*) request.Get())->SetResult(result);
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_HANGUP_CALL) {
        // try {
        ECode ec = callsManager->GetForegroundCall((ICall**)&call);
        if (call != NULL) {
            ec = callsManager->DisconnectCall(call);
        }
        // } finally {
        AutoPtr<IBoolean> result;
        CBoolean::New(call != NULL, (IBoolean**)&result);
        ((MainThreadRequest*) request.Get())->SetResult(result);
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_SEND_DTMF) {
        // try {
        ECode ec = NOERROR;
        do {
            callsManager->GetForegroundCall((ICall**)&call);
            if (call != NULL) {
                // TODO: Consider making this a queue instead of starting/stopping
                // in quick succession.
                ec = callsManager->PlayDtmfTone(call, (Char32) ((MainThreadRequest*) request.Get())->mParam);
                if (FAILED(ec)) break;
                ec = callsManager->StopDtmfTone(call);
            }
        } while(FALSE);
        // } finally {
        AutoPtr<IBoolean> result;
        CBoolean::New(call != NULL, (IBoolean**)&result);
        ((MainThreadRequest*) request.Get())->SetResult(result);
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_PROCESS_CHLD) {
        Boolean result = FALSE;
        // try {
        ECode ec = mHost->ProcessChld(((MainThreadRequest*) request.Get())->mParam, &result);
        // } finally {
        AutoPtr<IBoolean> r;
        CBoolean::New(result, (IBoolean**)&r);
        ((MainThreadRequest*) request.Get())->SetResult(r);
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_GET_SUBSCRIBER_NUMBER) {
        String address(NULL);
        ECode ec = NOERROR;
        // try {
        do {
            AutoPtr<IPhoneAccount> account = mHost->GetBestPhoneAccount();
            if (account != NULL) {
                AutoPtr<IUri> addressUri;
                ec = account->GetAddress((IUri**)&addressUri);
                if (FAILED(ec)) break;
                if (addressUri != NULL) {
                    ec = addressUri->GetSchemeSpecificPart(&address);
                    if (FAILED(ec)) break;
                }
            }
            if (TextUtils::IsEmpty(address)) {
                AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
                CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
                AutoPtr<ITelephonyManager> telephonyManager;
                ec = telephonyManagerHelper->From(IContext::Probe(this), (ITelephonyManager**)&telephonyManager);
                if (FAILED(ec)) break;
                ec = telephonyManager->GetLine1Number(&address);
                if (FAILED(ec)) break;
                /* if address is still null then while loop in
                 * sendSynchronousRequest will never be terminated and all
                 * subsequent requests will keep on waiting */
                if (TextUtils::IsEmpty(address)) {
                    address = "";
                }
            }
        } while(FALSE);
        // } finally {
        ((MainThreadRequest*) request.Get())->SetResult(StringUtils::ParseCharSequence(address));
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_GET_NETWORK_OPERATOR) {
        String label(NULL);
        ECode ec = NOERROR;
        // try {
        do {
            AutoPtr<IPhoneAccount> account = mHost->GetBestPhoneAccount();
            if (account != NULL) {
                AutoPtr<ICharSequence> cs;
                ec = account->GetLabel((ICharSequence**)&cs);
                if (FAILED(ec)) break;
                cs->ToString(&label);
            } else {
                // Finally, just get the network name from telephony.
                AutoPtr<ITelephonyManagerHelper> telephonyManagerHelper;
                CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&telephonyManagerHelper);
                AutoPtr<ITelephonyManager> tm;
                ec = telephonyManagerHelper->From(IContext::Probe(this), (ITelephonyManager**)&tm);
                if (FAILED(ec)) break;
                ec = tm->GetNetworkOperatorName(&label);
            }
        } while(FALSE);
        // } finally {
        ((MainThreadRequest*) request.Get())->SetResult(StringUtils::ParseCharSequence(label));
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_LIST_CURRENT_CALLS) {
        // try {
        ECode ec = mHost->SendListOfCalls(((MainThreadRequest*) request.Get())->mParam == 1);
        // } finally {
        AutoPtr<IBoolean> r;
        CBoolean::New(TRUE, (IBoolean**)&r);
        ((MainThreadRequest*) request.Get())->SetResult(r);
        if (FAILED(ec)) return ec;
        // }
    }
    else if (msgWhat == MSG_QUERY_PHONE_STATE) {
        // try {
        ECode ec = mHost->UpdateHeadsetWithCallState(TRUE /* force */);
        // } finally {
        if (request != NULL) {
        AutoPtr<IBoolean> r;
        CBoolean::New(TRUE, (IBoolean**)&r);
        ((MainThreadRequest*) request.Get())->SetResult(r);
        }
        if (FAILED(ec)) return ec;
        // }
    }
    return NOERROR;
}

//=============================================================================
// BluetoothPhoneService::SubCallsManagerListenerBase
//=============================================================================
BluetoothPhoneService::SubCallsManagerListenerBase::SubCallsManagerListenerBase(
    /* [in] */ BluetoothPhoneService* host)
    : mHost(host)
{}

ECode BluetoothPhoneService::SubCallsManagerListenerBase::OnCallAdded(
    /* [in] */ ICall* call)
{
    mHost->UpdateHeadsetWithCallState(FALSE /* force */);
    return NOERROR;
}

ECode BluetoothPhoneService::SubCallsManagerListenerBase::OnCallRemoved(
    /* [in] */ ICall* call)
{
    mHost->mClccIndexMap->Remove(call);
    mHost->UpdateHeadsetWithCallState(FALSE /* force */);
    return NOERROR;
}

ECode BluetoothPhoneService::SubCallsManagerListenerBase::OnCallStateChanged(
    /* [in] */ ICall* call,
    /* [in] */ Int32 oldState,
    /* [in] */ Int32 newState)
{
    // If a call is being put on hold because of a new connecting call, ignore the
    // CONNECTING since the BT state update needs to send out the numHeld = 1 + dialing
    // state atomically.
    // When the call later transitions to DIALING/DISCONNECTED we will then send out the
    // aggregated update.
    if (oldState == ICallState::ACTIVE && newState == ICallState::ON_HOLD) {
        AutoPtr<IList> calls;
        CallsManager::GetInstance()->GetCalls((IList**)&calls);
        AutoPtr<IIterator> it;
        calls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> otherCall = ICall::Probe(obj);
            Int32 state;
            ((Call*) otherCall.Get())->GetState(&state);
            if (state == ICallState::CONNECTING) {
                return NOERROR;
            }
        }
    }
    // To have an active call and another dialing at the same time is an invalid BT
    // state. We can assume that the active call will be automatically held which will
    // send another update at which point we will be in the right state.
    AutoPtr<ICall> activeCall;
    CallsManager::GetInstance()->GetActiveCall((ICall**)&activeCall);
    if (activeCall != NULL
            && oldState == ICallState::CONNECTING && newState == ICallState::DIALING) {
        return NOERROR;
    }
    mHost->UpdateHeadsetWithCallState(FALSE /* force */);
    return NOERROR;
}

ECode BluetoothPhoneService::SubCallsManagerListenerBase::OnForegroundCallChanged(
    /* [in] */ ICall* oldForegroundCall,
    /* [in] */ ICall* newForegroundCall)
{
    // The BluetoothPhoneService does not need to respond to changes in foreground calls,
    // which are always accompanied by call state changes anyway.
    return NOERROR;
}

ECode BluetoothPhoneService::SubCallsManagerListenerBase::OnIsConferencedChanged(
    /* [in] */ ICall* call)
{
    /*
     * Filter certain onIsConferencedChanged callbacks. Unfortunately this needs to be done
     * because conference change events are not atomic and multiple callbacks get fired
     * when two calls are conferenced together. This confuses updateHeadsetWithCallState
     * if it runs in the middle of two calls being conferenced and can cause spurious and
     * incorrect headset state updates. One of the scenarios is described below for CDMA
     * conference calls.
     *
     * 1) Call 1 and Call 2 are being merged into conference Call 3.
     * 2) Call 1 has its parent set to Call 3, but Call 2 does not have a parent yet.
     * 3) updateHeadsetWithCallState now thinks that there are two active calls (Call 2 and
     * Call 3) when there is actually only one active call (Call 3).
     */
    AutoPtr<ICall> parent;
    ((Call*) call)->GetParentCall((ICall**)&parent);
    if (parent != NULL) {
        // If this call is newly conferenced, ignore the callback. We only care about the
        // one sent for the parent conference call.
        Log::D(TAG, "Ignoring onIsConferenceChanged from child call with new parent");
        return NOERROR;
    }
    AutoPtr<IList> childCalls;
    ((Call*) call)->GetChildCalls((IList**)&childCalls);
    Int32 size;
    childCalls->GetSize(&size);
    if (size == 1) {
        // If this is a parent call with only one child, ignore the callback as well since
        // the minimum number of child calls to start a conference call is 2. We expect
        // this to be called again when the parent call has another child call added.
        Log::D(TAG, "Ignoring onIsConferenceChanged from parent with only one child call");
        return NOERROR;
    }
    mHost->UpdateHeadsetWithCallState(FALSE /* force */);
    return NOERROR;
}

//=============================================================================
// BluetoothPhoneService::SubBluetoothProfileServiceListener
//=============================================================================
CAR_INTERFACE_IMPL(BluetoothPhoneService::SubBluetoothProfileServiceListener, Object, IBluetoothProfileServiceListener)

BluetoothPhoneService::SubBluetoothProfileServiceListener::SubBluetoothProfileServiceListener(
    /* [in] */ BluetoothPhoneService* host)
    : mHost(host)
{}

ECode BluetoothPhoneService::SubBluetoothProfileServiceListener::OnServiceConnected(
    /* [in] */ Int32 profile,
    /* [in] */ IBluetoothProfile* proxy)
{
    mHost->mBluetoothHeadset = IBluetoothHeadset::Probe(proxy);
    return NOERROR;
}

ECode BluetoothPhoneService::SubBluetoothProfileServiceListener::OnServiceDisconnected(
    /* [in] */ Int32 profile)
{
    mHost->mBluetoothHeadset = NULL;
    return NOERROR;
}

//=============================================================================
// BluetoothPhoneService::SubBroadcastReceiver
//=============================================================================
BluetoothPhoneService::SubBroadcastReceiver::SubBroadcastReceiver(
    /* [in] */ BluetoothPhoneService* host)
    : mHost(host)
{}

ECode BluetoothPhoneService::SubBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Int32 state;
    intent->GetInt32Extra(IBluetoothAdapter::EXTRA_STATE, IBluetoothAdapter::ERROR, &state);
    Log::D(BluetoothPhoneService::TAG, "Bluetooth Adapter state: %d", state);
    if (state == IBluetoothAdapter::STATE_ON) {
        Boolean bNoUse;
        mHost->mHandler->SendEmptyMessage(MSG_QUERY_PHONE_STATE, &bNoUse);
    }
    return NOERROR;
}

//=============================================================================
// BluetoothPhoneService
//=============================================================================
CAR_INTERFACE_IMPL(BluetoothPhoneService, Service, IBluetoothPhoneService)

const String BluetoothPhoneService::TAG("BluetoothPhoneService");
const Int32 BluetoothPhoneService::MSG_ANSWER_CALL = 1;
const Int32 BluetoothPhoneService::MSG_HANGUP_CALL = 2;
const Int32 BluetoothPhoneService::MSG_SEND_DTMF = 3;
const Int32 BluetoothPhoneService::MSG_PROCESS_CHLD = 4;
const Int32 BluetoothPhoneService::MSG_GET_NETWORK_OPERATOR = 5;
const Int32 BluetoothPhoneService::MSG_LIST_CURRENT_CALLS = 6;
const Int32 BluetoothPhoneService::MSG_QUERY_PHONE_STATE = 7;
const Int32 BluetoothPhoneService::MSG_GET_SUBSCRIBER_NUMBER = 8;
const Int32 BluetoothPhoneService::CALL_STATE_ACTIVE = 0;
const Int32 BluetoothPhoneService::CALL_STATE_HELD = 1;
const Int32 BluetoothPhoneService::CALL_STATE_DIALING = 2;
const Int32 BluetoothPhoneService::CALL_STATE_ALERTING = 3;
const Int32 BluetoothPhoneService::CALL_STATE_INCOMING = 4;
const Int32 BluetoothPhoneService::CALL_STATE_WAITING = 5;
const Int32 BluetoothPhoneService::CALL_STATE_IDLE = 6;
const Int32 BluetoothPhoneService::CHLD_TYPE_RELEASEHELD = 0;
const Int32 BluetoothPhoneService::CHLD_TYPE_RELEASEACTIVE_ACCEPTHELD = 1;
const Int32 BluetoothPhoneService::CHLD_TYPE_HOLDACTIVE_ACCEPTHELD = 2;
const Int32 BluetoothPhoneService::CHLD_TYPE_ADDHELDTOCONF = 3;

BluetoothPhoneService::BluetoothPhoneService()
    : mNumActiveCalls(0)
    , mNumHeldCalls(0)
    , mBluetoothCallState(CALL_STATE_IDLE)
    , mRingingAddress(NULL)
    , mRingingAddressType(0)
    , mHeadsetUpdatedRecently(FALSE)
{
    mBinder = new SubIBluetoothHeadsetPhone();
    ((SubIBluetoothHeadsetPhone*) mBinder.Get())->constructor(TO_IINTERFACE(this));
    mHandler = new SubHandler(this);
    mCallsManagerListener = new SubCallsManagerListenerBase(this);
    mProfileListener = new SubBluetoothProfileServiceListener(this);
    mBluetoothAdapterReceiver = new SubBroadcastReceiver(this);
    CHashMap::New((IMap**)&mClccIndexMap);
}

ECode BluetoothPhoneService::constructor()
{
    Log::V(TAG, "Constructor");
    return NOERROR;
}

ECode BluetoothPhoneService::Start(
    /* [in] */ IContext* context)
{
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> adapter;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    if (adapter != NULL) {
        AutoPtr<IIntent> intent;
        CIntent::New(context, ECLSID_CBluetoothPhoneService, (IIntent**)&intent);
        AutoPtr<IComponentName> componentName;
        context->StartService(intent, (IComponentName**)&componentName);
    }
    return NOERROR;
}

ECode BluetoothPhoneService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** result)
{
    VALIDATE_NOT_NULL(result)

    Log::D(TAG, "Binding service");
    *result = IBinder::Probe(mBinder);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode BluetoothPhoneService::OnCreate()
{
    Log::D(TAG, "onCreate");
    AutoPtr<IBluetoothAdapterHelper> bluetoothAdapterHelper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&bluetoothAdapterHelper);
    bluetoothAdapterHelper->GetDefaultAdapter((IBluetoothAdapter**)&mBluetoothAdapter);
    if (mBluetoothAdapter == NULL) {
        Log::D(TAG, "BluetoothPhoneService shutting down, no BT Adapter found.");
        return NOERROR;
    }
    Boolean bNoUse;
    mBluetoothAdapter->GetProfileProxy(this, mProfileListener, IBluetoothProfile::HEADSET, &bNoUse);
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New(IBluetoothAdapter::ACTION_STATE_CHANGED, (IIntentFilter**)&intentFilter);
    AutoPtr<IIntent> iNoUse;
    RegisterReceiver(mBluetoothAdapterReceiver, intentFilter, (IIntent**)&iNoUse);
    CallsManager::GetInstance()->AddListener(mCallsManagerListener);
    UpdateHeadsetWithCallState(FALSE /* force */);
    return NOERROR;
}

ECode BluetoothPhoneService::OnDestroy()
{
    Log::D(TAG, "onDestroy");
    CallsManager::GetInstance()->RemoveListener(mCallsManagerListener);
    Service::OnDestroy();
    return NOERROR;
}

ECode BluetoothPhoneService::ProcessChld(
    /* [in] */ Int32 chld,
    /* [out] */ Boolean* result)
{
    AutoPtr<CallsManager> callsManager = CallsManager::GetInstance();
    AutoPtr<ICall> activeCall;
    callsManager->GetActiveCall((ICall**)&activeCall);
    AutoPtr<ICall> ringingCall;
    callsManager->GetRingingCall((ICall**)&ringingCall);
    AutoPtr<ICall> heldCall;
    callsManager->GetHeldCall((ICall**)&heldCall);
    // TODO: Keeping as Log.i for now.  Move to Log.d after L release if BT proves stable.
    Log::I(TAG, "Active: %s\nRinging: %s\nHeld: %s", TO_CSTR(activeCall), TO_CSTR(ringingCall), TO_CSTR(heldCall));
    if (chld == CHLD_TYPE_RELEASEHELD) {
        if (ringingCall != NULL) {
            callsManager->RejectCall(ringingCall, FALSE, String(NULL));
            *result = TRUE;
            return NOERROR;
        } else if (heldCall != NULL) {
            callsManager->DisconnectCall(heldCall);
            *result = TRUE;
            return NOERROR;
        }
    } else if (chld == CHLD_TYPE_RELEASEACTIVE_ACCEPTHELD) {
        if (activeCall != NULL) {
            callsManager->DisconnectCall(activeCall);
            if (ringingCall != NULL) {
                callsManager->AnswerCall(ringingCall, 0);
            } else if (heldCall != NULL) {
                callsManager->UnholdCall(heldCall);
            }
            *result = TRUE;
            return NOERROR;
        }
    } else if (chld == CHLD_TYPE_HOLDACTIVE_ACCEPTHELD) {
        Boolean isCanSwap;
        ((Call*) activeCall.Get())->Can(IPhoneCapabilities::SWAP_CONFERENCE, &isCanSwap);
        Boolean isCanHold;
        ((Call*) activeCall.Get())->Can(IPhoneCapabilities::HOLD, &isCanHold);
        if (activeCall != NULL && isCanSwap) {
            ((Call*) activeCall.Get())->SwapConference();
            Log::I(TAG, "CDMA calls in conference swapped, updating headset");
            UpdateHeadsetWithCallState(TRUE /* force */);
            *result = TRUE;
            return NOERROR;
        } else if (ringingCall != NULL) {
            callsManager->AnswerCall(ringingCall, 0);
            *result = TRUE;
            return NOERROR;
        } else if (heldCall != NULL) {
            // CallsManager will hold any active calls when unhold() is called on a
            // currently-held call.
            callsManager->UnholdCall(heldCall);
            *result = TRUE;
            return NOERROR;
        } else if (activeCall != NULL && isCanHold) {
            callsManager->HoldCall(activeCall);
            *result = TRUE;
            return NOERROR;
        }
    } else if (chld == CHLD_TYPE_ADDHELDTOCONF) {
        if (activeCall != NULL) {
            Boolean isCanMerge;
            ((Call*) activeCall.Get())->Can(IPhoneCapabilities::MERGE_CONFERENCE, &isCanMerge);
            if (isCanMerge) {
                ((Call*) activeCall.Get())->MergeConference();
                *result = TRUE;
                return NOERROR;
            } else {
                AutoPtr<IList> conferenceable;
                ((Call*) activeCall.Get())->GetConferenceableCalls((IList**)&conferenceable);
                Boolean isEmpty;
                conferenceable->IsEmpty(&isEmpty);
                if (!isEmpty) {
                    AutoPtr<IInterface> obj;
                    conferenceable->Get(0, (IInterface**)&obj);
                    callsManager->Conference(activeCall, ICall::Probe(obj));
                    *result = TRUE;
                    return NOERROR;
               }
            }
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode BluetoothPhoneService::EnforceModifyPermission()
{
    return EnforceCallingOrSelfPermission(Manifest::permission::MODIFY_PHONE_STATE, String(NULL));
}

ECode BluetoothPhoneService::SendSynchronousRequest(
    /* [in] */ Int32 message,
    /* [out] */ IInterface** result)
{
    return SendSynchronousRequest(message, 0, result);
}

ECode BluetoothPhoneService::SendSynchronousRequest(
    /* [in] */ Int32 message,
    /* [in] */ Int32 param,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<MainThreadRequest> request = new MainThreadRequest(this);
    request->constructor(param);
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(message, TO_IINTERFACE(request), (IMessage**)&msg);
    msg->SendToTarget();
    {
        AutoLock lock(request);
        while (request->mResult == NULL) {
            // try {
            request->Wait();
            // } catch (InterruptedException e) {
                // Do nothing, go back and wait until the request is complete.
            // }
        }
    }
    if (request->mResult != NULL) {
        // @SuppressWarnings("unchecked")
        *result = request->mResult;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode BluetoothPhoneService::SendListOfCalls(
    /* [in] */ Boolean shouldLog)
{
    AutoPtr<IList> calls;
    GetCallsManager()->GetCalls((IList**)&calls);
    AutoPtr<IIterator> it;
    calls->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICall> call = ICall::Probe(obj);
        // We don't send the parent conference call to the bluetooth device.
        Boolean isConference;
        ((Call*) call.Get())->IsConference(&isConference);
        if (!isConference) {
            SendClccForCall(call, shouldLog);
        }
    }
    SendClccEndMarker();
    return NOERROR;
}

ECode BluetoothPhoneService::SendClccForCall(
    /* [in] */ ICall* call,
    /* [in] */ Boolean shouldLog)
{
    AutoPtr<ICall> foregroundCall;
    GetCallsManager()->GetForegroundCall((ICall**)&foregroundCall);
    Boolean isForeground = foregroundCall.Get() == call;
    Int32 callState;
    ((Call*) call)->GetState(&callState);
    Int32 state = ConvertCallState(callState, isForeground);
    Boolean isPartOfConference = FALSE;
    if (state == CALL_STATE_IDLE) {
        return NOERROR;
    }
    AutoPtr<ICall> conferenceCall;
    ((Call*) call)->GetParentCall((ICall**)&conferenceCall);
    if (conferenceCall != NULL) {
        isPartOfConference = TRUE;
        // Run some alternative states for Conference-level merge/swap support.
        // Basically, if call supports swapping or merging at the conference-level, then we need
        // to expose the calls as having distinct states (ACTIVE vs HOLD) or the functionality
        // won't show up on the bluetooth device.
        // Before doing any special logic, ensure that we are dealing with an ACTIVE call and
        // that the conference itself has a notion of the current "active" child call.
        AutoPtr<ICall> activeChild;
        ((Call*) conferenceCall.Get())->GetConferenceLevelActiveCall((ICall**)&activeChild);
        if (state == CALL_STATE_ACTIVE && activeChild != NULL) {
            // Reevaluate state if we can MERGE or if we can SWAP without previously having
            // MERGED.
            Boolean canMerge;
            ((Call*) conferenceCall.Get())->Can(IPhoneCapabilities::MERGE_CONFERENCE, &canMerge);
            Boolean canSwap;
            ((Call*) conferenceCall.Get())->Can(IPhoneCapabilities::SWAP_CONFERENCE, &canSwap);
            Boolean wasConferencePreviouslyMerged;
            ((Call*) conferenceCall.Get())->WasConferencePreviouslyMerged(&wasConferencePreviouslyMerged);
            Boolean shouldReevaluateState = canMerge || (canSwap && !wasConferencePreviouslyMerged);
            if (shouldReevaluateState) {
                isPartOfConference = FALSE;
                if (call == activeChild) {
                    state = CALL_STATE_ACTIVE;
                } else {
                    // At this point we know there is an "active" child and we know that it is
                    // not this call, so set it to HELD instead.
                    state = CALL_STATE_HELD;
                }
            }
        }
    }
    Int32 index = GetIndexForCall(call);
    Boolean isIncoming;
    ((Call*) call)->IsIncoming(&isIncoming);
    Int32 direction = isIncoming ? 1 : 0;
    AutoPtr<IUri> addressUri;
    AutoPtr<IGatewayInfo> gatewayInfo;
    ((Call*) call)->GetGatewayInfo((IGatewayInfo**)&gatewayInfo);
    if (gatewayInfo != NULL) {
        gatewayInfo->GetOriginalAddress((IUri**)&addressUri);
    } else {
        ((Call*) call)->GetHandle((IUri**)&addressUri);
    }
    String address;
    if (addressUri != NULL) {
        addressUri->GetSchemeSpecificPart(&address);
    }
    Int32 addressType = -1;
    if (address == NULL) {
        AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
        CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
        phoneNumberUtilsHelper->ToaFromString(address, &addressType);
    }
    if (shouldLog) {
        Log::I(TAG, "sending clcc for call %d, %d, %d, %d, %s, %d",
                index, direction, state, isPartOfConference, Log::PiiHandle(StringUtils::ParseCharSequence(address)).string(), addressType);
    }
    if (mBluetoothHeadset != NULL) {
        mBluetoothHeadset->ClccResponse(
                index, direction, state, 0, isPartOfConference, address, addressType);
    }
    return NOERROR;
}

ECode BluetoothPhoneService::SendClccEndMarker()
{
    // End marker is recognized with an index value of 0. All other parameters are ignored.
    if (mBluetoothHeadset != NULL) {
        mBluetoothHeadset->ClccResponse(0 /* index */, 0, 0, 0, FALSE, String(NULL), 0);
    }
    return NOERROR;
}

Int32 BluetoothPhoneService::GetIndexForCall(
    /* [in] */ ICall* call)
{
    Int32 rev;
    Boolean isContainsKey;
    mClccIndexMap->ContainsKey(call, &isContainsKey);
    if (isContainsKey) {
        AutoPtr<IInterface> obj;
        mClccIndexMap->Get(call, (IInterface**)&obj);
        IInteger32::Probe(obj)->GetValue(&rev);
        return rev;
    }
    Int32 i = 1;  // Indexes for bluetooth clcc are 1-based.
    Boolean isContainsValue;
    AutoPtr<IInteger32> i32;
    CInteger32::New(i, (IInteger32**)&i32);
    while (mClccIndexMap->ContainsValue(i32, &isContainsValue), isContainsValue) {
        i++;
        i32 = NULL;
        CInteger32::New(i, (IInteger32**)&i32);
    }
    // NOTE: Indexes are removed in {@link #onCallRemoved}.
    i32 = NULL;
    CInteger32::New(i, (IInteger32**)&i32);
    mClccIndexMap->Put(call, i32);
    return i;
}

ECode BluetoothPhoneService::UpdateHeadsetWithCallState(
    /* [in] */ Boolean force)
{
    AutoPtr<CallsManager> callsManager = GetCallsManager();
    AutoPtr<ICall> activeCall;
    callsManager->GetActiveCall((ICall**)&activeCall);
    AutoPtr<ICall> ringingCall;
    callsManager->GetRingingCall((ICall**)&ringingCall);
    AutoPtr<ICall> heldCall;
    callsManager->GetHeldCall((ICall**)&heldCall);
    Int32 bluetoothCallState;
    GetBluetoothCallStateForUpdate(&bluetoothCallState);
    String ringingAddress(NULL);
    Int32 ringingAddressType = 128;
    if (ringingCall != NULL) {
        AutoPtr<IUri> handle;
        ((Call*) ringingCall.Get())->GetHandle((IUri**)&handle);
        handle->GetSchemeSpecificPart(&ringingAddress);
        if (ringingAddress != NULL) {
            AutoPtr<IPhoneNumberUtils> phoneNumberUtilsHelper;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&phoneNumberUtilsHelper);
            phoneNumberUtilsHelper->ToaFromString(ringingAddress, &ringingAddressType);
        }
    }
    if (ringingAddress == NULL) {
        ringingAddress = "";
    }
    Int32 numActiveCalls = activeCall == NULL ? 0 : 1;
    Int32 numHeldCalls;
    callsManager->GetNumHeldCalls(&numHeldCalls);
    Boolean callsSwitched = (numHeldCalls == 2);
    // For conference calls which support swapping the active call within the conference
    // (namely CDMA calls) we need to expose that as a held call in order for the BT device
    // to show "swap" and "merge" functionality.
    Boolean ignoreHeldCallChange = FALSE;
    Boolean isConference;
    ((Call*) activeCall.Get())->IsConference(&isConference);
    if (activeCall != NULL && isConference) {
        Boolean canSwap;
        ((Call*) activeCall.Get())->Can(IPhoneCapabilities::SWAP_CONFERENCE, &canSwap);
        Boolean canMerge;
        ((Call*) activeCall.Get())->Can(IPhoneCapabilities::MERGE_CONFERENCE, &canMerge);
        if (canSwap) {
            // Indicate that BT device should show SWAP command by indicating that there is a
            // call on hold, but only if the conference wasn't previously merged.
            Boolean wasConferencePreviouslyMerged;
            ((Call*) activeCall.Get())->WasConferencePreviouslyMerged(&wasConferencePreviouslyMerged);
            numHeldCalls = wasConferencePreviouslyMerged ? 0 : 1;
        } else if (canMerge) {
            numHeldCalls = 1;  // Merge is available, so expose via numHeldCalls.
        }
        AutoPtr<IList> childCalls;
        ((Call*) activeCall.Get())->GetChildCalls((IList**)&childCalls);
        AutoPtr<IIterator> it;
        childCalls->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<ICall> childCall = ICall::Probe(obj);
            // Held call has changed due to it being combined into a CDMA conference. Keep
            // track of this and ignore any future update since it doesn't really count as
            // a call change.
            if (mOldHeldCall == childCall) {
                ignoreHeldCallChange = TRUE;
                break;
            }
        }
    }
    if (mBluetoothHeadset != NULL &&
            (numActiveCalls != mNumActiveCalls ||
             numHeldCalls != mNumHeldCalls ||
             bluetoothCallState != mBluetoothCallState ||
             !TextUtils::Equals(ringingAddress, mRingingAddress) ||
             ringingAddressType != mRingingAddressType ||
             (heldCall != mOldHeldCall && !ignoreHeldCallChange) ||
             force) && !callsSwitched) {
        // If the call is transitioning into the alerting state, send DIALING first.
        // Some devices expect to see a DIALING state prior to seeing an ALERTING state
        // so we need to send it first.
        Boolean sendDialingFirst = mBluetoothCallState != bluetoothCallState &&
                bluetoothCallState == CALL_STATE_ALERTING;
        mOldHeldCall = heldCall;
        mNumActiveCalls = numActiveCalls;
        mNumHeldCalls = numHeldCalls;
        mBluetoothCallState = bluetoothCallState;
        mRingingAddress = ringingAddress;
        mRingingAddressType = ringingAddressType;
        if (sendDialingFirst) {
            // Log in full to make logs easier to debug.
            Log::I(TAG, "updateHeadsetWithCallState "
                    "numActive %d, "
                    "numHeld %d, "
                    "callState %d, "
                    "ringing number %d, "
                    "ringing type %d",
                    mNumActiveCalls,
                    mNumHeldCalls,
                    CALL_STATE_DIALING,
                    Log::Pii(StringUtils::ParseCharSequence(mRingingAddress)).string(),
                    mRingingAddressType);
            mBluetoothHeadset->PhoneStateChanged(
                    mNumActiveCalls,
                    mNumHeldCalls,
                    CALL_STATE_DIALING,
                    mRingingAddress,
                    mRingingAddressType);
        }
        Log::I(TAG, "updateHeadsetWithCallState "
                "numActive %d, "
                "numHeld %d, "
                "callState %d, "
                "ringing number %d, "
                "ringing type %d",
                mNumActiveCalls,
                mNumHeldCalls,
                mBluetoothCallState,
                Log::Pii(StringUtils::ParseCharSequence(mRingingAddress)).string(),
                mRingingAddressType);
        mBluetoothHeadset->PhoneStateChanged(
                mNumActiveCalls,
                mNumHeldCalls,
                mBluetoothCallState,
                mRingingAddress,
                mRingingAddressType);
        mHeadsetUpdatedRecently = TRUE;
    }
    return NOERROR;
}

ECode BluetoothPhoneService::GetBluetoothCallStateForUpdate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<CallsManager> callsManager = GetCallsManager();
    AutoPtr<ICall> ringingCall;
    callsManager->GetRingingCall((ICall**)&ringingCall);
    AutoPtr<ICall> dialingCall;
    callsManager->GetDialingCall((ICall**)&dialingCall);
    //
    // !! WARNING !!
    // You will note that CALL_STATE_WAITING, CALL_STATE_HELD, and CALL_STATE_ACTIVE are not
    // used in this version of the call state mappings.  This is on purpose.
    // phone_state_change() in btif_hf.c is not written to handle these states. Only with the
    // listCalls*() method are WAITING and ACTIVE used.
    // Using the unsupported states here caused problems with inconsistent state in some
    // bluetooth devices (like not getting out of ringing state after answering a call).
    //
    Int32 bluetoothCallState = CALL_STATE_IDLE;
    if (ringingCall != NULL) {
        bluetoothCallState = CALL_STATE_INCOMING;
    } else if (dialingCall != NULL) {
        bluetoothCallState = CALL_STATE_ALERTING;
    }
    *result = bluetoothCallState;
    return NOERROR;
}

Int32 BluetoothPhoneService::ConvertCallState(
    /* [in] */ Int32 callState,
    /* [in] */ Boolean isForegroundCall)
{
    switch (callState) {
        case ICallState::NEW:
        case ICallState::ABORTED:
        case ICallState::DISCONNECTED:
        case ICallState::CONNECTING:
        case ICallState::PRE_DIAL_WAIT:
            return CALL_STATE_IDLE;
        case ICallState::ACTIVE:
            return CALL_STATE_ACTIVE;
        case ICallState::DIALING:
            // Yes, this is correctly returning ALERTING.
            // "Dialing" for BT means that we have sent information to the service provider
            // to place the call but there is no confirmation that the call is going through.
            // When there finally is confirmation, the ringback is played which is referred to
            // as an "alert" tone, thus, ALERTING.
            // TODO: We should consider using the ALERTING terms in Telecom because that
            // seems to be more industry-standard.
            return CALL_STATE_ALERTING;
        case ICallState::ON_HOLD:
            return CALL_STATE_HELD;
        case ICallState::RINGING:
            if (isForegroundCall) {
                return CALL_STATE_INCOMING;
            } else {
                return CALL_STATE_WAITING;
            }
    }
    return CALL_STATE_IDLE;
}

AutoPtr<CallsManager> BluetoothPhoneService::GetCallsManager()
{
    return CallsManager::GetInstance();
}

AutoPtr<IPhoneAccount> BluetoothPhoneService::GetBestPhoneAccount()
{
    AutoPtr<IApplication> obj;
    GetApplication((IApplication**)&obj);
    AutoPtr<TelecomApp> app = (TelecomApp*) IObject::Probe(obj);
    AutoPtr<PhoneAccountRegistrar> registry;
    app->GetPhoneAccountRegistrar((PhoneAccountRegistrar**)&registry);
    AutoPtr<ICall> call;
    GetCallsManager()->GetForegroundCall((ICall**)&call);
    AutoPtr<IPhoneAccount> account;
    if (call != NULL) {
        // First try to get the network name of the foreground call.
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        ((Call*) call.Get())->GetTargetPhoneAccount((IPhoneAccountHandle**)&phoneAccountHandle);
        registry->GetPhoneAccount(phoneAccountHandle, (IPhoneAccount**)&account);
    }
    if (account == NULL) {
        // Second, Try to get the label for the default Phone Account.
        AutoPtr<IPhoneAccountHandle> phoneAccountHandle;
        registry->GetDefaultOutgoingPhoneAccount(IPhoneAccount::SCHEME_TEL, (IPhoneAccountHandle**)&phoneAccountHandle);
        registry->GetPhoneAccount(phoneAccountHandle, (IPhoneAccount**)&account);
    }
    return account;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
