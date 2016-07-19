#include "Elastos.Droid.Provider.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CContentValues.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/CIntentFilter.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include "elastos/droid/internal/telephony/ModemStackController.h"
#include "elastos/droid/internal/telephony/SubInfoRecordUpdater.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"
#include "elastos/droid/internal/telephony/uicc/UiccController.h"
#include "elastos/droid/telephony/CSubInfoRecord.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/telephony/CSubInfoRecord.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/internal/telephony/IccUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/IntegralToString.h>
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Internal::Telephony::ICommandsInterface;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::IIccConstants;
using Elastos::Droid::Internal::Telephony::Uicc::IIccFileHandler;
using Elastos::Droid::Internal::Telephony::Uicc::IIccUtils;
//using Elastos::Droid::Internal::Telephony::Uicc::CSpnOverride;
using Elastos::Droid::Internal::Telephony::Uicc::ISpnOverride;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::UiccController;
//using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Telephony::SubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::CSubInfoRecord;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::IntegralToString;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

static Int32 InitPhoneCount()
{
    AutoPtr<ITelephonyManager> telephonyMgr;
    CTelephonyManager::GetDefault((ITelephonyManager**)&telephonyMgr);

    Int32 count;
    telephonyMgr->GetPhoneCount(&count);
    return count;
}

static AutoPtr<ArrayOf<CardState> > InitCardState()
{
    AutoPtr<ArrayOf<CardState> > sArray = ArrayOf<CardState>::Alloc(InitPhoneCount());
    return sArray;
}

static AutoPtr<ArrayOf<IIccFileHandler*> > InitIccFileHandler()
{
    AutoPtr<ArrayOf<IIccFileHandler*> > sArray = ArrayOf<IIccFileHandler*>::Alloc(InitPhoneCount());
    return sArray;
}

static AutoPtr<ArrayOf<String> > InitIccId()
{
    AutoPtr<ArrayOf<String> > sArray = ArrayOf<String>::Alloc(InitPhoneCount());
    return sArray;
}

static AutoPtr<ArrayOf<Int32> > InitInsertSimState()
{
    AutoPtr<ArrayOf<Int32> > sArray = ArrayOf<Int32>::Alloc(InitPhoneCount());
    return sArray;
}

static Boolean IsCardPresent(
    /* [in] */ CardState state)
{
    return state == Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_PRESENT;
}

const String SubInfoRecordUpdater::LOGTAG("SUB");
const Int32 SubInfoRecordUpdater::PROJECT_SIM_NUM = InitPhoneCount();
const Int32 SubInfoRecordUpdater::EVENT_OFFSET = 8;
const Int32 SubInfoRecordUpdater::EVENT_QUERY_ICCID_DONE = 1;
const Int32 SubInfoRecordUpdater::EVENT_ICC_CHANGED = 2;
const Int32 SubInfoRecordUpdater::EVENT_STACK_READY = 3;
const String SubInfoRecordUpdater::ICCID_STRING_FOR_NO_SIM("");
const String SubInfoRecordUpdater::ICCID_STRING_FOR_NV("DUMMY_NV_ID");

AutoPtr<ArrayOf<CardState> > SubInfoRecordUpdater::sCardState = InitCardState();
AutoPtr<ArrayOf<IIccFileHandler*> > SubInfoRecordUpdater::sFh = InitIccFileHandler();
AutoPtr<ArrayOf<String> > SubInfoRecordUpdater::sIccId = InitIccId();
AutoPtr<ArrayOf<Int32> > SubInfoRecordUpdater::sInsertSimState = InitInsertSimState();

AutoPtr<ArrayOf<IPhone*> > SubInfoRecordUpdater::sPhone;
AutoPtr<IContext> SubInfoRecordUpdater::sContext;
AutoPtr<IUiccController> SubInfoRecordUpdater::mUiccController;
AutoPtr<ArrayOf<ICommandsInterface*> > SubInfoRecordUpdater::sCi;
AutoPtr<ITelephonyManager> SubInfoRecordUpdater::sTelephonyMgr;
AutoPtr<ITelephonyManagerHelper> SubInfoRecordUpdater::sTelephonyManagerHelper;

// To prevent repeatedly update flow every time receiver SIM_STATE_CHANGE
Boolean SubInfoRecordUpdater::sNeedUpdate = TRUE;

SubInfoRecordUpdater::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ SubInfoRecordUpdater* host)
    : mHost(host)
{
    BroadcastReceiver::constructor();
}

