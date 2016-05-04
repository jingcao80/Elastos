/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.internal.telephony.cdma;

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IMccTable;
using Elastos::Droid::Internal::Telephony::IEventLogTags;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IccCardApplicationStatus::IAppState;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;

using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellInfoLte;
using Elastos::Droid::Telephony::ICellSignalStrengthLte;
using Elastos::Droid::Telephony::ICellIdentityLte;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IEventLog;

using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDcTrackerBase;
using Elastos::Droid::Internal::Telephony::IProxyController;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class CdmaLteServiceStateTracker extends CdmaServiceStateTracker {
    private CDMALTEPhone mCdmaLtePhone;
    private final CellInfoLte mCellInfoLte;
    protected Int32 mNewRilRadioTechnology = 0;
    private static const Int32 EVENT_ALL_DATA_DISCONNECTED = 1001;

    private CellIdentityLte mNewCellIdentityLte = new CellIdentityLte();
    private CellIdentityLte mLasteCellIdentityLte = new CellIdentityLte();

    public CdmaLteServiceStateTracker(CDMALTEPhone phone) {
        Super(phone, new CellInfoLte());
        mCdmaLtePhone = phone;
        mCdmaLtePhone->RegisterForSimRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
        mCellInfoLte = (CellInfoLte) mCellInfo;

        ((CellInfoLte)mCellInfo).SetCellSignalStrength(new CellSignalStrengthLte());
        ((CellInfoLte)mCellInfo).SetCellIdentity(new CellIdentityLte());

        If (DBG) Log("CdmaLteServiceStateTracker Constructors");
    }

    //@Override
    CARAPI Dispose() {
        mPhone->UnregisterForSimRecordsLoaded(this);
        super->Dispose();
    }

    //@Override
    CARAPI HandleMessage(Message msg) {
        AsyncResult ar;
        Int32[] ints;
        String[] strings;

        If (!mPhone.mIsTheCurrentActivePhone) {
            Loge("Received message " + msg + "[" + msg.what + "]" +
                    " while being destroyed. Ignoring.");
            return;
        }

        If (DBG) Log("handleMessage: " + msg.what);
        Switch (msg.what) {
        case EVENT_POLL_STATE_GPRS:
            If (DBG) Log("handleMessage EVENT_POLL_STATE_GPRS");
            ar = (AsyncResult)msg.obj;
            HandlePollStateResult(msg.what, ar);
            break;
        case EVENT_RUIM_RECORDS_LOADED:
            UpdatePhoneObject();
            RuimRecords ruim = (RuimRecords)mIccRecords;
            If (ruim != NULL) {
                If (ruim->IsProvisioned()) {
                    mMdn = ruim->GetMdn();
                    mMin = ruim->GetMin();
                    ParseSidNid(ruim->GetSid(), ruim->GetNid());
                    mPrlVersion = ruim->GetPrlVersion();
                    mIsMinInfoReady = TRUE;
                }
                UpdateOtaspState();
            }
            // reload eri in case of IMSI changed
            // eri.xml can be defined by mcc mnc
            mPhone->PrepareEri();
            // SID/NID/PRL is loaded. Poll service state
            // again to update to the roaming state with
            // the latest variables.
            PollState();
            break;
        case EVENT_SIM_RECORDS_LOADED:
            UpdatePhoneObject();
            break;
        case EVENT_ALL_DATA_DISCONNECTED:
            Int64 dds = SubscriptionManager->GetDefaultDataSubId();
            ProxyController->GetInstance()->UnregisterForAllDataDisconnected(dds, this);
            Synchronized(this) {
                If (mPendingRadioPowerOffAfterDataOff) {
                    If (DBG) Log("EVENT_ALL_DATA_DISCONNECTED, turn radio off now.");
                    HangupAndPowerOff();
                    mPendingRadioPowerOffAfterDataOff = FALSE;
                } else {
                    Log("EVENT_ALL_DATA_DISCONNECTED is stale");
                }
            }
            break;
        default:
            super->HandleMessage(msg);
        }
    }

    /**
     * Handle the result of one of the PollState()-related requests
     */
    //@Override
    protected void HandlePollStateResultMessage(Int32 what, AsyncResult ar) {
        If (what == EVENT_POLL_STATE_GPRS) {
            String states[] = (String[])ar.result;
            If (DBG) {
                Log("handlePollStateResultMessage: EVENT_POLL_STATE_GPRS states.length=" +
                        states.length + " states=" + states);
            }

            Int32 type = 0;
            Int32 regState = -1;
            If (states.length > 0) {
                try {
                    regState = Integer->ParseInt(states[0]);

                    // states[3] (if present) is the current radio technology
                    If (states.length >= 4 && states[3] != NULL) {
                        type = Integer->ParseInt(states[3]);
                    }
                } Catch (NumberFormatException ex) {
                    Loge("handlePollStateResultMessage: error parsing GprsRegistrationState: "
                                    + ex);
                }
                If (states.length >= 10) {
                    Int32 mcc;
                    Int32 mnc;
                    Int32 tac;
                    Int32 pci;
                    Int32 eci;
                    Int32 csgid;
                    String operatorNumeric = NULL;

                    try {
                        operatorNumeric = mNewSS->GetOperatorNumeric();
                        mcc = Integer->ParseInt(operatorNumeric->Substring(0,3));
                    } Catch (Exception e) {
                        try {
                            operatorNumeric = mSS->GetOperatorNumeric();
                            mcc = Integer->ParseInt(operatorNumeric->Substring(0,3));
                        } Catch (Exception ex) {
                            Loge("handlePollStateResultMessage: bad mcc operatorNumeric=" +
                                    operatorNumeric + " ex=" + ex);
                            operatorNumeric = "";
                            mcc = Integer.MAX_VALUE;
                        }
                    }
                    try {
                        mnc = Integer->ParseInt(operatorNumeric->Substring(3));
                    } Catch (Exception e) {
                        Loge("handlePollStateResultMessage: bad mnc operatorNumeric=" +
                                operatorNumeric + " e=" + e);
                        mnc = Integer.MAX_VALUE;
                    }

                    // Use Integer#decode to be generous in what we receive and allow
                    // decimal, hex or octal values.
                    try {
                        tac = Integer->Decode(states[6]);
                    } Catch (Exception e) {
                        Loge("handlePollStateResultMessage: bad tac states[6]=" +
                                states[6] + " e=" + e);
                        tac = Integer.MAX_VALUE;
                    }
                    try {
                        pci = Integer->Decode(states[7]);
                    } Catch (Exception e) {
                        Loge("handlePollStateResultMessage: bad pci states[7]=" +
                                states[7] + " e=" + e);
                        pci = Integer.MAX_VALUE;
                    }
                    try {
                        eci = Integer->Decode(states[8]);
                    } Catch (Exception e) {
                        Loge("handlePollStateResultMessage: bad eci states[8]=" +
                                states[8] + " e=" + e);
                        eci = Integer.MAX_VALUE;
                    }
                    try {
                        csgid = Integer->Decode(states[9]);
                    } Catch (Exception e) {
                        // FIX: Always bad so don't pollute the logs
                        // Loge("handlePollStateResultMessage: bad csgid states[9]=" +
                        //        states[9] + " e=" + e);
                        csgid = Integer.MAX_VALUE;
                    }
                    mNewCellIdentityLte = new CellIdentityLte(mcc, mnc, eci, pci, tac);
                    If (DBG) {
                        Log("handlePollStateResultMessage: mNewLteCellIdentity=" +
                                mNewCellIdentityLte);
                    }
                }
            }

            mNewSS->SetRilDataRadioTechnology(type);
            Int32 dataRegState = RegCodeToServiceState(regState);
            mNewSS->SetDataRegState(dataRegState);
            If (DBG) {
                Log("handlPollStateResultMessage: CdmaLteSST setDataRegState=" + dataRegState
                        + " regState=" + regState
                        + " dataRadioTechnology=" + type);
            }
            mDataRoaming = RegCodeIsRoaming(regState);

            If (mDataRoaming) mNewSS->SetRoaming(TRUE);
        } else {
            super->HandlePollStateResultMessage(what, ar);
        }
    }

    //@Override
    CARAPI PollState() {
        mPollingContext = new Int32[1];
        mPollingContext[0] = 0;

        Switch (mCi->GetRadioState()) {
            case RADIO_UNAVAILABLE:
                mNewSS->SetStateOutOfService();
                mNewCellLoc->SetStateInvalid();
                SetSignalStrengthDefaultValues();
                mGotCountryCode = FALSE;

                PollStateDone();
                break;

            case RADIO_OFF:
                mNewSS->SetStateOff();
                mNewCellLoc->SetStateInvalid();
                SetSignalStrengthDefaultValues();
                mGotCountryCode = FALSE;

                PollStateDone();

                /**
                 * If iwlan feature is enabled then we do get
                 * voice_network_change indication from RIL. At this moment we
                 * dont know the current RAT since we are in Airplane mode.
                 * We have to request for current registration state and hence
                 * fallthrough to default case only if iwlan feature is
                 * applicable.
                 */
                If (!IsIwlanFeatureAvailable()) {
                    /* fall-through */
                    break;
                }

            default:
                // Issue all poll-related commands at once, then count
                // down the responses which are allowed to arrive
                // out-of-order.

                mPollingContext[0]++;
                // RIL_REQUEST_OPERATOR is necessary for CDMA
                mCi->GetOperator(ObtainMessage(EVENT_POLL_STATE_OPERATOR_CDMA, mPollingContext));

                mPollingContext[0]++;
                // RIL_REQUEST_VOICE_REGISTRATION_STATE is necessary for CDMA
                mCi->GetVoiceRegistrationState(ObtainMessage(EVENT_POLL_STATE_REGISTRATION_CDMA,
                        mPollingContext));

                mPollingContext[0]++;
                // RIL_REQUEST_DATA_REGISTRATION_STATE
                mCi->GetDataRegistrationState(ObtainMessage(EVENT_POLL_STATE_GPRS,
                                            mPollingContext));
                break;
        }
    }

    //@Override
    protected void PollStateDone() {
        // Some older CDMA/LTE RILs only report VoiceRadioTechnology which results in network
        // Unknown. In these cases return RilVoiceRadioTechnology for RilDataRadioTechnology.
        Boolean oldRil = mCi->NeedsOldRilFeature("usevoicetechfordata");
        If (mNewSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE && oldRil) {
            // LTE out of service, get CDMA Service State
            mNewRilRadioTechnology = mNewSS->GetRilVoiceRadioTechnology();
            mNewSS->SetDataRegState(RadioTechnologyToDataServiceState(mNewRilRadioTechnology));
            mNewSS->SetRilDataRadioTechnology(mNewRilRadioTechnology);
            Log("pollStateDone CDMA STATE_IN_SERVICE mNewRilRadioTechnology = " +
                    mNewRilRadioTechnology + " mNewSS->GetDataRegState() = " +
                    mNewSS->GetDataRegState());
        }

        Log("pollStateDone: lte 1 ss=[" + mSS + "] newSS=[" + mNewSS + "]");

        UseDataRegStateForDataOnlyDevices();

        Boolean hasRegistered = mSS->GetVoiceRegState() != ServiceState.STATE_IN_SERVICE
                && mNewSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE;

        Boolean hasDeregistered = mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE
                && mNewSS->GetVoiceRegState() != ServiceState.STATE_IN_SERVICE;

        Boolean hasCdmaDataConnectionAttached =
            mSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE
                && mNewSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE;

        Boolean hasCdmaDataConnectionDetached =
                mSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE
                && mNewSS->GetDataRegState() != ServiceState.STATE_IN_SERVICE;

        Boolean hasCdmaDataConnectionChanged =
            mSS->GetDataRegState() != mNewSS->GetDataRegState();

        Boolean hasVoiceRadioTechnologyChanged = mSS->GetRilVoiceRadioTechnology()
                != mNewSS->GetRilVoiceRadioTechnology();

        Boolean hasDataRadioTechnologyChanged = mSS->GetRilDataRadioTechnology()
                != mNewSS->GetRilDataRadioTechnology();

        Boolean hasChanged = !mNewSS->Equals(mSS);

        Boolean hasRoamingOn = !mSS->GetRoaming() && mNewSS->GetRoaming();

        Boolean hasRoamingOff = mSS->GetRoaming() && !mNewSS->GetRoaming();

        Boolean hasLocationChanged = !mNewCellLoc->Equals(mCellLoc);

        If (mCi->GetRadioState() == CommandsInterface.RadioState.RADIO_OFF) {
            Boolean resetIwlanRatVal = FALSE;
            Log("set service state as POWER_OFF");
            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mNewSS->GetRilDataRadioTechnology())) {
                Log("pollStateDone: mNewSS = " + mNewSS);
                Log("pollStateDone: reset iwlan RAT value");
                resetIwlanRatVal = TRUE;
            }
            mNewSS->SetStateOff();
            If (resetIwlanRatVal) {
                mNewSS->SetRilDataRadioTechnology(ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN);
                Log("pollStateDone: mNewSS = " + mNewSS);
                resetIwlanRatVal = FALSE;
            }
        }

        Boolean has4gHandoff =
                mNewSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE &&
                (((mSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) &&
                  (mNewSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD)) ||
                 ((mSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD) &&
                  (mNewSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_LTE)));

        Boolean hasMultiApnSupport =
                (((mNewSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) ||
                  (mNewSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD)) &&
                 ((mSS->GetRilDataRadioTechnology() != ServiceState.RIL_RADIO_TECHNOLOGY_LTE) &&
                  (mSS->GetRilDataRadioTechnology() != ServiceState.RIL_RADIO_TECHNOLOGY_EHRPD)));

        Boolean hasLostMultiApnSupport =
            ((mNewSS->GetRilDataRadioTechnology() >= ServiceState.RIL_RADIO_TECHNOLOGY_IS95A) &&
             (mNewSS->GetRilDataRadioTechnology() <= ServiceState.RIL_RADIO_TECHNOLOGY_EVDO_A));

        Boolean needNotifyData = (mSS->GetCssIndicator() != mNewSS->GetCssIndicator());

        If (DBG) {
            Log("pollStateDone:"
                + " hasRegistered=" + hasRegistered
                + " hasDeegistered=" + hasDeregistered
                + " hasCdmaDataConnectionAttached=" + hasCdmaDataConnectionAttached
                + " hasCdmaDataConnectionDetached=" + hasCdmaDataConnectionDetached
                + " hasCdmaDataConnectionChanged=" + hasCdmaDataConnectionChanged
                + " hasVoiceRadioTechnologyChanged= " + hasVoiceRadioTechnologyChanged
                + " hasDataRadioTechnologyChanged=" + hasDataRadioTechnologyChanged
                + " hasChanged=" + hasChanged
                + " hasRoamingOn=" + hasRoamingOn
                + " hasRoamingOff=" + hasRoamingOff
                + " hasLocationChanged=" + hasLocationChanged
                + " has4gHandoff = " + has4gHandoff
                + " hasMultiApnSupport=" + hasMultiApnSupport
                + " hasLostMultiApnSupport=" + hasLostMultiApnSupport);
        }
        // Add an event log when connection state changes
        If (mSS->GetVoiceRegState() != mNewSS->GetVoiceRegState()
                || mSS->GetDataRegState() != mNewSS->GetDataRegState()) {
            EventLog->WriteEvent(EventLogTags.CDMA_SERVICE_STATE_CHANGE, mSS->GetVoiceRegState(),
                    mSS->GetDataRegState(), mNewSS->GetVoiceRegState(), mNewSS->GetDataRegState());
        }

        ServiceState tss;
        tss = mSS;
        mSS = mNewSS;
        mNewSS = tss;
        // clean slate for next time
        mNewSS->SetStateOutOfService();

        CdmaCellLocation tcl = mCellLoc;
        mCellLoc = mNewCellLoc;
        mNewCellLoc = tcl;

        mNewSS->SetStateOutOfService(); // clean slate for next time

        If (hasVoiceRadioTechnologyChanged) {
            UpdatePhoneObject();
        }

        If (hasDataRadioTechnologyChanged) {
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
                    ServiceState->RilRadioTechnologyToString(mSS->GetRilDataRadioTechnology()));

            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                Log("pollStateDone: IWLAN enabled");
            }
        }

        If (hasRegistered) {
            mNetworkAttachedRegistrants->NotifyRegistrants();
        }

        If (hasChanged) {
            Boolean hasBrandOverride = mUiccController->GetUiccCard() == NULL ? FALSE :
                    (mUiccController->GetUiccCard()->GetOperatorBrandOverride() != NULL);
            Boolean forceEriFromXml =
                           SystemProperties->GetBoolean("ro.ril.force_eri_from_xml", FALSE);
            If (!hasBrandOverride && (mCi->GetRadioState()->IsOn()) && (mPhone->IsEriFileLoaded())
                && (!mIsSubscriptionFromRuim || forceEriFromXml)) {
                String eriText;
                // Now the CDMAPhone sees the new ServiceState so it can get the
                // new ERI text
                If (mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE ||
                        (mSS->GetDataRegState() == ServiceState.STATE_IN_SERVICE)) {
                    eriText = mPhone->GetCdmaEriText();
                } else If (mSS->GetVoiceRegState() == ServiceState.STATE_POWER_OFF) {
                    eriText = (mIccRecords != NULL) ? mIccRecords->GetServiceProviderName() : NULL;
                    If (TextUtils->IsEmpty(eriText)) {
                        // Sets operator alpha property by retrieving from
                        // build-time system property
                        eriText = SystemProperties->Get("ro.cdma.home.operator.alpha");
                    }
                } else {
                    // Note that ServiceState.STATE_OUT_OF_SERVICE is valid used
                    // for mRegistrationState 0,2,3 and 4
                    eriText = mPhone->GetContext()
                            .GetText(R::string::roamingTextSearching).ToString();
                }
                mSS->SetOperatorAlphaLong(eriText);
            }

            If (mUiccApplcation != NULL && mUiccApplcation->GetState() == AppState.APPSTATE_READY &&
                    mIccRecords != NULL &&
                    !SystemProperties->GetBoolean("ro.cdma.force_plmn_lookup", FALSE)) {
                // SIM is found on the device. If ERI roaming is OFF, and SID/NID matches
                // one configured in SIM, use operator name  from CSIM record.
                // If force plmn lookup, keep the lookup value instead
                Boolean showSpn =
                    ((RuimRecords)mIccRecords).GetCsimSpnDisplayCondition();
                Int32 iconIndex = mSS->GetCdmaEriIconIndex();

                If (showSpn && (iconIndex == EriInfo.ROAMING_INDICATOR_OFF) &&
                    IsInHomeSidNid(mSS->GetSystemId(), mSS->GetNetworkId()) &&
                    mIccRecords != NULL) {
                    mSS->SetOperatorAlphaLong(mIccRecords->GetServiceProviderName());
                }
            }

            String operatorNumeric;

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ALPHA,
                    mSS->GetOperatorAlphaLong());

            String prevOperatorNumeric =
                    SystemProperties->Get(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, "");
            operatorNumeric = mSS->GetOperatorNumeric();
            // try to fix the invalid Operator Numeric
            If (IsInvalidOperatorNumeric(operatorNumeric)) {
                Int32 sid = mSS->GetSystemId();
                operatorNumeric = FixUnknownMcc(operatorNumeric, sid);
            }
            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
            UpdateCarrierMccMncConfiguration(operatorNumeric,
                    prevOperatorNumeric, mPhone->GetContext());

            If (IsInvalidOperatorNumeric(operatorNumeric)) {
                If (DBG) Log("operatorNumeric is NULL");
                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY, "");
                mGotCountryCode = FALSE;
            } else {
                String isoCountryCode = "";
                String mcc = operatorNumeric->Substring(0, 3);
                try {
                    isoCountryCode = MccTable->CountryCodeForMcc(Integer->ParseInt(operatorNumeric
                            .Substring(0, 3)));
                } Catch (NumberFormatException ex) {
                    Loge("countryCodeForMcc error" + ex);
                } Catch (StringIndexOutOfBoundsException ex) {
                    Loge("countryCodeForMcc error" + ex);
                }

                mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISO_COUNTRY,
                        isoCountryCode);
                mGotCountryCode = TRUE;

                SetOperatorIdd(operatorNumeric);

                If (ShouldFixTimeZoneNow(mPhone, operatorNumeric, prevOperatorNumeric,
                        mNeedFixZone)) {
                    FixTimeZone(isoCountryCode);
                }
            }

            mPhone->SetSystemProperty(TelephonyProperties.PROPERTY_OPERATOR_ISROAMING,
                    mSS->GetRoaming() ? "TRUE" : "FALSE");

            UpdateSpnDisplay();
            mPhone->NotifyServiceStateChanged(mSS);
        }

        // First notify detached, then rat changed, then attached - that's the way it
        // happens in the modem.
        // Behavior of Recipients (DcTracker, for instance) depends on this sequence
        // since DcTracker reloads profiles on "rat_changed" notification and sets up
        // data call on "attached" notification.
        If (hasCdmaDataConnectionDetached) {
            mDetachedRegistrants->NotifyRegistrants();
        }

        If ((hasCdmaDataConnectionChanged || hasDataRadioTechnologyChanged)) {
            NotifyDataRegStateRilRadioTechnologyChanged();
            If (IsIwlanFeatureAvailable()
                    && (ServiceState.RIL_RADIO_TECHNOLOGY_IWLAN
                        == mSS->GetRilDataRadioTechnology())) {
                mPhone->NotifyDataConnection(Phone.REASON_IWLAN_AVAILABLE);
                needNotifyData = FALSE;
                mIwlanRatAvailable = TRUE;
            } else {
                ProcessIwlanToWwanTransition(mSS);

                needNotifyData = TRUE;
                mIwlanRatAvailable = FALSE;
            }
        }

        If (needNotifyData) {
            mPhone->NotifyDataConnection(NULL);
        }

        If (hasCdmaDataConnectionAttached || has4gHandoff) {
            mAttachedRegistrants->NotifyRegistrants();
        }

        If (hasRoamingOn) {
            mRoamingOnRegistrants->NotifyRegistrants();
        }

        If (hasRoamingOff) {
            mRoamingOffRegistrants->NotifyRegistrants();
        }

        If (hasLocationChanged) {
            mPhone->NotifyLocationChanged();
        }

        ArrayList<CellInfo> arrayCi = new ArrayList<CellInfo>();
        Synchronized(mCellInfo) {
            CellInfoLte cil = (CellInfoLte)mCellInfo;

            Boolean cidChanged = ! mNewCellIdentityLte->Equals(mLasteCellIdentityLte);
            If (hasRegistered || hasDeregistered || cidChanged) {
                // TODO: Handle the absence of LteCellIdentity
                Int64 timeStamp = SystemClock->ElapsedRealtime() * 1000;
                Boolean registered = mSS->GetVoiceRegState() == ServiceState.STATE_IN_SERVICE;
                mLasteCellIdentityLte = mNewCellIdentityLte;

                cil->SetRegistered(registered);
                cil->SetCellIdentity(mLasteCellIdentityLte);
                If (DBG) {
                    Log("pollStateDone: hasRegistered=" + hasRegistered +
                            " hasDeregistered=" + hasDeregistered +
                            " cidChanged=" + cidChanged +
                            " mCellInfo=" + mCellInfo);
                }
                arrayCi->Add(mCellInfo);
            }
            mPhoneBase->NotifyCellInfo(arrayCi);
        }
    }

    //@Override
    protected Boolean OnSignalStrengthResult(AsyncResult ar, Boolean isGsm) {
        If (mSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) {
            isGsm = TRUE;
        }
        Boolean ssChanged = super->OnSignalStrengthResult(ar, isGsm);

        Synchronized (mCellInfo) {
            If (mSS->GetRilDataRadioTechnology() == ServiceState.RIL_RADIO_TECHNOLOGY_LTE) {
                mCellInfoLte->SetTimeStamp(SystemClock->ElapsedRealtime() * 1000);
                mCellInfoLte->SetTimeStampType(CellInfo.TIMESTAMP_TYPE_JAVA_RIL);
                mCellInfoLte->GetCellSignalStrength()
                                .Initialize(mSignalStrength,SignalStrength.INVALID);
            }
            If (mCellInfoLte->GetCellIdentity() != NULL) {
                ArrayList<CellInfo> arrayCi = new ArrayList<CellInfo>();
                arrayCi->Add(mCellInfoLte);
                mPhoneBase->NotifyCellInfo(arrayCi);
            }
        }
        return ssChanged;
    }

    //@Override
    public Boolean IsConcurrentVoiceAndDataAllowed() {
        // Using the Conncurrent Service Supported flag for CdmaLte devices.
        return mSS->GetCssIndicator() == 1;
    }

    /**
     * Check whether the specified SID and NID pair appears in the HOME SID/NID list
     * read from NV or SIM.
     *
     * @return TRUE if provided sid/nid pair belongs to operator's home network.
     */
    private Boolean IsInHomeSidNid(Int32 sid, Int32 nid) {
        // if SID/NID is not available, assume this is home network.
        If (IsSidsAllZeros()) return TRUE;

        // length of SID/NID shold be same
        If (mHomeSystemId.length != mHomeNetworkId.length) return TRUE;

        If (sid == 0) return TRUE;

        For (Int32 i = 0; i < mHomeSystemId.length; i++) {
            // Use SID only if NID is a reserved value.
            // SID 0 and NID 0 and 65535 are reserved. (C.0005 2.6.5.2)
            If ((mHomeSystemId[i] == sid) &&
                ((mHomeNetworkId[i] == 0) || (mHomeNetworkId[i] == 65535) ||
                 (nid == 0) || (nid == 65535) || (mHomeNetworkId[i] == nid))) {
                return TRUE;
            }
        }
        // SID/NID are not in the list. So device is not in home network
        return FALSE;
    }

    /**
     * TODO: Remove when we get new ril/modem for Galaxy Nexus.
     *
     * @return all available cell information, the returned List maybe empty but never NULL.
     */
    //@Override
    public List<CellInfo> GetAllCellInfo() {
        If (mCi->GetRilVersion() >= 8) {
            return super->GetAllCellInfo();
        } else {
            ArrayList<CellInfo> arrayList = new ArrayList<CellInfo>();
            CellInfo ci;
            Synchronized(mCellInfo) {
                arrayList->Add(mCellInfoLte);
            }
            If (DBG) Log ("getAllCellInfo: arrayList=" + arrayList);
            return arrayList;
        }
    }

    //@Override
    protected UiccCardApplication GetUiccCardApplication() {
            return  mUiccController->GetUiccCardApplication(((CDMALTEPhone)mPhone).
                    GetPhoneId(), UiccController.APP_FAM_3GPP2);
    }

    protected void UpdateCdmaSubscription() {
        mCi->GetCDMASubscription(ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION));
    }

    /**
     * Clean up existing voice and data connection then turn off radio power.
     *
     * Hang up the existing voice calls to decrease call drop rate.
     */
    //@Override
    CARAPI PowerOffRadioSafely(DcTrackerBase dcTracker) {
        Synchronized (this) {
            If (!mPendingRadioPowerOffAfterDataOff) {
                Int64 dds = SubscriptionManager->GetDefaultDataSubId();
                // To minimize race conditions we call cleanUpAllConnections on
                // both if else paths instead of before this isDisconnected test.
                If (dcTracker->IsDisconnected()
                        && (dds == mPhone->GetSubId()
                            || (dds != mPhone->GetSubId()
                                && ProxyController->GetInstance()->IsDataDisconnected(dds)))) {
                    // To minimize race conditions we do this after isDisconnected
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    If (DBG) Log("Data disconnected, turn off radio right away.");
                    HangupAndPowerOff();
                } else {
                    dcTracker->CleanUpAllConnections(Phone.REASON_RADIO_TURNED_OFF);
                    If (dds != mPhone->GetSubId()
                            && !ProxyController->GetInstance()->IsDataDisconnected(dds)) {
                        If (DBG) Log("Data is active on DDS.  Wait for all data disconnect");
                        // Data is not disconnected on DDS. Wait for the data disconnect complete
                        // before sending the RADIO_POWER off.
                        ProxyController->GetInstance()->RegisterForAllDataDisconnected(dds, this,
                                EVENT_ALL_DATA_DISCONNECTED, NULL);
                        mPendingRadioPowerOffAfterDataOff = TRUE;
                    }
                    Message msg = Message->Obtain(this);
                    msg.what = EVENT_SET_RADIO_POWER_OFF;
                    msg.arg1 = ++mPendingRadioPowerOffAfterDataOffTag;
                    If (SendMessageDelayed(msg, 30000)) {
                        If (DBG) Log("Wait upto 30s for data to disconnect, then turn off radio.");
                        mPendingRadioPowerOffAfterDataOff = TRUE;
                    } else {
                        Log("Cannot send delayed Msg, turn off radio right away.");
                        HangupAndPowerOff();
                        mPendingRadioPowerOffAfterDataOff = FALSE;
                    }
                }
            }
        }
    }

    //@Override
    protected void UpdatePhoneObject() {
        Int32 voiceRat = mSS->GetRilVoiceRadioTechnology();
        If (mPhone->GetContext()->GetResources().
                GetBoolean(R.bool.config_switch_phone_on_voice_reg_state_change)) {
            // For CDMA-LTE phone don't update phone to GSM
            // if replacement RAT is unknown
            // If there is a  real need to switch to LTE, then it will be done via
            // RIL_UNSOL_VOICE_RADIO_TECH_CHANGED from RIL.

            Int32 volteReplacementRat = mPhoneBase->GetContext()->GetResources().GetInteger(
                    R.integer.config_volte_replacement_rat);
            Rlog->D(LOG_TAG, "updatePhoneObject: volteReplacementRat=" + volteReplacementRat);

            If (voiceRat == ServiceState.RIL_RADIO_TECHNOLOGY_LTE &&
                    volteReplacementRat == ServiceState.RIL_RADIO_TECHNOLOGY_UNKNOWN) {
                voiceRat = ServiceState.RIL_RADIO_TECHNOLOGY_1xRTT;
            }
            mPhoneBase->UpdatePhoneObject(voiceRat);
        }
    }

    //@Override
    protected void Log(String s) {
        Rlog->D(LOG_TAG, "[CdmaLteSST] " + s);
    }

    //@Override
    protected void Loge(String s) {
        Rlog->E(LOG_TAG, "[CdmaLteSST] " + s);
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        pw->Println("CdmaLteServiceStateTracker extends:");
        super->Dump(fd, pw, args);
        pw->Println(" mCdmaLtePhone=" + mCdmaLtePhone);
    }
}
