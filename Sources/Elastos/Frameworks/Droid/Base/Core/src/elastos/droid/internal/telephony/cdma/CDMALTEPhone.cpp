#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CCdmaLteServiceStateTracker.h"
#include "elastos/droid/internal/telephony/cdma/CdmaCallTracker.h"
#include "elastos/droid/internal/telephony/cdma/CDMALTEPhone.h"
#include "elastos/droid/internal/telephony/cdma/CdmaSubscriptionSourceManager.h"
#include "elastos/droid/internal/telephony/cdma/CEriManager.h"
#include "elastos/droid/internal/telephony/cdma/CRuimPhoneBookInterfaceManager.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfo.h"
#include "elastos/droid/internal/telephony/dataconnection/CDcTracker.h"
#include "elastos/droid/internal/telephony/MccTable.h"
#include "elastos/droid/internal/telephony/PhoneProxy.h"
#include "elastos/droid/internal/telephony/Subscription.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/provider/Telephony.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Internal::Telephony::CPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::DataConnection::CDcTracker;
using Elastos::Droid::Internal::Telephony::DataConnection::IDcTracker;
using Elastos::Droid::Internal::Telephony::IPhoneSubInfo;
using Elastos::Droid::Internal::Telephony::MccTable;
using Elastos::Droid::Internal::Telephony::PhoneProxy;
using Elastos::Droid::Internal::Telephony::Subscription;
using Elastos::Droid::Internal::Telephony::SubscriptionController;
using Elastos::Droid::Internal::Telephony::Uicc::IRuimRecords;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Provider::ITelephonyCarriers;
using Elastos::Droid::Provider::Telephony;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Cdma {

//=====================================================================
//                             CDMALTEPhone
//=====================================================================
CAR_INTERFACE_IMPL(CDMALTEPhone, CDMAPhone, ICDMALTEPhone);

const String CDMALTEPhone::LOG_LTE_TAG("CDMALTEPhone");
const Boolean CDMALTEPhone::DBG = TRUE;

CDMALTEPhone::CDMALTEPhone()
{
}

ECode CDMALTEPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Int32 phoneId)
{
    return constructor(context, ci, notifier, FALSE, phoneId);
}

ECode CDMALTEPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode,
    /* [in] */ Int32 phoneId)
{
    CDMAPhone::constructor(context, ci, notifier, phoneId);

    Logger::D(LOGTAG, "CDMALTEPhone: constructor: sub = %d", mPhoneId);

    CDcTracker::New(this, (IDcTracker**)&mDcTracker);
    return NOERROR;
}

ECode CDMALTEPhone::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier)
{
    return CDMAPhone::constructor(context, ci, notifier, FALSE);
}

ECode CDMALTEPhone::Dispose()
{
    AutoLock lock(PhoneProxy::lockForRadioTechnologyChange);
    if (mSimRecords != NULL) {
        IIccRecords::Probe(mSimRecords)->UnregisterForRecordsLoaded(this);
    }
    CDMAPhone::Dispose();

    return NOERROR;
}

ECode CDMALTEPhone::RemoveReferences()
{
    return CDMAPhone::RemoveReferences();
}

ECode CDMALTEPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<AsyncResult> ar;
    AutoPtr<IMessage> onComplete;

    // messages to be handled whether or not the phone is being destroyed
    // should only include messages which are being re-directed and do not use
    // resources of the phone being destroyed
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        // handle the select network completion callbacks.
        case EVENT_SET_NETWORK_MANUAL_COMPLETE:
        case EVENT_SET_NETWORK_AUTOMATIC_COMPLETE:
            return CDMAPhone::HandleMessage(msg);
    }

    if (!mIsTheCurrentActivePhone) {
        // Logger::E(LOGTAG, "Received message " + msg +
        //         "[" + msg.what + "] while being destroyed. Ignoring.");
        return NOERROR;
    }
    switch(what) {
        case EVENT_SIM_RECORDS_LOADED:
            mSimRecordsLoadedRegistrants->NotifyRegistrants();
            break;

        case IPhoneConstants::EVENT_SUBSCRIPTION_ACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_ACTIVATED"));
            OnSubscriptionActivated();
            break;

        case IPhoneConstants::EVENT_SUBSCRIPTION_DEACTIVATED:
            Log(String("EVENT_SUBSCRIPTION_DEACTIVATED"));
            OnSubscriptionDeactivated();
            break;

        default:
            CDMAPhone::HandleMessage(msg);
    }
     return NOERROR;
}

