#include <Elastos.Droid.Net.h>
#include "elastos/droid/server/connectivity/DataConnectionStats.h"
#include "elastos/droid/server/am/CBatteryStatsService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Telephony::EIID_IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_UNKNOWN;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_ABSENT;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PIN_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PUK_REQUIRED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERSO_LOCKED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_NOT_READY;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_PERM_DISABLED;
using Elastos::Droid::Internal::Telephony::IccCardConstantsState_CARD_IO_ERROR;
using Elastos::Droid::Server::Am::CBatteryStatsService;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

//=====================================================================
// DataConnectionStats::MyPhoneStateListener
//=====================================================================
// CAR_INTERFACE_IMPL(DataConnectionStats::MyPhoneStateListener, Object, IPhoneStateListener)

DataConnectionStats::MyPhoneStateListener::MyPhoneStateListener(
    /* [in] */ DataConnectionStats* host)
    : mHost(host)
{}

ECode DataConnectionStats::MyPhoneStateListener::OnSignalStrengthsChanged(
    /* [in] */ ISignalStrength* signalStrength)
{
    mHost->mSignalStrength = signalStrength;
    return NOERROR;
}

ECode DataConnectionStats::MyPhoneStateListener::OnServiceStateChanged(
    /* [in] */ IServiceState* state)
{
    mHost->mServiceState = state;
    mHost->NotePhoneDataConnectionState();
    return NOERROR;
}

ECode DataConnectionStats::MyPhoneStateListener::OnDataConnectionStateChanged(
    /* [in] */ Int32 state,
    /* [in] */ Int32 networkType)
{
    mHost->mDataState = state;
    mHost->NotePhoneDataConnectionState();
    return NOERROR;
}

ECode DataConnectionStats::MyPhoneStateListener::OnDataActivity(
    /* [in] */ Int32 direction)
{
    mHost->NotePhoneDataConnectionState();
    return NOERROR;
}

//=====================================================================
// DataConnectionStats
//=====================================================================

const String DataConnectionStats::TAG("DataConnectionStats");
const Boolean DataConnectionStats::DEBUG = FALSE;

DataConnectionStats::DataConnectionStats(
    /* [in] */ IContext* context)
{
    mSimState = IccCardConstantsState_READY;
    mDataState = ITelephonyManager::DATA_DISCONNECTED;
    mPhoneStateListener = (IPhoneStateListener*)new MyPhoneStateListener(this);

    mContext = context;
    mBatteryStats = CBatteryStatsService::GetService();
}

ECode DataConnectionStats::StartMonitoring()
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> phone = ITelephonyManager::Probe(obj);
    if (phone != NULL) {
        phone->Listen(mPhoneStateListener,
                IPhoneStateListener::LISTEN_SERVICE_STATE
                | IPhoneStateListener::LISTEN_SIGNAL_STRENGTHS
                | IPhoneStateListener::LISTEN_DATA_CONNECTION_STATE
                | IPhoneStateListener::LISTEN_DATA_ACTIVITY);
    }
    else {
        Logger::E(TAG, "TELEPHONY_SERVICE is not ok");
        return E_NOT_IMPLEMENTED;
    }

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New((IIntentFilter**)&filter);
    filter->AddAction(ITelephonyIntents::ACTION_SIM_STATE_CHANGED);
    filter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    filter->AddAction(IConnectivityManager::INET_CONDITION_ACTION);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(this, filter, (IIntent**)&intent);
    return NOERROR;
}

ECode DataConnectionStats::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(ITelephonyIntents::ACTION_SIM_STATE_CHANGED)) {
        UpdateSimState(intent);
        NotePhoneDataConnectionState();
    }
    else if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION) ||
        action.Equals(IConnectivityManager::INET_CONDITION_ACTION)) {
        NotePhoneDataConnectionState();
    }
    return NOERROR;
}

 void DataConnectionStats::NotePhoneDataConnectionState()
 {
    if (mServiceState == NULL) {
        return;
    }
    Boolean simReadyOrUnknown = mSimState == IccCardConstantsState_READY
        || mSimState == IccCardConstantsState_UNKNOWN;
    Boolean visible = (simReadyOrUnknown || IsCdma()) // we only check the sim state for GSM
        && HasService()
        && mDataState == ITelephonyManager::DATA_CONNECTED;
    Int32 networkType;
    mServiceState->GetDataNetworkType(&networkType);
    if (DEBUG) {
        Logger::D(TAG,"Noting data connection for network type %d: %svisible",
            networkType, visible ? "" : "not ");
    }
    // try {
    ECode ec = mBatteryStats->NotePhoneDataConnectionState(networkType, visible);
    // } catch (RemoteException e) {
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        Logger::W(TAG, "Error noting data connection state");
    }
    // }
}

void DataConnectionStats::UpdateSimState(
    /* [in] */ IIntent* intent)
{
    String stateExtra;
    intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &stateExtra);
    if (IIccCardConstants::INTENT_VALUE_ICC_ABSENT.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_ABSENT;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_READY.Equals(stateExtra)) {
        mSimState = IccCardConstantsState_READY;
    }
    else if (IIccCardConstants::INTENT_VALUE_ICC_LOCKED.Equals(stateExtra)) {
        String lockedReason;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_LOCKED_REASON, &lockedReason);
        if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PIN.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PIN_REQUIRED;
        }
        else if (IIccCardConstants::INTENT_VALUE_LOCKED_ON_PUK.Equals(lockedReason)) {
            mSimState = IccCardConstantsState_PUK_REQUIRED;
        }
        else {
            mSimState = IccCardConstantsState_PERSO_LOCKED;
        }
    }
    else {
        mSimState = IccCardConstantsState_UNKNOWN;
    }
}

Boolean DataConnectionStats::IsCdma()
{
    Boolean result = FALSE;
    if (mSignalStrength != NULL) {
        Boolean isGsm;
        mSignalStrength->IsGsm(&isGsm);
        result = !isGsm;
    }
    return result;
}

Boolean DataConnectionStats::HasService()
{
    Boolean result = FALSE;
    if (mServiceState != NULL) {
        Int32 state;
        mServiceState->GetState(&state);
        result = state != IServiceState::STATE_OUT_OF_SERVICE
            && state != IServiceState::STATE_POWER_OFF;
    }
    return result;
}


} // Connectivity
} // Server
} // Droid
} // Elastos
