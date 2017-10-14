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
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CResources.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CDMAPhone.h"
#include "elastos/droid/internal/telephony/cdma/CdmaServiceStateTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/internal/telephony/HbpcdUtils.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/Operators.h"
#include "elastos/droid/internal/telephony/ServiceStateTracker.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CRegistrant.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/R.h"
#include "elastos/droid/telephony/cdma/CCdmaCellLocation.h"
#include "elastos/droid/telephony/CCellInfoCdma.h"
#include "elastos/droid/telephony/CServiceState.h"
#include "elastos/droid/telephony/CSignalStrength.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Telephony::CPhoneFactory;
using Elastos::Droid::Internal::Telephony::HbpcdUtils;
using Elastos::Droid::Internal::Telephony::ICommandsInterfaceRadioState;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::Operators;
using Elastos::Droid::Internal::Telephony::ServiceStateTracker;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::CRegistrant;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::R;
using Elastos::Droid::Telephony::Cdma::CCdmaCellLocation;
using Elastos::Droid::Telephony::CCellInfoCdma;
using Elastos::Droid::Telephony::CServiceState;
using Elastos::Droid::Telephony::CSignalStrength;
using Elastos::Droid::Telephony::ICellInfoCdma;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::CArrayOf;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger64;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Core::StringUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::IDate;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//            CdmaServiceStateTracker::InnerContentObserver
//=====================================================================
CdmaServiceStateTracker::InnerContentObserver::InnerContentObserver(
    /* [in] */ CdmaServiceStateTracker* owner)
    : mOwner(owner)
{
}

ECode CdmaServiceStateTracker::InnerContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (DBG) mOwner->Log(String("Auto time state changed"));
    mOwner->RevertToNitzTime();
    return NOERROR;
}

//=====================================================================
//            CdmaServiceStateTracker::InnerContentObserver1
//=====================================================================
CdmaServiceStateTracker::InnerContentObserver1::InnerContentObserver1(
    /* [in] */ CdmaServiceStateTracker* owner)
    : mOwner(owner)
{
}

ECode CdmaServiceStateTracker::InnerContentObserver1::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (DBG) mOwner->Log(String("Auto time zone state changed"));
    mOwner->RevertToNitzTimeZone();
    return NOERROR;
}

//=====================================================================
//                       CdmaServiceStateTracker
//=====================================================================
CAR_INTERFACE_IMPL(CdmaServiceStateTracker, ServiceStateTracker, ICdmaServiceStateTracker);

const String CdmaServiceStateTracker::LOGTAG("CdmaSST");
const String CdmaServiceStateTracker::INVALID_MCC("000");
const String CdmaServiceStateTracker::DEFAULT_MNC("00");
const String CdmaServiceStateTracker::UNACTIVATED_MIN2_VALUE("000000");
const String CdmaServiceStateTracker::UNACTIVATED_MIN_VALUE("1111110111");
const Int32 CdmaServiceStateTracker::MS_PER_HOUR;
const Int32 CdmaServiceStateTracker::NITZ_UPDATE_SPACING_DEFAULT;
const Int32 CdmaServiceStateTracker::NITZ_UPDATE_DIFF_DEFAULT;
const String CdmaServiceStateTracker::WAKELOCK_TAG("ServiceStateTracker");

CdmaServiceStateTracker::CdmaServiceStateTracker()
{
}

ECode CdmaServiceStateTracker::constructor()
{
    return NOERROR;
}

ECode CdmaServiceStateTracker::constructor(
    /* [in] */ ICDMAPhone* phone)
{
    AutoPtr<ICellInfoCdma> ci;
    CCellInfoCdma::New((ICellInfoCdma**)&ci);
    constructor(phone, ICellInfo::Probe(ci));
    return NOERROR;
}

ECode CdmaServiceStateTracker::Dispose()
{
    CheckCorrectThread();
    Log(String("ServiceStateTracker dispose"));

    // Unregister for all events.
    mCi->UnregisterForRadioStateChanged(this);
    mCi->UnregisterForVoiceNetworkStateChanged(this);
    mCi->UnregisterForCdmaOtaProvision(this);
    mPhone->UnregisterForEriFileLoaded(this);
    UnregisterForRuimEvents();
    mCi->UnSetOnNITZTime(this);
    mCr->UnregisterContentObserver(mAutoTimeObserver);
    mCr->UnregisterContentObserver(mAutoTimeZoneObserver);
    mCdmaSSM->Dispose(this);
    mCi->UnregisterForCdmaPrlChanged(this);
    ServiceStateTracker::Dispose();
    return NOERROR;
}

ECode CdmaServiceStateTracker::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(h);
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IRegistrant> r;
    CRegistrant::New(h, what, obj, (IRegistrant**)&r);
    mCdmaForSubscriptionInfoReadyRegistrants->Add(r);

    Boolean b;
    if (IsMinInfoReady(&b), b) {
        r->NotifyRegistrant();
    }
    return NOERROR;
}

ECode CdmaServiceStateTracker::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    VALIDATE_NOT_NULL(h);
    mCdmaForSubscriptionInfoReadyRegistrants->Remove(h);
    return NOERROR;
}