ECode CDMALTEPhone::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    PhoneConstantsDataState ret = PhoneConstantsDataState_DISCONNECTED;

    Int32 val;
    Boolean b;
    if (mSST == NULL) {
        // Radio Technology Change is ongoing, dispose() and
        // removeReferences() have already been called

        ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else if ((IServiceStateTracker::Probe(mSST)->GetCurrentDataConnectionState(&val), val)
            != IServiceState::STATE_IN_SERVICE && mOosIsDisconnect) {
        ret = PhoneConstantsDataState_DISCONNECTED;
        Log(String("getDataConnectionState: Data is Out of Service. ret = ")
                + StringUtils::ToString(ret));
    }
    else if ((mDcTracker->IsApnTypeEnabled(apnType, &b), b) == FALSE) {
        ret = PhoneConstantsDataState_DISCONNECTED;
    }
    else {
        DctConstantsState state;
        mDcTracker->GetState(apnType, &state);
        switch (state) {
            case DctConstantsState_RETRYING:
            case DctConstantsState_FAILED:
            case DctConstantsState_IDLE:
                ret = PhoneConstantsDataState_DISCONNECTED;
                break;

            case DctConstantsState_CONNECTED:
            case DctConstantsState_DISCONNECTING:
                if (((CdmaCallTracker*)mCT.Get())->mState != PhoneConstantsState_IDLE &&
                        (IServiceStateTracker::Probe(mSST)->IsConcurrentVoiceAndDataAllowed(&b), !b)) {
                    ret = PhoneConstantsDataState_SUSPENDED;
                }
                else {
                    ret = PhoneConstantsDataState_CONNECTED;
                }
                break;

            case DctConstantsState_CONNECTING:
            case DctConstantsState_SCANNING:
                ret = PhoneConstantsDataState_CONNECTING;
                break;
        }
    }

    Log(String("getDataConnectionState apnType=") + apnType + " ret=" + StringUtils::ToString(ret));
    *result = ret;
    return NOERROR;
}

ECode CDMALTEPhone::UpdateCurrentCarrierInProvider(
    /* [in] */ const String& operatorNumeric,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean retVal;
    AutoPtr<IUiccCardApplication> uca;
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_3GPP,
            (IUiccCardApplication**)&uca);

    if (uca == NULL) {
        if (DBG) Log(String("updateCurrentCarrierInProvider APP_FAM_3GPP == NULL"));
        CDMAPhone::UpdateCurrentCarrierInProvider(operatorNumeric, &retVal);
    }
    else {
        if (DBG) Log(String("updateCurrentCarrierInProvider not updated"));
        retVal = TRUE;
    }
    if (DBG) Log(String("updateCurrentCarrierInProvider X retVal=")
            + StringUtils::BooleanToString(retVal));

    *result = retVal;
    return NOERROR;
}

ECode CDMALTEPhone::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 currentDds;
    SubscriptionManager::GetDefaultDataSubId(&currentDds);
    String operatorNumeric;
    GetOperatorNumeric(&operatorNumeric);

    Int64 sid;
    GetSubId(&sid);
    Logger::D(LOGTAG, "updateCurrentCarrierInProvider: mSubscription = %d currentDds = %ld operatorNumeric = %s",
            sid, currentDds, operatorNumeric.string());

    if (!operatorNumeric.IsEmpty() && (sid == currentDds)) {
        // try {
        AutoPtr<IUri> uri;
        Uri::WithAppendedPath(Elastos::Droid::Provider::Telephony::Carriers::CONTENT_URI,
                String("current"), (IUri**)&uri);
        AutoPtr<IContentValues> map;
        CContentValues::New((IContentValues**)&map);
        map->Put(ITelephonyCarriers::NUMERIC, operatorNumeric);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<IUri> tmp;
        cr->Insert(uri, map, (IUri**)&tmp);
        *result = TRUE;
        return NOERROR;
        // } catch (SQLException e) {
        //     Logger::E(LOGTAG, "Can't store current operator", e);
        // }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CDMALTEPhone::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IIccRecords> r;
    if (mIccRecords != NULL) {
        AutoPtr<IInterface> obj;
        mIccRecords->Get((IInterface**)&obj);
        r = IIccRecords::Probe(obj);
    }
    else {
        r = NULL;
    }

    if (r != NULL) {
        String imsi;
        r->GetIMSI(&imsi);

        if (!imsi.IsEmpty()) {
            Log(String("IMSI = ") + imsi);
            *result = imsi;
            return NOERROR;
        }
    }

    Log(String("IMSI undefined"));
    *result = String("");
    return NOERROR;
}

