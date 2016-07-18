
#include "elastos/droid/internal/telephony/uicc/UiccCard.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

//=====================================================================
//            UiccCard::InnerDialogInterfaceOnClickListener1
//=====================================================================
CAR_INTERFACE_IMPL(UiccCard::InnerDialogInterfaceOnClickListener1, Object, IDialogInterfaceOnClickListener);

UiccCard::InnerDialogInterfaceOnClickListener1::InnerDialogInterfaceOnClickListener1(
    /* [in] */ UiccCard* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode UiccCard::InnerDialogInterfaceOnClickListener1::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (which == DialogInterface.BUTTON_POSITIVE) {
    //         if (DBG) log("Reboot due to SIM swap");
    //         PowerManager pm = (PowerManager) mContext
    //                 .getSystemService(Context.POWER_SERVICE);
    //         pm.reboot("SIM is added.");
    //     }
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                        UiccCard::InnerHandler
//=====================================================================
UiccCard::InnerHandler::InnerHandler(
    /* [in] */ UiccCard* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode UiccCard::InnerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (mDestroyed) {
    //     loge("Received message " + msg + "[" + msg.what
    //             + "] while being destroyed. Ignoring.");
    //     return;
    // }
    //
    // switch (msg.what) {
    //     case EVENT_CARD_REMOVED:
    //         onIccSwap(false);
    //         break;
    //     case EVENT_CARD_ADDED:
    //         onIccSwap(true);
    //         break;
    //     case EVENT_OPEN_LOGICAL_CHANNEL_DONE:
    //     case EVENT_CLOSE_LOGICAL_CHANNEL_DONE:
    //     case EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE:
    //     case EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE:
    //     case EVENT_SIM_IO_DONE:
    //     case EVENT_SIM_GET_ATR_DONE:
    //         AsyncResult ar = (AsyncResult)msg.obj;
    //         if (ar.exception != null) {
    //            if (DBG)
    //              log("Error in SIM access with exception" + ar.exception);
    //         }
    //         AsyncResult.forMessage((Message)ar.userObj, ar.result, ar.exception);
    //         ((Message)ar.userObj).sendToTarget();
    //         break;
    //     case EVENT_CARRIER_PRIVILIGES_LOADED:
    //         onCarrierPriviligesLoadedMessage();
    //         break;
    //     default:
    //         loge("Unknown Event " + msg.what);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                               UiccCard
//=====================================================================
CAR_INTERFACE_IMPL(UiccCard, Object, IUiccCard);

const String UiccCard::LOGTAG("UiccCard");
const Boolean UiccCard::DBG = TRUE;
const String UiccCard::OPERATOR_BRAND_OVERRIDE_PREFIX("operator_branding_");
const Int32 UiccCard::EVENT_CARD_REMOVED;
const Int32 UiccCard::EVENT_CARD_ADDED;
const Int32 UiccCard::EVENT_OPEN_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_CLOSE_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE;
const Int32 UiccCard::EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE;
const Int32 UiccCard::EVENT_SIM_IO_DONE;
const Int32 UiccCard::EVENT_CARRIER_PRIVILIGES_LOADED;
const Int32 UiccCard::EVENT_SIM_GET_ATR_DONE;

UiccCard::UiccCard()
{
    mUiccApplications = ArrayOf<IUiccCardApplication*>::Alloc(IIccCardStatus::CARD_MAX_APPS);
}

ECode UiccCard::constructor(
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IIccCardStatus* ics,
    /* [in] */ Int32 slotId)
{
    // ==================before translated======================
    // mCardState = ics.mCardState;
    // mSlotId = slotId;
    // update(c, ci, ics);
    return NOERROR;
}

ECode UiccCard::Dispose()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (DBG) log("Disposing card");
    //     if (mCatService != null) CatServiceFactory.disposeCatService(mSlotId);
    //     for (UiccCardApplication app : mUiccApplications) {
    //         if (app != null) {
    //             app.dispose();
    //         }
    //     }
    //     mCatService = null;
    //     mUiccApplications = null;
    //     mCarrierPrivilegeRules = null;
    //     mUICCConfig = null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::Update(
    /* [in] */ IContext* c,
    /* [in] */ ICommandsInterface* ci,
    /* [in] */ IIccCardStatus* ics)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (mDestroyed) {
    //         loge("Updated after destroyed! Fix me!");
    //         return;
    //     }
    //     CardState oldState = mCardState;
    //     mCardState = ics.mCardState;
    //     mUniversalPinState = ics.mUniversalPinState;
    //     mGsmUmtsSubscriptionAppIndex = ics.mGsmUmtsSubscriptionAppIndex;
    //     mCdmaSubscriptionAppIndex = ics.mCdmaSubscriptionAppIndex;
    //     mImsSubscriptionAppIndex = ics.mImsSubscriptionAppIndex;
    //     mContext = c;
    //     mCi = ci;
    //     //update applications
    //     if (mUICCConfig == null)
    //         mUICCConfig = new UICCConfig();
    //     if (DBG) log(ics.mApplications.length + " applications");
    //     for ( int i = 0; i < mUiccApplications.length; i++) {
    //         if (mUiccApplications[i] == null) {
    //             //Create newly added Applications
    //             if (i < ics.mApplications.length) {
    //                 mUiccApplications[i] = new UiccCardApplication(this,
    //                         ics.mApplications[i], mContext, mCi);
    //             }
    //         } else if (i >= ics.mApplications.length) {
    //             //Delete removed applications
    //             mUiccApplications[i].dispose();
    //             mUiccApplications[i] = null;
    //         } else {
    //             //Update the rest
    //             mUiccApplications[i].update(ics.mApplications[i], mContext, mCi);
    //         }
    //     }
    //
    //     createAndUpdateCatService();
    //
    //     // Reload the carrier privilege rules if necessary.
    //     log("Before privilege rules: " + mCarrierPrivilegeRules + " : " + mCardState);
    //     if (mCarrierPrivilegeRules == null && mCardState == CardState.CARDSTATE_PRESENT) {
    //         mCarrierPrivilegeRules = new UiccCarrierPrivilegeRules(this,
    //                 mHandler.obtainMessage(EVENT_CARRIER_PRIVILIGES_LOADED));
    //     } else if (mCarrierPrivilegeRules != null && mCardState != CardState.CARDSTATE_PRESENT) {
    //         mCarrierPrivilegeRules = null;
    //     }
    //
    //     sanitizeApplicationIndexes();
    //
    //     RadioState radioState = mCi.getRadioState();
    //     if (DBG) log("update: radioState=" + radioState + " mLastRadioState="
    //             + mLastRadioState);
    //     // No notifications while radio is off or we just powering up
    //     if (radioState == RadioState.RADIO_ON && mLastRadioState == RadioState.RADIO_ON) {
    //         if (oldState != CardState.CARDSTATE_ABSENT &&
    //                 mCardState == CardState.CARDSTATE_ABSENT) {
    //             if (DBG) log("update: notify card removed");
    //             mAbsentRegistrants.notifyRegistrants();
    //             mHandler.sendMessage(mHandler.obtainMessage(EVENT_CARD_REMOVED, null));
    //         } else if (oldState == CardState.CARDSTATE_ABSENT &&
    //                 mCardState != CardState.CARDSTATE_ABSENT) {
    //             if (DBG) log("update: notify card added");
    //             mHandler.sendMessage(mHandler.obtainMessage(EVENT_CARD_ADDED, null));
    //         }
    //     }
    //     mLastRadioState = radioState;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCatService(
    /* [out] */ ICatService** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCatService;
    assert(0);
    return NOERROR;
}

ECode UiccCard::RegisterForAbsent(
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
    //     if (mCardState == CardState.CARDSTATE_ABSENT) {
    //         r.notifyRegistrant();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::UnregisterForAbsent(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mAbsentRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::RegisterForCarrierPrivilegeRulesLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     Registrant r = new Registrant (h, what, obj);
    //
    //     mCarrierPrivilegeRegistrants.add(r);
    //
    //     if (areCarrierPriviligeRulesLoaded()) {
    //         r.notifyRegistrant();
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::UnregisterForCarrierPrivilegeRulesLoaded(
    /* [in] */ IHandler* h)
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mCarrierPrivilegeRegistrants.remove(h);
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::IsApplicationOnIcc(
    /* [in] */ AppType type,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     for (int i = 0 ; i < mUiccApplications.length; i++) {
    //         if (mUiccApplications[i] != null && mUiccApplications[i].getType() == type) {
    //             return true;
    //         }
    //     }
    //     return false;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCardState(
    /* [out] */ CardState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mCardState;
    // }
    Logger::E("leliang", "TODO not implemented line:%d, func:%s\n", __LINE__, __func__);
    *result = Uicc::CARDSTATE_PRESENT;
    return NOERROR;
}

ECode UiccCard::GetUniversalPinState(
    /* [out] */ PinState* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mUniversalPinState;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetSlotId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     return mSlotId;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetApplication(
    /* [in] */ Int32 family,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    Int32 index = IIccCardStatus::CARD_MAX_APPS;
    switch (family) {
        case IUiccController::APP_FAM_3GPP:
            index = mGsmUmtsSubscriptionAppIndex;
            break;
        case IUiccController::APP_FAM_3GPP2:
            index = mCdmaSubscriptionAppIndex;
            break;
        case IUiccController::APP_FAM_IMS:
            index = mImsSubscriptionAppIndex;
            break;
    }
    if (index >= 0 && index < mUiccApplications->GetLength()) {
        *result = (*mUiccApplications)[index];
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    *result = NULL;
    return NOERROR;
}

ECode UiccCard::GetApplicationIndex(
    /* [in] */ Int32 index,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     if (index >= 0 && index < mUiccApplications.length) {
    //         return mUiccApplications[index];
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetApplicationByType(
    /* [in] */ Int32 type,
    /* [out] */ IUiccCardApplication** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // synchronized (mLock) {
    //     for (int i = 0 ; i < mUiccApplications.length; i++) {
    //         if (mUiccApplications[i] != null &&
    //                 mUiccApplications[i].getType().ordinal() == type) {
    //             return mUiccApplications[i];
    //         }
    //     }
    //     return null;
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::IccOpenLogicalChannel(
    /* [in] */ const String& AID,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.iccOpenLogicalChannel(AID,
    //         mHandler.obtainMessage(EVENT_OPEN_LOGICAL_CHANNEL_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::IccCloseLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.iccCloseLogicalChannel(channel,
    //         mHandler.obtainMessage(EVENT_CLOSE_LOGICAL_CHANNEL_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::IccTransmitApduLogicalChannel(
    /* [in] */ Int32 channel,
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.iccTransmitApduLogicalChannel(channel, cla, command, p1, p2, p3,
    //         data, mHandler.obtainMessage(EVENT_TRANSMIT_APDU_LOGICAL_CHANNEL_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::IccTransmitApduBasicChannel(
    /* [in] */ Int32 cla,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& data,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.iccTransmitApduBasicChannel(cla, command, p1, p2, p3,
    //         data, mHandler.obtainMessage(EVENT_TRANSMIT_APDU_BASIC_CHANNEL_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::IccExchangeSimIO(
    /* [in] */ Int32 fileID,
    /* [in] */ Int32 command,
    /* [in] */ Int32 p1,
    /* [in] */ Int32 p2,
    /* [in] */ Int32 p3,
    /* [in] */ const String& pathID,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.iccIO(command, fileID, pathID, p1, p2, p3, null, null,
    //         mHandler.obtainMessage(EVENT_SIM_IO_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetAtr(
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.getAtr(mHandler.obtainMessage(EVENT_SIM_GET_ATR_DONE, response));
    assert(0);
    return NOERROR;
}

ECode UiccCard::SendEnvelopeWithStatus(
    /* [in] */ const String& contents,
    /* [in] */ IMessage* response)
{
    // ==================before translated======================
    // mCi.sendEnvelopeWithStatus(contents, response);
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetNumApplications(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 count = 0;
    for (Int32 i = 0; i < mUiccApplications->GetLength(); ++i) {
        if ((*mUiccApplications)[i] != NULL) {
            count++;
        }
    }
    *result = count;
    return NOERROR;
}

ECode UiccCard::AreCarrierPriviligeRulesLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierPrivilegeRules != null
    //     && mCarrierPrivilegeRules.areCarrierPriviligeRulesLoaded();
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatus(
    /* [in] */ ISignature* signature,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierPrivilegeRules == null ?
    //     TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
    //     mCarrierPrivilegeRules.getCarrierPrivilegeStatus(signature, packageName);
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatus(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ const String& packageName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierPrivilegeRules == null ?
    //     TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
    //     mCarrierPrivilegeRules.getCarrierPrivilegeStatus(packageManager, packageName);
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCarrierPrivilegeStatusForCurrentTransaction(
    /* [in] */ IPackageManager* packageManager,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierPrivilegeRules == null ?
    //     TelephonyManager.CARRIER_PRIVILEGE_STATUS_RULES_NOT_LOADED :
    //     mCarrierPrivilegeRules.getCarrierPrivilegeStatusForCurrentTransaction(packageManager);
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetCarrierPackageNamesForIntent(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ IIntent* intent,
    /* [out] */ IList/*<String>*/** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mCarrierPrivilegeRules == null ? null :
    //     mCarrierPrivilegeRules.getCarrierPackageNamesForIntent(
    //             packageManager, intent);
    assert(0);
    return NOERROR;
}

ECode UiccCard::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // log("setOperatorBrandOverride: " + brand);
    // log("current iccId: " + getIccId());
    //
    // String iccId = getIccId();
    // if (TextUtils.isEmpty(iccId)) {
    //     return false;
    // }
    //
    // SharedPreferences.Editor spEditor =
    //         PreferenceManager.getDefaultSharedPreferences(mContext).edit();
    // String key = OPERATOR_BRAND_OVERRIDE_PREFIX + iccId;
    // if (brand == null) {
    //     spEditor.remove(key).commit();
    // } else {
    //     spEditor.putString(key, brand).commit();
    // }
    // return true;
    assert(0);
    return NOERROR;
}

ECode UiccCard::GetOperatorBrandOverride(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // String iccId = getIccId();
    // if (TextUtils.isEmpty(iccId)) {
    //     return null;
    // }
    // SharedPreferences sp = PreferenceManager.getDefaultSharedPreferences(mContext);
    // return sp.getString(OPERATOR_BRAND_OVERRIDE_PREFIX + iccId, null);
    Logger::E("leliang", "TODO not implemented line:%d, func:%s\n", __LINE__, __func__);
    *result = String("Unicom");
    return NOERROR;
}

ECode UiccCard::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // // ICCID should be same across all the apps.
    // for (UiccCardApplication app : mUiccApplications) {
    //     if (app != null) {
    //         IccRecords ir = app.getIccRecords();
    //         if (ir != null && ir.getIccId() != null) {
    //             return ir.getIccId();
    //         }
    //     }
    // }
    // return null;
    Logger::E("leliang", "TODO not implemented line:%d, func:%s\n", __LINE__, __func__);
    *result = String("89860115831000000001");// unicom code
    return NOERROR;
}

ECode UiccCard::GetUICCConfig(
    /* [out] */ IUICCConfig** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // return mUICCConfig;
    assert(0);
    return NOERROR;
}

ECode UiccCard::OnRefresh(
    /* [in] */ IIccRefreshResponse* refreshResponse)
{
    // ==================before translated======================
    // for ( int i = 0; i < mUiccApplications.length; i++) {
    //     if (mUiccApplications[i] != null) {
    //         // Let the app know that the refresh occurred
    //         mUiccApplications[i].onRefresh(refreshResponse);
    //     }
    // }
    assert(0);
    return NOERROR;
}

ECode UiccCard::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    // ==================before translated======================
    // pw.println("UiccCard:");
    // pw.println(" mCi=" + mCi);
    // pw.println(" mDestroyed=" + mDestroyed);
    // pw.println(" mLastRadioState=" + mLastRadioState);
    // pw.println(" mCatService=" + mCatService);
    // pw.println(" mAbsentRegistrants: size=" + mAbsentRegistrants.size());
    // for (int i = 0; i < mAbsentRegistrants.size(); i++) {
    //     pw.println("  mAbsentRegistrants[" + i + "]="
    //             + ((Registrant)mAbsentRegistrants.get(i)).getHandler());
    // }
    // for (int i = 0; i < mCarrierPrivilegeRegistrants.size(); i++) {
    //     pw.println("  mCarrierPrivilegeRegistrants[" + i + "]="
    //             + ((Registrant)mCarrierPrivilegeRegistrants.get(i)).getHandler());
    // }
    // pw.println(" mCardState=" + mCardState);
    // pw.println(" mUniversalPinState=" + mUniversalPinState);
    // pw.println(" mGsmUmtsSubscriptionAppIndex=" + mGsmUmtsSubscriptionAppIndex);
    // pw.println(" mCdmaSubscriptionAppIndex=" + mCdmaSubscriptionAppIndex);
    // pw.println(" mImsSubscriptionAppIndex=" + mImsSubscriptionAppIndex);
    // pw.println(" mImsSubscriptionAppIndex=" + mImsSubscriptionAppIndex);
    // pw.println(" mUiccApplications: length=" + mUiccApplications.length);
    // for (int i = 0; i < mUiccApplications.length; i++) {
    //     if (mUiccApplications[i] == null) {
    //         pw.println("  mUiccApplications[" + i + "]=" + null);
    //     } else {
    //         pw.println("  mUiccApplications[" + i + "]="
    //                 + mUiccApplications[i].getType() + " " + mUiccApplications[i]);
    //     }
    // }
    // pw.println();
    // // Print details of all applications
    // for (UiccCardApplication app : mUiccApplications) {
    //     if (app != null) {
    //         app.dump(fd, pw, args);
    //         pw.println();
    //     }
    // }
    // // Print details of all IccRecords
    // for (UiccCardApplication app : mUiccApplications) {
    //     if (app != null) {
    //         IccRecords ir = app.getIccRecords();
    //         if (ir != null) {
    //             ir.dump(fd, pw, args);
    //             pw.println();
    //         }
    //     }
    // }
    // pw.flush();
    assert(0);
    return NOERROR;
}

void UiccCard::CreateAndUpdateCatService()
{
    // ==================before translated======================
    // if (mUiccApplications.length > 0 && mUiccApplications[0] != null) {
    //     // Initialize or Reinitialize CatService
    //     if (mCatService == null) {
    //         mCatService = CatServiceFactory.makeCatService(mCi, mContext, this, mSlotId);
    //     }
    // } else {
    //     if (mCatService != null) {
    //         CatServiceFactory.disposeCatService(mSlotId);
    //     }
    //     mCatService = null;
    // }
    assert(0);
}

ECode UiccCard::Finalize()
{
    // ==================before translated======================
    // if (DBG) log("UiccCard finalized");
    assert(0);
    return NOERROR;
}

void UiccCard::Log(
    /* [in] */ const String& msg)
{
    // ==================before translated======================
    // Rlog.d(LOGTAG, msg);
    assert(0);
}

AutoPtr<ArrayOf<IUiccCardApplication*> > UiccCard::MiddleInitMuiccapplications()
{
    // ==================before translated======================
    // UiccCardApplication[] result = new UiccCardApplication[IccCardStatus.CARD_MAX_APPS];
    assert(0);
    AutoPtr<ArrayOf<IUiccCardApplication*> > empty;
    return empty;
}

void UiccCard::SanitizeApplicationIndexes()
{
    // ==================before translated======================
    // mGsmUmtsSubscriptionAppIndex =
    //         checkIndex(mGsmUmtsSubscriptionAppIndex, AppType.APPTYPE_SIM, AppType.APPTYPE_USIM);
    // mCdmaSubscriptionAppIndex =
    //         checkIndex(mCdmaSubscriptionAppIndex, AppType.APPTYPE_RUIM, AppType.APPTYPE_CSIM);
    // mImsSubscriptionAppIndex =
    //         checkIndex(mImsSubscriptionAppIndex, AppType.APPTYPE_ISIM, null);
    assert(0);
}

Int32 UiccCard::CheckIndex(
    /* [in] */ Int32 index,
    /* [in] */ AppType expectedAppType,
    /* [in] */ AppType altExpectedAppType)
{
    // ==================before translated======================
    // if (mUiccApplications == null || index >= mUiccApplications.length) {
    //     loge("App index " + index + " is invalid since there are no applications");
    //     return -1;
    // }
    //
    // if (index < 0) {
    //     // This is normal. (i.e. no application of this type)
    //     return -1;
    // }
    //
    // if (mUiccApplications[index].getType() != expectedAppType &&
    //     mUiccApplications[index].getType() != altExpectedAppType) {
    //     loge("App index " + index + " is invalid since it's not " +
    //             expectedAppType + " and not " + altExpectedAppType);
    //     return -1;
    // }
    //
    // // Seems to be valid
    // return index;
    assert(0);
    return 0;
}

void UiccCard::OnIccSwap(
    /* [in] */ Boolean isAdded)
{
    // ==================before translated======================
    //
    // boolean isHotSwapSupported = mContext.getResources().getBoolean(
    //         com.android.internal.R.bool.config_hotswapCapable);
    //
    // if (isHotSwapSupported) {
    //     log("onIccSwap: isHotSwapSupported is true, don't prompt for rebooting");
    //     return;
    // }
    // log("onIccSwap: isHotSwapSupported is false, prompt for rebooting");
    //
    // synchronized (mLock) {
    //     // TODO: Here we assume the device can't handle SIM hot-swap
    //     //      and has to reboot. We may want to add a property,
    //     //      e.g. REBOOT_ON_SIM_SWAP, to indicate if modem support
    //     //      hot-swap.
    //     DialogInterface.OnClickListener listener = null;
    //
    //
    //     // TODO: SimRecords is not reset while SIM ABSENT (only reset while
    //     //       Radio_off_or_not_available). Have to reset in both both
    //     //       added or removed situation.
    //     listener = new DialogInterface.OnClickListener() {
    //         @Override
    //         public void onClick(DialogInterface dialog, int which) {
    //             synchronized (mLock) {
    //                 if (which == DialogInterface.BUTTON_POSITIVE) {
    //                     if (DBG) log("Reboot due to SIM swap");
    //                     PowerManager pm = (PowerManager) mContext
    //                             .getSystemService(Context.POWER_SERVICE);
    //                     pm.reboot("SIM is added.");
    //                 }
    //             }
    //         }
    //
    //     };
    //
    //     Resources r = Resources.getSystem();
    //
    //     String title = (isAdded) ? r.getString(R.string.sim_added_title) :
    //         r.getString(R.string.sim_removed_title);
    //     String message = (isAdded) ? r.getString(R.string.sim_added_message) :
    //         r.getString(R.string.sim_removed_message);
    //     String buttonTxt = r.getString(R.string.sim_restart_button);
    //
    //     AlertDialog dialog = new AlertDialog.Builder(mContext)
    //     .setTitle(title)
    //     .setMessage(message)
    //     .setPositiveButton(buttonTxt, listener)
    //     .create();
    //     dialog.getWindow().setType(WindowManager.LayoutParams.TYPE_SYSTEM_ALERT);
    //     dialog.show();
    // }
    assert(0);
}

void UiccCard::OnCarrierPriviligesLoadedMessage()
{
    // ==================before translated======================
    // synchronized (mLock) {
    //     mCarrierPrivilegeRegistrants.notifyRegistrants();
    // }
    assert(0);
}

void UiccCard::Loge(
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