ECode CdmaServiceStateTracker::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<AsyncResult> ar;
    AutoPtr<ArrayOf<Int32> > ints;
    AutoPtr<ArrayOf<String> > strings;

    if (!((CDMAPhone*)mPhone.Get())->mIsTheCurrentActivePhone) {
        // Loge(String("Received message ") + msg + "[" + msg.what + "]" +
        //         " while being destroyed. Ignoring.");
        return NOERROR;
    }

    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
    case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED: {
        Int32 val;
        mCdmaSSM->GetCdmaSubscriptionSource(&val);
        HandleCdmaSubscriptionSource(val);
        break;
    }
    case EVENT_RUIM_READY: {
        Int32 networkType;
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        Int32 pid;
        IPhone::Probe(mPhone)->GetPhoneId(&pid);
        AutoPtr<IPhoneFactory> pf;
        CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
        pf->CalculatePreferredNetworkType(ctx, pid, &networkType);
        mCi->SetPreferredNetworkType(networkType, NULL);

        if (DBG) Log(String("Receive EVENT_RUIM_READY"));
        PollState();
        AutoPtr<IResources> res;
        ctx->GetResources((IResources**)&res);
        Boolean skipRestoringSelection;
        res->GetBoolean(
                R::bool_::skip_restoring_network_selection, &skipRestoringSelection);
        if (!skipRestoringSelection) {
             // Only support automatic selection mode in CDMA.
             IPhone::Probe(mPhone)->SetNetworkSelectionModeAutomatic(NULL);
        }

        mPhone->PrepareEri();
        break;
    }
    case EVENT_NV_READY:
        UpdatePhoneObject();

        // Only support automatic selection mode in CDMA.
        IPhone::Probe(mPhone)->SetNetworkSelectionModeAutomatic(NULL);

        // For Non-RUIM phones, the subscription information is stored in
        // Non Volatile. Here when Non-Volatile is ready, we can poll the CDMA
        // subscription info.
        GetSubscriptionInfoAndStartPollingThreads();
        break;

    case EVENT_RADIO_STATE_CHANGED: {
        ICommandsInterfaceRadioState state;
        mCi->GetRadioState(&state);
        if(state == Elastos::Droid::Internal::Telephony::RADIO_ON) {
            Int32 val;
            mCdmaSSM->GetCdmaSubscriptionSource(&val);
            HandleCdmaSubscriptionSource(val);

            // Signal strength polling stops when radio is off.
            QueueNextSignalStrengthPoll();
        }
        // This will do nothing in the 'radio not available' case.
        SetPowerStateToDesired();
        PollState();
        break;
    }
    case EVENT_NETWORK_STATE_CHANGED_CDMA:
        PollState();
        break;

    case EVENT_GET_SIGNAL_STRENGTH: {
        // This callback is called when signal strength is polled
        // all by itself.

        ICommandsInterfaceRadioState state;
        mCi->GetRadioState(&state);
// TODO: Need Elastos::Droid::Internal::Telephony::IsOn
        // if (!(state->IsOn())) {
        //     // Polling will continue when radio turns back on.
        //     return;
        // }
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        OnSignalStrengthResult(ar, FALSE);
        QueueNextSignalStrengthPoll();

        break;
    }
    case EVENT_GET_LOC_DONE_CDMA: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);

        if (ar->mException == NULL) {
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 len;
            iArray->GetLength(&len);
            AutoPtr<ArrayOf<String> > states = ArrayOf<String>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                obj = NULL;
                iArray->Get(i, (IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&(*states)[i]);
            }

            Int32 baseStationId = -1;
            Int32 baseStationLatitude = ICdmaCellLocation::INVALID_LAT_LONG;
            Int32 baseStationLongitude = ICdmaCellLocation::INVALID_LAT_LONG;
            Int32 systemId = -1;
            Int32 networkId = -1;

            if (states->GetLength() > 9) {
                // try {
                if ((*states)[4] != NULL) {
                    baseStationId = StringUtils::ParseInt32((*states)[4]);
                }
                if ((*states)[5] != NULL) {
                    baseStationLatitude = StringUtils::ParseInt32((*states)[5]);
                }
                if ((*states)[6] != NULL) {
                    baseStationLongitude = StringUtils::ParseInt32((*states)[6]);
                }
                // Some carriers only return lat-lngs of 0,0
                if (baseStationLatitude == 0 && baseStationLongitude == 0) {
                    baseStationLatitude  = ICdmaCellLocation::INVALID_LAT_LONG;
                    baseStationLongitude = ICdmaCellLocation::INVALID_LAT_LONG;
                }
                if ((*states)[8] != NULL) {
                    systemId = StringUtils::ParseInt32((*states)[8]);
                }
                if ((*states)[9] != NULL) {
                    networkId = StringUtils::ParseInt32((*states)[9]);
                }
                // } catch (NumberFormatException ex) {
                //     Loge(String("error parsing cell location data: ") + ex);
                // }
            }

            mCellLoc->SetCellLocationData(baseStationId, baseStationLatitude,
                    baseStationLongitude, systemId, networkId);
            ((CDMAPhone*)mPhone.Get())->NotifyLocationChanged();
        }

        // Release any temporary cell lock, which could have been
        // acquired to allow a single-shot location update.
        DisableSingleLocationUpdate();
        break;
    }
    case EVENT_POLL_STATE_REGISTRATION_CDMA:
    case EVENT_POLL_STATE_OPERATOR_CDMA:
    case EVENT_POLL_STATE_GPRS: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        HandlePollStateResult(what, ar);
        break;
    }
    case EVENT_POLL_STATE_CDMA_SUBSCRIPTION:  {// Handle RIL_CDMA_SUBSCRIPTION
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);

        if (ar->mException == NULL) {
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 len;
            iArray->GetLength(&len);
            AutoPtr<ArrayOf<String> > cdmaSubscription = ArrayOf<String>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                obj = NULL;
                iArray->Get(i, (IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&(*cdmaSubscription)[i]);
            }

            if (cdmaSubscription != NULL && cdmaSubscription->GetLength() >= 5) {
                if (NULL != (*cdmaSubscription)[0]) {
                    mMdn = (*cdmaSubscription)[0];
                }
                ParseSidNid((*cdmaSubscription)[1], (*cdmaSubscription)[2]);

                if (NULL != (*cdmaSubscription)[3]) {
                    mMin = (*cdmaSubscription)[3];
                }
                if (NULL != (*cdmaSubscription)[4]) {
                    mPrlVersion = (*cdmaSubscription)[4];
                }
                if (DBG) Log(String("GET_CDMA_SUBSCRIPTION: MDN=") + mMdn);

                mIsMinInfoReady = TRUE;

                UpdateOtaspState();
                if (!mIsSubscriptionFromRuim && mIccRecords != NULL) {
                    if (DBG) {
                        Log(String("GET_CDMA_SUBSCRIPTION set imsi in mIccRecords"));
                    }
                    String str;
                    GetImsi(&str);
                    mIccRecords->SetImsi(str);
                } else {
                    if (DBG) {
                        Log(String("GET_CDMA_SUBSCRIPTION either mIccRecords is NULL  or NV type device - not setting Imsi in mIccRecords"));
                    }
                }
            } else {
                if (DBG) {
                    Log(String("GET_CDMA_SUBSCRIPTION: error parsing cdmaSubscription params num=")
                        + StringUtils::ToString(cdmaSubscription->GetLength()));
                }
            }
        }
        break;
    }
    case EVENT_POLL_SIGNAL_STRENGTH: {
        // Just poll signal strength...not part of PollState()
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_GET_SIGNAL_STRENGTH, (IMessage**)&msg);
        mCi->GetSignalStrength(msg);
        break;
    }
    case EVENT_NITZ_TIME: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);

        AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
        obj = NULL;
        iArray->Get(0, (IInterface**)&obj);
        String nitzString;
        ICharSequence::Probe(obj)->ToString(&nitzString);

        obj = NULL;
        iArray->Get(1, (IInterface**)&obj);
        Int64 nitzReceiveTime;
        IInteger64::Probe(obj)->GetValue(&nitzReceiveTime);

        SetTimeFromNITZString(nitzString, nitzReceiveTime);
        break;
    }
    case EVENT_SIGNAL_STRENGTH_UPDATE: {
        // This is a notification from CommandsInterface.setOnSignalStrengthUpdate.

        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);

        // The radio is telling us about signal strength changes,
        // so we don't have to ask it.
        mDontPollSignalStrength = TRUE;

        OnSignalStrengthResult(ar, FALSE);
        break;
    }
    case EVENT_RUIM_RECORDS_LOADED: {
        Log(String("EVENT_RUIM_RECORDS_LOADED: what=") + StringUtils::ToString(what));
        UpdatePhoneObject();
        AutoPtr<IRuimRecords> ruim = IRuimRecords::Probe(mIccRecords);
        Boolean b;
        if ((ruim != NULL) && (IIccRecords::Probe(ruim)->IsProvisioned(&b), b)) {
            ruim->GetMdn(&mMdn);
            ruim->GetMin(&mMin);
            String sid, nid;
            ruim->GetSid(&sid);
            ruim->GetNid(&nid);
            ParseSidNid(sid, nid);
            ruim->GetPrlVersion(&mPrlVersion);
            mIsMinInfoReady = TRUE;
            UpdateOtaspState();
        }
        // SID/NID/PRL is loaded. Poll service state
        // again to update to the roaming state with
        // the latest variables.
        GetSubscriptionInfoAndStartPollingThreads();
        break;
    }
    case EVENT_LOCATION_UPDATES_ENABLED: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);

        if (ar->mException == NULL) {
            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_GET_LOC_DONE_CDMA, NULL, (IMessage**)&msg);
            mCi->GetVoiceRegistrationState(msg);
        }
        break;
    }
    case EVENT_ERI_FILE_LOADED:
        // Repoll the state once the ERI file has been loaded.
        if (DBG) Log(String("[CdmaServiceStateTracker] ERI file has been loaded, repolling."));
        PollState();
        break;

    case EVENT_OTA_PROVISION_STATUS_CHANGE: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        if (ar->mException == NULL) {
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            AutoPtr<IInterface> obj;
            iArray->Get(0, (IInterface**)&obj);
            Int32 otaStatus;
            IInteger32::Probe(obj)->GetValue(&otaStatus);

            if (otaStatus == IPhone::CDMA_OTA_PROVISION_STATUS_COMMITTED
                || otaStatus == IPhone::CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED) {
                if (DBG) Log(String("EVENT_OTA_PROVISION_STATUS_CHANGE: Complete, Reload MDN"));
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION, (IMessage**)&msg);
                mCi->GetCDMASubscription(msg);
            }
        }
        break;
    }
    case EVENT_CDMA_PRL_VERSION_CHANGED: {
        AutoPtr<IInterface> obj;
        msg->GetObj((IInterface**)&obj);
        ar = (AsyncResult*)IAsyncResult::Probe(obj);
        if (ar->mException == NULL) {
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            AutoPtr<IInterface> obj;
            iArray->Get(0, (IInterface**)&obj);
            Int32 val;
            IInteger32::Probe(obj)->GetValue(&val);

            mPrlVersion = StringUtils::ToString(val);
        }
        break;
    }
    case EVENT_CHANGE_IMS_STATE:
        if (DBG) Log(String("EVENT_CHANGE_IMS_STATE"));
        SetPowerStateToDesired();
        break;

    default:
        ServiceStateTracker::HandleMessage(msg);
        break;
    }
    return NOERROR;
}

ECode CdmaServiceStateTracker::PollState()
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

    default:
        // Issue all poll-related commands at once, then count
        // down the responses which are allowed to arrive
        // out-of-order.

        AutoPtr<IArrayOf> iArray;
        CArrayOf::New(EIID_IInteger32, mPollingContext->GetLength(), (IArrayOf**)&iArray);
        for (Int32 i = 0; i < mPollingContext->GetLength(); i++) {
            iArray->Set(i, CoreUtils::Convert((*mPollingContext)[i]));
        }

        iArray->Set(0, CoreUtils::Convert(++(*mPollingContext)[0]));        // mPollingContext[0]++;

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
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetCurrentDataConnectionState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mSS->GetDataRegState(result);
}

ECode CdmaServiceStateTracker::IsConcurrentVoiceAndDataAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // Note: it needs to be confirmed which CDMA network types
    // can support voice and data calls concurrently.
    // For the time-being, the return value will be FALSE.
    *result = FALSE;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetMdnNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMdn;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMin;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPrlVersion;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetImsi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // TODO: When RUIM is enabled, IMSI will come from RUIM not build-time props.
    String operatorNumeric;
    GetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC,
            String(""), &operatorNumeric);

    String str;
    GetCdmaMin(&str);
    if (!TextUtils::IsEmpty(operatorNumeric) && str != NULL) {
        *result = (operatorNumeric + str);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CdmaServiceStateTracker::IsMinInfoReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsMinInfoReady;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetOtasp(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 provisioningState;
    // for ruim, min is NULL means require otasp.
    if (mIsSubscriptionFromRuim && mMin == NULL) {
        *result = OTASP_NEEDED;
    }
    if (mMin == NULL || (mMin.GetLength() < 6)) {
        if (DBG) Log(String("getOtasp: bad mMin='") + mMin + "'");
        provisioningState = OTASP_UNKNOWN;
    }
    else {
        Boolean b;
        if ((mMin.Equals(UNACTIVATED_MIN_VALUE)
                || mMin.Substring(0,6).Equals(UNACTIVATED_MIN2_VALUE))
                || (SystemProperties::GetBoolean(String("test_cdma_setup"), FALSE, &b), b)) {
            provisioningState = OTASP_NEEDED;
        }
        else {
            provisioningState = OTASP_NOT_NEEDED;
        }
    }
    if (DBG) Log(String("getOtasp: state=") + StringUtils::ToString(provisioningState));
    *result = provisioningState;
    return NOERROR;
}

ECode CdmaServiceStateTracker::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CdmaServiceStateTracker extends:"));
    ServiceStateTracker::Dump(fd, pw, args);
    // pw->Println(" mPhone=" + mPhone);
    // pw->Println(" mSS=" + mSS);
    // pw->Println(" mNewSS=" + mNewSS);
    // pw->Println(" mCellLoc=" + mCellLoc);
    // pw->Println(" mNewCellLoc=" + mNewCellLoc);
    // pw->Println(" mCurrentOtaspMode=" + mCurrentOtaspMode);
    // pw->Println(" mCdmaRoaming=" + mCdmaRoaming);
    // pw->Println(" mRoamingIndicator=" + mRoamingIndicator);
    // pw->Println(" mIsInPrl=" + mIsInPrl);
    // pw->Println(" mDefaultRoamingIndicator=" + mDefaultRoamingIndicator);
    // pw->Println(" mRegistrationState=" + mRegistrationState);
    // pw->Println(" mNeedFixZone=" + mNeedFixZone);
    // pw->Println(" mZoneOffset=" + mZoneOffset);
    // pw->Println(" mZoneDst=" + mZoneDst);
    // pw->Println(" mZoneTime=" + mZoneTime);
    // pw->Println(" mGotCountryCode=" + mGotCountryCode);
    // pw->Println(" mSavedTimeZone=" + mSavedTimeZone);
    // pw->Println(" mSavedTime=" + mSavedTime);
    // pw->Println(" mSavedAtTime=" + mSavedAtTime);
    // pw->Println(" mWakeLock=" + mWakeLock);
    // pw->Println(" mCurPlmn=" + mCurPlmn);
    // pw->Println(" mCurShowPmn=" + mCurShowPlmn);
    // pw->Println(" mCurSpn=" + mCurSpn);
    // pw->Println(" mCurShowSpn=" + mCurShowSpn);
    // pw->Println(" mMdn=" + mMdn);
    // pw->Println(" mHomeSystemId=" + mHomeSystemId);
    // pw->Println(" mHomeNetworkId=" + mHomeNetworkId);
    // pw->Println(" mMin=" + mMin);
    // pw->Println(" mPrlVersion=" + mPrlVersion);
    // pw->Println(" mIsMinInfoReady=" + mIsMinInfoReady);
    // pw->Println(" mIsEriTextLoaded=" + mIsEriTextLoaded);
    // pw->Println(" mIsSubscriptionFromRuim=" + mIsSubscriptionFromRuim);
    // pw->Println(" mCdmaSSM=" + mCdmaSSM);
    // pw->Println(" mRegistrationDeniedReason=" + mRegistrationDeniedReason);
    // pw->Println(" mCurrentCarrier=" + mCurrentCarrier);
    return NOERROR;
}

