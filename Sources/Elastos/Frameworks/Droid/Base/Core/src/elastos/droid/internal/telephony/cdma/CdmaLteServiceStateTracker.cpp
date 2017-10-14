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

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

#include "elastos/droid/internal/telephony/cdma/CdmaLteServiceStateTracker.h"
#include "elastos/droid/internal/telephony/cdma/CDMALTEPhone.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/ProxyController.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/CCellIdentityLte.h"
#include "elastos/droid/telephony/CCellInfoLte.h"
#include "elastos/droid/telephony/CCellSignalStrengthLte.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::ProxyController;
using Elastos::Droid::Internal::Telephony::Uicc::AppState;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::CCellIdentityLte;
using Elastos::Droid::Telephony::CCellInfoLte;
using Elastos::Droid::Telephony::CCellSignalStrengthLte;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::ICellSignalStrengthLte;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                      CdmaLteServiceStateTracker
//=====================================================================
CAR_INTERFACE_IMPL(CdmaLteServiceStateTracker, CdmaServiceStateTracker, ICdmaLteServiceStateTracker);
const Int32 CdmaLteServiceStateTracker::EVENT_ALL_DATA_DISCONNECTED;

CdmaLteServiceStateTracker::CdmaLteServiceStateTracker()
    : mNewRilRadioTechnology(0)
{
}

