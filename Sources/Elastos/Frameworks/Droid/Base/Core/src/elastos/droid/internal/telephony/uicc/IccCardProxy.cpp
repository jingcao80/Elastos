
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/internal/telephony/uicc/IccCardProxy.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//                             IccCardProxy
//=====================================================================
CAR_INTERFACE_IMPL(IccCardProxy, Handler, IIccCard);

const Boolean IccCardProxy::DBG = TRUE;
const String IccCardProxy::LOGTAG("IccCardProxy");
const Int32 IccCardProxy::EVENT_RADIO_OFF_OR_UNAVAILABLE;
const Int32 IccCardProxy::EVENT_RADIO_ON;
const Int32 IccCardProxy::EVENT_ICC_CHANGED;
const Int32 IccCardProxy::EVENT_ICC_ABSENT;
const Int32 IccCardProxy::EVENT_ICC_LOCKED;
const Int32 IccCardProxy::EVENT_APP_READY;
const Int32 IccCardProxy::EVENT_RECORDS_LOADED;
const Int32 IccCardProxy::EVENT_IMSI_READY;
const Int32 IccCardProxy::EVENT_PERSO_LOCKED;
const Int32 IccCardProxy::EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED;
const Int32 IccCardProxy::EVENT_ICC_RECORD_EVENTS;
const Int32 IccCardProxy::EVENT_SUBSCRIPTION_ACTIVATED;
const Int32 IccCardProxy::EVENT_SUBSCRIPTION_DEACTIVATED;
const Int32 IccCardProxy::EVENT_CARRIER_PRIVILIGES_LOADED;

IccCardProxy::IccCardProxy()
{
}

ECode IccCardProxy::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci)
{
    // ==================before translated======================
    // log("Creating");
    // mContext = context;
    // mCi = ci;
    // mCdmaSSM = CdmaSubscriptionSourceManager.getInstance(context,
    //         ci, this, EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED, null);
    // mUiccController = UiccController.getInstance();
    // mUiccController.registerForIccChanged(this, EVENT_ICC_CHANGED, null);
    // ci.registerForOn(this,EVENT_RADIO_ON, null);
    // ci.registerForOffOrNotAvailable(this, EVENT_RADIO_OFF_OR_UNAVAILABLE, null);
    // setExternalState(State.NOT_READY);
    return NOERROR;
}

ECode IccCardProxy::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ Int32 cardIndex)
{
    // ==================before translated======================
    // this(context, ci);
    //
    // mCardIndex = cardIndex;
    //
    // resetProperties();
    // setExternalState(State.NOT_READY, false);
    return NOERROR;
}

