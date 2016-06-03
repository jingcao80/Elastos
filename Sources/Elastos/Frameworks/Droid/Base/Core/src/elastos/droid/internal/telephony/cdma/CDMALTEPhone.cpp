#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

#include "elastos/droid/internal/telephony/cdma/CDMALTEPhone.h"

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
const Boolean CDMALTEPhone::DBG = true;

CDMALTEPhone::CDMALTEPhone(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Int32 phoneId)
{
    // ==================before translated======================
    // this(context, ci, notifier, false, phoneId);
}

CDMALTEPhone::CDMALTEPhone(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier,
    /* [in] */ Boolean unitTestMode,
    /* [in] */ Int32 phoneId)
{
    // ==================before translated======================
    // super(context, ci, notifier, phoneId);
    //
    // Rlog.d(LOG_TAG, "CDMALTEPhone: constructor: sub = " + mPhoneId);
    //
    // mDcTracker = new DcTracker(this);
}

CDMALTEPhone::CDMALTEPhone(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // super(context, ci, notifier, false);
}

ECode CDMALTEPhone::Dispose()
{
    // ==================before translated======================
    // synchronized(PhoneProxy.lockForRadioTechnologyChange) {
    //     if (mSimRecords != null) {
    //         mSimRecords.unregisterForRecordsLoaded(this);
    //     }
    //     super.dispose();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::RemoveReferences()
{
    // ==================before translated======================
    // super.removeReferences();
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // AsyncResult ar;
    // Message onComplete;
    //
    // // messages to be handled whether or not the phone is being destroyed
    // // should only include messages which are being re-directed and do not use
    // // resources of the phone being destroyed
    // switch (msg.what) {
    //     // handle the select network completion callbacks.
    //     case EVENT_SET_NETWORK_MANUAL_COMPLETE:
    //     case EVENT_SET_NETWORK_AUTOMATIC_COMPLETE:
    //         super.handleMessage(msg);
    //         return;
    // }
    //
    // if (!mIsTheCurrentActivePhone) {
    //     Rlog.e(LOG_TAG, "Received message " + msg +
    //             "[" + msg.what + "] while being destroyed. Ignoring.");
    //     return;
    // }
    // switch(msg.what) {
    //     case EVENT_SIM_RECORDS_LOADED:
    //         mSimRecordsLoadedRegistrants.notifyRegistrants();
    //         break;
    //
    //     case EVENT_SUBSCRIPTION_ACTIVATED:
    //         log("EVENT_SUBSCRIPTION_ACTIVATED");
    //         onSubscriptionActivated();
    //         break;
    //
    //     case EVENT_SUBSCRIPTION_DEACTIVATED:
    //         log("EVENT_SUBSCRIPTION_DEACTIVATED");
    //         onSubscriptionDeactivated();
    //         break;
    //
    //     default:
    //         super.handleMessage(msg);
    // }
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // PhoneConstants.DataState ret = PhoneConstants.DataState.DISCONNECTED;
    //
    // if (mSST == null) {
    //     // Radio Technology Change is ongoing, dispose() and
    //     // removeReferences() have already been called
    //
    //     ret = PhoneConstants.DataState.DISCONNECTED;
    // } else if (mSST.getCurrentDataConnectionState() != ServiceState.STATE_IN_SERVICE &&
    //                     mOosIsDisconnect) {
    //     ret = PhoneConstants.DataState.DISCONNECTED;
    //     log("getDataConnectionState: Data is Out of Service. ret = " + ret);
    // } else if (mDcTracker.isApnTypeEnabled(apnType) == false) {
    //     ret = PhoneConstants.DataState.DISCONNECTED;
    // } else {
    //     switch (mDcTracker.getState(apnType)) {
    //         case RETRYING:
    //         case FAILED:
    //         case IDLE:
    //             ret = PhoneConstants.DataState.DISCONNECTED;
    //             break;
    //
    //         case CONNECTED:
    //         case DISCONNECTING:
    //             if (mCT.mState != PhoneConstants.State.IDLE &&
    //                     !mSST.isConcurrentVoiceAndDataAllowed()) {
    //                 ret = PhoneConstants.DataState.SUSPENDED;
    //             } else {
    //                 ret = PhoneConstants.DataState.CONNECTED;
    //             }
    //             break;
    //
    //         case CONNECTING:
    //         case SCANNING:
    //             ret = PhoneConstants.DataState.CONNECTING;
    //             break;
    //     }
    // }
    //
    // log("getDataConnectionState apnType=" + apnType + " ret=" + ret);
    // return ret;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::UpdateCurrentCarrierInProvider(
    /* [in] */ const String& operatorNumeric,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // boolean retVal;
    // if (mUiccController.getUiccCardApplication(mPhoneId, UiccController.APP_FAM_3GPP) == null) {
    //     if (DBG) log("updateCurrentCarrierInProvider APP_FAM_3GPP == null");
    //     retVal = super.updateCurrentCarrierInProvider(operatorNumeric);
    // } else {
    //     if (DBG) log("updateCurrentCarrierInProvider not updated");
    //     retVal = true;
    // }
    // if (DBG) log("updateCurrentCarrierInProvider X retVal=" + retVal);
    // return retVal;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    // ==================before translated======================
    // long currentDds = SubscriptionManager.getDefaultDataSubId();
    // String operatorNumeric = getOperatorNumeric();
    //
    // Rlog.d(LOG_TAG, "updateCurrentCarrierInProvider: mSubscription = " + getSubId()
    //         + " currentDds = " + currentDds + " operatorNumeric = " + operatorNumeric);
    //
    // if (!TextUtils.isEmpty(operatorNumeric) && (getSubId() == currentDds)) {
    //     try {
    //         Uri uri = Uri.withAppendedPath(Telephony.Carriers.CONTENT_URI, "current");
    //         ContentValues map = new ContentValues();
    //         map.put(Telephony.Carriers.NUMERIC, operatorNumeric);
    //         mContext.getContentResolver().insert(uri, map);
    //         return true;
    //     } catch (SQLException e) {
    //         Rlog.e(LOG_TAG, "Can't store current operator", e);
    //     }
    // }
    // return false;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // IccRecords r = (mIccRecords != null) ? mIccRecords.get() : null;
    // if (r != null) {
    //     String imsi = r.getIMSI();
    //     if (!TextUtils.isEmpty(imsi)) {
    //         log("IMSI = " + imsi);
    //         return imsi;
    //     }
    // }
    //
    // log("IMSI undefined");
    // return "";
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // if (TelephonyManager.getLteOnCdmaModeStatic() == PhoneConstants.LTE_ON_CDMA_TRUE) {
    //     return mImei;
    // } else {
    //     return super.getDeviceId();
    // }
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return (mSimRecords != null) ? mSimRecords.getGid1() : "";
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mImei;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mImeiSv;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return mIsimUiccRecords;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    // ==================before translated======================
    // return (mSimRecords != null) ? mSimRecords.getMsisdnNumber() : null;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    VALIDATE_NOT_NULL(response);
    // ==================before translated======================
    // mCi.getAvailableNetworks(response);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& value)
{
    // ==================before translated======================
    // if(getUnitTestMode()) {
    //     return;
    // }
    // TelephonyManager.setTelephonyProperty(property, getSubId(), value);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ const String& defValue,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if(getUnitTestMode()) {
    //     return null;
    // }
    // return TelephonyManager.getTelephonyProperty(property, getSubId(), defValue);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::UpdateDataConnectionTracker()
{
    // ==================before translated======================
    // ((DcTracker)mDcTracker).update();
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg)
{
    VALIDATE_NOT_NULL(onCompleteMsg);
    // ==================before translated======================
    // ((DcTracker)mDcTracker)
    //         .setInternalDataEnabled(enable, onCompleteMsg);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return ((DcTracker)mDcTracker)
    //          .setInternalDataEnabledFlag(enable);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::GetOperatorNumeric(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String operatorNumeric = null;
    // IccRecords curIccRecords = null;
    // if (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_NV) {
    //     operatorNumeric = SystemProperties.get("ro.cdma.home.operator.numeric");
    // } else if (mCdmaSubscriptionSource == CDMA_SUBSCRIPTION_RUIM_SIM) {
    //     curIccRecords = mSimRecords;
    //     if (curIccRecords != null) {
    //         operatorNumeric = curIccRecords.getOperatorNumeric();
    //     } else {
    //         curIccRecords = mIccRecords.get();
    //         if (curIccRecords != null && (curIccRecords instanceof RuimRecords)) {
    //             RuimRecords csim = (RuimRecords) curIccRecords;
    //             operatorNumeric = csim.getOperatorNumeric();
    //         }
    //     }
    // }
    // if (operatorNumeric == null) {
    //     Rlog.e(LOG_TAG, "getOperatorNumeric: Cannot retrieve operatorNumeric:"
    //             + " mCdmaSubscriptionSource = " + mCdmaSubscriptionSource + " mIccRecords = "
    //             + ((curIccRecords != null) ? curIccRecords.getRecordsLoaded() : null));
    // }
    //
    // Rlog.d(LOG_TAG, "getOperatorNumeric: mCdmaSubscriptionSource = " + mCdmaSubscriptionSource
    //         + " operatorNumeric = " + operatorNumeric);
    //
    // return operatorNumeric;
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // ((DcTracker)mDcTracker)
    //        .registerForAllDataDisconnected(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // ((DcTracker)mDcTracker)
    //         .unregisterForAllDataDisconnected(h);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::RegisterForSimRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // mSimRecordsLoadedRegistrants.addUnique(h, what, obj);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::UnregisterForSimRecordsLoaded(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // mSimRecordsLoadedRegistrants.remove(h);
    assert(0);
    return NOERROR;
}

ECode CDMALTEPhone::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("CDMALTEPhone extends:");
    // super.dump(fd, pw, args);
    assert(0);
    return NOERROR;
}

void CDMALTEPhone::InitSstIcc()
{
    // ==================before translated======================
    // mSST = new CdmaLteServiceStateTracker(this);
    assert(0);
}

void CDMALTEPhone::OnUpdateIccAvailability()
{
    // ==================before translated======================
    // if (mSimRecords != null) {
    //     mSimRecords.unregisterForRecordsLoaded(this);
    // }
    //
    // if (mUiccController == null ) {
    //     return;
    // }
    //
    // // Update IsimRecords
    // UiccCardApplication newUiccApplication =
    //         mUiccController.getUiccCardApplication(mPhoneId, UiccController.APP_FAM_IMS);
    // IsimUiccRecords newIsimUiccRecords = null;
    //
    // if (newUiccApplication != null) {
    //     newIsimUiccRecords = (IsimUiccRecords) newUiccApplication.getIccRecords();
    // }
    // mIsimUiccRecords = newIsimUiccRecords;
    //
    // // Update UsimRecords
    // newUiccApplication = mUiccController.getUiccCardApplication(mPhoneId,
    //         UiccController.APP_FAM_3GPP);
    // SIMRecords newSimRecords = null;
    // if (newUiccApplication != null) {
    //     newSimRecords = (SIMRecords) newUiccApplication.getIccRecords();
    // }
    // mSimRecords = newSimRecords;
    // if (mSimRecords != null) {
    //     mSimRecords.registerForRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, null);
    // }
    //
    // super.onUpdateIccAvailability();
    assert(0);
}

void CDMALTEPhone::Init(
    /* [in] */ IContext* context,
    /* [in] */ IPhoneNotifier* notifier)
{
    // ==================before translated======================
    // mCi.setPhoneType(PhoneConstants.PHONE_TYPE_CDMA);
    // mCT = new CdmaCallTracker(this);
    // mCdmaSSM = CdmaSubscriptionSourceManager.getInstance(context, mCi, this,
    //         EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, null);
    // mRuimPhoneBookInterfaceManager = new RuimPhoneBookInterfaceManager(this);
    // mSubInfo = new PhoneSubInfo(this);
    // mEriManager = new EriManager(this, context, EriManager.ERI_FROM_XML);
    //
    // mCi.registerForAvailable(this, EVENT_RADIO_AVAILABLE, null);
    // mCi.registerForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_NOT_AVAILABLE, null);
    // mCi.registerForOn(this, EVENT_RADIO_ON, null);
    // mCi.setOnSuppServiceNotification(this, EVENT_SSN, null);
    // mSST.registerForNetworkAttached(this, EVENT_REGISTERED_TO_NETWORK, null);
    // mCi.setEmergencyCallbackMode(this, EVENT_EMERGENCY_CALLBACK_MODE_ENTER, null);
    // mCi.registerForExitEmergencyCallbackMode(this, EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE,
    //         null);
    //
    // PowerManager pm
    //     = (PowerManager) context.getSystemService(Context.POWER_SERVICE);
    // mWakeLock = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK,LOG_TAG);
    //
    // // This is needed to handle phone process crashes
    // String inEcm = SystemProperties.get(TelephonyProperties.PROPERTY_INECM_MODE, "false");
    // mIsPhoneInEcmState = inEcm.equals("true");
    // if (mIsPhoneInEcmState) {
    //     // Send a message which will invoke handleExitEmergencyCallbackMode
    //     mCi.exitEmergencyCallbackMode(obtainMessage(EVENT_EXIT_EMERGENCY_CALLBACK_RESPONSE));
    // }
    //
    // // get the string that specifies the carrier OTA Sp number
    // mCarrierOtaSpNumSchema = SystemProperties.get(
    //         TelephonyProperties.PROPERTY_OTASP_NUM_SCHEMA,"");
    //
    // setProperties();
    assert(0);
}

void CDMALTEPhone::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOG_LTE_TAG, s);
    assert(0);
}