//@Override
ECode SubInfoRecordUpdater::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    Logd(String("[Receiver]+"));
    String action;
    intent->GetAction(&action);
    Logd(String("Action: ") + action);
    Int32 slotId;
    if (action.Equals(ITelephonyIntents::ACTION_SIM_STATE_CHANGED)) {
        String simStatus;
        intent->GetStringExtra(IIccCardConstants::INTENT_KEY_ICC_STATE, &simStatus);
        intent->GetInt32Extra(IPhoneConstants::SLOT_KEY, ISubscriptionManager::INVALID_SLOT_ID, &slotId);
        Logd(String("slotId: ") + IntegralToString::ToString(slotId) + String(" simStatus: ") + simStatus);
        if (slotId == ISubscriptionManager::INVALID_SLOT_ID) {
            return NOERROR;
        }

        if (IIccCardConstants::INTENT_VALUE_ICC_LOADED.Equals(simStatus)) {
            if (sTelephonyMgr == NULL) {
                sTelephonyManagerHelper->From(sContext, (ITelephonyManager**)&sTelephonyMgr);
            }

            Int64 subId;
            intent->GetInt64Extra(IPhoneConstants::SUBSCRIPTION_KEY, ISubscriptionManager::INVALID_SUB_ID, &subId);

            Boolean bRet;
            if (SubscriptionManager::IsValidSubId(subId, &bRet), bRet) {
                AutoPtr<ITelephonyManager> defaultTelephonyMgr;
                sTelephonyManagerHelper->GetDefault((ITelephonyManager**)&defaultTelephonyMgr);

                String msisdn;
                defaultTelephonyMgr->GetLine1NumberForSubscriber(subId, &msisdn);

                AutoPtr<IContentResolver> contentResolver;
                sContext->GetContentResolver((IContentResolver**)&contentResolver);

                if (msisdn != NULL) {
                    AutoPtr<IContentValues> number;
                    CContentValues::New(1, (IContentValues**)&number);
                    number->Put(ISubscriptionManager::NUMBER, msisdn);
                    AutoPtr<IUri> uri;
                    SubscriptionManager::GetCONTENT_URI((IUri**)&uri);

                    Int32 rowsAffected;
                    contentResolver->Update(uri, number, String("_id") + String("=") + IntegralToString::ToString(subId), NULL, &rowsAffected);
//                    contentResolver->Update(uri, number, IBaseColumns::ID + String("=") + IntegralToString::ToString(subId), &rowsAffected);
                }

                AutoPtr<ISubInfoRecord> subInfo;
                SubscriptionManager::GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&subInfo);
                assert(0 && "TODO");
                // Int32 nameSource;
//                if (subInfo != NULL && (subInfo->GetNameSource(&nameSource), nameSource != ISubscriptionManager::NAME_SOURCE_USER_INPUT)) {
//                    AutoPtr<ISpnOverride> mSpnOverride;
//                    CSpnOverride::New((ISpnOverride**)&mSpnOverride);
                    String nameToSet;
                    String Carrier;
                    defaultTelephonyMgr->GetSimOperator(subId, &Carrier);
                    Logd(String("Carrier = ") + Carrier);
                    String CarrierName;
                    defaultTelephonyMgr->GetSimOperatorName(subId, &CarrierName);
                    Logd(String("CarrierName = ") + CarrierName);
//
//                    if (mSpnOverride->ContainsCarrier(Carrier)) {
//                        mSpnOverride->GetSpn(Carrier, &nameToSet);
//                        Logd(String("Found, name = ") + nameToSet);
//                    } else if (CarrierName != "") {
//                        nameToSet = CarrierName;
//                    } else {
//                        nameToSet = String("SIM ") + IntegralToString::ToString(slotId + 1);
//                        Logd(String("Not found, name = ") + nameToSet);
//                    }
//
                    AutoPtr<IContentValues> name;
                    CContentValues::New(1, (IContentValues**)&name);
                    name->Put(ISubscriptionManager::DISPLAY_NAME, nameToSet);
                    AutoPtr<IUri> uri;
                    SubscriptionManager::GetCONTENT_URI((IUri**)&uri);
                    Int32 rowsAffected;
                    contentResolver->Update(uri, name, String("_id") + String("=") + IntegralToString::ToString(subId), NULL, &rowsAffected);
//                    contentResolver->Update(uri, name, IBaseColumns::ID + String("=") + IntegralToString::ToString(subId), NULL, &rowsAffected);
//                }
            } else {
                Logd(String("[Receiver] Invalid subId, could not update ContentResolver"));
            }
        }
    }
    Logd(String("[Receiver]-"));
    return NOERROR;
}