ECode CdmaServiceStateTracker::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    Log(String("ImsRegistrationState - registered : ")
            + StringUtils::BooleanToString(registered));

    if (mImsRegistrationOnOff && !registered) {
        if (mAlarmSwitch) {
            mImsRegistrationOnOff = registered;

            AutoPtr<IContext> context;
            IPhone::Probe(mPhone)->GetContext((IContext**)&context);
            AutoPtr<IInterface> obj;
            context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
            AutoPtr<IAlarmManager> am = IAlarmManager::Probe(obj);
            am->Cancel(mRadioOffIntent);
            mAlarmSwitch = FALSE;

            AutoPtr<IMessage> msg;
            ObtainMessage(EVENT_CHANGE_IMS_STATE, (IMessage**)&msg);
            Boolean b;
            SendMessage(msg, &b);
            return NOERROR;
        }
    }
    mImsRegistrationOnOff = registered;
    return NOERROR;
}

ECode CdmaServiceStateTracker::constructor(
    /* [in] */ ICDMAPhone* phone,
    /* [in] */ ICellInfo* cellInfo)
{
    ServiceStateTracker::constructor(IPhoneBase::Probe(phone),
            ((CDMAPhone*)phone)->mCi, cellInfo);

    mPhone = phone;
    AutoPtr<IContext> ctx;
    IPhone::Probe(phone)->GetContext((IContext**)&ctx);
    ctx->GetContentResolver((IContentResolver**)&mCr);
    CCdmaCellLocation::New((ICdmaCellLocation**)&mCellLoc);
    CCdmaCellLocation::New((ICdmaCellLocation**)&mNewCellLoc);

    mCdmaSSM = CdmaSubscriptionSourceManager::GetInstance(ctx, mCi, this,
            EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
    Int32 val;
    mCdmaSSM->GetCdmaSubscriptionSource(&val);
    mIsSubscriptionFromRuim = (val ==
            ICdmaSubscriptionSourceManager::SUBSCRIPTION_FROM_RUIM);

    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(obj);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_TAG,
            (IPowerManagerWakeLock**)&mWakeLock);

    mCi->RegisterForRadioStateChanged(this, EVENT_RADIO_STATE_CHANGED, NULL);

    mCi->RegisterForVoiceNetworkStateChanged(this, EVENT_NETWORK_STATE_CHANGED_CDMA, NULL);
    mCi->SetOnNITZTime(this, EVENT_NITZ_TIME, NULL);

    mCi->RegisterForCdmaPrlChanged(this, EVENT_CDMA_PRL_VERSION_CHANGED, NULL);
    phone->RegisterForEriFileLoaded(this, EVENT_ERI_FILE_LOADED, NULL);
    mCi->RegisterForCdmaOtaProvision(this,EVENT_OTA_PROVISION_STATUS_CHANGE, NULL);

    // System setting property AIRPLANE_MODE_ON is set in Settings.
    Int32 airplaneMode;
    Settings::Global::GetInt32(mCr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &airplaneMode);
    mDesiredPowerState = ! (airplaneMode > 0);

    AutoPtr<IUri> uri;
    Settings::Global::GetUriFor(ISettingsGlobal::AUTO_TIME, (IUri**)&uri);
    mCr->RegisterContentObserver(uri, TRUE, mAutoTimeObserver);
    uri = NULL;
    Settings::Global::GetUriFor(ISettingsGlobal::AUTO_TIME_ZONE, (IUri**)&uri);
    mCr->RegisterContentObserver(uri, TRUE, mAutoTimeZoneObserver);
    SetSignalStrengthDefaultValues();

    mHbpcdUtils = new HbpcdUtils(ctx);

    // Reset OTASP state in case previously set by another service
    IPhoneBase::Probe(phone)->NotifyOtaspChanged(OTASP_UNINITIALIZED);
    return NOERROR;
}

void CdmaServiceStateTracker::Finalize()
{
    if (DBG) Log(String("CdmaServiceStateTracker finalized"));
}

ECode CdmaServiceStateTracker::SetPowerStateToDesired()
{
    // If we want it on and it's off, turn it on
    ICommandsInterfaceRadioState state;
    mCi->GetRadioState(&state);
    if (mDesiredPowerState
        && state == Elastos::Droid::Internal::Telephony::RADIO_OFF) {
        mCi->SetRadioPower(TRUE, NULL);
    }
// TODO: Need ICommandsInterfaceRadioState.isOn,isAvailable
    // else if (!mDesiredPowerState && state.isOn()) {
    //     AutoPtr<IDcTrackerBase> dcTracker = ((CDMAPhone*)mPhone.Get())->mDcTracker;

    //     // If it's on and available and we want it off gracefully
    //     PowerOffRadioSafely(dcTracker);
    // }
    // else if (mDeviceShuttingDown && state.isAvailable()) {
    //     mCi->RequestShutdown(NULL);
    // }
    return NOERROR;
}

ECode CdmaServiceStateTracker::UpdateSpnDisplay()
{
    // mOperatorAlphaLong contains the ERI text
    String plmn;
    String spn;
    Boolean showPlmn = FALSE;
    Boolean showSpn = FALSE;
    Int32 rule = 0;
    Int32 combinedRegState;

    String str;
    mSS->GetOperatorNumeric(&str);
    Int32 val;
    mIccRecords->GetDisplayRule(str, &val);
    rule = (mIccRecords != NULL) ? val : 0;
    combinedRegState = GetCombinedRegState();

    if (combinedRegState == IServiceState::STATE_OUT_OF_SERVICE) {
        // display out of service
        showPlmn = TRUE;
        AutoPtr<IResources> res = CResources::GetSystem();
        AutoPtr<ICharSequence> cs;
        res->GetText(R::string::lockscreen_carrier_default, (ICharSequence**)&cs);
        cs->ToString(&plmn);
        if (DBG) Log(String("updateSpnDisplay: radio is on but out ") +
                "of service, set plmn='" + plmn + "'");
    }
    else if (combinedRegState == IServiceState::STATE_IN_SERVICE) {
        mSS->GetOperatorAlphaLong(&str);
        String str2;
        plmn = TextUtils::IsEmpty(str) ? (SystemProperties::Get(
                String("ro.cdma.home.operator.alpha"), String(""), &str2), str2) : str;
        // depends on the rule and whether plmn or spn is NULL
        showPlmn = ( !TextUtils::IsEmpty(plmn)) &&
                ((rule & IIccRecords::SPN_RULE_SHOW_PLMN) == IIccRecords::SPN_RULE_SHOW_PLMN);
        mIccRecords->GetServiceProviderName(&str);
        spn = (mIccRecords != NULL) ? str : String("");
        showSpn = (!TextUtils::IsEmpty(spn)) &&
                ((rule & IIccRecords::SPN_RULE_SHOW_SPN) == IIccRecords::SPN_RULE_SHOW_SPN);
    }
    else {
        // power off state (airplane mode), show nothing
    }

    // Update if any value changes
    if (showPlmn != mCurShowPlmn
            || showSpn != mCurShowSpn
            || !TextUtils::Equals(spn, mCurSpn)
            || !TextUtils::Equals(plmn, mCurPlmn)) {

        if (DBG) {
            Logger::D(LOGTAG, "updateSpnDisplay: changed sending intent showPlmn='%d' plmn='%s'"
                    , showPlmn, plmn.string());
        }
        AutoPtr<IIntent> intent;
        CIntent::New(ITelephonyIntents::SPN_STRINGS_UPDATED_ACTION, (IIntent**)&intent);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(ITelephonyIntents::EXTRA_SHOW_SPN, showSpn);
        intent->PutExtra(ITelephonyIntents::EXTRA_SPN, spn);
        intent->PutExtra(ITelephonyIntents::EXTRA_SHOW_PLMN, showPlmn);
        intent->PutExtra(ITelephonyIntents::EXTRA_PLMN, plmn);
        Int32 pid;
        IPhone::Probe(mPhone)->GetPhoneId(&pid);
        SubscriptionManager::PutPhoneIdAndSubIdExtra(intent, pid);
        AutoPtr<IContext> ctx;
        IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
        ctx->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
    }

    mCurShowSpn = showSpn;
    mCurShowPlmn = showPlmn;
    mCurSpn = spn;
    mCurPlmn = plmn;
    return NOERROR;
}