void CDMALTEPhone::Loge(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.e(LOG_LTE_TAG, s);
    assert(0);
}

//void CDMALTEPhone::Loge(
//    /* [in] */ const String& s,
//    /* [in] */ Throwable* e)
//{
//    // ==================before translated======================
//    // Rlog.e(LOG_LTE_TAG, s, e);
//    assert(0);
//}

void CDMALTEPhone::OnSubscriptionActivated()
{
    // ==================before translated======================
    // //        mSubscriptionData = SubscriptionManager.getCurrentSubscription(mSubscription);
    //
    //         log("SUBSCRIPTION ACTIVATED : slotId : " + mSubscriptionData.slotId
    //                 + " appid : " + mSubscriptionData.m3gpp2Index
    //                 + " subId : " + mSubscriptionData.subId
    //                 + " subStatus : " + mSubscriptionData.subStatus);
    //
    //         // Make sure properties are set for proper subscription.
    //         setProperties();
    //
    //         onUpdateIccAvailability();
    //         mSST.sendMessage(mSST.obtainMessage(ServiceStateTracker.EVENT_ICC_CHANGED));
    //         ((CdmaLteServiceStateTracker)mSST).updateCdmaSubscription();
    //         ((DcTracker)mDcTracker).updateRecords();
    assert(0);
}