ECode SubInfoRecordUpdater::MyBroadcastReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = String("SubInfoRecordUpdater::MyBroadcastReceiver: ");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}

/**
 *@hide
 */
CAR_INTERFACE_IMPL(SubInfoRecordUpdater, Handler, ISubInfoRecordUpdater)
ECode SubInfoRecordUpdater::constructor(
    /* [in] */ IContext* context,
    /* [in] */ ArrayOf<IPhone*>* phoneProxy,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Handler::constructor();
    Logd(String("Constructor invoked"));

    sReceiver = new MyBroadcastReceiver(this);
    sContext = context;
    sPhone = phoneProxy;
    sCi = ci;
    SubscriptionHelper::Init(context, ci);

    mUiccController = UiccController::GetInstance();
    mUiccController->RegisterForIccChanged(this, EVENT_ICC_CHANGED, NULL);

    ModemStackController::GetInstance()->RegisterForStackReady(this, EVENT_STACK_READY, NULL);
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        (*sCardState)[i] = Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_ABSENT;
    }
    AutoPtr<IIntentFilter> intentFilter;
    AutoPtr<IIntent> intent;
    CIntentFilter::New(ITelephonyIntents::ACTION_SIM_STATE_CHANGED, (IIntentFilter**)&intentFilter);
    sContext->RegisterReceiver(sReceiver, intentFilter, (IIntent**)&intent);

    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&sTelephonyManagerHelper);
    isNVSubAvailable = FALSE;
    return NOERROR;
}

Int32 SubInfoRecordUpdater::EncodeEventId(
    /* [in] */ Int32 event,
    /* [in] */ Int32 slotId)
{
    return event << (slotId * EVENT_OFFSET);
}

Boolean SubInfoRecordUpdater::IsAllIccIdQueryDone()
{
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if ((*sIccId)[i] == NULL) {
            Logd(String("Wait for SIM") + IntegralToString::ToString(i + 1) + String(" IccId"));
            return FALSE;
        }
    }
    Logd(String("All IccIds query complete"));
    return TRUE;
}

ECode SubInfoRecordUpdater::SetDisplayNameForNewSub(
    /* [in] */ const String& newSubName,
    /* [in] */ Int32 subId,
    /* [in] */ Int32 newNameSource)
{
    AutoPtr<ISubInfoRecord> subInfo;
    SubscriptionManager::GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&subInfo);
    if (subInfo != NULL) {
        // overwrite SIM display name if it is not assigned by user
        assert(0 && "TODO");
        Int32 oldNameSource;// = subInfo.nameSource;
        String oldSubName;// = subInfo.displayName;
        Logd(String("[setDisplayNameForNewSub] mSubInfoIdx = ") + /*IntegralToString::ToString(subInfo.subId) + */String(", oldSimName = ")
                + oldSubName + String(", oldNameSource = ") + IntegralToString::ToString(oldNameSource) + String(", newSubName = ")
                + newSubName + String(", newNameSource = ") + IntegralToString::ToString(newNameSource));
        if (oldSubName == NULL ||
            (oldNameSource == ISubscriptionManager::NAME_SOURCE_DEFAULT_SOURCE && newSubName != NULL) ||
            (oldNameSource == ISubscriptionManager::NAME_SOURCE_SIM_SOURCE && newSubName != NULL
                    && !newSubName.Equals(oldSubName))) {
            assert(0 && "TODO");
//            SubscriptionManager::SetDisplayName(newSubName, subInfo.subId, newNameSource);
        }
    } else {
        Logd(String("SUB") + IntegralToString::ToString(subId + 1) + String(" SubInfo not created yet"));
    }
    return NOERROR;
}