ECode CDMALTEPhone::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 val;
    CTelephonyManager::GetLteOnCdmaModeStatic(&val);
    if (val == IPhoneConstants::LTE_ON_CDMA_TRUE) {
        *result = mImei;
    }
    else {
        return CDMAPhone::GetDeviceId(result);
    }
    return NOERROR;
}

ECode CDMALTEPhone::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mSimRecords != NULL) {
        return IIccRecords::Probe(mSimRecords)->GetGid1(result);
    }
    else {
        *result = String("");
    }
    return NOERROR;
}

ECode CDMALTEPhone::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImei;
    return NOERROR;
}

ECode CDMALTEPhone::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mImeiSv;
    return NOERROR;
}

ECode CDMALTEPhone::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result);
    *result = IIsimRecords::Probe(mIsimUiccRecords);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDMALTEPhone::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    if (mSimRecords != NULL) {
        return IIccRecords::Probe(mSimRecords)->GetMsisdnNumber(result);
    }
    else {
        *result = String(NULL);
    }
    return NOERROR;
}

ECode CDMALTEPhone::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return mCi->GetAvailableNetworks(response);
}

ECode CDMALTEPhone::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    Boolean b;
    if (GetUnitTestMode(&b), b) {
        return NOERROR;
    }
    Int64 sid;
    GetSubId(&sid);
    return CTelephonyManager::SetTelephonyProperty(property, sid, value);
}

ECode CDMALTEPhone::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    if (GetUnitTestMode(&b), b) {
        *result = NULL;
        return NOERROR;
    }
    Int64 sid;
    GetSubId(&sid);
    return CTelephonyManager::GetTelephonyProperty(property, sid, defValue, result);
}

ECode CDMALTEPhone::UpdateDataConnectionTracker()
{
    return IDcTracker::Probe(mDcTracker)->Update();
}

ECode CDMALTEPhone::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg)
{
    VALIDATE_NOT_NULL(onCompleteMsg);
    Boolean b;
    return IDcTracker::Probe(mDcTracker)->SetInternalDataEnabled(enable, onCompleteMsg, &b);
}

ECode CDMALTEPhone::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Boolean b;
    return IDcTracker::Probe(mDcTracker)->SetInternalDataEnabledFlag(enable, &b);
}

ECode CDMALTEPhone::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String operatorNumeric;
    AutoPtr<IIccRecords> curIccRecords;
    if (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
        SystemProperties::Get(String("ro.cdma.home.operator.numeric"), &operatorNumeric);
    }
    else if (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_RUIM_SIM) {
        curIccRecords = IIccRecords::Probe(mSimRecords);
        if (curIccRecords != NULL) {
            curIccRecords->GetOperatorNumeric(&operatorNumeric);
        }
        else {
            AutoPtr<IInterface> obj;
            mIccRecords->Get((IInterface**)&obj);
            curIccRecords = IIccRecords::Probe(obj);
            if (curIccRecords != NULL && (IRuimRecords::Probe(curIccRecords) != NULL)) {
                AutoPtr<IRuimRecords> csim = IRuimRecords::Probe(curIccRecords);
                csim->GetOperatorNumeric(&operatorNumeric);
            }
        }
    }
    if (operatorNumeric.IsNull()) {
        // Logger::E(LOGTAG, "getOperatorNumeric: Cannot retrieve operatorNumeric:"
        //         + " mCdmaSubscriptionSource = " + mCdmaSubscriptionSource + " mIccRecords = "
        //         + ((curIccRecords != NULL) ? curIccRecords.getRecordsLoaded() : NULL));
    }

    Logger::D(LOGTAG, "getOperatorNumeric: mCdmaSubscriptionSource = %d operatorNumeric = %s",
            mCdmaSubscriptionSource, operatorNumeric.string());

    *result = operatorNumeric;
    return NOERROR;
}