void CDMALTEPhone::OnSubscriptionDeactivated()
{
    // ==================before translated======================
    // log("SUBSCRIPTION DEACTIVATED");
    // // resetSubSpecifics
    // mSubscriptionData = null;
    assert(0);
}

void CDMALTEPhone::SetProperties()
{
    // ==================before translated======================
    // //Change the system property
    // setSystemProperty(TelephonyProperties.CURRENT_ACTIVE_PHONE,
    //         new Integer(PhoneConstants.PHONE_TYPE_CDMA).toString());
    // // Sets operator alpha property by retrieving from build-time system property
    // String operatorAlpha = SystemProperties.get("ro.cdma.home.operator.alpha");
    // if (!TextUtils.isEmpty(operatorAlpha)) {
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, operatorAlpha);
    // }
    //
    // // Sets operator numeric property by retrieving from build-time system property
    // String operatorNumeric = SystemProperties.get(PROPERTY_CDMA_HOME_OPERATOR_NUMERIC);
    // log("update icc_operator_numeric=" + operatorNumeric);
    // if (!TextUtils.isEmpty(operatorNumeric)) {
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, operatorNumeric);
    //
    //     SubscriptionController.getInstance().setMccMnc(operatorNumeric, getSubId());
    //     // Sets iso country property by retrieving from build-time system property
    //     setIsoCountryProperty(operatorNumeric);
    //     // Updates MCC MNC device configuration information
    //     log("update mccmnc=" + operatorNumeric);
    //     MccTable.updateMccMncConfiguration(mContext, operatorNumeric, false);
    // }
    // // Sets current entry in the telephony carrier table
    // updateCurrentCarrierInProvider();
    assert(0);
}

} // namespace Cdma
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