ECode CdmaServiceStateTracker::GetPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IPhone::Probe(mPhone);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CdmaServiceStateTracker::HandlePollStateResultMessage(
    /* [in] */ Int32 what,
    /* [in] */ AsyncResult* ar)
{
    AutoPtr<ArrayOf<Int32> > ints;
    AutoPtr<ArrayOf<String> > states;
    switch (what) {
        case EVENT_POLL_STATE_GPRS: {
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 len;
            iArray->GetLength(&len);
            states = ArrayOf<String>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                iArray->Get(i, (IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&(*states)[i]);
            }

            if (DBG) {
                // Log(String("handlePollStateResultMessage: EVENT_POLL_STATE_GPRS states.length=") +
                //         states->GetLength() + " states=" + states);
            }

            Int32 regState = IServiceState::RIL_REG_STATE_UNKNOWN;
            Int32 dataRadioTechnology = 0;

            if (states->GetLength() > 0) {
                // try {
                regState = StringUtils::ParseInt32((*states)[0]);

                // states[3] (if present) is the current radio technology
                if (states->GetLength() >= 4 && (*states)[3] != NULL) {
                    dataRadioTechnology = StringUtils::ParseInt32((*states)[3]);
                }
                // } catch (NumberFormatException ex) {
                //     Loge(String("handlePollStateResultMessage: error parsing GprsRegistrationState: ")
                //                     + ex);
                // }
            }

            Int32 dataRegState = RegCodeToServiceState(regState);
            mNewSS->SetDataRegState(dataRegState);
            mNewSS->SetRilDataRadioTechnology(dataRadioTechnology);
            if (DBG) {
                Log(String("handlPollStateResultMessage: cdma setDataRegState=")
                        + StringUtils::ToString(dataRegState)
                        + " regState=" + StringUtils::ToString(regState)
                        + " dataRadioTechnology=" + StringUtils::ToString(dataRadioTechnology));
            }
            break;
        }
        case EVENT_POLL_STATE_REGISTRATION_CDMA: {// Handle RIL_REQUEST_REGISTRATION_STATE.
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 len;
            iArray->GetLength(&len);
            states = ArrayOf<String>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                iArray->Get(i, (IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&(*states)[i]);
            }

            Int32 registrationState = 4;     //[0] registrationState
            Int32 radioTechnology = -1;      //[3] radioTechnology
            Int32 baseStationId = -1;        //[4] baseStationId
            //[5] baseStationLatitude
            Int32 baseStationLatitude = ICdmaCellLocation::INVALID_LAT_LONG;
            //[6] baseStationLongitude
            Int32 baseStationLongitude = ICdmaCellLocation::INVALID_LAT_LONG;
            Int32 cssIndicator = 0;          //[7] init with 0, because it is treated as a Boolean
            Int32 systemId = 0;              //[8] systemId
            Int32 networkId = 0;             //[9] networkId
            Int32 roamingIndicator = IEriInfo::ROAMING_INDICATOR_OFF;     //[10] Roaming indicator
            Int32 systemIsInPrl = 0;         //[11] Indicates if current system is in PRL
            Int32 defaultRoamingIndicator = IEriInfo::ROAMING_INDICATOR_OFF;  //[12] def RI from PRL
            Int32 reasonForDenial = 0;       //[13] Denial reason if registrationState = 3

            if (states->GetLength() >= 14) {
                // try {
                if ((*states)[0] != NULL) {
                    registrationState = StringUtils::ParseInt32((*states)[0]);
                }
                if ((*states)[3] != NULL) {
                    radioTechnology = StringUtils::ParseInt32((*states)[3]);
                }
                if ((*states)[4] != NULL) {
                    baseStationId = StringUtils::ParseInt32((*states)[4]);
                }
                if ((*states)[5] != NULL) {
                    baseStationLatitude = StringUtils::ParseInt32((*states)[5]);
                }
                if ((*states)[6] != NULL) {
                    baseStationLongitude = StringUtils::ParseInt32((*states)[6]);
                }
                // Some carriers only return lat-lngs of 0,0
                if (baseStationLatitude == 0 && baseStationLongitude == 0) {
                    baseStationLatitude  = ICdmaCellLocation::INVALID_LAT_LONG;
                    baseStationLongitude = ICdmaCellLocation::INVALID_LAT_LONG;
                }
                if ((*states)[7] != NULL) {
                    cssIndicator = StringUtils::ParseInt32((*states)[7]);
                }
                if ((*states)[8] != NULL) {
                    systemId = StringUtils::ParseInt32((*states)[8]);
                }
                if ((*states)[9] != NULL) {
                    networkId = StringUtils::ParseInt32((*states)[9]);
                }
                if ((*states)[10] != NULL) {
                    roamingIndicator = StringUtils::ParseInt32((*states)[10]);
                }
                if ((*states)[11] != NULL) {
                    systemIsInPrl = StringUtils::ParseInt32((*states)[11]);
                }
                if ((*states)[12] != NULL) {
                    defaultRoamingIndicator = StringUtils::ParseInt32((*states)[12]);
                }
                if ((*states)[13] != NULL) {
                    reasonForDenial = StringUtils::ParseInt32((*states)[13]);
                }
                // } catch (NumberFormatException ex) {
                //     Loge(String("EVENT_POLL_STATE_REGISTRATION_CDMA: error parsing: ") + ex);
                // }
            }
            else {
                // throw new RuntimeException("Warning! Wrong number of parameters returned from "
                //                      + "RIL_REQUEST_REGISTRATION_STATE: expected 14 or more "
                //                      + "strings and got " + states->GetLength() + " strings");
                return E_RUNTIME_EXCEPTION;
            }

            mRegistrationState = registrationState;
            // When registration state is roaming and TSB58
            // roaming indicator is not in the carrier-specified
            // list of ERIs for home system, mCdmaRoaming is TRUE.
            mCdmaRoaming =
                    RegCodeIsRoaming(registrationState) && !IsRoamIndForHomeSystem((*states)[10]);
            mCdmaRoaming = mCdmaRoaming || mDataRoaming;
            mNewSS->SetState (RegCodeToServiceState(registrationState));

            mNewSS->SetRilVoiceRadioTechnology(radioTechnology);

            mNewSS->SetCssIndicator(cssIndicator);
            mNewSS->SetSystemAndNetworkId(systemId, networkId);
            mRoamingIndicator = roamingIndicator;
            mIsInPrl = (systemIsInPrl == 0) ? FALSE : TRUE;
            mDefaultRoamingIndicator = defaultRoamingIndicator;


            // Values are -1 if not available.
            mNewCellLoc->SetCellLocationData(baseStationId, baseStationLatitude,
                    baseStationLongitude, systemId, networkId);

            if (reasonForDenial == 0) {
                mRegistrationDeniedReason = ServiceStateTracker::REGISTRATION_DENIED_GEN;
            }
            else if (reasonForDenial == 1) {
                mRegistrationDeniedReason = ServiceStateTracker::REGISTRATION_DENIED_AUTH;
            }
            else {
                mRegistrationDeniedReason = String("");
            }

            if (mRegistrationState == 3) {
                if (DBG) Log(String("Registration denied, ") + mRegistrationDeniedReason);
            }
            break;
        }
        case EVENT_POLL_STATE_OPERATOR_CDMA: {// Handle RIL_REQUEST_OPERATOR
            AutoPtr<IArrayOf> iArray = IArrayOf::Probe(ar->mResult);
            Int32 len;
            iArray->GetLength(&len);
            AutoPtr<ArrayOf<String> > opNames = ArrayOf<String>::Alloc(len);
            for (Int32 i = 0; i < len; i++) {
                AutoPtr<IInterface> obj;
                iArray->Get(i, (IInterface**)&obj);
                ICharSequence::Probe(obj)->ToString(&(*opNames)[i]);
            }

            if (opNames != NULL && opNames->GetLength() >= 3) {
                // TODO: Do we care about overriding in this case.
                // If the NUMERIC field isn't valid use PROPERTY_CDMA_HOME_OPERATOR_NUMERIC
                if (((*opNames)[2].IsNull()) || ((*opNames)[2].GetLength() < 5)
                        || ((*opNames)[2].Equals("00000"))) {
                    SystemProperties::Get(ICDMAPhone::PROPERTY_CDMA_HOME_OPERATOR_NUMERIC,
                            String("00000"), &(*opNames)[2]);
                    if (DBG) {
                        Log(String("RIL_REQUEST_OPERATOR.response[2], the numeric, ") +
                                " is bad. Using SystemProperties '" +
                                        ICDMAPhone::PROPERTY_CDMA_HOME_OPERATOR_NUMERIC +
                                "'= " + (*opNames)[2]);
                    }
                }

                if (!mIsSubscriptionFromRuim) {
                    // In CDMA in case on NV, the ss.mOperatorAlphaLong is set later with the
                    // ERI text, so here it is ignored what is coming from the modem.
                    mNewSS->SetOperatorName(String(NULL), (*opNames)[1], (*opNames)[2]);
                }
                else {
                    AutoPtr<IUiccCard> card;
                    mUiccController->GetUiccCard((IUiccCard**)&card);
                    String str;
                    String brandOverride = card != NULL ?
                        (card->GetOperatorBrandOverride(&str), str) : String(NULL);
                    Boolean b;
                    if (brandOverride != NULL) {
                        mNewSS->SetOperatorName(brandOverride, brandOverride, (*opNames)[2]);
                    }
                    else if (SystemProperties::GetBoolean(
                            String("ro.cdma.force_plmn_lookup"), FALSE, &b), b) {
                        mNewSS->SetOperatorName(
                            Operators::OperatorReplace((*opNames)[2]),
                            (*opNames)[1],
                            (*opNames)[2]);
                    }
                    else {
                        mNewSS->SetOperatorName((*opNames)[0], (*opNames)[1], (*opNames)[2]);
                    }
                }
            }
            else {
                if (DBG) Log(String("EVENT_POLL_STATE_OPERATOR_CDMA: error parsing opNames"));
            }
            break;
        }
        default:
            Loge(String("handlePollStateResultMessage: RIL response handle in wrong phone!")
                    + " Expected CDMA RIL request and get GSM RIL request.");
            break;
    }
    return NOERROR;
}

ECode CdmaServiceStateTracker::HandlePollStateResult(
    /* [in] */ Int32 what,
    /* [in] */ AsyncResult* ar)
{
    // Ignore stale requests from last poll.
// TODO:
    // if (!Object::Equals(ar->mUserObj, mPollingContext)) return NOERROR;

// TODO: Need CommandException
    // if (ar->mException != NULL) {
    //     CommandException.Error err=NULL;

    //     if (ar->mException instanceof CommandException) {
    //         err = ((CommandException)(ar->mException))->GetCommandError();
    //     }

    //     if (err == CommandException.Error.RADIO_NOT_AVAILABLE) {
    //         // Radio has crashed or turned off.
    //         cancelPollState();
    //         return;
    //     }


    //     if (err != CommandException.Error.OP_NOT_ALLOWED_BEFORE_REG_NW) {
    //         Loge(String("handlePollStateResult: RIL returned an error where it must succeed")
    //                 + ar->mException);
    //     }
    // } else try {
    //     handlePollStateResultMessage(what, ar);
    // } catch (RuntimeException ex) {
    //     Loge(String("handlePollStateResult: Exception while polling service state. ")
    //             + "Probably malformed RIL response." + ex);
    // }

    (*mPollingContext)[0]--;

    if ((*mPollingContext)[0] == 0) {
        Boolean namMatch = FALSE;
        Int32 systemId;
        mNewSS->GetSystemId(&systemId);
        if (!IsSidsAllZeros() && IsHomeSid(systemId)) {
            namMatch = TRUE;
        }

        // Setting SS Roaming (general)
        if (mIsSubscriptionFromRuim) {
            mNewSS->SetRoaming(IsRoamingBetweenOperators(mCdmaRoaming, mNewSS));
        }
        else {
            mNewSS->SetRoaming(mCdmaRoaming);
        }

        // Setting SS CdmaRoamingIndicator and CdmaDefaultRoamingIndicator
        mNewSS->SetCdmaDefaultRoamingIndicator(mDefaultRoamingIndicator);
        mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
        Boolean isPrlLoaded = TRUE;
        if (TextUtils::IsEmpty(mPrlVersion)) {
            isPrlLoaded = FALSE;
        }

        Int32 rilVoiceRadioTechnology;
        mNewSS->GetRilVoiceRadioTechnology(&rilVoiceRadioTechnology);
        if (!isPrlLoaded || (rilVoiceRadioTechnology
                 == IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN)) {
            Log(String("Turn off roaming indicator if !isPrlLoaded or voice RAT is unknown"));
            mNewSS->SetCdmaRoamingIndicator(IEriInfo::ROAMING_INDICATOR_OFF);
        }
        else if (!IsSidsAllZeros()) {
            if (!namMatch && !mIsInPrl) {
                // Use default
                mNewSS->SetCdmaRoamingIndicator(mDefaultRoamingIndicator);
            }
            else if (namMatch && !mIsInPrl) {
                // TODO this will be removed when we handle roaming on LTE on CDMA+LTE phones
                if (rilVoiceRadioTechnology
                        == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) {
                    Log(String("Turn off roaming indicator as voice is LTE"));
                    mNewSS->SetCdmaRoamingIndicator(IEriInfo::ROAMING_INDICATOR_OFF);
                }
                else {
                    mNewSS->SetCdmaRoamingIndicator(IEriInfo::ROAMING_INDICATOR_FLASH);
                }
            }
            else if (!namMatch && mIsInPrl) {
                // Use the one from PRL/ERI
                mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
            }
            else {
                // It means namMatch && mIsInPrl
                if ((mRoamingIndicator <= 2)) {
                    mNewSS->SetCdmaRoamingIndicator(IEriInfo::ROAMING_INDICATOR_OFF);
                }
                else {
                    // Use the one from PRL/ERI
                    mNewSS->SetCdmaRoamingIndicator(mRoamingIndicator);
                }
            }
        }

        Int32 roamingIndicator;
        mNewSS->GetCdmaRoamingIndicator(&roamingIndicator);
        Int32 val;
        ((CDMAPhone*)mPhone.Get())->mEriManager->GetCdmaEriIconIndex(
                roamingIndicator, mDefaultRoamingIndicator, &val);
        mNewSS->SetCdmaEriIconIndex(val);
        ((CDMAPhone*)mPhone.Get())->mEriManager->GetCdmaEriIconMode(
                roamingIndicator, mDefaultRoamingIndicator, &val);
        mNewSS->SetCdmaEriIconMode(val);

        // NOTE: Some operator may require overriding mCdmaRoaming
        // (set by the modem), depending on the mRoamingIndicator.

        if (DBG) {
            // Log(String("Set CDMA Roaming Indicator to: ") + roamingIndicator
            //     + ". mCdmaRoaming = " + mCdmaRoaming + ", isPrlLoaded = " + isPrlLoaded
            //     + ". namMatch = " + namMatch + " , mIsInPrl = " + mIsInPrl
            //     + ", mRoamingIndicator = " + mRoamingIndicator
            //     + ", mDefaultRoamingIndicator= " + mDefaultRoamingIndicator);
        }
        PollStateDone();
    }
    return NOERROR;
}