//@Override
CARAPI SubInfoRecordUpdater::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgNum, arg1, arg2;
    msg->GetWhat(&msgNum);
    Logger::D(LOGTAG, "[TODO]  ======SubInfoRecordUpdater::HandleMessage=what:%d====", msgNum);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject::Probe(obj));
    Int32 slotId;
    for (slotId = IPhoneConstants::SUB1; slotId <= IPhoneConstants::SUB3; slotId++) {
        Int32 pivot = 1 << (slotId * EVENT_OFFSET);
        if (msgNum >= pivot) {
            continue;
        } else {
            break;
        }
    }
    slotId--;
    Int32 event = msgNum >> (slotId * EVENT_OFFSET);
    switch (event) {
        case EVENT_QUERY_ICCID_DONE:
            Logd(String("handleMessage : <EVENT_QUERY_ICCID_DONE> SIM") + IntegralToString::ToString(slotId + 1));
            if (ar != NULL && ar->mException == NULL) {
                if (ar->mResult != NULL) {
                    AutoPtr<IArrayOf> byteArray = IArrayOf::Probe(ar->mResult);
                    Int32 size;
                    byteArray->GetLength(&size);
                    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(size);
                    for(Int32 i = 0; i < size; ++i) {
                        AutoPtr<IInterface> ele;
                        byteArray->Get(i, (IInterface**)&ele);
                        Byte b;
                        IByte::Probe(ele)->GetValue(&b);
                        data->Set(i, b);
                    }

                    (*sIccId)[slotId] = IccUtils::BcdToString(data, 0, size);
                } else {
                    Logd(String("Null ar"));
                    (*sIccId)[slotId] = ICCID_STRING_FOR_NO_SIM;
                }
            } else {
                (*sIccId)[slotId] = ICCID_STRING_FOR_NO_SIM;
                Logd(String("Query IccId fail: "));// + ar.exception);
            }
            Logd(String("(*sIccId)[") + IntegralToString::ToString(slotId) + String("] = ") + (*sIccId)[slotId]);
            if (IsAllIccIdQueryDone() && sNeedUpdate) {
                UpdateSimInfoByIccId();
            }
            break;
        case EVENT_ICC_CHANGED: {
            AutoPtr<IInteger32> cardIndex;
            CInteger32::New(IPhoneConstants::DEFAULT_CARD_INDEX, (IInteger32**)&cardIndex);
            if (ar != NULL && ar->mResult != NULL) {
                cardIndex = IInteger32::Probe(ar->mResult);
            } else {
                //TODO Rlog->E(LOGTAG, "Error: Invalid card index EVENT_ICC_CHANGED ");
                Logger::E(LOGTAG, "Error: Invalid card index EVENT_ICC_CHANGED ");
                return NOERROR;
            }
            Int32 value;
            cardIndex->GetValue(&value);
            UpdateIccAvailability(value);
            break;
        }
        case EVENT_STACK_READY:
            Logd(String("EVENT_STACK_READY" ));
            if (IsAllIccIdQueryDone() && PROJECT_SIM_NUM > 1) {
                SubscriptionHelper::GetInstance()->UpdateSubActivation(sInsertSimState, TRUE);
            }
            break;
        default:
            Logd(String("Unknown msg:") + IntegralToString::ToString(msgNum));
    }
    return NOERROR;
}