ECode CdmaLteServiceStateTracker::constructor(
    /* [in] */ ICDMALTEPhone* phone)
{
    AutoPtr<ICellInfoLte> ci;
    CCellInfoLte::New((ICellInfoLte**)&ci);
    CdmaServiceStateTracker::constructor(ICDMAPhone::Probe(phone), ICellInfo::Probe(ci));
    mCdmaLtePhone = phone;
    IPhoneBase::Probe(mCdmaLtePhone)->RegisterForSimRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
    mCellInfoLte = ICellInfoLte::Probe(mCellInfo);

    AutoPtr<ICellSignalStrengthLte> css;
    CCellSignalStrengthLte::New((ICellSignalStrengthLte**)&css);
    ICellInfoLte::Probe(mCellInfo)->SetCellSignalStrength(css);
    AutoPtr<ICellIdentityLte> ciLte;
    CCellIdentityLte::New((ICellIdentityLte**)&ciLte);
    ICellInfoLte::Probe(mCellInfo)->SetCellIdentity(ciLte);

    if (DBG) Log(String("CdmaLteServiceStateTracker Constructors"));
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::Dispose()
{
    IPhoneBase::Probe(mPhone)->UnregisterForSimRecordsLoaded(this);
    CdmaServiceStateTracker::Dispose();
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<AsyncResult> ar;
    AutoPtr<ArrayOf<Int32> > ints;
    AutoPtr<ArrayOf<String> > strings;

    if (!((CDMAPhone*)mPhone.Get())->mIsTheCurrentActivePhone) {
        // Loge("Received message " + msg + "[" + msg.what + "]" +
        //         " while being destroyed. Ignoring.");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    if (DBG) Log(String("handleMessage: ") + StringUtils::ToString(what));

    switch (what) {
        case EVENT_POLL_STATE_GPRS: {
            if (DBG) Log(String("handleMessage EVENT_POLL_STATE_GPRS"));
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            HandlePollStateResult(what, ar);
            break;
        }
        case EVENT_RUIM_RECORDS_LOADED: {
            UpdatePhoneObject();
            AutoPtr<IRuimRecords> ruim = IRuimRecords::Probe(mIccRecords);
            if (ruim != NULL) {
                Boolean b;
                if (IIccRecords::Probe(ruim)->IsProvisioned(&b), b) {
                    ruim->GetMdn(&mMdn);
                    ruim->GetMin(&mMin);
                    String sid, nid;
                    ruim->GetSid(&sid);
                    ruim->GetNid(&nid);
                    ParseSidNid(sid, nid);
                    ruim->GetPrlVersion(&mPrlVersion);
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
        }
        case EVENT_SIM_RECORDS_LOADED:
            UpdatePhoneObject();
            break;
        case EVENT_ALL_DATA_DISCONNECTED: {
            Int64 dds;
            SubscriptionManager::GetDefaultDataSubId(&dds);
            AutoPtr<IProxyController> pc;
            ProxyController::GetInstance((IProxyController**)&pc);
            pc->UnregisterForAllDataDisconnected(dds, this);
            {
                AutoLock lock(this);
                if (mPendingRadioPowerOffAfterDataOff) {
                    if (DBG) Log(String("EVENT_ALL_DATA_DISCONNECTED, turn radio off now."));
                    HangupAndPowerOff();
                    mPendingRadioPowerOffAfterDataOff = FALSE;
                }
                else {
                    Log(String("EVENT_ALL_DATA_DISCONNECTED is stale"));
                }
            }
            break;
        }
        default:
            CdmaServiceStateTracker::HandleMessage(msg);
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::PollState()
{
    mPollingContext = ArrayOf<Int32>::Alloc(1);
    (*mPollingContext)[0] = 0;

    ICommandsInterfaceRadioState state;
    mCi->GetRadioState(&state);
    switch (state) {
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
            if (!IsIwlanFeatureAvailable()) {
                /* fall-through */
                break;
            }

        default: {
            // Issue all poll-related commands at once, then count
            // down the responses which are allowed to arrive
            // out-of-order.

            AutoPtr<IArrayOf> iArray;
            CArrayOf::New(EIID_IInteger32, mPollingContext->GetLength(), (IArrayOf**)&iArray);
            for (Int32 i = 0; i < mPollingContext->GetLength(); i++) {
                iArray->Set(i, CoreUtils::Convert((*mPollingContext)[i]));
            }

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            // RIL_REQUEST_OPERATOR is necessary for CDMA
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_POLL_STATE_OPERATOR_CDMA, iArray, (IMessage**)&msg);
            mCi->GetOperator(msg);

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            // RIL_REQUEST_VOICE_REGISTRATION_STATE is necessary for CDMA
            msg = NULL;
            ObtainMessage(EVENT_POLL_STATE_REGISTRATION_CDMA, iArray, (IMessage**)&msg);
            mCi->GetVoiceRegistrationState(msg);

            iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // (*mPollingContext)[0]++;
            // RIL_REQUEST_DATA_REGISTRATION_STATE
            msg = NULL;
            ObtainMessage(EVENT_POLL_STATE_GPRS, iArray, (IMessage**)&msg);
            mCi->GetDataRegistrationState(msg);
            break;
        }
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::IsConcurrentVoiceAndDataAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Using the Conncurrent Service Supported flag for CdmaLte devices.
    Int32 cssIndicator;
    mSS->GetCssIndicator(&cssIndicator);
    *result = cssIndicator == 1;
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::GetAllCellInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 ver;
    mCi->GetRilVersion(&ver);
    if (ver >= 8) {
        return CdmaServiceStateTracker::GetAllCellInfo(result);
    }
    else {
        AutoPtr<IArrayList> arrayList;
        CArrayList::New((IArrayList**)&arrayList);
        AutoPtr<ICellInfo> ci;
        {
            AutoLock lock(mCellInfo);
            arrayList->Add(mCellInfoLte);
        }
        // if (DBG) Log(String("getAllCellInfo: arrayList=") + arrayList);
        *result = IList::Probe(arrayList);
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::PowerOffRadioSafely(
    /* [in] */ IDcTrackerBase* dcTracker)
{
    AutoLock lock(this);
    if (!mPendingRadioPowerOffAfterDataOff) {
        Int64 dds;
        SubscriptionManager::GetDefaultDataSubId(&dds);
        // To minimize race conditions we call cleanUpAllConnections on
        // both if else paths instead of before this isDisconnected test.
        Int64 sid;
        IPhoneBase::Probe(mPhone)->GetSubId(&sid);
        AutoPtr<IProxyController> pc;
        ProxyController::GetInstance((IProxyController**)&pc);

        Boolean b1, b2;
        pc->IsDataDisconnected(dds, &b1);
        if ((dcTracker->IsDisconnected(&b2), b2)
                && (dds == sid || (dds != sid && b1))) {
            // To minimize race conditions we do this after isDisconnected
            dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);
            if (DBG) Log(String("Data disconnected, turn off radio right away."));
            HangupAndPowerOff();
        }
        else {
            dcTracker->CleanUpAllConnections(IPhone::REASON_RADIO_TURNED_OFF);
            if (dds != sid && !b1) {
                if (DBG) Log(String("Data is active on DDS.  Wait for all data disconnect"));
                // Data is not disconnected on DDS. Wait for the data disconnect complete
                // before sending the RADIO_POWER off.
                pc->RegisterForAllDataDisconnected(dds, this,
                        EVENT_ALL_DATA_DISCONNECTED, NULL);
                mPendingRadioPowerOffAfterDataOff = TRUE;
            }
            AutoPtr<IMessage> msg = CMessage::Obtain(this);
            msg->SetWhat(EVENT_SET_RADIO_POWER_OFF);
            msg->SetArg1(++mPendingRadioPowerOffAfterDataOffTag);
            if (SendMessageDelayed(msg, 30000, &b1), b1) {
                if (DBG) Log(String("Wait upto 30s for data to disconnect, then turn off radio."));
                mPendingRadioPowerOffAfterDataOff = TRUE;
            }
            else {
                Log(String("Cannot send delayed Msg, turn off radio right away."));
                HangupAndPowerOff();
                mPendingRadioPowerOffAfterDataOff = FALSE;
            }
        }
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CdmaLteServiceStateTracker extends:"));
    CdmaServiceStateTracker::Dump(fd, pw, args);
    String str;
    IObject::Probe(mCdmaLtePhone)->ToString(&str);
    pw->Println(String(" mCdmaLtePhone=") + str);
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::HandlePollStateResultMessage(
    /* [in] */ Int32 what,
    /* [in] */ AsyncResult* ar)
{
    if (what == EVENT_POLL_STATE_GPRS) {
        AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
        Int32 size;
        iArray->GetLength(&size);
        AutoPtr<ArrayOf<String> > states = ArrayOf<String>::Alloc(size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            iArray->Get(i, (IInterface**)&obj);
            String str;
            ICharSequence::Probe(obj)->ToString(&str);
            (*states)[i] = str;
        }

        if (DBG) {
            // Log(String("handlePollStateResultMessage: EVENT_POLL_STATE_GPRS states.length=") +
            //         size + " states=" + states);
        }

        Int32 type = 0;
        Int32 regState = -1;
        if (size > 0) {
            // try {
            regState = StringUtils::ParseInt32((*states)[0]);

            // (*states)[3] (if present) is the current radio technology
            if (size >= 4 && (*states)[3] != NULL) {
                type = StringUtils::ParseInt32((*states)[3]);
            }
            // } catch (NumberFormatException ex) {
            //     Loge("handlePollStateResultMessage: error parsing GprsRegistrationState: "
            //                     + ex);
            // }
            if (size >= 10) {
                Int32 mcc = 0;
                Int32 mnc = 0;
                Int32 tac = 0;
                Int32 pci = 0;
                Int32 eci = 0;
                Int32 csgid = 0;
                String operatorNumeric;

                // try {
                mNewSS->GetOperatorNumeric(&operatorNumeric);
                mcc = StringUtils::ParseInt32(operatorNumeric.Substring(0,3));
                // } catch (Exception e) {
                //     try {
                //         operatorNumeric = mSS.getOperatorNumeric();
                //         mcc = StringUtils::ParseInt32(operatorNumeric.substring(0,3));
                //     } catch (Exception ex) {
                //         Loge("handlePollStateResultMessage: bad mcc operatorNumeric=" +
                //                 operatorNumeric + " ex=" + ex);
                //         operatorNumeric = "";
                //         mcc = Integer.MAX_VALUE;
                //     }
                // }
                // try {
                mnc = StringUtils::ParseInt32(operatorNumeric.Substring(3));
                // } catch (Exception e) {
                //     Loge("handlePollStateResultMessage: bad mnc operatorNumeric=" +
                //             operatorNumeric + " e=" + e);
                //     mnc = Integer.MAX_VALUE;
                // }

                // Use Integer#decode to be generous in what we receive and allow
                // decimal, hex or octal values.
                // try {
// TODO: Need Integer.decode
                // tac = Integer.decode((*states)[6]);
                // } catch (Exception e) {
                //     Loge("handlePollStateResultMessage: bad tac (*states)[6]=" +
                //             (*states)[6] + " e=" + e);
                //     tac = Integer.MAX_VALUE;
                // }
                // try {
// TODO: Need Integer.decode
                // pci = Integer.decode((*states)[7]);
                // } catch (Exception e) {
                //     Loge("handlePollStateResultMessage: bad pci (*states)[7]=" +
                //             (*states)[7] + " e=" + e);
                //     pci = Integer.MAX_VALUE;
                // }
                // try {
// TODO: Need Integer.decode
                // eci = Integer.decode((*states)[8]);
                // } catch (Exception e) {
                //     Loge("handlePollStateResultMessage: bad eci (*states)[8]=" +
                //             (*states)[8] + " e=" + e);
                //     eci = Integer.MAX_VALUE;
                // }
                // try {
// TODO: Need Integer.decode
                // csgid = Integer.decode((*states)[9]);
                // } catch (Exception e) {
                //     // FIX: Always bad so don't pollute the logs
                //     // Loge("handlePollStateResultMessage: bad csgid (*states)[9]=" +
                //     //        (*states)[9] + " e=" + e);
                //     csgid = Integer.MAX_VALUE;
                // }
                CCellIdentityLte::New(mcc, mnc, eci, pci, tac, (ICellIdentityLte**)&mNewCellIdentityLte);
                if (DBG) {
                    // Log(String("handlePollStateResultMessage: mNewLteCellIdentity=") +
                    //         mNewCellIdentityLte);
                }
            }
        }

        mNewSS->SetRilDataRadioTechnology(type);
        Int32 dataRegState = RegCodeToServiceState(regState);
        mNewSS->SetDataRegState(dataRegState);
        if (DBG) {
            Log(String("handlPollStateResultMessage: CdmaLteSST setDataRegState=")
                    + StringUtils::ToString(dataRegState)
                    + " regState=" + StringUtils::ToString(regState)
                    + " dataRadioTechnology=" + StringUtils::ToString(type));
        }
        mDataRoaming = RegCodeIsRoaming(regState);

        if (mDataRoaming) mNewSS->SetRoaming(TRUE);
    }
    else {
        CdmaServiceStateTracker::HandlePollStateResultMessage(what, ar);
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::PollStateDone()
{
    // Some older CDMA/LTE RILs only report VoiceRadioTechnology which results in network
    // Unknown. In these cases return RilVoiceRadioTechnology for RilDataRadioTechnology.
    Boolean oldRil;
    mCi->NeedsOldRilFeature(String("usevoicetechfordata"), &oldRil);
    Int32 newDataRegState;
    mNewSS->GetDataRegState(&newDataRegState);
    if (newDataRegState != IServiceState::STATE_IN_SERVICE && oldRil) {
        // LTE out of service, get CDMA Service State
        mNewSS->GetRilVoiceRadioTechnology(&mNewRilRadioTechnology);
        mNewSS->SetDataRegState(RadioTechnologyToDataServiceState(mNewRilRadioTechnology));
        mNewSS->SetRilDataRadioTechnology(mNewRilRadioTechnology);
        Log(String("pollStateDone CDMA STATE_IN_SERVICE mNewRilRadioTechnology = ") +
                StringUtils::ToString(mNewRilRadioTechnology)
                + " mNewSS.getDataRegState() = " +
                StringUtils::ToString(newDataRegState));
    }

    // Log(String("pollStateDone: lte 1 ss=[") + mSS + "] newSS=[" + mNewSS + "]");

    UseDataRegStateForDataOnlyDevices();

    Int32 voiceRegState;
    mSS->GetVoiceRegState(&voiceRegState);
    Int32 newVoiceRegState;
    mNewSS->GetVoiceRegState(&newVoiceRegState);
    Boolean hasRegistered = voiceRegState != IServiceState::STATE_IN_SERVICE
            && newVoiceRegState == IServiceState::STATE_IN_SERVICE;

    Boolean hasDeregistered = voiceRegState == IServiceState::STATE_IN_SERVICE
            && newVoiceRegState != IServiceState::STATE_IN_SERVICE;

    Int32 dataRegState;
    mSS->GetDataRegState(&dataRegState);
    Boolean hasCdmaDataConnectionAttached =
        dataRegState != IServiceState::STATE_IN_SERVICE
            && newDataRegState == IServiceState::STATE_IN_SERVICE;

    Boolean hasCdmaDataConnectionDetached =
            dataRegState == IServiceState::STATE_IN_SERVICE
            && newDataRegState != IServiceState::STATE_IN_SERVICE;

    Boolean hasCdmaDataConnectionChanged =
        dataRegState != newDataRegState;

    Int32 rilVoiceRadioTechnology;
    mSS->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
    Int32 newRilVoiceRadioTechnology;
    mNewSS->GetRilVoiceRadioTechnology(&newRilVoiceRadioTechnology);
    Boolean hasVoiceRadioTechnologyChanged = rilVoiceRadioTechnology
            != newRilVoiceRadioTechnology;

    Int32 rilDataRadioTechnology;
    mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    Int32 newRilDataRadioTechnology;
    mNewSS->GetRilDataRadioTechnology(&newRilDataRadioTechnology);
    Boolean hasDataRadioTechnologyChanged = rilDataRadioTechnology
            != newRilDataRadioTechnology;

    Boolean b1, b2;
    Boolean hasChanged = (IObject::Probe(mNewSS)->Equals(mSS, &b1), !b1);

    Boolean hasRoamingOn = (mSS->GetRoaming(&b1), !b1) && (mNewSS->GetRoaming(&b2), b2);

    Boolean hasRoamingOff = (mSS->GetRoaming(&b1), b1) && (mNewSS->GetRoaming(&b2), !b2);

    Boolean hasLocationChanged = (IObject::Probe(mNewCellLoc)->Equals(mCellLoc, &b1), !b1);

    ICommandsInterfaceRadioState rs;
    mCi->GetRadioState(&rs);
    if (rs == Elastos::Droid::Internal::Telephony::RADIO_OFF) {
        Boolean resetIwlanRatVal = FALSE;
        Log(String("set service state as POWER_OFF"));
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == newRilDataRadioTechnology)) {
            // Log(String("pollStateDone: mNewSS = ") + mNewSS);
            Log(String("pollStateDone: reset iwlan RAT value"));
            resetIwlanRatVal = TRUE;
        }
        mNewSS->SetStateOff();
        if (resetIwlanRatVal) {
            mNewSS->SetRilDataRadioTechnology(IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN);
            // Log(String("pollStateDone: mNewSS = ") + mNewSS);
            resetIwlanRatVal = FALSE;
        }
    }

    Boolean has4gHandoff =
            newDataRegState == IServiceState::STATE_IN_SERVICE &&
            (((rilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) &&
              (newRilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD)) ||
             ((rilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD) &&
              (newRilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_LTE)));

    Boolean hasMultiApnSupport =
            (((newRilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) ||
              (newRilDataRadioTechnology == IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD)) &&
             ((rilDataRadioTechnology != IServiceState::RIL_RADIO_TECHNOLOGY_LTE) &&
              (rilDataRadioTechnology != IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD)));

    Boolean hasLostMultiApnSupport =
        ((newRilDataRadioTechnology >= IServiceState::RIL_RADIO_TECHNOLOGY_IS95A) &&
         (newRilDataRadioTechnology <= IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A));

    Int32 cssIndicator;
    mSS->GetCssIndicator(&cssIndicator);
    Int32 newCssIndicator;
    mNewSS->GetCssIndicator(&newCssIndicator);
    Boolean needNotifyData = (cssIndicator != newCssIndicator);

    if (DBG) {
        Log(String("pollStateDone:")
            + " hasRegistered=" + StringUtils::BooleanToString(hasRegistered)
            + " hasDeegistered=" + StringUtils::BooleanToString(hasDeregistered)
            + " hasCdmaDataConnectionAttached=" + StringUtils::BooleanToString(hasCdmaDataConnectionAttached)
            + " hasCdmaDataConnectionDetached=" + StringUtils::BooleanToString(hasCdmaDataConnectionDetached)
            + " hasCdmaDataConnectionChanged=" + StringUtils::BooleanToString(hasCdmaDataConnectionChanged)
            + " hasVoiceRadioTechnologyChanged= " + StringUtils::BooleanToString(hasVoiceRadioTechnologyChanged)
            + " hasDataRadioTechnologyChanged=" + StringUtils::BooleanToString(hasDataRadioTechnologyChanged)
            + " hasChanged=" + StringUtils::BooleanToString(hasChanged)
            + " hasRoamingOn=" + StringUtils::BooleanToString(hasRoamingOn)
            + " hasRoamingOff=" + StringUtils::BooleanToString(hasRoamingOff)
            + " hasLocationChanged=" + StringUtils::BooleanToString(hasLocationChanged)
            + " has4gHandoff = " + StringUtils::BooleanToString(has4gHandoff)
            + " hasMultiApnSupport=" + StringUtils::BooleanToString(hasMultiApnSupport)
            + " hasLostMultiApnSupport=" + StringUtils::BooleanToString(hasLostMultiApnSupport));
    }
    // Add an event log when connection state changes
    if (voiceRegState != newVoiceRegState
            || dataRegState != newDataRegState) {
        // EventLog::WriteEvent(EventLogTags.CDMA_SERVICE_STATE_CHANGE, voiceRegState,
        //         dataRegState, newVoiceRegState, newDataRegState);
    }

    AutoPtr<IServiceState> tss;
    tss = mSS;
    mSS = mNewSS;
    mNewSS = tss;
    // clean slate for next time
    mNewSS->SetStateOutOfService();

    AutoPtr<ICdmaCellLocation> tcl = mCellLoc;
    mCellLoc = mNewCellLoc;
    mNewCellLoc = tcl;

    mNewSS->SetStateOutOfService(); // clean slate for next time

    if (hasVoiceRadioTechnologyChanged) {
        UpdatePhoneObject();
    }

    String str;
    if (hasDataRadioTechnologyChanged) {
        mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE,
                (CServiceState::RilRadioTechnologyToString(rilDataRadioTechnology, &str), str));

        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            Log(String("pollStateDone: IWLAN enabled"));
        }
    }

    if (hasRegistered) {
        mNetworkAttachedRegistrants->NotifyRegistrants();
    }

    Boolean b;
    if (hasChanged) {
        AutoPtr<IUiccCard> uc;
        mUiccController->GetUiccCard((IUiccCard**)&uc);
        Boolean hasBrandOverride = uc == NULL ? FALSE :
                (!(uc->GetOperatorBrandOverride(&str),str).IsNull());
        Boolean forceEriFromXml;
        ICommandsInterfaceRadioState state;
        mCi->GetRadioState(&state);
        Boolean isOn = FALSE;
// TODO: Need ICommandsInterfaceRadioState::IsOn
        // state->IsOn(&ison);
        SystemProperties::GetBoolean(String("ro.ril.force_eri_from_xml"), FALSE, &forceEriFromXml);
        if (!hasBrandOverride && isOn && (mPhone->IsEriFileLoaded(&b), b)
            && (!mIsSubscriptionFromRuim || forceEriFromXml)) {
            String eriText;
            // Now the CDMAPhone sees the new ServiceState so it can get the
            // new ERI text
            mSS->GetVoiceRegState(&voiceRegState);
            if (voiceRegState == IServiceState::STATE_IN_SERVICE ||
                    (dataRegState == IServiceState::STATE_IN_SERVICE)) {
                IPhone::Probe(mPhone)->GetCdmaEriText(&eriText);
            }
            else if (voiceRegState == IServiceState::STATE_POWER_OFF) {
                eriText = (mIccRecords != NULL) ? (mIccRecords->GetServiceProviderName(&str), str) : String(NULL);
                if (eriText.IsEmpty()) {
                    // Sets operator alpha property by retrieving from
                    // build-time system property
                    SystemProperties::Get(String("ro.cdma.home.operator.alpha"), &eriText);
                }
            }
            else {
                // Note that IServiceState::STATE_OUT_OF_SERVICE is valid used
                // for mRegistrationState 0,2,3 and 4
                AutoPtr<IContext> ctx;
                IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
                AutoPtr<ICharSequence> cs;
                ctx->GetText(R::string::roamingTextSearching, (ICharSequence**)&cs);
                cs->ToString(&eriText);
            }
            mSS->SetOperatorAlphaLong(eriText);
        }

        SystemProperties::GetBoolean(String("ro.cdma.force_plmn_lookup"), FALSE, &b);
        AppState appState;
        if (mUiccApplcation != NULL &&
                (mUiccApplcation->GetState(&appState), appState) ==
                Elastos::Droid::Internal::Telephony::Uicc::APPSTATE_READY &&
                mIccRecords != NULL && !b) {
            // SIM is found on the device. If ERI roaming is OFF, and SID/NID matches
            // one configured in SIM, use operator name  from CSIM record.
            // If force plmn lookup, keep the lookup value instead
            Boolean showSpn;
            IRuimRecords::Probe(mIccRecords)->GetCsimSpnDisplayCondition(&showSpn);
            Int32 iconIndex;
            mSS->GetCdmaEriIconIndex(&iconIndex);

            Int32 sysId, netId;
            mSS->GetSystemId(&sysId);
            mSS->GetNetworkId(&netId);
            if (showSpn && (iconIndex == IEriInfo::ROAMING_INDICATOR_OFF) &&
                IsInHomeSidNid(sysId, netId) &&
                mIccRecords != NULL) {
                mIccRecords->GetServiceProviderName(&str);
                mSS->SetOperatorAlphaLong(str);
            }
        }

        String operatorNumeric;

        String operatorAlphaLong;
        mSS->GetOperatorAlphaLong(&operatorAlphaLong);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ALPHA, operatorAlphaLong);

        String prevOperatorNumeric;
        SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, String(""), &prevOperatorNumeric);
        mSS->GetOperatorNumeric(&operatorNumeric);
        // try to fix the invalid Operator Numeric
        if (IsInvalidOperatorNumeric(operatorNumeric)) {
            Int32 sid;
            mSS->GetSystemId(&sid);
            operatorNumeric = FixUnknownMcc(operatorNumeric, sid);
        }
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        UpdateCarrierMccMncConfiguration(operatorNumeric, prevOperatorNumeric, ctx);

        if (IsInvalidOperatorNumeric(operatorNumeric)) {
            if (DBG) Log(String("operatorNumeric is NULL"));
            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""));
            mGotCountryCode = FALSE;
        }
        else {
            String isoCountryCode = String("");
            String mcc = operatorNumeric.Substring(0, 3);
            // try {
            isoCountryCode = MccTable::CountryCodeForMcc(
                    StringUtils::ParseInt32(operatorNumeric.Substring(0, 3)));
            // } catch (NumberFormatException ex) {
            //     Loge("countryCodeForMcc error" + ex);
            // } catch (StringIndexOutOfBoundsException ex) {
            //     Loge("countryCodeForMcc error" + ex);
            // }

            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY,
                    isoCountryCode);
            mGotCountryCode = TRUE;

            SetOperatorIdd(operatorNumeric);

            if (ShouldFixTimeZoneNow(IPhoneBase::Probe(mPhone), operatorNumeric, prevOperatorNumeric,
                    mNeedFixZone)) {
                FixTimeZone(isoCountryCode);
            }
        }

        Boolean b;
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISROAMING,
                (mSS->GetRoaming(&b), b) ? String("TRUE") : String("FALSE"));

        UpdateSpnDisplay();
        ((CDMAPhone*)mPhone.Get())->NotifyServiceStateChanged(mSS);
    }

    // First notify detached, then rat changed, then attached - that's the way it
    // happens in the modem.
    // Behavior of recipients (DcTracker, for instance) depends on this sequence
    // since DcTracker reloads profiles on "rat_changed" notification and sets up
    // data call on "attached" notification.
    if (hasCdmaDataConnectionDetached) {
        mDetachedRegistrants->NotifyRegistrants();
    }

    if ((hasCdmaDataConnectionChanged || hasDataRadioTechnologyChanged)) {
        NotifyDataRegStateRilRadioTechnologyChanged();
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            IPhoneBase::Probe(mPhone)->NotifyDataConnection(IPhone::REASON_IWLAN_AVAILABLE);
            needNotifyData = FALSE;
            mIwlanRatAvailable = TRUE;
        }
        else {
            ProcessIwlanToWwanTransition(mSS);

            needNotifyData = TRUE;
            mIwlanRatAvailable = FALSE;
        }
    }

    if (needNotifyData) {
        IPhoneBase::Probe(mPhone)->NotifyDataConnection(String(NULL));
    }

    if (hasCdmaDataConnectionAttached || has4gHandoff) {
        mAttachedRegistrants->NotifyRegistrants();
    }

    if (hasRoamingOn) {
        mRoamingOnRegistrants->NotifyRegistrants();
    }

    if (hasRoamingOff) {
        mRoamingOffRegistrants->NotifyRegistrants();
    }

    if (hasLocationChanged) {
        ((CDMAPhone*)mPhone.Get())->NotifyLocationChanged();
    }

    AutoPtr<IArrayList> arrayCi;
    CArrayList::New((IArrayList**)&arrayCi);

    {
        AutoLock lock(mCellInfo);
        AutoPtr<ICellInfoLte> cil = ICellInfoLte::Probe(mCellInfo);

        Boolean cidChanged = (IObject::Probe(mNewCellIdentityLte)->Equals(mLasteCellIdentityLte, &b), !b);
        if (hasRegistered || hasDeregistered || cidChanged) {
            // TODO: Handle the absence of LteCellIdentity
            Int64 timeStamp = SystemClock::GetElapsedRealtime() * 1000;
            mSS->GetVoiceRegState(&voiceRegState);
            Boolean registered = voiceRegState == IServiceState::STATE_IN_SERVICE;
            mLasteCellIdentityLte = mNewCellIdentityLte;

            ICellInfo::Probe(cil)->SetRegistered(registered);
            cil->SetCellIdentity(mLasteCellIdentityLte);
            if (DBG) {
                // Log(String("pollStateDone: hasRegistered=") + hasRegistered +
                //         " hasDeregistered=" + hasDeregistered +
                //         " cidChanged=" + cidChanged +
                //         " mCellInfo=" + mCellInfo);
            }
            arrayCi->Add(mCellInfo);
        }
        mPhoneBase->NotifyCellInfo(IList::Probe(arrayCi));
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::OnSignalStrengthResult(
    /* [in] */ AsyncResult* ar,
    /* [in] */ Boolean isGsm,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 val;
    mSS->GetRilDataRadioTechnology(&val);
    if (val == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) {
        isGsm = TRUE;
    }
    Boolean ssChanged = CdmaServiceStateTracker::OnSignalStrengthResult(ar, isGsm);

    AutoLock lock(mCellInfo);
    mSS->GetRilDataRadioTechnology(&val);
    if (val == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) {
        ICellInfo::Probe(mCellInfoLte)->SetTimeStamp(SystemClock::GetElapsedRealtime() * 1000);
        ICellInfo::Probe(mCellInfoLte)->SetTimeStampType(ICellInfo::TIMESTAMP_TYPE_JAVA_RIL);
        AutoPtr<ICellSignalStrengthLte> css;
        mCellInfoLte->GetCellSignalStrength((ICellSignalStrengthLte**)&css);
        css->Initialize(mSignalStrength, ISignalStrength::INVALID);
    }

    AutoPtr<ICellIdentityLte> ci;
    mCellInfoLte->GetCellIdentity((ICellIdentityLte**)&ci);
    if (ci != NULL) {
        AutoPtr<IArrayList> arrayCi;
        CArrayList::New((IArrayList**)&arrayCi);
        arrayCi->Add(mCellInfoLte);
        mPhoneBase->NotifyCellInfo(IList::Probe(arrayCi));
    }

    *result = ssChanged;
    return NOERROR;
}

AutoPtr<IUiccCardApplication> CdmaLteServiceStateTracker::GetUiccCardApplication()
{
    Int32 id;
    ((CDMALTEPhone*)mPhone.Get())->GetPhoneId(&id);
    AutoPtr<IUiccCardApplication> app;
    mUiccController->GetUiccCardApplication(id, IUiccController::APP_FAM_3GPP2, (IUiccCardApplication**)&app);
    return app;
}

void CdmaLteServiceStateTracker::UpdateCdmaSubscription()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION, (IMessage**)&msg);
    mCi->GetCDMASubscription(msg);
}