void CdmaServiceStateTracker::SetSignalStrengthDefaultValues()
{
    CSignalStrength::New(FALSE, (ISignalStrength**)&mSignalStrength);
}

void CdmaServiceStateTracker::FixTimeZone(
    /* [in] */ const String& isoCountryCode)
{
    AutoPtr<ITimeZone> zone;
    // If the offset is (0, FALSE) and the time zone property
    // is set, use the time zone property rather than GMT.
    String zoneName;
    SystemProperties::Get(TIMEZONE_PROPERTY, &zoneName);
    Int32 index;
    if (DBG) {
        Log(String("fixTimeZone zoneName='") + zoneName +
            "' mZoneOffset=" + StringUtils::ToString(mZoneOffset) +
            " mZoneDst=" + StringUtils::ToString(mZoneDst) +
            " iso-cc='" + isoCountryCode +
            "' iso-cc-idx=" +
            StringUtils::ToString(Arrays::BinarySearch(GMT_COUNTRY_CODES, isoCountryCode, &index), index));
    }
    if ((mZoneOffset == 0) && (mZoneDst == FALSE) && (zoneName != NULL)
            && (zoneName.GetLength() > 0)
            && ((Arrays::BinarySearch(GMT_COUNTRY_CODES, isoCountryCode, &index), index) < 0)) {
        // For NITZ string without time zone,
        // need adjust time to reflect default time zone setting
        AutoPtr<ITimeZoneHelper> tzHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
        tzHelper->GetDefault((ITimeZone**)&zone);
        if (mNeedFixZone) {
            Int64 ctm;
            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->GetCurrentTimeMillis(&ctm);
            Int32 tzOffset;
            zone->GetOffset(ctm, &tzOffset);
            if (DBG) {
                Log(String("fixTimeZone: tzOffset=") + StringUtils::ToString(tzOffset) +
                        " ltod=" + TimeUtils::LogTimeOfDay(ctm));
            }
            if (GetAutoTime()) {
                Int64 adj = ctm - tzOffset;
                if (DBG) Log(String("fixTimeZone: adj ltod=") + TimeUtils::LogTimeOfDay(adj));
                SetAndBroadcastNetworkSetTime(adj);
            }
            else {
                // Adjust the saved NITZ time to account for tzOffset.
                mSavedTime = mSavedTime - tzOffset;
                if (DBG) Log(String("fixTimeZone: adj mSavedTime=")
                        + StringUtils::ToString(mSavedTime));
            }
        }
        if (DBG) Log(String("fixTimeZone: using default TimeZone"));
    }
    else if (isoCountryCode.Equals("")) {
        // Country code not found. This is likely a test network.
        // Get a TimeZone based only on the NITZ parameters (best guess).
        zone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
        if (DBG) Log(String("fixTimeZone: using NITZ TimeZone"));
    }
    else {
        zone = TimeUtils::GetTimeZone(mZoneOffset, mZoneDst, mZoneTime, isoCountryCode);
        if (DBG) Log(String("fixTimeZone: using getTimeZone(off, dst, time, iso)"));
    }

    mNeedFixZone = FALSE;

    if (zone != NULL) {
        String id;
        zone->GetID(&id);
        Log(String("fixTimeZone: zone != NULL zone->GetID=") + id);
        if (GetAutoTimeZone()) {
            SetAndBroadcastNetworkSetTimeZone(id);
        }
        else {
            Log(String("fixTimeZone: skip changing zone as GetAutoTimeZone was FALSE"));
        }
        SaveNitzTimeZone(id);
    }
    else {
        Log(String("fixTimeZone: zone == NULL, do nothing for zone"));
    }
}

ECode CdmaServiceStateTracker::PollStateDone()
{
    // if (DBG) Log(String("PollStateDone: cdma oldSS=[") + mSS + "] newSS=[" + mNewSS + "]");

    Boolean b;
    SystemProperties::GetBoolean(PROP_FORCE_ROAMING, FALSE, &b);
    if (Build::IS_DEBUGGABLE && b) {
        mNewSS->SetRoaming(TRUE);
    }

    UseDataRegStateForDataOnlyDevices();

    Int32 voiceRegState;
    mSS->GetVoiceRegState(&voiceRegState);
    Int32 newVoiceRegState;
    mNewSS->GetVoiceRegState(&newVoiceRegState);
    Boolean hasRegistered =
        voiceRegState != IServiceState::STATE_IN_SERVICE
        && newVoiceRegState == IServiceState::STATE_IN_SERVICE;

    Boolean hasDeregistered =
        voiceRegState == IServiceState::STATE_IN_SERVICE
        && newVoiceRegState != IServiceState::STATE_IN_SERVICE;

    Int32 dataRegState;
    mSS->GetDataRegState(&dataRegState);
    Int32 newDataRegState;
    mNewSS->GetDataRegState(&newDataRegState);
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
    Boolean hasRilVoiceRadioTechnologyChanged =
            rilVoiceRadioTechnology != newRilVoiceRadioTechnology;

    Int32 rilDataRadioTechnology;
    mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
    Int32 newRilDataRadioTechnology;
    mNewSS->GetRilDataRadioTechnology(&newRilDataRadioTechnology);
    Boolean hasRilDataRadioTechnologyChanged =
            rilDataRadioTechnology != newRilDataRadioTechnology;

    Boolean hasChanged;
    IObject::Probe(mNewSS)->Equals(mSS, &hasChanged);
    hasChanged = !hasChanged;

    Boolean roaming;
    mSS->GetRoaming(&roaming);
    Boolean newRoaming;
    mNewSS->GetRoaming(&newRoaming);
    Boolean hasRoamingOn = !roaming && newRoaming;

    Boolean hasRoamingOff = roaming && !newRoaming;

    Boolean hasLocationChanged;
    IObject::Probe(mNewCellLoc)->Equals(mCellLoc, &hasLocationChanged);
    hasLocationChanged = !hasLocationChanged;

    ICommandsInterfaceRadioState state;
    mCi->GetRadioState(&state);
    if (state == Elastos::Droid::Internal::Telephony::RADIO_OFF) {
        Boolean resetIwlanRatVal = FALSE;
        Log(String("set service state as POWER_OFF"));
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == newRilDataRadioTechnology)) {
            // Log(String("PollStateDone: mNewSS = ") + mNewSS);
            Log(String("PollStateDone: reset iwlan RAT value"));
            resetIwlanRatVal = TRUE;
        }
        mNewSS->SetStateOff();
        if (resetIwlanRatVal) {
            mNewSS->SetRilDataRadioTechnology(IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN);
            // Log(String("PollStateDone: mNewSS = ") + mNewSS);
            resetIwlanRatVal = FALSE;
        }
    }

    // Add an event log when connection state changes
    if (voiceRegState != newVoiceRegState ||
            dataRegState != newDataRegState) {
        // EventLog::WriteEvent(EventLogTags.CDMA_SERVICE_STATE_CHANGE,
        //         mSS->GetVoiceRegState(), mSS->GetDataRegState(),
        //         mNewSS->GetVoiceRegState(), mNewSS->GetDataRegState());
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

    if (hasRilVoiceRadioTechnologyChanged) {
        UpdatePhoneObject();
    }

    if (hasRilDataRadioTechnologyChanged) {
        Int32 rilDataRadioTechnology;
        mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        String str;
        CServiceState::RilRadioTechnologyToString(rilDataRadioTechnology, &str);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_DATA_NETWORK_TYPE, str);

        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            Log(String("PollStateDone: IWLAN enabled"));
        }
    }

    if (hasRegistered) {
        mNetworkAttachedRegistrants->NotifyRegistrants();
    }

    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    mCi->GetRadioState(&state);
    if (hasChanged) {
        Boolean b = FALSE;
// TODO: NeedICommandsInterfaceRadioState::IsOn
        // state->IsOn(&b);
        if (b && (!mIsSubscriptionFromRuim)) {
            String eriText;
            // Now the CDMAPhone sees the new ServiceState so it can get the new ERI text
            Int32 voiceRegState;
            mSS->GetVoiceRegState(&voiceRegState);
            if (voiceRegState == IServiceState::STATE_IN_SERVICE) {
                IPhone::Probe(mPhone)->GetCdmaEriText(&eriText);
            }
            else {
                // Note that IServiceState::STATE_OUT_OF_SERVICE is valid used for
                // mRegistrationState 0,2,3 and 4
                AutoPtr<ICharSequence> cs;
                ctx->GetText(R::string::roamingTextSearching, (ICharSequence**)&cs);
                cs->ToString(&eriText);
            }
            mSS->SetOperatorAlphaLong(eriText);
        }

        String operatorNumeric;

        String operatorAlphaLong;
        mSS->GetOperatorAlphaLong(&operatorAlphaLong);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(
                ITelephonyProperties::PROPERTY_OPERATOR_ALPHA, operatorAlphaLong);

        String prevOperatorNumeric;
        SystemProperties::Get(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC,
                String(""), &prevOperatorNumeric);
        mSS->GetOperatorNumeric(&operatorNumeric);

        // try to fix the invalid Operator Numeric
        if (IsInvalidOperatorNumeric(operatorNumeric)) {
            Int32 sid;
            mSS->GetSystemId(&sid);
            operatorNumeric = FixUnknownMcc(operatorNumeric, sid);
        }

        IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_NUMERIC, operatorNumeric);
        UpdateCarrierMccMncConfiguration(operatorNumeric,
                prevOperatorNumeric, ctx);

        if (IsInvalidOperatorNumeric(operatorNumeric)) {
            if (DBG) Log(String("operatorNumeric ")+ operatorNumeric +"is invalid");
            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""));
            mGotCountryCode = FALSE;
        }
        else {
            String isoCountryCode = String("");
            String mcc = operatorNumeric.Substring(0, 3);
            // try{
                isoCountryCode = MccTable::CountryCodeForMcc(StringUtils::ParseInt32(
                        operatorNumeric.Substring(0,3)));
            // } catch ( NumberFormatException ex){
            //     Loge(String("PollStateDone: countryCodeForMcc error") + ex);
            // } catch ( StringIndexOutOfBoundsException ex) {
            //     Loge(String("PollStateDone: countryCodeForMcc error") + ex);
            // }

            IPhoneBase::Probe(mPhone)->SetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY,
                    isoCountryCode);
            mGotCountryCode = TRUE;

            SetOperatorIdd(operatorNumeric);

            if (ShouldFixTimeZoneNow(
                    IPhoneBase::Probe(mPhone), operatorNumeric,
                    prevOperatorNumeric, mNeedFixZone)) {
                FixTimeZone(isoCountryCode);
            }
        }

        Boolean roaming;
        mSS->GetRoaming(&roaming);
        IPhoneBase::Probe(mPhone)->SetSystemProperty(
                ITelephonyProperties::PROPERTY_OPERATOR_ISROAMING,
                roaming ? String("TRUE") : String("FALSE"));

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

    if (hasCdmaDataConnectionChanged || hasRilDataRadioTechnologyChanged) {
        NotifyDataRegStateRilRadioTechnologyChanged();
        Int32 rilDataRadioTechnology;
        mSS->GetRilDataRadioTechnology(&rilDataRadioTechnology);
        if (IsIwlanFeatureAvailable()
                && (IServiceState::RIL_RADIO_TECHNOLOGY_IWLAN
                    == rilDataRadioTechnology)) {
            IPhoneBase::Probe(mPhone)->NotifyDataConnection(IPhone::REASON_IWLAN_AVAILABLE);
            mIwlanRatAvailable = TRUE;
        }
        else {
            ProcessIwlanToWwanTransition(mSS);

            IPhoneBase::Probe(mPhone)->NotifyDataConnection(String(NULL));
            mIwlanRatAvailable = FALSE;
        }
    }

    if (hasCdmaDataConnectionAttached) {
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
    // TODO: Add CdmaCellIdenity updating, see CdmaLteServiceStateTracker.
    return NOERROR;
}