void SubInfoRecordUpdater::UpdateIccAvailability(
    /* [in] */ Int32 slotId)
{
    if (NULL == mUiccController) {
        Logger::E("SubInfoRecordUpdater", " UpdateIccAvailability mUiccController is NULL");
        return;
    }

    AutoPtr<ISubscriptionHelper> subHelper = SubscriptionHelper::GetInstance();
    Logd(String("updateIccAvailability: Enter, slotId ") + IntegralToString::ToString(slotId));
    Boolean bTmp;
    if (PROJECT_SIM_NUM > 1 && !(subHelper->ProceedToHandleIccEvent(slotId, &bTmp), bTmp)) {
        Logd(String("updateIccAvailability: radio is OFF/unavailable, ignore "));
        Boolean isApmSIMNotPwdn;
        if (!(subHelper->IsApmSIMNotPwdn(&isApmSIMNotPwdn), isApmSIMNotPwdn)) {
            // set the iccid to NULL so that once SIM card detected
            //  ICCID will be read from the card again.
            (*sIccId)[slotId] = NULL;
        }
        return;
    }

    CardState newState = Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_ABSENT;
    AutoPtr<IUiccCard> newCard;
    mUiccController->GetUiccCard(slotId, (IUiccCard**)&newCard);
    if (newCard != NULL) {
        newCard->GetCardState(&newState);
    }
    CardState oldState = (*sCardState)[slotId];
    Logger::E("SubInfoRecordUpdater", "TODO UpdateIccAvailability does the oldState is right???");
    (*sCardState)[slotId] = newState;
    Logd(String("Slot[") + IntegralToString::ToString(slotId) + String("]: New Card State = ")
            + IntegralToString::ToString(newState) + String(" ") + String("Old Card State = ") + IntegralToString::ToString(oldState));

    if (!IsCardPresent(newState)) {
        //Card moved to ABSENT State
        if ((*sIccId)[slotId] != NULL && !(*sIccId)[slotId].Equals(ICCID_STRING_FOR_NO_SIM)) {
            Logd(String("SIM") + IntegralToString::ToString(slotId + 1) + String(" hot plug out"));
            sNeedUpdate = TRUE;
        }
        (*sFh)[slotId] = NULL;
        (*sIccId)[slotId] = ICCID_STRING_FOR_NO_SIM;
        if (IsAllIccIdQueryDone() && sNeedUpdate) {
            UpdateSimInfoByIccId();
        }
    }
    else if (!IsCardPresent(oldState) && IsCardPresent(newState)) {
        // Card moved to PRESENT State.
        if ((*sIccId)[slotId] != NULL && (*sIccId)[slotId].Equals(ICCID_STRING_FOR_NO_SIM)) {
            Logd(String("SIM") + IntegralToString::ToString(slotId + 1) + String(" hot plug in"));
            (*sIccId)[slotId] = NULL;
            sNeedUpdate = TRUE;
        }
        QueryIccId(slotId);
    }
    else if (IsCardPresent(oldState) && IsCardPresent(newState) &&
            (!(subHelper->IsApmSIMNotPwdn(&bTmp), bTmp)) && ((*sIccId)[slotId] == NULL)) {
        Logd(String("SIM") + IntegralToString::ToString(slotId + 1) + String(" powered up from APM "));
        (*sFh)[slotId] = NULL;
        sNeedUpdate = TRUE;
        QueryIccId(slotId);
    }
}

void SubInfoRecordUpdater::QueryIccId(
    /* [in] */ Int32 slotId)
{
    Logd(String("queryIccId: slotid=") + IntegralToString::ToString(slotId));
    if ((*sFh)[slotId] == NULL) {
        Logd(String("Getting IccFileHandler"));

        AutoPtr<IUiccCardApplication> validApp;
        AutoPtr<IUiccCard> uiccCard;
        mUiccController->GetUiccCard(slotId, (IUiccCard**)&uiccCard);
        Int32 numApps;
        uiccCard->GetNumApplications(&numApps);
        for (Int32 i = 0; i < numApps; i++) {
            AutoPtr<IUiccCardApplication> app;
            uiccCard->GetApplicationIndex(i, (IUiccCardApplication**)&app);
            AppType aType;
            if (app != NULL && (app->GetType(&aType), aType != Elastos::Droid::Internal::Telephony::Uicc::APPTYPE_UNKNOWN)) {
                validApp = app;
                break;
            }
        }
        if (validApp != NULL) validApp->GetIccFileHandler((IIccFileHandler**)&(*sFh)[slotId]);
    }

    if ((*sFh)[slotId] != NULL) {
        String iccId = (*sIccId)[slotId];
        if (iccId == NULL) {
            Logd(String("Querying IccId"));
            AutoPtr<IMessage> msg;
            ObtainMessage(EncodeEventId(EVENT_QUERY_ICCID_DONE, slotId), (IMessage**)&msg);
            (*sFh)[slotId]->LoadEFTransparent(IIccConstants::EF_ICCID, msg);
        } else {
            Logd(String("NOT Querying IccId its already set (*sIccId)[") + IntegralToString::ToString(slotId) + String("]=") + iccId);
        }
    }
    else {
        //Reset to CardState to ABSENT so that on next EVENT_ICC_CHANGED, ICCID can be read.
        (*sCardState)[slotId] = Elastos::Droid::Internal::Telephony::Uicc::CARDSTATE_ABSENT;
        Logd(String("sFh[") + IntegralToString::ToString(slotId) + String("] is NULL, SIM not inserted"));
    }
}

CARAPI SubInfoRecordUpdater::UpdateSubIdForNV(
    /* [in] */ Int32 slotId)
{
    (*sIccId)[slotId] = ICCID_STRING_FOR_NV;
    sNeedUpdate = TRUE;
    Logd(String("[updateSubIdForNV]+ Start"));
    if (IsAllIccIdQueryDone()) {
        Logd(String("[updateSubIdForNV]+ updating"));
        UpdateSimInfoByIccId();
        isNVSubAvailable = TRUE;
    }
    return NOERROR;
}