ECode CdmaLteServiceStateTracker::UpdatePhoneObject()
{
    Int32 voiceRat;
    mSS->GetRilVoiceRadioTechnology(&voiceRat);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    Boolean b;
    res->GetBoolean(R::bool_::config_switch_phone_on_voice_reg_state_change, &b);
    if (b) {
        // For CDMA-LTE phone don't update phone to GSM
        // if replacement RAT is unknown
        // If there is a  real need to switch to LTE, then it will be done via
        // RIL_UNSOL_VOICE_RADIO_TECH_CHANGED from RIL.

        ctx = NULL;
        IPhone::Probe(mPhoneBase)->GetContext((IContext**)&ctx);
        res = NULL;
        ctx->GetResources((IResources**)&res);
        Int32 volteReplacementRat;
        res->GetInteger(R::integer::config_volte_replacement_rat, &volteReplacementRat);
        Logger::D("CdmaLteServiceStateTracker", "updatePhoneObject: volteReplacementRat=%d", volteReplacementRat);

        if (voiceRat == IServiceState::RIL_RADIO_TECHNOLOGY_LTE &&
                volteReplacementRat == IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN) {
            voiceRat = IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT;
        }
        IPhone::Probe(mPhoneBase)->UpdatePhoneObject(voiceRat);
    }
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::Log(
    /* [in] */ const String& s)
{
    Logger::D("CdmaLteServiceStateTracker", "[CdmaLteSST] %s", s.string());
    return NOERROR;
}

ECode CdmaLteServiceStateTracker::Loge(
    /* [in] */ const String& s)
{
    Logger::E("CdmaLteServiceStateTracker", "[CdmaLteSST] %s", s.string());
    return NOERROR;
}

Boolean CdmaLteServiceStateTracker::IsInHomeSidNid(
    /* [in] */ Int32 sid,
    /* [in] */ Int32 nid)
{
    // if SID/NID is not available, assume this is home network.
    if (IsSidsAllZeros()) return TRUE;

    // length of SID/NID shold be same
    if (mHomeSystemId->GetLength() != mHomeNetworkId->GetLength()) return TRUE;

    if (sid == 0) return TRUE;

    for (Int32 i = 0; i < mHomeSystemId->GetLength(); i++) {
        // Use SID only if NID is a reserved value.
        // SID 0 and NID 0 and 65535 are reserved. (C.0005 2.6.5.2)
        if (((*mHomeSystemId)[i] == sid) &&
            (((*mHomeNetworkId)[i] == 0) || ((*mHomeNetworkId)[i] == 65535) ||
             (nid == 0) || (nid == 65535) || ((*mHomeNetworkId)[i] == nid))) {
            return TRUE;
        }
    }
    // SID/NID are not in the list. So device is not in home network
    return FALSE;
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