Boolean CdmaServiceStateTracker::IsInvalidOperatorNumeric(
    /* [in] */ const String& operatorNumeric)
{
    return operatorNumeric == NULL || operatorNumeric.GetLength() < 5 ||
                operatorNumeric.StartWith(INVALID_MCC);
}

String CdmaServiceStateTracker::FixUnknownMcc(
    /* [in] */ const String& _operatorNumeric,
    /* [in] */ Int32 sid)
{
    String operatorNumeric = _operatorNumeric;

    if (sid <= 0) {
        // no cdma information is available, do nothing
        return operatorNumeric;
    }

    // resolve the mcc from sid;
    // if mSavedTimeZone is NULL, TimeZone would get the default timeZone,
    // and the fixTimeZone couldn't help, because it depends on operator Numeric;
    // if the sid is conflict and timezone is unavailable, the mcc may be not right.
    Boolean isNitzTimeZone = FALSE;
    Int32 timeZone = 0;
    AutoPtr<ITimeZone> tzone;
    if (mSavedTimeZone != NULL) {
        AutoPtr<ITimeZoneHelper> tzHelper;
        CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
        tzHelper->GetTimeZone(mSavedTimeZone, (ITimeZone**)&tzone);
        Int32 offset;
        tzone->GetRawOffset(&offset);
        timeZone = offset/MS_PER_HOUR;
        isNitzTimeZone = TRUE;
    }
    else {
        tzone = GetNitzTimeZone(mZoneOffset, mZoneDst, mZoneTime);
        if (tzone != NULL) {
            Int32 val;
            tzone->GetRawOffset(&val);
            timeZone = val / MS_PER_HOUR;
        }
    }

    Int32 mcc;
    mHbpcdUtils->GetMcc(sid,
            timeZone, (mZoneDst ? 1 : 0), isNitzTimeZone, &mcc);
    if (mcc > 0) {
        operatorNumeric = StringUtils::ToString(mcc) + DEFAULT_MNC;
    }
    return operatorNumeric;
}

void CdmaServiceStateTracker::SetOperatorIdd(
    /* [in] */ const String& operatorNumeric)
{
    // Retrieve the current country information
    // with the MCC got from opeatorNumeric.
    String idd;
    mHbpcdUtils->GetIddByMcc(
            StringUtils::ParseInt32(operatorNumeric.Substring(0,3)), &idd);
    if (idd != NULL && !idd.IsEmpty()) {
        IPhoneBase::Probe(mPhone)->SetSystemProperty(
                ITelephonyProperties::PROPERTY_OPERATOR_IDP_STRING, idd);
    }
    else {
        // use default "+", since we don't know the current IDP
        IPhoneBase::Probe(mPhone)->SetSystemProperty(
                ITelephonyProperties::PROPERTY_OPERATOR_IDP_STRING, String("+"));
    }
}

Int32 CdmaServiceStateTracker::RadioTechnologyToDataServiceState(
    /* [in] */ Int32 code)
{
    Int32 retVal = IServiceState::STATE_OUT_OF_SERVICE;
    switch(code) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        break;
    case 6: // RADIO_TECHNOLOGY_1xRTT
    case 7: // RADIO_TECHNOLOGY_EVDO_0
    case 8: // RADIO_TECHNOLOGY_EVDO_A
    case 12: // RADIO_TECHNOLOGY_EVDO_B
    case 13: // RADIO_TECHNOLOGY_EHRPD
        retVal = IServiceState::STATE_IN_SERVICE;
        break;
    default:
        Loge(String("radioTechnologyToDataServiceState: Wrong radioTechnology code."));
    break;
    }
    return(retVal);
}

Int32 CdmaServiceStateTracker::RegCodeToServiceState(
    /* [in] */ Int32 code)
{
    switch (code) {
    case 0: // Not searching and not registered
        return IServiceState::STATE_OUT_OF_SERVICE;
    case 1:
        return IServiceState::STATE_IN_SERVICE;
    case 2: // 2 is "searching", fall through
    case 3: // 3 is "registration denied", fall through
    case 4: // 4 is "unknown", not valid in current baseband
        return IServiceState::STATE_OUT_OF_SERVICE;
    case 5:// 5 is "Registered, roaming"
        return IServiceState::STATE_IN_SERVICE;

    default:
        Loge(String("RegCodeToServiceState: unexpected service state ") + StringUtils::ToString(code));
    return IServiceState::STATE_OUT_OF_SERVICE;
    }
}

Boolean CdmaServiceStateTracker::RegCodeIsRoaming(
    /* [in] */ Int32 code)
{
    // 5 is  "in service -- roam"
    return 5 == code;
}