//synchronized
CARAPI SubInfoRecordUpdater::UpdateSimInfoByIccId()
{
    AutoLock lock(THIS);
    Logd(String("[updateSimInfoByIccId]+ Start"));
    sNeedUpdate = FALSE;

    SubscriptionManager::ClearSubInfo();

    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        (*sInsertSimState)[i] = SIM_NOT_CHANGE;
    }

    Int32 insertedSimCount = PROJECT_SIM_NUM;
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if (ICCID_STRING_FOR_NO_SIM.Equals((*sIccId)[i])) {
            insertedSimCount--;
            (*sInsertSimState)[i] = SIM_NOT_INSERT;
        }
    }
    Logd(String("insertedSimCount = ") + IntegralToString::ToString(insertedSimCount));

    Int32 index = 0;
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if ((*sInsertSimState)[i] == SIM_NOT_INSERT) {
            continue;
        }
        index = 2;
        for (Int32 j = i + 1; j < PROJECT_SIM_NUM; j++) {
            if ((*sInsertSimState)[j] == SIM_NOT_CHANGE && (*sIccId)[i].Equals((*sIccId)[j])) {
                (*sInsertSimState)[i] = 1;
                (*sInsertSimState)[j] = index;
                index++;
            }
        }
    }

    AutoPtr<IContentResolver> contentResolver;
    sContext->GetContentResolver((IContentResolver**)&contentResolver);
    AutoPtr<ArrayOf<String> > oldIccId = ArrayOf<String>::Alloc(PROJECT_SIM_NUM);
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        (*oldIccId)[i] = NULL;
        AutoPtr<IList> oldSubInfo;// SubInfoRecord
        SubscriptionController::GetInstance()->GetSubInfoUsingSlotIdWithCheck(i, FALSE, (IList**)&oldSubInfo);
        if (oldSubInfo != NULL) {
            AutoPtr<IInterface> obj;
            oldSubInfo->Get(0, (IInterface**)&obj);
            ISubInfoRecord* sir = ISubInfoRecord::Probe(obj);
            CSubInfoRecord* csir = (CSubInfoRecord*)(sir);
            (*oldIccId)[i] = csir->mIccId;
            Logd(String("oldSubId = ") + StringUtils::ToString(csir->mSubId));
            if ((*sInsertSimState)[i] == SIM_NOT_CHANGE && !(*sIccId)[i].Equals((*oldIccId)[i])) {
                (*sInsertSimState)[i] = SIM_CHANGED;
            }
            if ((*sInsertSimState)[i] != SIM_NOT_CHANGE) {
                AutoPtr<IContentValues> value;
                CContentValues::New(1, (IContentValues**)&value);
                value->Put(ISubscriptionManager::SIM_ID, ISubscriptionManager::INVALID_SLOT_ID);
                AutoPtr<IUri> uri;
                SubscriptionManager::GetCONTENT_URI((IUri**)&uri);
                Int32 rowAffected;
                contentResolver->Update(uri, value,
                        IBaseColumns::ID + String("=")
                        + IntegralToString::ToString(csir->mSubId), NULL, &rowAffected);
            }
        } else {
            if ((*sInsertSimState)[i] == SIM_NOT_CHANGE) {
                // no SIM inserted last time, but there is one SIM inserted now
                (*sInsertSimState)[i] = SIM_CHANGED;
            }
            (*oldIccId)[i] = ICCID_STRING_FOR_NO_SIM;
            Logd(String("No SIM in slot ") + IntegralToString::ToString(i) + String(" last time"));
        }
    }

    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        Logd(String("oldIccId[") + IntegralToString::ToString(i) + String("] = ") +
            (*oldIccId)[i] + String(", (*sIccId)[") + IntegralToString::ToString(i) + String("] = ") + (*sIccId)[i]);
    }

    //check if the inserted SIM is new SIM
    Int32 nNewCardCount = 0;
    Int32 nNewSimStatus = 0;
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if ((*sInsertSimState)[i] == SIM_NOT_INSERT) {
            Logd(String("No SIM inserted in slot ") + IntegralToString::ToString(i) + String(" this time"));
        } else {
            AutoPtr<IUri> uri;
            if ((*sInsertSimState)[i] > 0) {
                //some special SIMs may have the same IccIds, add suffix to distinguish them
                //FIXME: addSubInfoRecord can return an error.
                SubscriptionManager::AddSubInfoRecord((*sIccId)[i]
                        + IntegralToString::ToString((*sInsertSimState)[i]), i, (IUri**)&uri);
                Logd(String("SUB") + IntegralToString::ToString(i + 1) + String(" has invalid IccId"));
            } else /*if ((*sInsertSimState)[i] != SIM_NOT_INSERT)*/ {
                SubscriptionManager::AddSubInfoRecord((*sIccId)[i], i, (IUri**)&uri);
            }
            if (IsNewSim((*sIccId)[i], oldIccId.Get())) {
                nNewCardCount++;
                switch (i) {
                    case IPhoneConstants::SUB1:
                        nNewSimStatus |= STATUS_SIM1_INSERTED;
                        break;
                    case IPhoneConstants::SUB2:
                        nNewSimStatus |= STATUS_SIM2_INSERTED;
                        break;
                    case IPhoneConstants::SUB3:
                        nNewSimStatus |= STATUS_SIM3_INSERTED;
                        break;
                    //case IPhoneConstants::SUB3:
                    //    nNewSimStatus |= STATUS_SIM4_INSERTED;
                    //    break;
                }

                (*sInsertSimState)[i] = SIM_NEW;
            }
        }
    }

    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if ((*sInsertSimState)[i] == SIM_CHANGED) {
            (*sInsertSimState)[i] = SIM_REPOSITION;
        }
    }
    SubscriptionHelper::GetInstance()->UpdateNwMode();
    if (ModemStackController::GetInstance()->IsStackReady() && PROJECT_SIM_NUM > 1) {
        SubscriptionHelper::GetInstance()->UpdateSubActivation(sInsertSimState, FALSE);
    }

    AutoPtr<IList> subInfos;// SubInfoRecord
    SubscriptionManager::GetActiveSubInfoList((IList**)&subInfos);
    Int32 nSubCount;// = (subInfos == NULL) ? 0 : subInfos->Size();
    if (subInfos == NULL) {
        nSubCount = 0;
    }
    else {
        subInfos->GetSize(&nSubCount);
    }
    Logd(String("nSubCount = ") + IntegralToString::ToString(nSubCount));
    AutoPtr<ITelephonyManager> defaultTelephonyMgr;
    sTelephonyManagerHelper->GetDefault((ITelephonyManager**)&defaultTelephonyMgr);
    for (Int32 i = 0; i < nSubCount; i++) {
        AutoPtr<IInterface> obj;
        subInfos->Get(i, (IInterface**)&obj);
        ISubInfoRecord* temp = ISubInfoRecord::Probe(obj);
        CSubInfoRecord* csir = (CSubInfoRecord*)(temp);

        String msisdn;
        defaultTelephonyMgr->GetLine1NumberForSubscriber(csir->mSubId, &msisdn);

        if (msisdn != NULL) {
            AutoPtr<IContentValues> value;
            CContentValues::New(1, (IContentValues**)&value);
            value->Put(ISubscriptionManager::NUMBER, msisdn);
            AutoPtr<IUri> uri;
            SubscriptionManager::GetCONTENT_URI((IUri**)&uri);
            Int32 rowAffected;
            contentResolver->Update(uri, value,
                    IBaseColumns::ID + String("=") + IntegralToString::ToString(csir->mSubId), NULL, &rowAffected);
        }
    }

    // TRUE if any slot has no SIM this time, but has SIM last time
    Boolean hasSimRemoved = FALSE;
    for (Int32 i=0; i < PROJECT_SIM_NUM; i++) {
        if ((*sIccId)[i] != NULL && (*sIccId)[i].Equals(ICCID_STRING_FOR_NO_SIM)
                && !(*oldIccId)[i].Equals("")) {
            hasSimRemoved = TRUE;
            break;
        }
    }

    if (nNewCardCount == 0) {
        Int32 i;
        if (hasSimRemoved) {
            // no new SIM, at least one SIM is removed, check if any SIM is repositioned first
            for (i=0; i < PROJECT_SIM_NUM; i++) {
                if ((*sInsertSimState)[i] == SIM_REPOSITION) {
                    Logd(String("No new SIM detected and SIM repositioned"));
                    SetUpdatedData(ISubscriptionManager::EXTRA_VALUE_REPOSITION_SIM,
                            nSubCount, nNewSimStatus);
                    break;
                }
            }
            if (i == PROJECT_SIM_NUM) {
                // no new SIM, no SIM is repositioned => at least one SIM is removed
                Logd(String("No new SIM detected and SIM removed"));
                SetUpdatedData(ISubscriptionManager::EXTRA_VALUE_REMOVE_SIM,
                        nSubCount, nNewSimStatus);
            }
        } else {
            // no SIM is removed, no new SIM, just check if any SIM is repositioned
            for (i=0; i< PROJECT_SIM_NUM; i++) {
                if ((*sInsertSimState)[i] == SIM_REPOSITION) {
                    Logd(String("No new SIM detected and SIM repositioned"));
                    SetUpdatedData(ISubscriptionManager::EXTRA_VALUE_REPOSITION_SIM,
                            nSubCount, nNewSimStatus);
                    break;
                }
            }
            if (i == PROJECT_SIM_NUM) {
                // all status remain unchanged
                Logd(String("[updateSimInfoByIccId] All SIM inserted into the same slot"));
                SetUpdatedData(ISubscriptionManager::EXTRA_VALUE_NOCHANGE,
                        nSubCount, nNewSimStatus);
            }
        }
    } else {
        Logd(String("New SIM detected"));
        SetUpdatedData(ISubscriptionManager::EXTRA_VALUE_NEW_SIM, nSubCount, nNewSimStatus);
    }

    Logd(String("[updateSimInfoByIccId]- SimInfo update complete"));
    return NOERROR;
}

