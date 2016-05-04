/*
 * Copyright (C) 2006 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IPreciseCallState;
using Elastos::Droid::Telephony::IDisconnectCause;

using Elastos::Droid::Internal::Telephony::ICall;
using Elastos::Droid::Internal::Telephony::ICallManager;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

using Elastos::Utility::IList;

/**
 * broadcast intents
 */
public class DefaultPhoneNotifier implements PhoneNotifier {
    private static const String LOG_TAG = "DefaultPhoneNotifier";
    private static const Boolean DBG = FALSE; // STOPSHIP if TRUE

    protected ITelephonyRegistry mRegistry;

    /*package*/
    protected DefaultPhoneNotifier() {
        mRegistry = ITelephonyRegistry.Stub->AsInterface(ServiceManager->GetService(
                    "telephony.registry"));
    }

    //@Override
    CARAPI NotifyPhoneState(Phone sender) {
        Call ringingCall = sender->GetRingingCall();
        Int64 subId = sender->GetSubId();
        String incomingNumber = "";
        If (ringingCall != NULL && ringingCall->GetEarliestConnection() != NULL){
            incomingNumber = ringingCall->GetEarliestConnection()->GetAddress();
        }
        try {
            If (mRegistry != NULL) {
                  mRegistry->NotifyCallStateForSubscriber(subId,
                        ConvertCallState(sender->GetState()), incomingNumber);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyServiceState(Phone sender) {
        ServiceState ss = sender->GetServiceState();
        Int32 phoneId = sender->GetPhoneId();
        Int64 subId = sender->GetSubId();

        Rlog->D(LOG_TAG, "nofityServiceState: mRegistry=" + mRegistry + " ss=" + ss
                + " sender=" + sender + " phondId=" + phoneId + " subId=" + subId);
        If (ss == NULL) {
            ss = new ServiceState();
            ss->SetStateOutOfService();
        }
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyServiceStateForPhoneId(phoneId, subId, ss);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifySignalStrength(Phone sender) {
        Int64 subId = sender->GetSubId();
        Rlog->D(LOG_TAG, "notifySignalStrength: mRegistry=" + mRegistry
                + " ss=" + sender->GetSignalStrength() + " sender=" + sender);
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifySignalStrengthForSubscriber(subId, sender->GetSignalStrength());
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyMessageWaitingChanged(Phone sender) {
        Int32 phoneId = sender->GetPhoneId();
        Int64 subId = sender->GetSubId();

        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyMessageWaitingChangedForPhoneId(phoneId, subId,
                        sender->GetMessageWaitingIndicator());
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyCallForwardingChanged(Phone sender) {
        Int64 subId = sender->GetSubId();
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyCallForwardingChangedForSubscriber(subId,
                        sender->GetCallForwardingIndicator());
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyDataActivity(Phone sender) {
        Int64 subId = sender->GetSubId();
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyDataActivityForSubscriber(subId,
                        ConvertDataActivityState(sender->GetDataActivityState()));
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyDataConnection(Phone sender, String reason, String apnType,
            PhoneConstants.DataState state) {
        DoNotifyDataConnection(sender, reason, apnType, state);
    }

    private void DoNotifyDataConnection(Phone sender, String reason, String apnType,
            PhoneConstants.DataState state) {
        Int64 subId = sender->GetSubId();
        Int64 dds = SubscriptionManager->GetDefaultDataSubId();
        If (DBG) Log("subId = " + subId + ", DDS = " + dds);

        // TODO
        // use apnType as the key to which connection we're talking about.
        // pass apnType back up to fetch particular for this one.
        TelephonyManager telephony = TelephonyManager->GetDefault();
        LinkProperties linkProperties = NULL;
        NetworkCapabilities networkCapabilities = NULL;
        Boolean roaming = FALSE;

        If (state == PhoneConstants.DataState.CONNECTED) {
            linkProperties = sender->GetLinkProperties(apnType);
            networkCapabilities = sender->GetNetworkCapabilities(apnType);
        }
        ServiceState ss = sender->GetServiceState();
        If (ss != NULL) roaming = ss->GetRoaming();

        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyDataConnectionForSubscriber(subId,
                    ConvertDataState(state),
                    sender->IsDataConnectivityPossible(apnType), reason,
                    sender->GetActiveApnHost(apnType),
                    apnType,
                    linkProperties,
                    networkCapabilities,
                    ((telephony!=NULL) ? telephony->GetNetworkType() :
                    TelephonyManager.NETWORK_TYPE_UNKNOWN),
                    roaming);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyDataConnectionFailed(Phone sender, String reason, String apnType) {
        Int64 subId = sender->GetSubId();
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyDataConnectionFailedForSubscriber(subId, reason, apnType);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyCellLocation(Phone sender) {
        Int64 subId = sender->GetSubId();
        Bundle data = new Bundle();
        sender->GetCellLocation()->FillInNotifierBundle(data);
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyCellLocationForSubscriber(subId, data);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyCellInfo(Phone sender, List<CellInfo> cellInfo) {
        Int64 subId = sender->GetSubId();
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyCellInfoForSubscriber(subId, cellInfo);
            }
        } Catch (RemoteException ex) {

        }
    }

    //@Override
    CARAPI NotifyDataConnectionRealTimeInfo(Phone sender,
                                                 DataConnectionRealTimeInfo dcRtInfo) {
        try {
            mRegistry->NotifyDataConnectionRealTimeInfo(dcRtInfo);
        } Catch (RemoteException ex) {

        }
    }

    //@Override
    CARAPI NotifyOtaspChanged(Phone sender, Int32 otaspMode) {
        // FIXME: subId?
        try {
            If (mRegistry != NULL) {
                mRegistry->NotifyOtaspChanged(otaspMode);
            }
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    CARAPI NotifyPreciseCallState(Phone sender) {
        // FIXME: subId?
        Call ringingCall = sender->GetRingingCall();
        Call foregroundCall = sender->GetForegroundCall();
        Call backgroundCall = sender->GetBackgroundCall();
        If (ringingCall != NULL && foregroundCall != NULL && backgroundCall != NULL) {
            try {
                mRegistry->NotifyPreciseCallState(
                        ConvertPreciseCallState(ringingCall->GetState()),
                        ConvertPreciseCallState(foregroundCall->GetState()),
                        ConvertPreciseCallState(backgroundCall->GetState()));
            } Catch (RemoteException ex) {
                // system process is dead
            }
        }
    }

    CARAPI NotifyDisconnectCause(Int32 cause, Int32 preciseCause) {
        // FIXME: subId?
        try {
            mRegistry->NotifyDisconnectCause(cause, preciseCause);
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    CARAPI NotifyPreciseDataConnectionFailed(Phone sender, String reason, String apnType,
            String apn, String failCause) {
        // FIXME: subId?
        try {
            mRegistry->NotifyPreciseDataConnectionFailed(reason, apnType, apn, failCause);
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyVoLteServiceStateChanged(Phone sender, VoLteServiceState lteState) {
        // FIXME: subID
        try {
            mRegistry->NotifyVoLteServiceStateChanged(lteState);
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    //@Override
    CARAPI NotifyOemHookRawEventForSubscriber(Int64 subId, Byte[] rawData) {
        try {
            mRegistry->NotifyOemHookRawEventForSubscriber(subId, rawData);
        } Catch (RemoteException ex) {
            // system process is dead
        }
    }

    /**
     * Convert the {@link PhoneConstants.State} enum into the TelephonyManager.CALL_STATE_*
     * constants for the public API.
     */
    public static Int32 ConvertCallState(PhoneConstants.State state) {
        Switch (state) {
            case RINGING:
                return TelephonyManager.CALL_STATE_RINGING;
            case OFFHOOK:
                return TelephonyManager.CALL_STATE_OFFHOOK;
            default:
                return TelephonyManager.CALL_STATE_IDLE;
        }
    }

    /**
     * Convert the TelephonyManager.CALL_STATE_* constants into the
     * {@link PhoneConstants.State} enum for the public API.
     */
    public static PhoneConstants.State ConvertCallState(Int32 state) {
        Switch (state) {
            case TelephonyManager.CALL_STATE_RINGING:
                return PhoneConstants.State.RINGING;
            case TelephonyManager.CALL_STATE_OFFHOOK:
                return PhoneConstants.State.OFFHOOK;
            default:
                return PhoneConstants.State.IDLE;
        }
    }

    /**
     * Convert the {@link PhoneConstants.DataState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    public static Int32 ConvertDataState(PhoneConstants.DataState state) {
        Switch (state) {
            case CONNECTING:
                return TelephonyManager.DATA_CONNECTING;
            case CONNECTED:
                return TelephonyManager.DATA_CONNECTED;
            case SUSPENDED:
                return TelephonyManager.DATA_SUSPENDED;
            default:
                return TelephonyManager.DATA_DISCONNECTED;
        }
    }

    /**
     * Convert the TelephonyManager.DATA_* constants into {@link PhoneConstants.DataState} enum
     * for the public API.
     */
    public static PhoneConstants.DataState ConvertDataState(Int32 state) {
        Switch (state) {
            case TelephonyManager.DATA_CONNECTING:
                return PhoneConstants.DataState.CONNECTING;
            case TelephonyManager.DATA_CONNECTED:
                return PhoneConstants.DataState.CONNECTED;
            case TelephonyManager.DATA_SUSPENDED:
                return PhoneConstants.DataState.SUSPENDED;
            default:
                return PhoneConstants.DataState.DISCONNECTED;
        }
    }

    /**
     * Convert the {@link Phone.DataActivityState} enum into the TelephonyManager.DATA_* constants
     * for the public API.
     */
    public static Int32 ConvertDataActivityState(Phone.DataActivityState state) {
        Switch (state) {
            case DATAIN:
                return TelephonyManager.DATA_ACTIVITY_IN;
            case DATAOUT:
                return TelephonyManager.DATA_ACTIVITY_OUT;
            case DATAINANDOUT:
                return TelephonyManager.DATA_ACTIVITY_INOUT;
            case DORMANT:
                return TelephonyManager.DATA_ACTIVITY_DORMANT;
            default:
                return TelephonyManager.DATA_ACTIVITY_NONE;
        }
    }

    /**
     * Convert the TelephonyManager.DATA_* constants into the {@link Phone.DataActivityState} enum
     * for the public API.
     */
    public static Phone.DataActivityState ConvertDataActivityState(Int32 state) {
        Switch (state) {
            case TelephonyManager.DATA_ACTIVITY_IN:
                return Phone.DataActivityState.DATAIN;
            case TelephonyManager.DATA_ACTIVITY_OUT:
                return Phone.DataActivityState.DATAOUT;
            case TelephonyManager.DATA_ACTIVITY_INOUT:
                return Phone.DataActivityState.DATAINANDOUT;
            case TelephonyManager.DATA_ACTIVITY_DORMANT:
                return Phone.DataActivityState.DORMANT;
            default:
                return Phone.DataActivityState.NONE;
        }
    }

    /**
     * Convert the {@link State} enum into the PreciseCallState.PRECISE_CALL_STATE_* constants
     * for the public API.
     */
    public static Int32 ConvertPreciseCallState(Call.State state) {
        Switch (state) {
            case ACTIVE:
                return PreciseCallState.PRECISE_CALL_STATE_ACTIVE;
            case HOLDING:
                return PreciseCallState.PRECISE_CALL_STATE_HOLDING;
            case DIALING:
                return PreciseCallState.PRECISE_CALL_STATE_DIALING;
            case ALERTING:
                return PreciseCallState.PRECISE_CALL_STATE_ALERTING;
            case INCOMING:
                return PreciseCallState.PRECISE_CALL_STATE_INCOMING;
            case WAITING:
                return PreciseCallState.PRECISE_CALL_STATE_WAITING;
            case DISCONNECTED:
                return PreciseCallState.PRECISE_CALL_STATE_DISCONNECTED;
            case DISCONNECTING:
                return PreciseCallState.PRECISE_CALL_STATE_DISCONNECTING;
            default:
                return PreciseCallState.PRECISE_CALL_STATE_IDLE;
        }
    }

    /**
     * Convert the Call.State.* constants into the {@link State} enum
     * for the public API.
     */
    public static Call.State ConvertPreciseCallState(Int32 state) {
        Switch (state) {
            case PreciseCallState.PRECISE_CALL_STATE_ACTIVE:
                return Call.State.ACTIVE;
            case PreciseCallState.PRECISE_CALL_STATE_HOLDING:
                return Call.State.HOLDING;
            case PreciseCallState.PRECISE_CALL_STATE_DIALING:
                return Call.State.DIALING;
            case PreciseCallState.PRECISE_CALL_STATE_ALERTING:
                return Call.State.ALERTING;
            case PreciseCallState.PRECISE_CALL_STATE_INCOMING:
                return Call.State.INCOMING;
            case PreciseCallState.PRECISE_CALL_STATE_WAITING:
                return Call.State.WAITING;
            case PreciseCallState.PRECISE_CALL_STATE_DISCONNECTED:
                return Call.State.DISCONNECTED;
            case PreciseCallState.PRECISE_CALL_STATE_DISCONNECTING:
                return Call.State.DISCONNECTING;
            default:
                return Call.State.IDLE;
        }
    }

    public interface IDataStateChangedCallback {
        void OnDataStateChanged(Int64 subId, String state, String reason, String apnName,
            String apnType, Boolean unavailable);
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, s);
    }
}