Boolean CdmaServiceStateTracker::IsSidsAllZeros()
{
    if (mHomeSystemId != NULL) {
        for (Int32 i = 0; i < mHomeSystemId->GetLength(); i++) {
            if ((*mHomeSystemId)[i] != 0) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

ECode CdmaServiceStateTracker::HangupAndPowerOff()
{
    // hang up all active voice calls
    ICall::Probe(((CdmaCallTracker*)((CDMAPhone*)mPhone.Get())->mCT.Get())->mRingingCall)->HangupIfAlive();
    ICall::Probe(((CdmaCallTracker*)((CDMAPhone*)mPhone.Get())->mCT.Get())->mBackgroundCall)->HangupIfAlive();
    ICall::Probe(((CdmaCallTracker*)((CDMAPhone*)mPhone.Get())->mCT.Get())->mForegroundCall)->HangupIfAlive();
    mCi->SetRadioPower(FALSE, NULL);
    return NOERROR;
}

void CdmaServiceStateTracker::ParseSidNid(
    /* [in] */ const String& sidStr,
    /* [in] */ const String& nidStr)
{
    if (sidStr != NULL) {
        AutoPtr<ArrayOf<String> > sid;
        StringUtils::Split(sidStr, ",", (ArrayOf<String>**)&sid);
        mHomeSystemId = ArrayOf<Int32>::Alloc(sid->GetLength());
        for (Int32 i = 0; i < sid->GetLength(); i++) {
            // try {
            (*mHomeSystemId)[i] = StringUtils::ParseInt32((*sid)[i]);
            // } catch (NumberFormatException ex) {
            //     Loge(String("error parsing system id: ") + ex);
            // }
        }
    }
    if (DBG) Log(String("CDMA_SUBSCRIPTION: SID=") + sidStr);

    if (nidStr != NULL) {
        AutoPtr<ArrayOf<String> > nid;
        StringUtils::Split(nidStr, ",", (ArrayOf<String>**)&nid);
        mHomeNetworkId = ArrayOf<Int32>::Alloc(nid->GetLength());
        for (Int32 i = 0; i < nid->GetLength(); i++) {
            // try {
            (*mHomeNetworkId)[i] = StringUtils::ParseInt32((*nid)[i]);
            // } catch (NumberFormatException ex) {
            //     Loge(String("CDMA_SUBSCRIPTION: error parsing network id: ") + ex);
            // }
        }
    }
    if (DBG) Log(String("CDMA_SUBSCRIPTION: NID=") + nidStr);
}

void CdmaServiceStateTracker::UpdateOtaspState()
{
    Int32 otaspMode;
    GetOtasp(&otaspMode);
    Int32 oldOtaspMode = mCurrentOtaspMode;
    mCurrentOtaspMode = otaspMode;

    // Notify apps subscription info is ready
    if (mCdmaForSubscriptionInfoReadyRegistrants != NULL) {
        if (DBG) Log(String("CDMA_SUBSCRIPTION: call notifyRegistrants()"));
        mCdmaForSubscriptionInfoReadyRegistrants->NotifyRegistrants();
    }
    if (oldOtaspMode != mCurrentOtaspMode) {
        if (DBG) {
            Log(String("CDMA_SUBSCRIPTION: call notifyOtaspChanged old otaspMode=") +
                StringUtils::ToString(oldOtaspMode) + " new otaspMode=" +
                StringUtils::ToString(mCurrentOtaspMode));
        }
        IPhoneBase::Probe(mPhone)->NotifyOtaspChanged(mCurrentOtaspMode);
    }
}

AutoPtr<IUiccCardApplication> CdmaServiceStateTracker::GetUiccCardApplication()
{
    Int32 pid;
    IPhone::Probe(mPhone)->GetPhoneId(&pid);
    AutoPtr<IUiccCardApplication> app;
    mUiccController->GetUiccCardApplication(pid,
            IUiccController::APP_FAM_3GPP2, (IUiccCardApplication**)&app);
    return app;
}

ECode CdmaServiceStateTracker::OnUpdateIccAvailability()
{
    if (mUiccController == NULL ) {
        return NOERROR;
    }

    AutoPtr<IUiccCardApplication> newUiccApplication = GetUiccCardApplication();

    if (mUiccApplcation != newUiccApplication) {
        Log(String("Removing stale icc objects."));
        UnregisterForRuimEvents();
        mIccRecords = NULL;
        mUiccApplcation = NULL;
        if (newUiccApplication != NULL) {
            Log(String("New card found"));
            mUiccApplcation = newUiccApplication;
            mUiccApplcation->GetIccRecords((IIccRecords**)&mIccRecords);

            if (mIsSubscriptionFromRuim) {
                RegisterForRuimEvents();
            }
        }
    }
    return NOERROR;
}

ECode CdmaServiceStateTracker::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOGTAG, "[CdmaSST] %s", s.string());
    return NOERROR;
}

ECode CdmaServiceStateTracker::Loge(
    /* [in] */ const String& s)
{
    Logger::E(LOGTAG, "[CdmaSST] %s", s.string());
    return NOERROR;
}

void CdmaServiceStateTracker::SaveCdmaSubscriptionSource(
    /* [in] */ Int32 source)
{
    Log(String("Storing cdma subscription source: ") + StringUtils::ToString(source));
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Boolean b;
    Settings::Global::PutInt32(cr, ISettingsGlobal::CDMA_SUBSCRIPTION_MODE,
            source, &b);
}

void CdmaServiceStateTracker::GetSubscriptionInfoAndStartPollingThreads()
{
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_POLL_STATE_CDMA_SUBSCRIPTION, (IMessage**)&msg);
    mCi->GetCDMASubscription(msg);

    // Get Registration Information
    PollState();
}

void CdmaServiceStateTracker::HandleCdmaSubscriptionSource(
    /* [in] */ Int32 newSubscriptionSource)
{
    Log(String("Subscription Source : ") + StringUtils::ToString(newSubscriptionSource));
    mIsSubscriptionFromRuim =
        (newSubscriptionSource == ICdmaSubscriptionSourceManager::SUBSCRIPTION_FROM_RUIM);
    SaveCdmaSubscriptionSource(newSubscriptionSource);
    if (!mIsSubscriptionFromRuim) {
        // Unregister from any previous RUIM events if registered
        // (switching from RUIM/SIM to NV)
        UnregisterForRuimEvents();
        // NV is ready when subscription source is NV
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_NV_READY, (IMessage**)&msg);
        Boolean b;
        SendMessage(msg, &b);
    }
    else {
        RegisterForRuimEvents();
    }
}

Int32 CdmaServiceStateTracker::GetCombinedRegState()
{
    Int32 regState;
    mSS->GetVoiceRegState(&regState);
    Int32 dataRegState;
    mSS->GetDataRegState(&dataRegState);

    if ((regState == IServiceState::STATE_OUT_OF_SERVICE)
            && (dataRegState == IServiceState::STATE_IN_SERVICE)) {
        Log(String("getCombinedRegState: return STATE_IN_SERVICE as Data is in service"));
        regState = dataRegState;
    }

    return regState;
}

AutoPtr<ITimeZone> CdmaServiceStateTracker::GetNitzTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean dst,
    /* [in] */ Int64 when)
{
    AutoPtr<ITimeZone> guess = FindTimeZone(offset, dst, when);
    if (guess == NULL) {
        // Couldn't find a proper timezone.  Perhaps the DST data is wrong.
        guess = FindTimeZone(offset, !dst, when);
    }
    // if (DBG) Log(String("GetNitzTimeZone returning ") + (guess == NULL ? guess : guess->GetID()));
    return guess;
}

AutoPtr<ITimeZone> CdmaServiceStateTracker::FindTimeZone(
    /* [in] */ Int32 offset,
    /* [in] */ Boolean dst,
    /* [in] */ Int64 when)
{
    Int32 rawOffset = offset;
    if (dst) {
        rawOffset -= MS_PER_HOUR;
    }
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ArrayOf<String> > zones;
    tzHelper->GetAvailableIDs(rawOffset, (ArrayOf<String>**)&zones);
    AutoPtr<ITimeZone> guess;
    AutoPtr<IDate> d;
    CDate::New(when, (IDate**)&d);

    for (Int32 i = 0; i < zones->GetLength(); i++) {
        String zone = (*zones)[i];
        AutoPtr<ITimeZone> tz;
        tzHelper->GetTimeZone(zone, (ITimeZone**)&tz);
        Int32 os;
        tz->GetOffset(when, &os);
        Boolean b;
        tz->InDaylightTime(d, &b);
        if (os == offset && b == dst) {
            guess = tz;
            break;
        }
    }

    return guess;
}

void CdmaServiceStateTracker::QueueNextSignalStrengthPoll()
{
    if (mDontPollSignalStrength) {
        // The radio is telling us about signal strength changes
        // we don't have to ask it
        return;
    }

    AutoPtr<IMessage> msg;

    ObtainMessage((IMessage**)&msg);
    msg->SetWhat(EVENT_POLL_SIGNAL_STRENGTH);

    // TODO Don't poll signal strength if screen is off
    Boolean b;
    SendMessageDelayed(msg, POLL_PERIOD_MILLIS, &b);
}

Boolean CdmaServiceStateTracker::IsRoamIndForHomeSystem(
    /* [in] */ const String& roamInd)
{
    // retrieve the carrier-specified list of ERIs for home system
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > homeRoamIndicators;
    res->GetStringArray(R::array::config_cdma_home_system,
            (ArrayOf<String>**)&homeRoamIndicators);

    if (homeRoamIndicators != NULL) {
        // searches through the comma-separated list for a match,
        // return TRUE if one is found.
        for (Int32 i = 0; i < homeRoamIndicators->GetLength(); i++) {
            String homeRoamInd = (*homeRoamIndicators)[i];
            if (homeRoamInd.Equals(roamInd)) {
                return TRUE;
            }
        }
        // no matches found against the list!
        return FALSE;
    }

    // no system property found for the roaming indicators for home system
    return FALSE;
}

Boolean CdmaServiceStateTracker::IsRoamingBetweenOperators(
    /* [in] */ Boolean cdmaRoaming,
    /* [in] */ IServiceState* s)
{
    String spn;
    GetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, String("empty"), &spn);

    // NOTE: in case of RUIM we should completely ignore the ERI data file and
    // mOperatorAlphaLong is set from RIL_REQUEST_OPERATOR response 0 (alpha ONS)
    String onsl;
    s->GetOperatorAlphaLong(&onsl);
    String onss;
    s->GetOperatorAlphaShort(&onss);

    Boolean equalsOnsl = onsl != NULL && spn.Equals(onsl);
    Boolean equalsOnss = onss != NULL && spn.Equals(onss);

    return cdmaRoaming && !(equalsOnsl || equalsOnss);
}