ECode CDMALTEPhone::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return IDcTracker::Probe(mDcTracker)->RegisterForAllDataDisconnected(h, what, obj);
}

ECode CDMALTEPhone::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    return IDcTracker::Probe(mDcTracker)->UnregisterForAllDataDisconnected(h);
}

ECode CDMALTEPhone::RegisterForSimRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    mSimRecordsLoadedRegistrants->AddUnique(h, what, obj);
    return NOERROR;
}

ECode CDMALTEPhone::UnregisterForSimRecordsLoaded(
    /* [in] */ IHandler* h)
{
    mSimRecordsLoadedRegistrants->Remove(h);
    return NOERROR;
}

ECode CDMALTEPhone::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    pw->Println(String("CDMALTEPhone extends:"));
    CDMAPhone::Dump(fd, pw, args);
    return NOERROR;
}

void CDMALTEPhone::InitSstIcc()
{
    AutoPtr<ICdmaLteServiceStateTracker> obj;
    CCdmaLteServiceStateTracker::New(this, (ICdmaLteServiceStateTracker**)&obj);
    mSST =ICdmaServiceStateTracker::Probe(obj);
 }

ECode CDMALTEPhone::OnUpdateIccAvailability()
{
    if (mSimRecords != NULL) {
        IIccRecords::Probe(mSimRecords)->UnregisterForRecordsLoaded(this);
    }

    if (mUiccController == NULL ) {
        return NOERROR;
    }

    // Update IsimRecords
    AutoPtr<IUiccCardApplication> newUiccApplication;
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_IMS,
            (IUiccCardApplication**)&newUiccApplication);
    AutoPtr<IIsimUiccRecords> newIsimUiccRecords;

    if (newUiccApplication != NULL) {
        AutoPtr<IIccRecords> ir;
        newUiccApplication->GetIccRecords((IIccRecords**)&ir);
        newIsimUiccRecords = IIsimUiccRecords::Probe(ir);
    }
    mIsimUiccRecords = newIsimUiccRecords;

    // Update UsimRecords
    mUiccController->GetUiccCardApplication(mPhoneId, IUiccController::APP_FAM_3GPP,
            (IUiccCardApplication**)&newUiccApplication);
    AutoPtr<ISIMRecords> newSimRecords;
    if (newUiccApplication != NULL) {
        AutoPtr<IIccRecords> ir;
        newUiccApplication->GetIccRecords((IIccRecords**)&ir);
        newSimRecords = ISIMRecords::Probe(ir);
    }
    mSimRecords = newSimRecords;
    if (mSimRecords != NULL) {
        IIccRecords::Probe(mSimRecords)->RegisterForRecordsLoaded(
                this, EVENT_SIM_RECORDS_LOADED, NULL);
    }

    CDMAPhone::OnUpdateIccAvailability();
    return NOERROR;
}

void CDMALTEPhone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    mCi->SetPhoneType(IPhoneConstants::PHONE_TYPE_CDMA);
    CCdmaCallTracker::New(this, (ICdmaCallTracker**)&mCT);

    mCdmaSSM = CdmaSubscriptionSourceManager::GetInstance(context, mCi, this,
            EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, NULL);
    CRuimPhoneBookInterfaceManager::New(this,
            (IRuimPhoneBookInterfaceManager**)&mRuimPhoneBookInterfaceManager);
    CPhoneSubInfo::New(this, (IPhoneSubInfo**)&mSubInfo);
    CEriManager::New(this, context, CEriManager::ERI_FROM_XML, (IEriManager**)&mEriManager);

    mCi->RegisterForAvailable(this, EVENT_RADIO_AVAILABLE, NULL);
    mCi->RegisterForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, NULL);
    mCi->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCi->SetOnSuppServiceNotification(this, EVENT_SSN, NULL);
    IServiceStateTracker::Probe(mSST)->RegisterForNetworkAttached(this,
            EVENT_REGISTERED_TO_NETWORK, NULL);
    mCi->SetEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, NULL);
    mCi->RegisterForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
            NULL);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj);
    AutoPtr<IPowerManager> pm = IPowerManager::Probe(obj);
    pm->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, LOGTAG, (IPowerManagerWakeLock**)&mWakeLock);

    // This is needed to handle phone process crashes
    String inEcm;
    SystemProperties::Get(ITelephonyProperties::PROPERTY_INECM_MODE, String("FALSE"), &inEcm);
    mIsPhoneInEcmState = inEcm.Equals("TRUE");
    if (mIsPhoneInEcmState) {
        // Send a message which will invoke handleExitEmergencyCallbackMode
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE, (IMessage**)&msg);
        mCi->ExitEmergencyCallbackMode(msg);
    }

    // get the string that specifies the carrier OTA Sp number
    SystemProperties::Get(ITelephonyProperties::PROPERTY_OTASP_NUM_SCHEMA,
            String(""), &mCarrierOtaSpNumSchema);

    SetProperties();
}

