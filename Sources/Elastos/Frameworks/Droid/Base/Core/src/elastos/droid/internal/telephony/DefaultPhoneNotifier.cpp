
#include "elastos/droid/internal/telephony/DefaultPhoneNotifier.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/os/CBundle.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String DefaultPhoneNotifier::TAG("DefaultPhoneNotifier");
const Boolean DefaultPhoneNotifier::DBG = FALSE; // STOPSHIP if true

CAR_INTERFACE_IMPL(DefaultPhoneNotifier, Object, IPhoneNotifier);

/*package*/
DefaultPhoneNotifier::DefaultPhoneNotifier()
{
    mRegistry = IITelephonyRegistry::Probe(ServiceManager::GetService(
            String("telephony.registry")));
}

// @Override
ECode DefaultPhoneNotifier::NotifyPhoneState(
    /* [in] */ IPhone* sender)
{
    AutoPtr<ICall> ringingCall;
    sender->GetRingingCall((ICall**)&ringingCall);
    Int64 subId = 0;
    sender->GetSubId(&subId);
    String incomingNumber("");
    AutoPtr<IConnection> c;
    if (ringingCall != NULL
        && (ringingCall->GetEarliestConnection((IConnection**)&c), c.Get()) != NULL){
        c->GetAddress(&incomingNumber);
    }
    // try {
    if (mRegistry != NULL) {
        PhoneConstantsState pcs;
        sender->GetState(&pcs);
        mRegistry->NotifyCallStateForSubscriber(subId,
                ConvertCallState1(pcs), incomingNumber);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyServiceState(
    /* [in] */ IPhone* sender)
{
    AutoPtr<IServiceState> ss;
    sender->GetServiceState((IServiceState**)&ss);
    Int32 phoneId = 0;
    sender->GetPhoneId(&phoneId);
    Int64 subId = 0;
    sender->GetSubId(&subId);

    Logger::D(TAG, "nofityServiceState: mRegistry=%s ss=%s sender=%s phondId=%d subId=%lld"
            , TO_CSTR(mRegistry), TO_CSTR(ss)
            , TO_CSTR(sender), phoneId, subId);
    if (ss == NULL) {
        assert(0 && "TODO");
        // CServiceState::New((IServiceState**)&ss);
        ss->SetStateOutOfService();
    }
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifyServiceStateForPhoneId(phoneId, subId, ss);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifySignalStrength(
    /* [in] */ IPhone* sender)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    AutoPtr<ISignalStrength> ss;
    sender->GetSignalStrength((ISignalStrength**)&ss);
    Logger::D(TAG, "notifySignalStrength: mRegistry=%s ss=%s sender=%s"
        , TO_CSTR(mRegistry), TO_CSTR(ss), TO_CSTR(sender));
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifySignalStrengthForSubscriber(subId, ss);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyMessageWaitingChanged(
    /* [in] */ IPhone* sender)
{
    Int32 phoneId = 0;
    sender->GetPhoneId(&phoneId);
    Int64 subId = 0;
    sender->GetSubId(&subId);

    // try {
    if (mRegistry != NULL) {
        Boolean tmp = FALSE;
        sender->GetMessageWaitingIndicator(&tmp);
        mRegistry->NotifyMessageWaitingChangedForPhoneId(phoneId, subId, tmp);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyCallForwardingChanged(
    /* [in] */ IPhone* sender)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    // try {
    if (mRegistry != NULL) {
        Boolean tmp = FALSE;
        sender->GetCallForwardingIndicator(&tmp);
        mRegistry->NotifyCallForwardingChangedForSubscriber(subId, tmp);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyDataActivity(
    /* [in] */ IPhone* sender)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    // try {
    if (mRegistry != NULL) {
        IPhoneDataActivityState pdas;
        sender->GetDataActivityState(&pdas);
        mRegistry->NotifyDataActivityForSubscriber(subId,
                ConvertDataActivityState1(pdas));
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyDataConnection(
    /* [in] */ IPhone* sender,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ PhoneConstantsDataState state)
{
    DoNotifyDataConnection(sender, reason, apnType, state);
    return NOERROR;
}

void DefaultPhoneNotifier::DoNotifyDataConnection(
    /* [in] */ IPhone* sender,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ PhoneConstantsDataState state)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    Int64 dds = 0;
    SubscriptionManager::GetDefaultDataSubId(&dds);
    if (DBG) Log(String("subId = ") + StringUtils::ToString(subId)
        + ", DDS = " + StringUtils::ToString(dds));

    // TODO
    // use apnType as the key to which connection we're talking about.
    // pass apnType back up to fetch particular for this one.
    AutoPtr<ITelephonyManager> telephony;
    CTelephonyManager::GetDefault((ITelephonyManager**)&telephony);
    AutoPtr<ILinkProperties> linkProperties;
    AutoPtr<INetworkCapabilities> networkCapabilities;
    Boolean roaming = FALSE;

    if (state == PhoneConstantsDataState_CONNECTED) {
        sender->GetLinkProperties(apnType, (ILinkProperties**)&linkProperties);
        sender->GetNetworkCapabilities(apnType, (INetworkCapabilities**)&networkCapabilities);
    }
    AutoPtr<IServiceState> ss;
    sender->GetServiceState((IServiceState**)&ss);
    if (ss != NULL) {
        ss->GetRoaming(&roaming);
    }

    // try {
    if (mRegistry != NULL) {
        Boolean bv = FALSE;
        sender->IsDataConnectivityPossible(apnType, &bv);
        String v;
        sender->GetActiveApnHost(apnType, &v);
        Int32 iv = 0;
        mRegistry->NotifyDataConnectionForSubscriber(subId,
            ConvertDataState1(state),
            bv, reason, v, apnType, linkProperties, networkCapabilities,
            ((telephony != NULL) ? (telephony->GetNetworkType(&iv), iv) :
            ITelephonyManager::NETWORK_TYPE_UNKNOWN),
            roaming);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
}

// @Override
ECode DefaultPhoneNotifier::NotifyDataConnectionFailed(
    /* [in] */ IPhone* sender,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifyDataConnectionFailedForSubscriber(subId, reason, apnType);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyCellLocation(
    /* [in] */ IPhone* sender)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    AutoPtr<IBundle> data;
    CBundle::New((IBundle**)&data);
    AutoPtr<ICellLocation> c;
    sender->GetCellLocation((ICellLocation**)&c);
    c->FillInNotifierBundle(data);
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifyCellLocationForSubscriber(subId, data);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyCellInfo(
    /* [in] */ IPhone* sender,
    /* [in] */ IList* cellInfo)
{
    Int64 subId = 0;
    sender->GetSubId(&subId);
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifyCellInfoForSubscriber(subId, cellInfo);
    }
    // } catch (RemoteException ex) {

    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyDataConnectionRealTimeInfo(
    /* [in] */ IPhone* sender,
    /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo)
{
    // try {
    mRegistry->NotifyDataConnectionRealTimeInfo(dcRtInfo);
    // } catch (RemoteException ex) {

    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyOtaspChanged(
    /* [in] */ IPhone* sender,
    /* [in] */ Int32 otaspMode)
{
    // FIXME: subId?
    // try {
    if (mRegistry != NULL) {
        mRegistry->NotifyOtaspChanged(otaspMode);
    }
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

ECode DefaultPhoneNotifier::NotifyPreciseCallState(
    /* [in] */ IPhone* sender)
{
    // FIXME: subId?
    AutoPtr<ICall> ringingCall;
    sender->GetRingingCall((ICall**)&ringingCall);
    AutoPtr<ICall> foregroundCall;
    sender->GetForegroundCall((ICall**)&foregroundCall);
    AutoPtr<ICall> backgroundCall;
    sender->GetBackgroundCall((ICall**)&backgroundCall);
    if (ringingCall != NULL && foregroundCall != NULL && backgroundCall != NULL) {
        // try {
        ICallState cs;
        if (mRegistry) {
            mRegistry->NotifyPreciseCallState(
                ConvertPreciseCallState1((ringingCall->GetState(&cs), cs)),
                ConvertPreciseCallState1((foregroundCall->GetState(&cs), cs)),
                ConvertPreciseCallState1((backgroundCall->GetState(&cs), cs)));
        }
        // } catch (RemoteException ex) {
        //     // system process is dead
        // }
    }
    return NOERROR;
}

ECode DefaultPhoneNotifier::NotifyDisconnectCause(
    /* [in] */ Int32 cause,
    /* [in] */ Int32 preciseCause)
{
    // FIXME: subId?
    // try {
    if (mRegistry != NULL)
        mRegistry->NotifyDisconnectCause(cause, preciseCause);
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

ECode DefaultPhoneNotifier::NotifyPreciseDataConnectionFailed(
    /* [in] */ IPhone* sender,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnType,
    /* [in] */ const String& apn,
    /* [in] */ const String& failCause)
{
    // FIXME: subId?
    // try {
    if (mRegistry != NULL)
        mRegistry->NotifyPreciseDataConnectionFailed(reason, apnType, apn, failCause);
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyVoLteServiceStateChanged(
    /* [in] */ IPhone* sender,
    /* [in] */ IVoLteServiceState* lteState)
{
    // FIXME: subID
    // try {
    if (mRegistry != NULL)
        mRegistry->NotifyVoLteServiceStateChanged(lteState);
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

// @Override
ECode DefaultPhoneNotifier::NotifyOemHookRawEventForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ ArrayOf<Byte>* rawData)
{
    // try {
    if (mRegistry != NULL)
        mRegistry->NotifyOemHookRawEventForSubscriber(subId, rawData);
    else
        Logger::E("DefaultPhoneNotifier", "TODO mRegistry is NULL");
    // } catch (RemoteException ex) {
    //     // system process is dead
    // }
    return NOERROR;
}

/**
 * Convert the {@link PhoneConstants.State} enum into the TelephonyManager.CALL_STATE_*
 * constants for the public API.
 */
Int32 DefaultPhoneNotifier::ConvertCallState1(
    /* [in] */ PhoneConstantsState state)
{
    switch (state) {
        case PhoneConstantsState_RINGING:
            return ITelephonyManager::CALL_STATE_RINGING;
        case PhoneConstantsState_OFFHOOK:
            return ITelephonyManager::CALL_STATE_OFFHOOK;
        default:
            return ITelephonyManager::CALL_STATE_IDLE;
    }
}

/**
 * Convert the TelephonyManager.CALL_STATE_* constants into the
 * {@link PhoneConstants.State} enum for the public API.
 */
PhoneConstantsState DefaultPhoneNotifier::ConvertCallState2(
    /* [in] */ Int32 state)
{
    switch (state) {
        case ITelephonyManager::CALL_STATE_RINGING:
            return PhoneConstantsState_RINGING;
        case ITelephonyManager::CALL_STATE_OFFHOOK:
            return PhoneConstantsState_OFFHOOK;
        default:
            return PhoneConstantsState_IDLE;
    }
}

/**
 * Convert the {@link PhoneConstants.DataState} enum into the TelephonyManager.DATA_* constants
 * for the public API.
 */
Int32 DefaultPhoneNotifier::ConvertDataState1(
    /* [in] */ PhoneConstantsDataState state)
{
    switch (state) {
        case PhoneConstantsDataState_CONNECTING:
            return ITelephonyManager::DATA_CONNECTING;
        case PhoneConstantsDataState_CONNECTED:
            return ITelephonyManager::DATA_CONNECTED;
        case PhoneConstantsDataState_SUSPENDED:
            return ITelephonyManager::DATA_SUSPENDED;
        default:
            return ITelephonyManager::DATA_DISCONNECTED;
    }
}

/**
 * Convert the TelephonyManager.DATA_* constants into {@link PhoneConstants.DataState} enum
 * for the public API.
 */
PhoneConstantsDataState DefaultPhoneNotifier::ConvertDataState2(
    /* [in] */ Int32 state)
{
    switch (state) {
        case ITelephonyManager::DATA_CONNECTING:
            return PhoneConstantsDataState_CONNECTING;
        case ITelephonyManager::DATA_CONNECTED:
            return PhoneConstantsDataState_CONNECTED;
        case ITelephonyManager::DATA_SUSPENDED:
            return PhoneConstantsDataState_SUSPENDED;
        default:
            return PhoneConstantsDataState_DISCONNECTED;
    }
}

/**
 * Convert the {@link Phone.DataActivityState} enum into the TelephonyManager.DATA_* constants
 * for the public API.
 */
Int32 DefaultPhoneNotifier::ConvertDataActivityState1(
    /* [in] */ IPhoneDataActivityState state)
{
    switch (state) {
        case IPhoneDataActivityState_DATAIN:
            return ITelephonyManager::DATA_ACTIVITY_IN;
        case IPhoneDataActivityState_DATAOUT:
            return ITelephonyManager::DATA_ACTIVITY_OUT;
        case IPhoneDataActivityState_DATAINANDOUT:
            return ITelephonyManager::DATA_ACTIVITY_INOUT;
        case IPhoneDataActivityState_DORMANT:
            return ITelephonyManager::DATA_ACTIVITY_DORMANT;
        default:
            return ITelephonyManager::DATA_ACTIVITY_NONE;
    }
}

/**
 * Convert the TelephonyManager.DATA_* constants into the {@link Phone.DataActivityState} enum
 * for the public API.
 */
IPhoneDataActivityState DefaultPhoneNotifier::ConvertDataActivityState2(
    /* [in] */ Int32 state)
{
    switch (state) {
        case ITelephonyManager::DATA_ACTIVITY_IN:
            return IPhoneDataActivityState_DATAIN;
        case ITelephonyManager::DATA_ACTIVITY_OUT:
            return IPhoneDataActivityState_DATAOUT;
        case ITelephonyManager::DATA_ACTIVITY_INOUT:
            return IPhoneDataActivityState_DATAINANDOUT;
        case ITelephonyManager::DATA_ACTIVITY_DORMANT:
            return IPhoneDataActivityState_DORMANT;
        default:
            return IPhoneDataActivityState_NONE;
    }
}

/**
 * Convert the {@link State} enum into the PreciseCallState.PRECISE_CALL_STATE_* constants
 * for the public API.
 */
Int32 DefaultPhoneNotifier::ConvertPreciseCallState1(
    /* [in] */ ICallState state)
{
    switch (state) {
        case ICallState_ACTIVE:
            return IPreciseCallState::PRECISE_CALL_STATE_ACTIVE;
        case ICallState_HOLDING:
            return IPreciseCallState::PRECISE_CALL_STATE_HOLDING;
        case ICallState_DIALING:
            return IPreciseCallState::PRECISE_CALL_STATE_DIALING;
        case ICallState_ALERTING:
            return IPreciseCallState::PRECISE_CALL_STATE_ALERTING;
        case ICallState_INCOMING:
            return IPreciseCallState::PRECISE_CALL_STATE_INCOMING;
        case ICallState_WAITING:
            return IPreciseCallState::PRECISE_CALL_STATE_WAITING;
        case ICallState_DISCONNECTED:
            return IPreciseCallState::PRECISE_CALL_STATE_DISCONNECTED;
        case ICallState_DISCONNECTING:
            return IPreciseCallState::PRECISE_CALL_STATE_DISCONNECTING;
        default:
            return IPreciseCallState::PRECISE_CALL_STATE_IDLE;
    }
}

/**
 * Convert the Call.State.* constants into the {@link State} enum
 * for the public API.
 */
ICallState DefaultPhoneNotifier::ConvertPreciseCallState2(
    /* [in] */ Int32 state)
{
    switch (state) {
        case IPreciseCallState::PRECISE_CALL_STATE_ACTIVE:
            return ICallState_ACTIVE;
        case IPreciseCallState::PRECISE_CALL_STATE_HOLDING:
            return ICallState_HOLDING;
        case IPreciseCallState::PRECISE_CALL_STATE_DIALING:
            return ICallState_DIALING;
        case IPreciseCallState::PRECISE_CALL_STATE_ALERTING:
            return ICallState_ALERTING;
        case IPreciseCallState::PRECISE_CALL_STATE_INCOMING:
            return ICallState_INCOMING;
        case IPreciseCallState::PRECISE_CALL_STATE_WAITING:
            return ICallState_WAITING;
        case IPreciseCallState::PRECISE_CALL_STATE_DISCONNECTED:
            return ICallState_DISCONNECTED;
        case IPreciseCallState::PRECISE_CALL_STATE_DISCONNECTING:
            return ICallState_DISCONNECTING;
        default:
            return ICallState_IDLE;
    }
}

void DefaultPhoneNotifier::Log(
    /* [in] */ const String& s)
{
    Logger::D(TAG, s);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