void SubInfoRecordUpdater::SetUpdatedData(
    /* [in] */ Int32 detectedType,
    /* [in] */ Int32 subCount,
    /* [in] */ Int32 newSimStatus)
{
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED, (IIntent**)&intent);

    Logd(String("[setUpdatedData]+ "));

    if (detectedType == ISubscriptionManager::EXTRA_VALUE_NEW_SIM ) {
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_DETECT_STATUS,
                ISubscriptionManager::EXTRA_VALUE_NEW_SIM);
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_SIM_COUNT, subCount);
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_NEW_SIM_SLOT, newSimStatus);
    } else if (detectedType == ISubscriptionManager::EXTRA_VALUE_REPOSITION_SIM) {
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_DETECT_STATUS,
                ISubscriptionManager::EXTRA_VALUE_REPOSITION_SIM);
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_SIM_COUNT, subCount);
    } else if (detectedType == ISubscriptionManager::EXTRA_VALUE_REMOVE_SIM) {
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_DETECT_STATUS,
                ISubscriptionManager::EXTRA_VALUE_REMOVE_SIM);
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_SIM_COUNT, subCount);
    } else if (detectedType == ISubscriptionManager::EXTRA_VALUE_NOCHANGE) {
        intent->PutExtra(ISubscriptionManager::INTENT_KEY_DETECT_STATUS,
                ISubscriptionManager::EXTRA_VALUE_NOCHANGE);
    }

    Logd(String("broadcast intent ACTION_SUBINFO_RECORD_UPDATED : [") + IntegralToString::ToString(detectedType) + String(", ")
            + IntegralToString::ToString(subCount) + String(", ") + IntegralToString::ToString(newSimStatus) + String("]"));

    ActivityManagerNative::BroadcastStickyIntent(intent, Manifest::permission::READ_PHONE_STATE, IUserHandle::USER_ALL);
    Logd(String("[setUpdatedData]- "));
}

Boolean SubInfoRecordUpdater::IsNewSim(
    /* [in] */ const String& iccId,
    /* [in] */ ArrayOf<String>* oldIccId)
{
    Boolean newSim = TRUE;
    for (Int32 i = 0; i < PROJECT_SIM_NUM; i++) {
        if(iccId.Equals((*oldIccId)[i])) {
            newSim = FALSE;
            break;
        }
    }
    Logd(String("newSim = ") + IntegralToString::ToString(newSim));

    return newSim;
}

CARAPI SubInfoRecordUpdater::Dispose()
{
    Logd(String("[dispose]"));
    return sContext->UnregisterReceiver(sReceiver);
}

void SubInfoRecordUpdater::Logd(
    /* [in] */ const String& message)
{
//TODO
//    Rlog->D(LOGTAG, "[SubInfoRecordUpdater]" + message);
    Logger::E("SubInfoRecordUpdater", "TODO SubInfoRecordUpdater, Logd:%s\n", message.string());
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