void CdmaServiceStateTracker::SetTimeFromNITZString(
    /* [in] */ const String& nitz,
    /* [in] */ Int64 nitzReceiveTime)
{
    // "yy/mm/dd,hh:mm:ss(+/-)tz"
    // tz is in number of quarter-hours

    Int64 start = SystemClock::GetElapsedRealtime();
    if (DBG) {
        Log(String("NITZ: ") + nitz + ","
                + StringUtils::ToString(nitzReceiveTime) +
                " start=" + StringUtils::ToString(start)
                + " delay=" + StringUtils::ToString((start - nitzReceiveTime)));
    }

    // try {
    /* NITZ time (hour:min:sec) will be in UTC but it supplies the timezone
     * offset as well (which we won't worry about until later) */
    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    AutoPtr<ITimeZone> tz;
    tzHelper->GetTimeZone(String("GMT"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    AutoPtr<ICalendar> c;
    helper->GetInstance(tz, (ICalendar**)&c);

    c->Clear();
    c->Set(ICalendar::DST_OFFSET, 0);

    AutoPtr<ArrayOf<String> > nitzSubs;
    StringUtils::Split(nitz, String("[/:,+-]"), (ArrayOf<String>**)&nitzSubs);

    Int32 year = 2000 + StringUtils::ParseInt32((*nitzSubs)[0]);
    c->Set(ICalendar::YEAR, year);

    // month is 0 based!
    Int32 month = StringUtils::ParseInt32((*nitzSubs)[1]) - 1;
    c->Set(ICalendar::MONTH, month);

    Int32 date = StringUtils::ParseInt32((*nitzSubs)[2]);
    c->Set(ICalendar::DATE, date);

    Int32 hour = StringUtils::ParseInt32((*nitzSubs)[3]);
    c->Set(ICalendar::HOUR, hour);

    Int32 minute = StringUtils::ParseInt32((*nitzSubs)[4]);
    c->Set(ICalendar::MINUTE, minute);

    Int32 second = StringUtils::ParseInt32((*nitzSubs)[5]);
    c->Set(ICalendar::SECOND, second);

    Boolean sign = (nitz.IndexOf('-') == -1);

    Int32 tzOffset = StringUtils::ParseInt32((*nitzSubs)[6]);

    Int32 dst = (nitzSubs->GetLength() >= 8 ) ?
            StringUtils::ParseInt32((*nitzSubs)[7]) : 0;

    // The zone offset received from NITZ is for current local time,
    // so DST correction is already applied.  Don't add it again.
    //
    // tzOffset += dst * 4;
    //
    // We could unapply it if we wanted the raw offset.

    tzOffset = (sign ? 1 : -1) * tzOffset * 15 * 60 * 1000;

    AutoPtr<ITimeZone> zone;

    // As a special extension, the Android emulator appends the name of
    // the host computer's timezone to the nitz string. this is zoneinfo
    // timezone name of the form Area!Location or Area!Location!SubLocation
    // so we need to convert the ! into /
    if (nitzSubs->GetLength() >= 9) {
        String  tzname;
        StringUtils::Replace((*nitzSubs)[8], "!", "/", &tzname);
        tzHelper->GetTimeZone( tzname, (ITimeZone**)&zone);
    }

    String iso;
    GetSystemProperty(ITelephonyProperties::PROPERTY_OPERATOR_ISO_COUNTRY, String(""), &iso);

    if (zone == NULL) {
        if (mGotCountryCode) {
            if (iso != NULL && iso.GetLength() > 0) {
                Int64 t;
                c->GetTimeInMillis(&t);
                zone = TimeUtils::GetTimeZone(tzOffset, dst != 0,
                        t, iso);
            }
            else {
                // We don't have a valid iso country code.  This is
                // most likely because we're on a test network that's
                // using a bogus MCC (eg, "001"), so get a TimeZone
                // based only on the NITZ parameters.
                Int64 t;
                c->GetTimeInMillis(&t);
                zone = GetNitzTimeZone(tzOffset, (dst != 0), t);
            }
        }
    }

    if ((zone == NULL) || (mZoneOffset != tzOffset) || (mZoneDst != (dst != 0))){
        // We got the time before the country or the zone has changed
        // so we don't know how to identify the DST rules yet.  Save
        // the information and hope to fix it up later.

        mNeedFixZone = TRUE;
        mZoneOffset  = tzOffset;
        mZoneDst     = dst != 0;
        c->GetTimeInMillis(&mZoneTime);
    }
    if (DBG) {
        // Log(String("NITZ: tzOffset=") + tzOffset + " dst=" + dst + " zone=" +
        //         (zone!=NULL ? zone->GetID() : "NULL") +
        //         " iso=" + iso + " mGotCountryCode=" + mGotCountryCode +
        //         " mNeedFixZone=" + mNeedFixZone);
    }

    if (zone != NULL) {
        String id;
        zone->GetID(&id);
        if (GetAutoTimeZone()) {
            SetAndBroadcastNetworkSetTimeZone(id);
        }
        SaveNitzTimeZone(id);
    }

    String ignore;
    SystemProperties::Get(String("gsm.ignore-nitz"), &ignore);
    if (ignore != NULL && ignore.Equals("yes")) {
        if (DBG) Log(String("NITZ: Not setting clock because gsm.ignore-nitz is set"));
        return;
    }

    // try {
    mWakeLock->AcquireLock();

    /**
     * Correct the NITZ time by how Int64 its taken to get here.
     */
    Int64 millisSinceNitzReceived
            = SystemClock::GetElapsedRealtime() - nitzReceiveTime;

    if (millisSinceNitzReceived < 0) {
        // Sanity check: something is wrong
        if (DBG) {
            Log(String("NITZ: not setting time, clock has rolled ")
                            + "backwards since NITZ time was received, "
                            + nitz);
        }
        return;
    }

    if (millisSinceNitzReceived > Elastos::Core::Math::INT32_MAX_VALUE) {
        // If the time is this far off, something is wrong > 24 days!
        if (DBG) {
            Log(String("NITZ: not setting time, processing has taken ")
                        + StringUtils::ToString(millisSinceNitzReceived / (1000 * 60 * 60 * 24))
                        + " days");
        }
        return;
    }

    // Note: with range checks above, cast to Int32 is safe
    c->Add(ICalendar::MILLISECOND, (Int32)millisSinceNitzReceived);

    if (GetAutoTime()) {
        /**
         * Update system time automatically
         */
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 currentTimeMillis;
        system->GetCurrentTimeMillis(&currentTimeMillis);
        Int64 t;
        c->GetTimeInMillis(&t);
        Int64 gained = t - currentTimeMillis;
        Int64 timeSinceLastUpdate = SystemClock::GetElapsedRealtime() - mSavedAtTime;
        Int32 nitzUpdateSpacing;
        Settings::Global::GetInt32(mCr,
                ISettingsGlobal::NITZ_UPDATE_SPACING, mNitzUpdateSpacing, &nitzUpdateSpacing);
        Int32 nitzUpdateDiff;
        Settings::Global::GetInt32(mCr,
                ISettingsGlobal::NITZ_UPDATE_DIFF, mNitzUpdateDiff, &nitzUpdateDiff);

        if ((mSavedAtTime == 0) || (timeSinceLastUpdate > nitzUpdateSpacing)
                || (Elastos::Core::Math::Abs(gained) > nitzUpdateDiff)) {
            if (DBG) {
                // Log(String("NITZ: Auto updating time of day to ") + c->GetTime()
                //     + " NITZ receive delay=" + millisSinceNitzReceived
                //     + "ms gained=" + gained + "ms from " + nitz);
            }

            c->GetTimeInMillis(&t);
            SetAndBroadcastNetworkSetTime(t);
        }
        else {
            if (DBG) {
                Log(String("NITZ: ignore, a previous update was ")
                    + StringUtils::ToString(timeSinceLastUpdate)
                    + "ms ago and gained=" + StringUtils::ToString(gained) + "ms");
            }
            return;
        }
    }

    /**
     * Update properties and save the time we did the update
     */
    if (DBG) Log(String("NITZ: update nitz time property"));
    c->GetTimeInMillis(&mSavedTime);
    SystemProperties::Set(String("gsm.nitz.time"), StringUtils::ToString(mSavedTime));
    mSavedAtTime = SystemClock::GetElapsedRealtime();
    // } finally {
    Int64 end = SystemClock::GetElapsedRealtime();
    if (DBG) Log(String("NITZ: end=") + StringUtils::ToString(end)
            + " dur=" + StringUtils::ToString((end - start)));
    mWakeLock->ReleaseLock();
    // }
    // } catch (RuntimeException ex) {
    //     Loge(String("NITZ: Parsing NITZ time ") + nitz + " ex=" + ex);
    // }
}

Boolean CdmaServiceStateTracker::GetAutoTime()
{
    // try {
    Int32 val;
    Settings::Global::GetInt32(mCr, ISettingsGlobal::AUTO_TIME, &val);
    return val > 0;
    // } catch (SettingNotFoundException snfe) {
    //     return TRUE;
    // }
}

Boolean CdmaServiceStateTracker::GetAutoTimeZone()
{
    // try {
    Int32 val;
    Settings::Global::GetInt32(mCr, ISettingsGlobal::AUTO_TIME_ZONE, &val);
    return val > 0;
    // } catch (SettingNotFoundException snfe) {
    //     return TRUE;
    // }
}

void CdmaServiceStateTracker::SaveNitzTimeZone(
    /* [in] */ const String& zoneId)
{
    mSavedTimeZone = zoneId;
}

void CdmaServiceStateTracker::SetAndBroadcastNetworkSetTimeZone(
    /* [in] */ const String& zoneId)
{
    if (DBG) Log(String("SetAndBroadcastNetworkSetTimeZone: setTimeZone=") + zoneId);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IInterface> obj;
    ctx->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&obj);
    AutoPtr<IAlarmManager> alarm = IAlarmManager::Probe(obj);
    alarm->SetTimeZone(zoneId);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_NETWORK_SET_TIMEZONE, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(String("time-zone"), zoneId);
    ctx->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void CdmaServiceStateTracker::SetAndBroadcastNetworkSetTime(
    /* [in] */ Int64 time)
{
    if (DBG) Log(String("SetAndBroadcastNetworkSetTime: time=") + StringUtils::ToString(time) + "ms");
    SystemClock::SetCurrentTimeMillis(time);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_NETWORK_SET_TIME, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    intent->PutExtra(String("time"), time);
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    ctx->SendStickyBroadcastAsUser(intent, UserHandle::ALL);
}

void CdmaServiceStateTracker::RevertToNitzTime()
{
    Int32 val;
    Settings::Global::GetInt32(mCr, ISettingsGlobal::AUTO_TIME, 0, &val);
    if (val == 0) {
        return;
    }
    if (DBG) {
        Log(String("RevertToNitzTime: mSavedTime=") + StringUtils::ToString(mSavedTime) +
                " mSavedAtTime=" + StringUtils::ToString(mSavedAtTime));
    }
    if (mSavedTime != 0 && mSavedAtTime != 0) {
        SetAndBroadcastNetworkSetTime(mSavedTime
                + (SystemClock::GetElapsedRealtime() - mSavedAtTime));
    }
}

void CdmaServiceStateTracker::RevertToNitzTimeZone()
{
    AutoPtr<IContext> ctx;
    IPhone::Probe(mPhone)->GetContext((IContext**)&ctx);
    AutoPtr<IContentResolver> cr;
    ctx->GetContentResolver((IContentResolver**)&cr);
    Int32 val;
    Settings::Global::GetInt32(cr,
                ISettingsGlobal::AUTO_TIME_ZONE, 0, &val);
    if (val == 0) {
        return;
    }
    if (DBG) Log(String("RevertToNitzTimeZone: tz='") + mSavedTimeZone);
    if (!mSavedTimeZone.IsNull()) {
        SetAndBroadcastNetworkSetTimeZone(mSavedTimeZone);
    }
}

Boolean CdmaServiceStateTracker::IsHomeSid(
    /* [in] */ Int32 sid)
{
    if (mHomeSystemId != NULL) {
        for (Int32 i=0; i < mHomeSystemId->GetLength(); i++) {
            if (sid == (*mHomeSystemId)[i]) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

void CdmaServiceStateTracker::RegisterForRuimEvents()
{
    Log(String("registerForRuimEvents"));
    if (mUiccApplcation != NULL) {
        mUiccApplcation->RegisterForReady(this, EVENT_RUIM_READY, NULL);
    }
    if (mIccRecords != NULL) {
        mIccRecords->RegisterForRecordsLoaded(this, EVENT_RUIM_RECORDS_LOADED, NULL);
    }
}

void CdmaServiceStateTracker::UnregisterForRuimEvents()
{
    Log(String("UnregisterForRuimEvents"));
    if (mUiccApplcation != NULL) {
        mUiccApplcation->UnregisterForReady(this);
    }
    if (mIccRecords != NULL) {
        mIccRecords->UnregisterForRecordsLoaded(this);
    }
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