ECode IccCardProxy::Dispose()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     log("Disposing");
    //     //Cleanup icc references
    //     mUiccController.unregisterForIccChanged(this);
    //     mUiccController = null;
    //     mCi.unregisterForOn(this);
    //     mCi.unregisterForOffOrNotAvailable(this);
    //     mCdmaSSM.dispose(this);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SetVoiceRadioTech(
    /* [in] */ Int32 radioTech)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) {
    //         log("Setting radio tech " + ServiceState.rilRadioTechnologyToString(radioTech));
    //     }
    //     if (ServiceState.isGsm(radioTech)) {
    //         mCurrentAppType = UiccController.APP_FAM_3GPP;
    //     } else {
    //         mCurrentAppType = UiccController.APP_FAM_3GPP2;
    //     }
    //     updateQuietMode();
    //     updateActiveRecord();
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // switch (msg.what) {
    //     case EVENT_RADIO_OFF_OR_UNAVAILABLE:
    //         mRadioOn = false;
    //         if (CommandsInterface.RadioState.RADIO_UNAVAILABLE == mCi.getRadioState()) {
    //             setExternalState(State.NOT_READY);
    //         }
    //         break;
    //     case EVENT_RADIO_ON:
    //         mRadioOn = true;
    //         if (!mInitialized) {
    //             updateQuietMode();
    //         }
    //         // When the radio is off, if EVENT_ICC_CHANGED is received, the
    //         // mExternalState will be updated. After the radio turns on, if
    //         // EVENT_ICC_CHANGED is not received, the mExternalState will not be
    //         // be restored. Therefore, updateExternalState when the radio turns on.
    //         updateExternalState();
    //         break;
    //     case EVENT_ICC_CHANGED:
    //         mIsCardStatusAvailable = true;
    //         if (mInitialized) {
    //             updateIccAvailability();
    //         }
    //         break;
    //     case EVENT_ICC_ABSENT:
    //         mAbsentRegistrants.notifyRegistrants();
    //         setExternalState(State.ABSENT);
    //         break;
    //     case EVENT_ICC_LOCKED:
    //         processLockedState();
    //         break;
    //     case EVENT_APP_READY:
    //         setExternalState(State.READY);
    //         break;
    //     case EVENT_RECORDS_LOADED:
    //         if (mIccRecords != null) {
    //             String operator = mIccRecords.getOperatorNumeric();
    //             int slotId = mCardIndex;
    //
    //             log("operator = " + operator + " slotId = " + slotId);
    //
    //             if (operator != null) {
    //                 log("update icc_operator_numeric=" + operator);
    //                 setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, slotId, operator);
    //                 if (mCurrentAppType == UiccController.APP_FAM_3GPP) {
    //                     setSystemProperty(PROPERTY_APN_SIM_OPERATOR_NUMERIC,
    //                              slotId, operator);
    //                     log("update sim_operator_numeric=" + operator);
    //                 } else if (mCurrentAppType == UiccController.APP_FAM_3GPP2) {
    //                     setSystemProperty(PROPERTY_APN_RUIM_OPERATOR_NUMERIC,
    //                              slotId, operator);
    //                     log("update ruim_operator_numeric=" + operator);
    //                 }
    //                 String countryCode = operator.substring(0,3);
    //                 if (countryCode != null) {
    //                     setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, slotId,
    //                             MccTable.countryCodeForMcc(Integer.parseInt(countryCode)));
    //                 } else {
    //                     loge("EVENT_RECORDS_LOADED Country code is null");
    //                 }
    //
    //                 long[] subId = SubscriptionController.getInstance().getSubId(slotId);
    //                 // Update MCC MNC device configuration information only for default sub.
    //                 if (subId[0] == SubscriptionController.getInstance().getDefaultSubId()) {
    //                     log("update mccmnc=" + operator + " config for default subscription.");
    //                     MccTable.updateMccMncConfiguration(mContext, operator, false);
    //                 }
    //                 SubscriptionController.getInstance().setMccMnc(operator, subId[0]);
    //             } else {
    //                 loge("EVENT_RECORDS_LOADED Operator name is null");
    //             }
    //         }
    //         if (mUiccCard != null && !mUiccCard.areCarrierPriviligeRulesLoaded()) {
    //             mUiccCard.registerForCarrierPrivilegeRulesLoaded(
    //                 this, EVENT_CARRIER_PRIVILIGES_LOADED, null);
    //         } else {
    //             onRecordsLoaded();
    //         }
    //         break;
    //     case EVENT_IMSI_READY:
    //         broadcastIccStateChangedIntent(IccCardConstants.INTENT_VALUE_ICC_IMSI, null);
    //         break;
    //     case EVENT_PERSO_LOCKED:
    //         mPersoSubState = mUiccApplication.getPersoSubState();
    //         mPersoLockedRegistrants.notifyRegistrants((AsyncResult)msg.obj);
    //         setExternalState(State.PERSO_LOCKED);
    //         break;
    //     case EVENT_CDMA_SUBSCRIPTION_SOURCE_CHANGED:
    //         updateQuietMode();
    //         updateActiveRecord();
    //         break;
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
    //     case EVENT_ICC_RECORD_EVENTS:
    //         if ((mCurrentAppType == UiccController.APP_FAM_3GPP) && (mIccRecords != null)) {
    //             int slotId = mCardIndex;
    //             AsyncResult ar = (AsyncResult)msg.obj;
    //             int eventCode = (Integer) ar.result;
    //             if (eventCode == SIMRecords.EVENT_SPN) {
    //                 setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, slotId,
    //                         mIccRecords.getServiceProviderName());
    //             }
    //         }
    //         break;
    //
    //     case EVENT_CARRIER_PRIVILIGES_LOADED:
    //         log("EVENT_CARRIER_PRIVILEGES_LOADED");
    //         if (mUiccCard != null) {
    //             mUiccCard.unregisterForCarrierPrivilegeRulesLoaded(this);
    //         }
    //         onRecordsLoaded();
    //         break;
    //
    //     default:
    //         loge("Unhandled message with number: " + msg.what);
    //         break;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::ResetProperties()
{
    // ==================before translated======================
    // if (mCurrentAppType == UiccController.APP_FAM_3GPP) {
    //     log("update icc_operator_numeric=" + "");
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_NUMERIC, mCardIndex, "");
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ISO_COUNTRY, mCardIndex, "");
    //     setSystemProperty(PROPERTY_ICC_OPERATOR_ALPHA, mCardIndex, "");
    //  }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mIccRecords != null) {
    //         return mIccRecords.getRecordsLoaded();
    //     }
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetState(
    /* [out] */ IccCardConstantsState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mExternalState;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccRecords(
    /* [out] */ IIccRecords** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mIccRecords;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         return mUiccApplication.getIccFileHandler();
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::RegisterForAbsent(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //
    //     mAbsentRegistrants.add(r);
    //
    //     if (getState() == State.ABSENT) {
    //         r.notifyRegistrant();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::UnregisterForAbsent(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mAbsentRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::RegisterForPersoLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //
    //     mPersoLockedRegistrants.add(r);
    //
    //     if (getState() == State.PERSO_LOCKED) {
    //         r.notifyRegistrant(new AsyncResult(null, mPersoSubState.ordinal(), null));
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::UnregisterForPersoLocked(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mPersoLockedRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::RegisterForLocked(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //
    //     mPinLockedRegistrants.add(r);
    //
    //     if (getState().isPinLocked()) {
    //         r.notifyRegistrant();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::UnregisterForLocked(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mPinLockedRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SupplyPin(
    /* [in] */ const String& pin,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.supplyPin(pin, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SupplyPuk(
    /* [in] */ const String& puk,
    /* [in] */ const String& newPin,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.supplyPuk(puk, newPin, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SupplyPin2(
    /* [in] */ const String& pin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.supplyPin2(pin2, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SupplyPuk2(
    /* [in] */ const String& puk2,
    /* [in] */ const String& newPin2,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.supplyPuk2(puk2, newPin2, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SupplyDepersonalization(
    /* [in] */ const String& pin,
    /* [in] */ const String& type,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.supplyDepersonalization(pin, type, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("CommandsInterface is not set.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccLockEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     /* defaults to false, if ICC is absent/deactivated */
    //     Boolean retValue = mUiccApplication != null ?
    //             mUiccApplication.getIccLockEnabled() : false;
    //     return retValue;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccFdnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     Boolean retValue = mUiccApplication != null ?
    //             mUiccApplication.getIccFdnEnabled() : false;
    //     return retValue;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccFdnAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // boolean retValue = mUiccApplication != null ? mUiccApplication.getIccFdnAvailable() : false;
    // return retValue;
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccPin2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /* defaults to disabled */
    // Boolean retValue = mUiccApplication != null ? mUiccApplication.getIccPin2Blocked() : false;
    // return retValue;
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetIccPuk2Blocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // /* defaults to disabled */
    // Boolean retValue = mUiccApplication != null ? mUiccApplication.getIccPuk2Blocked() : false;
    // return retValue;
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SetIccLockEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.setIccLockEnabled(enabled, password, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::SetIccFdnEnabled(
    /* [in] */ Boolean enabled,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.setIccFdnEnabled(enabled, password, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::ChangeIccLockPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.changeIccLockPassword(oldPassword, newPassword, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::ChangeIccFdnPassword(
    /* [in] */ const String& oldPassword,
    /* [in] */ const String& newPassword,
    /* [in] */ IMessage* onComplete)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication != null) {
    //         mUiccApplication.changeIccFdnPassword(oldPassword, newPassword, onComplete);
    //     } else if (onComplete != null) {
    //         Exception e = new RuntimeException("ICC card is absent.");
    //         AsyncResult.forMessage(onComplete).exception = e;
    //         onComplete.sendToTarget();
    //         return;
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::GetServiceProviderName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mIccRecords != null) {
    //         return mIccRecords.getServiceProviderName();
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::IsApplicationOnIcc(
    /* [in] */  AppType type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     Boolean retValue = mUiccCard != null ? mUiccCard.isApplicationOnIcc(type) : false;
    //     return retValue;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::HasIccCard(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccCard != null && mUiccCard.getCardState() != CardState.CARDSTATE_ABSENT) {
    //         return true;
    //     }
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode IccCardProxy::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("IccCardProxy: " + this);
    // pw.println(" mContext=" + mContext);
    // pw.println(" mCi=" + mCi);
    // pw.println(" mAbsentRegistrants: size=" + mAbsentRegistrants.size());
    // for (int i = 0; i < mAbsentRegistrants.size(); i++) {
    //     pw.println("  mAbsentRegistrants[" + i + "]="
    //             + ((Registrant)mAbsentRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mPinLockedRegistrants: size=" + mPinLockedRegistrants.size());
    // for (int i = 0; i < mPinLockedRegistrants.size(); i++) {
    //     pw.println("  mPinLockedRegistrants[" + i + "]="
    //             + ((Registrant)mPinLockedRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mPersoLockedRegistrants: size=" + mPersoLockedRegistrants.size());
    // for (int i = 0; i < mPersoLockedRegistrants.size(); i++) {
    //     pw.println("  mPersoLockedRegistrants[" + i + "]="
    //             + ((Registrant)mPersoLockedRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mCurrentAppType=" + mCurrentAppType);
    // pw.println(" mUiccController=" + mUiccController);
    // pw.println(" mUiccCard=" + mUiccCard);
    // pw.println(" mUiccApplication=" + mUiccApplication);
    // pw.println(" mIccRecords=" + mIccRecords);
    // pw.println(" mCdmaSSM=" + mCdmaSSM);
    // pw.println(" mRadioOn=" + mRadioOn);
    // pw.println(" mQuietMode=" + mQuietMode);
    // pw.println(" mInitialized=" + mInitialized);
    // pw.println(" mExternalState=" + mExternalState);
    //
    // pw.flush();
    assert(0);
    return NOERROR;
}

void IccCardProxy::UpdateActiveRecord()
{
    // ==================before translated======================
    // log("updateActiveRecord app type = " + mCurrentAppType +
    //         "mIccRecords = " + mIccRecords);
    //
    // if (mIccRecords == null) {
    //     return;
    // }
    //
    // if (mCurrentAppType == UiccController.APP_FAM_3GPP2) {
    //     int newSubscriptionSource = mCdmaSSM.getCdmaSubscriptionSource();
    //     // Allow RUIM to fetch in CDMA LTE mode if NV LTE mode.
    //     // Fixes cases where iccid could be unknown on some CDMA NV devices.
    //     if (newSubscriptionSource == CdmaSubscriptionSourceManager.SUBSCRIPTION_FROM_RUIM
    //             || PhoneFactory.getDefaultPhone().getLteOnCdmaMode()
    //                == PhoneConstants.LTE_ON_CDMA_TRUE) {
    //         // Set this as the Active record.
    //         log("Setting Ruim Record as active");
    //         mIccRecords.recordsRequired();
    //     }
    // } else if (mCurrentAppType == UiccController.APP_FAM_3GPP) {
    //     log("Setting SIM Record as active");
    //     mIccRecords.recordsRequired();
    // }
    assert(0);
}

void IccCardProxy::UpdateQuietMode()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     boolean oldQuietMode = mQuietMode;
    //     boolean newQuietMode;
    //     // "config_lte_capable" is set to true when the device is
    //     // LTE capable
    //     boolean isLteCapable = mContext.getResources().getBoolean(
    //             com.android.internal.R.bool.config_lte_capable);
    //     int cdmaSource = Phone.CDMA_SUBSCRIPTION_UNKNOWN;
    //     if (mCurrentAppType == UiccController.APP_FAM_3GPP) {
    //         newQuietMode = false;
    //         if (DBG) log("updateQuietMode: 3GPP subscription -> newQuietMode=" + newQuietMode);
    //     } else {
    //         cdmaSource = mCdmaSSM != null ?
    //                 mCdmaSSM.getCdmaSubscriptionSource() : Phone.CDMA_SUBSCRIPTION_UNKNOWN;
    //
    //         if (isLteCapable) {
    //             // For a LTE capable device, always be out of Quiet Mode
    //             newQuietMode = false;
    //         } else {
    //             newQuietMode = (cdmaSource == Phone.CDMA_SUBSCRIPTION_NV)
    //                     && (mCurrentAppType == UiccController.APP_FAM_3GPP2);
    //         }
    //         if (DBG) log("updateQuietMode: 3GPP2 subscription -> newQuietMode=" + newQuietMode);
    //     }
    //
    //     if (mQuietMode == false && newQuietMode == true) {
    //         // Last thing to do before switching to quiet mode is
    //         // broadcast ICC_READY
    //         log("Switching to QuietMode.");
    //         setExternalState(State.READY);
    //         mQuietMode = newQuietMode;
    //     } else if (mQuietMode == true && newQuietMode == false) {
    //         if (DBG) {
    //             log("updateQuietMode: Switching out from QuietMode."
    //                     + " Force broadcast of current state=" + mExternalState);
    //         }
    //         mQuietMode = newQuietMode;
    //         setExternalState(mExternalState, true);
    //     }
    //     if (DBG) {
    //         log("updateQuietMode: QuietMode is " + mQuietMode + " (app_type="
    //             + mCurrentAppType + " cdmaSource=" + cdmaSource + ")");
    //     }
    //     mInitialized = true;
    //     //Send EVENT_ICC_CHANGED only if it is already received atleast once from RIL.
    //     if (mIsCardStatusAvailable) sendMessage(obtainMessage(EVENT_ICC_CHANGED));
    // }
    assert(0);
}

void IccCardProxy::OnSubscriptionActivated()
{
    // ==================before translated======================
    // //mSubscriptionData = SubscriptionManager.getCurrentSubscription(mCardIndex);
    //
    // updateIccAvailability();
    // updateStateProperty();
    assert(0);
}

void IccCardProxy::OnSubscriptionDeactivated()
{
    // ==================before translated======================
    // resetProperties();
    // mSubscriptionData = null;
    // updateIccAvailability();
    // updateStateProperty();
    assert(0);
}

void IccCardProxy::OnRecordsLoaded()
{
    // ==================before translated======================
    // broadcastIccStateChangedIntent(IccCardConstants.INTENT_VALUE_ICC_LOADED, null);
    assert(0);
}

void IccCardProxy::UpdateIccAvailability()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     UiccCard newCard = mUiccController.getUiccCard(mCardIndex);
    //     CardState state = CardState.CARDSTATE_ABSENT;
    //     UiccCardApplication newApp = null;
    //     IccRecords newRecords = null;
    //     if (newCard != null) {
    //         state = newCard.getCardState();
    //         newApp = newCard.getApplication(mCurrentAppType);
    //         if (newApp != null) {
    //             newRecords = newApp.getIccRecords();
    //         }
    //     }
    //
    //     if (mIccRecords != newRecords || mUiccApplication != newApp || mUiccCard != newCard) {
    //         if (DBG) log("Icc changed. Reregestering.");
    //         unregisterUiccCardEvents();
    //         mUiccCard = newCard;
    //         mUiccApplication = newApp;
    //         mIccRecords = newRecords;
    //         registerUiccCardEvents();
    //         updateActiveRecord();
    //     }
    //     updateExternalState();
    // }
    assert(0);
}

void IccCardProxy::UpdateExternalState()
{
    // ==================before translated======================
    //
    // // mUiccCard could be null at bootup, before valid card states have
    // // been received from UiccController.
    // if (mUiccCard == null) {
    //     setExternalState(State.NOT_READY);
    //     return;
    // }
    //
    // if (mUiccCard.getCardState() == CardState.CARDSTATE_ABSENT) {
    //     if (mRadioOn) {
    //         setExternalState(State.ABSENT);
    //     } else {
    //         setExternalState(State.NOT_READY);
    //     }
    //     return;
    // }
    //
    // if (mUiccCard.getCardState() == CardState.CARDSTATE_ERROR) {
    //     setExternalState(State.CARD_IO_ERROR);
    //     return;
    // }
    //
    // if (mUiccApplication == null) {
    //     setExternalState(State.NOT_READY);
    //     return;
    // }
    //
    // switch (mUiccApplication.getState()) {
    //     case APPSTATE_UNKNOWN:
    //         setExternalState(State.UNKNOWN);
    //         break;
    //     case APPSTATE_PIN:
    //         setExternalState(State.PIN_REQUIRED);
    //         break;
    //     case APPSTATE_PUK:
    //         setExternalState(State.PUK_REQUIRED);
    //         break;
    //     case APPSTATE_SUBSCRIPTION_PERSO:
    //         if (mUiccApplication.isPersoLocked()) {
    //             mPersoSubState = mUiccApplication.getPersoSubState();
    //             setExternalState(State.PERSO_LOCKED);
    //         } else {
    //             setExternalState(State.UNKNOWN);
    //         }
    //         break;
    //     case APPSTATE_READY:
    //         setExternalState(State.READY);
    //         break;
    // }
    assert(0);
}

void IccCardProxy::RegisterUiccCardEvents()
{
    // ==================before translated======================
    // if (mUiccCard != null) {
    //     mUiccCard.registerForAbsent(this, EVENT_ICC_ABSENT, null);
    // }
    // if (mUiccApplication != null) {
    //     mUiccApplication.registerForReady(this, EVENT_APP_READY, null);
    //     mUiccApplication.registerForLocked(this, EVENT_ICC_LOCKED, null);
    //     mUiccApplication.registerForPersoLocked(this, EVENT_PERSO_LOCKED, null);
    // }
    // if (mIccRecords != null) {
    //     mIccRecords.registerForImsiReady(this, EVENT_IMSI_READY, null);
    //     mIccRecords.registerForRecordsLoaded(this, EVENT_RECORDS_LOADED, null);
    //     mIccRecords.registerForRecordsEvents(this, EVENT_ICC_RECORD_EVENTS, null);
    // }
    assert(0);
}

void IccCardProxy::UnregisterUiccCardEvents()
{
    // ==================before translated======================
    // if (mUiccCard != null) mUiccCard.unregisterForAbsent(this);
    // if (mUiccApplication != null) mUiccApplication.unregisterForReady(this);
    // if (mUiccApplication != null) mUiccApplication.unregisterForLocked(this);
    // if (mUiccApplication != null) mUiccApplication.unregisterForPersoLocked(this);
    // if (mIccRecords != null) mIccRecords.unregisterForImsiReady(this);
    // if (mIccRecords != null) mIccRecords.unregisterForRecordsLoaded(this);
    // if (mIccRecords != null) mIccRecords.unregisterForRecordsEvents(this);
    assert(0);
}

void IccCardProxy::UpdateStateProperty()
{
    // ==================before translated======================
    // setSystemProperty(PROPERTY_SIM_STATE, mCardIndex,getState().toString());
    assert(0);
}

void IccCardProxy::BroadcastIccStateChangedIntent(
    /* [in] */ const String& value,
    /* [in] */ const String& reason)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mCardIndex == null) {
    //         loge("broadcastIccStateChangedIntent: Card Index is not set; Return!!");
    //         return;
    //     }
    //
    //     if (mQuietMode) {
    //         log("QuietMode: NOT Broadcasting intent ACTION_SIM_STATE_CHANGED " +  value
    //                 + " reason " + reason);
    //         return;
    //     }
    //
    //     Intent intent = new Intent(TelephonyIntents.ACTION_SIM_STATE_CHANGED);
    //     intent.addFlags(Intent.FLAG_RECEIVER_REPLACE_PENDING);
    //     intent.putExtra(PhoneConstants.PHONE_NAME_KEY, "Phone");
    //     intent.putExtra(IccCardConstants.INTENT_KEY_ICC_STATE, value);
    //     intent.putExtra(IccCardConstants.INTENT_KEY_LOCKED_REASON, reason);
    //     SubscriptionManager.putPhoneIdAndSubIdExtra(intent, mCardIndex);
    //     log("Broadcasting intent ACTION_SIM_STATE_CHANGED " +  value
    //         + " reason " + reason + " for mCardIndex : " + mCardIndex);
    //     ActivityManagerNative.broadcastStickyIntent(intent, READ_PHONE_STATE,
    //             UserHandle.USER_ALL);
    // }
    assert(0);
}

void IccCardProxy::SetExternalState(
    /* [in] */ IccCardConstantsState newState,
    /* [in] */ Boolean _override)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mCardIndex == null) {
    //         loge("setExternalState: Card Index is not set; Return!!");
    //         return;
    //     }
    //
    //     if (!_override && newState == mExternalState) {
    //         return;
    //     }
    //     mExternalState = newState;
    //     setSystemProperty(PROPERTY_SIM_STATE, mCardIndex, getState().toString());
    //     broadcastIccStateChangedIntent(getIccStateIntentString(mExternalState),
    //             getIccStateReason(mExternalState));
    //     // TODO: Need to notify registrants for other states as well.
    //     if ( State.ABSENT == mExternalState) {
    //         mAbsentRegistrants.notifyRegistrants();
    //     }
    // }
    assert(0);
}

void IccCardProxy::ProcessLockedState()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mUiccApplication == null) {
    //         //Don't need to do anything if non-existent application is locked
    //         return;
    //     }
    //     PinState pin1State = mUiccApplication.getPin1State();
    //     if (pin1State == PinState.PINSTATE_ENABLED_PERM_BLOCKED) {
    //         setExternalState(State.PERM_DISABLED);
    //         return;
    //     }
    //
    //     AppState appState = mUiccApplication.getState();
    //     switch (appState) {
    //         case APPSTATE_PIN:
    //             mPinLockedRegistrants.notifyRegistrants();
    //             setExternalState(State.PIN_REQUIRED);
    //             break;
    //         case APPSTATE_PUK:
    //             setExternalState(State.PUK_REQUIRED);
    //             break;
    //         case APPSTATE_DETECTED:
    //         case APPSTATE_READY:
    //         case APPSTATE_SUBSCRIPTION_PERSO:
    //         case APPSTATE_UNKNOWN:
    //             // Neither required
    //             break;
    //     }
    // }
    assert(0);
}

void IccCardProxy::SetExternalState(
    /* [in] */ IccCardConstantsState newState)
{
    // ==================before translated======================
    // setExternalState(newState, false);
    assert(0);
}

String IccCardProxy::GetIccStateIntentString(
    /* [in] */ IccCardConstantsState state)
{
    // ==================before translated======================
    // switch (state) {
    //     case ABSENT: return IccCardConstants.INTENT_VALUE_ICC_ABSENT;
    //     case PIN_REQUIRED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
    //     case PUK_REQUIRED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
    //     case PERSO_LOCKED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
    //     case READY: return IccCardConstants.INTENT_VALUE_ICC_READY;
    //     case NOT_READY: return IccCardConstants.INTENT_VALUE_ICC_NOT_READY;
    //     case PERM_DISABLED: return IccCardConstants.INTENT_VALUE_ICC_LOCKED;
    //     case CARD_IO_ERROR: return IccCardConstants.INTENT_VALUE_ICC_CARD_IO_ERROR;
    //     default: return IccCardConstants.INTENT_VALUE_ICC_UNKNOWN;
    // }
    assert(0);
    return String("");
}

String IccCardProxy::GetIccStateReason(
    /* [in] */ IccCardConstantsState state)
{
    // ==================before translated======================
    //  switch (state) {
    //      case PIN_REQUIRED: return IccCardConstants.INTENT_VALUE_LOCKED_ON_PIN;
    //      case PUK_REQUIRED: return IccCardConstants.INTENT_VALUE_LOCKED_ON_PUK;
    //      case PERSO_LOCKED: return IccCardConstants.INTENT_VALUE_LOCKED_PERSO;
    //      case PERM_DISABLED: return IccCardConstants.INTENT_VALUE_ABSENT_ON_PERM_DISABLED;
    //      case CARD_IO_ERROR: return IccCardConstants.INTENT_VALUE_ICC_CARD_IO_ERROR;
    //      default: return null;
    // }
    assert(0);
    return String("");
}

void IccCardProxy::SetSystemProperty(
    /* [in] */ const String& property,
    /* [in] */ Int32 slotId,
    /* [in] */ const String& value)
{
    // ==================before translated======================
    // long[] subId = SubscriptionController.getInstance().getSubId(slotId);
    // TelephonyManager.setTelephonyProperty(property, subId[0], value);
    assert(0);
}

void IccCardProxy::Log(
    /* [in] */ const String& s)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, s);
    assert(0);
}

void IccCardProxy::Loge(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.e(LOGTAG, msg);
    assert(0);
}

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