void CDMALTEPhone::Log(
    /* [in] */ const String& s)
{
    Logger::D(LOG_LTE_TAG, s);
}

void CDMALTEPhone::Loge(
    /* [in] */ const String& s)
{
    Logger::E(LOG_LTE_TAG, s);
}

void CDMALTEPhone::Loge(
   /* [in] */ const String& s,
   /* [in] */ IThrowable* e)
{
    Logger::E(LOG_LTE_TAG, s, e);
}

void CDMALTEPhone::OnSubscriptionActivated()
{
   // mSubscriptionData = SubscriptionManager.getCurrentSubscription(mSubscription);

    AutoPtr<Subscription> sc = (Subscription*)mSubscriptionData.Get();
    Log(String("SUBSCRIPTION ACTIVATED : slotId : ") + StringUtils::ToString(sc->mSlotId)
            + " appid : " + StringUtils::ToString(sc->m3gpp2Index)
            + " subId : " + StringUtils::ToString(sc->mSubId)
            + " subStatus : " + StringUtils::ToString(sc->mSubStatus));

    // Make sure properties are set for proper subscription.
    SetProperties();

    OnUpdateIccAvailability();
    AutoPtr<IMessage> msg;
    IHandler::Probe(mSST)->ObtainMessage(IDctConstants::EVENT_ICC_CHANGED, (IMessage**)&msg);
    Boolean b;
    IHandler::Probe(mSST)->SendMessage(msg, &b);
    ((CCdmaLteServiceStateTracker*)mSST.Get())->UpdateCdmaSubscription();
    IDcTracker::Probe(mDcTracker)->UpdateRecords();
}

void CDMALTEPhone::OnSubscriptionDeactivated()
{
    Log(String("SUBSCRIPTION DEACTIVATED"));
    // resetSubSpecifics
    mSubscriptionData = NULL;
}

void CDMALTEPhone::SetProperties()
{
    //Change the system property
    SetSystemProperty(ITelephonyProperties::CURRENT_ACTIVE_PHONE,
            StringUtils::ToString(IPhoneConstants::PHONE_TYPE_CDMA));
    // Sets operator alpha property by retrieving from build-time system property
    String operatorAlpha;
    SystemProperties::Get(String("ro.cdma.home.operator.alpha"), &operatorAlpha);
    if (!operatorAlpha.IsEmpty()) {
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
    }

    // Sets operator numeric property by retrieving from build-time system property
    String operatorNumeric;
    SystemProperties::Get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC, &operatorNumeric);
    Log(String("update icc_operator_numeric=") + operatorNumeric);
    if (!operatorNumeric.IsEmpty()) {
        SetSystemProperty(ITelephonyProperties::PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);

        Int64 sid;
        GetSubId(&sid);
        Int32 ret;
        SubscriptionController::GetInstance()->SetMccMnc(operatorNumeric, sid, &ret);
        // Sets iso country property by retrieving from build-time system property
        SetIsoCountryProperty(operatorNumeric);
        // Updates MCC MNC device configuration information
        Log(String("update mccmnc=") + operatorNumeric);
        MccTable::UpdateMccMncConfiguration(mContext, operatorNumeric, FALSE);
    }
    // Sets current entry in the telephony carrier table
    Boolean b;
    UpdateCurrentCarrierInProvider(&b);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
