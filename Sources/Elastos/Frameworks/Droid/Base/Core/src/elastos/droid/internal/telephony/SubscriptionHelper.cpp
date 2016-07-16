
#include "elastos/droid/internal/telephony/SubscriptionHelper.h"
#include "elastos/droid/internal/telephony/CSubscriptionHelper.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/CMessageHelper.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/internal/telephony/ModemBindingPolicyHandler.h"
#include "elastos/droid/internal/telephony/uicc/CUiccControllerHelper.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Internal::Telephony::IModemBindingPolicyHandler;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::Uicc::AppType;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::CUiccControllerHelper;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCard;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccCardApplication;
using Elastos::Droid::R;

using Elastos::Core::AutoLock;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  SubscriptionHelper::MyContentObserver::
//==============================================================

SubscriptionHelper::MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* hdl,
    /* [in] */ SubscriptionHelper* host)
    : mHost(host)
{
}

ECode SubscriptionHelper::MyContentObserver::OnChange(
    /* [in] */ Boolean selfUpdate)
{
    Logd(String("NwMode Observer onChange hit !!!"));
    if (!mNwModeUpdated) {
        return NOERROR;
    }
    //get nwMode from all slots in Db and update to subId table.
    mHost->UpdateNwModesInSubIdTable(TRUE);
    return NOERROR;
}

//==============================================================
//  SubscriptionHelper::
//==============================================================
const String SubscriptionHelper::LOGTAG("SubHelper");
AutoPtr<ISubscriptionHelper> SubscriptionHelper::sInstance;

Int32 SubscriptionHelper::sNumPhones;
Boolean SubscriptionHelper::sTriggerDds = FALSE;

const String SubscriptionHelper::APM_SIM_NOT_PWDN_PROPERTY("persist.radio.apm_sim_not_pwdn");

static Boolean InitPwdn()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Int32 num = 0;
    sp->GetInt32(
        SubscriptionHelper::APM_SIM_NOT_PWDN_PROPERTY, 0, &num);
    return num  == 1;
}
const Boolean SubscriptionHelper::sApmSIMNotPwdn = InitPwdn();

const Int32 SubscriptionHelper::EVENT_SET_UICC_SUBSCRIPTION_DONE = 1;
const Int32 SubscriptionHelper::EVENT_REFRESH = 2;
const Int32 SubscriptionHelper::EVENT_REFRESH_OEM = 3;

Boolean SubscriptionHelper::mNwModeUpdated = FALSE;

CAR_INTERFACE_IMPL(SubscriptionHelper, Handler, ISubscriptionHelper)

AutoPtr<ISubscriptionHelper> SubscriptionHelper::Init(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    {
        // AutoLock syncLock(SubscriptionHelper.class);
        if (sInstance == NULL) {
            CSubscriptionHelper::New(c, ci, (ISubscriptionHelper**)&sInstance);
        }
        else {
            // Log::Wtf(LOGTAG, "Init() called multiple times!  sInstance = " + sInstance);
        }
        return sInstance;
    }
}

AutoPtr<ISubscriptionHelper> SubscriptionHelper::GetInstance()
{
    if (sInstance == NULL) {
       // Log::Wtf(LOGTAG, "getInstance NULL");
    }

    return sInstance;
}

ECode SubscriptionHelper::constructor(
    /* [in] */ IContext* c,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    AutoPtr<IHandler> h;
    CHandler::New((IHandler**)&h);
    mNwModeObserver = new MyContentObserver(h, this);

    mContext = c;
    mCi = ci;
    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    tm->GetPhoneCount(&sNumPhones);
    mSubStatus = ArrayOf<Int32>::Alloc(sNumPhones);
    for (Int32 i = 0; i < sNumPhones; i++ ) {
        (*mSubStatus)[i] = SUB_INIT_STATE;
        AutoPtr<IInteger32> index;
        CInteger32::New(i, (IInteger32**)&index);
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Boolean b = FALSE;
        res->GetBoolean(
            R::bool_::config_sim_refresh_for_dual_mode_card, &b);
        if (b) {
            (*mCi)[i]->RegisterForSimRefreshEvent(this, EVENT_REFRESH_OEM, index);
        }
        else {
            (*mCi)[i]->RegisterForIccRefresh(this, EVENT_REFRESH, index);
        }
    }
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IUri> uri;
    sg->GetUriFor(
            ISettingsGlobal::PREFERRED_NETWORK_MODE, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, mNwModeObserver);


    // String str("SubscriptionHelper init by Context, num phones = ");
    // str += sNumPhones;
    // str += " ApmSIMNotPwdn = ";
    // str += sApmSIMNotPwdn;
    // Logd(str);
    return NOERROR;
}

void SubscriptionHelper::UpdateNwModesInSubIdTable(
    /* [in] */ Boolean _override)
{
    AutoPtr<ISubscriptionControllerHelper> schlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&schlp);
    AutoPtr<ISubscriptionController> subCtrlr;
    schlp->GetInstance((ISubscriptionController**)&subCtrlr);
    for (Int32 i = 0; i < sNumPhones; i++ ) {
        AutoPtr<ArrayOf<Int64> > subIdList;
        IISub::Probe(subCtrlr)->GetSubId(i, (ArrayOf<Int64>**)&subIdList);
        if (subIdList != NULL && (*subIdList)[0] > 0) {
            Int32 nwModeInDb = 0;
            // try {
            AutoPtr<IContentResolver> cr;
            mContext->GetContentResolver((IContentResolver**)&cr);
            AutoPtr<ITelephonyManagerHelper> tmhlp;
            CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
            tmhlp->GetInt32AtIndex(cr,
                    ISettingsGlobal::PREFERRED_NETWORK_MODE,
                    i,
                    &nwModeInDb);
            // } Catch (SettingNotFoundException snfe) {
            //     Loge("Settings Exception Reading Value At Index[" + i +
            //             "] Settings::Global::PREFERRED_NETWORK_MODE");
            //     nwModeInDb = RILConstants.PREFERRED_NETWORK_MODE;
            // }
            Int32 nwModeinSubIdTable = 0;
            subCtrlr->GetNwMode((*subIdList)[0], &nwModeinSubIdTable);
            // String str("updateNwModesInSubIdTable: nwModeinSubIdTable: ");
            // str += nwModeinSubIdTable;
            // str += ", nwModeInDb: ";
            // str += nwModeInDb;
            // Logd(str);

            //store Db value to table only if value in table is default
            //OR if override is set to True.
            if (_override || nwModeinSubIdTable == ISubscriptionManager::DEFAULT_NW_MODE) {
                subCtrlr->SetNwMode((*subIdList)[0], nwModeInDb);
            }
        }
    }
}

ECode SubscriptionHelper::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInteger32> index;
    CInteger32::New(IPhoneConstants::DEFAULT_CARD_INDEX, (IInteger32**)&index);
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<AsyncResult> ar;
    switch(what) {
        case EVENT_SET_UICC_SUBSCRIPTION_DONE: {
            Logd(String("EVENT_SET_UICC_SUBSCRIPTION_DONE"));
            ProcessSetUiccSubscriptionDone(msg);
            break;
        }
        case EVENT_REFRESH:
        case EVENT_REFRESH_OEM: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)(IObject*)obj.Get();
            index = IInteger32::Probe(ar->mUserObj);
            String str(" Received SIM refresh, reset sub state ");
            Int32 iIndex = 0;
            index->GetValue(&iIndex);
            // str += iIndex;
            // str += " old sub state ";
            // str += (*mSubStatus)[iIndex];
            // Logi(str);
            (*mSubStatus)[iIndex] = SUB_INIT_STATE;
            break;
        }
        default:
            break;
    }
    return NOERROR;
}

ECode SubscriptionHelper::UpdateSubActivation(
    /* [in] */ ArrayOf<Int32>* simStatus,
    /* [in] */ Boolean isStackReadyEvent)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean isPrimarySubFeatureEnable = FALSE;
    sp->GetBoolean(String("persist.radio.primarycard"), FALSE, &isPrimarySubFeatureEnable);
    AutoPtr<ISubscriptionControllerHelper> schlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&schlp);
    AutoPtr<ISubscriptionController> subCtrlr;
    schlp->GetInstance((ISubscriptionController**)&subCtrlr);
    Boolean setUiccSent = FALSE;
    // When isPrimarySubFeatureEnable is enabled apps will take care
    // of sending DDS on MMode SUB so no need of triggering DDS from here.
    if (isStackReadyEvent && !isPrimarySubFeatureEnable) {
        sTriggerDds = TRUE;
    }

    for (Int32 slotId = 0; slotId < sNumPhones; slotId++) {
        if ((*simStatus)[slotId] == SUB_SIM_NOT_INSERTED) {
            (*mSubStatus)[slotId] = (*simStatus)[slotId];
            // String str(" Sim not inserted in slot [");
            // str += slotId;
            // str += "] simStatus= ";
            // str += (*simStatus)[slotId];
            // Logd(str);
            continue;
        }
        AutoPtr<ArrayOf<Int64> > subId;
        IISub::Probe(subCtrlr)->GetSubId(slotId, (ArrayOf<Int64>**)&subId);
        Int32 subState = 0;
        IISub::Probe(subCtrlr)->GetSubState((*subId)[0], &subState);
        // String str("setUicc for [");
        // str += slotId;
        // str += "] = ";
        // str += subState;
        // str += "subId = ";
        // str += (*subId)[0];
        // str += " prev subState = ";
        // str += (*mSubStatus)[slotId];
        // str += " stackReady ";
        // str += isStackReadyEvent;
        // Logd(str);
        // Do not send SET_UICC if its already sent with same state
        if (((*mSubStatus)[slotId] != subState) || isStackReadyEvent) {
            // if sim card present in the slot, get the stored sub status and
            // perform the activation/deactivation of subscription
            SetUiccSubscription(slotId, subState);
            setUiccSent = TRUE;
        }
    }
    // if at least one uiccrequest sent, UpdateUserPrefs() will be called
    // from ProcessSetUiccSubscriptionDone()
    if (IsAllSubsAvailable() && (!setUiccSent)) {
        // String str("Received all sim info, update user pref subs, triggerDds= ");
        // str += sTriggerDds;
        // Logd(str);
        subCtrlr->UpdateUserPrefs(sTriggerDds);
        sTriggerDds = FALSE;
    }
    return NOERROR;
}

ECode SubscriptionHelper::UpdateNwMode()
{
    UpdateNwModesInSubIdTable(FALSE);
    AutoPtr<ModemBindingPolicyHandler> mbph = ModemBindingPolicyHandler::GetInstance();
    mbph->UpdatePrefNwTypeIfRequired();
    mNwModeUpdated = TRUE;
    return NOERROR;
}

ECode SubscriptionHelper::SetUiccSubscription(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 subStatus)
{
    AutoPtr<IUiccControllerHelper> uchlp;
    CUiccControllerHelper::AcquireSingleton((IUiccControllerHelper**)&uchlp);
    AutoPtr<IUiccController> uc;
    uchlp->GetInstance((IUiccController**)&uc);
    Boolean set3GPPDone = FALSE, set3GPP2Done = FALSE;
    AutoPtr<IUiccCard> uiccCard;
    uc->GetUiccCard(slotId, (IUiccCard**)&uiccCard);
    if (uiccCard == NULL) {
        // String str("setUiccSubscription: slotId:");
        // str += slotId;
        // str += " card info not available";
        // Logd(str);
        return NOERROR;
    }

    //Activate/Deactivate first 3GPP and 3GPP2 app in the sim, if available
    Int32 apps = 0;
    uiccCard->GetNumApplications(&apps);
    for (Int32 i = 0; i < apps; i++) {
        AutoPtr<IUiccCardApplication> uca;
        uiccCard->GetApplicationIndex(i, (IUiccCardApplication**)&uca);
        AppType type;
        uca->GetType(&type);
        Int32 appType = type;//TODO is this ok? type.Ordinal();
        AutoPtr<IMessageHelper> mhlp;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&mhlp);
        if (set3GPPDone == FALSE && (appType == IPhoneConstants::APPTYPE_USIM ||
                appType == IPhoneConstants::APPTYPE_SIM)) {
            AutoPtr<IMessage> msgSetUiccSubDone;
            mhlp->Obtain(
                    this, EVENT_SET_UICC_SUBSCRIPTION_DONE, slotId, subStatus,
                    (IMessage**)&msgSetUiccSubDone);
            (*mCi)[slotId]->SetUiccSubscription(slotId, i, slotId, subStatus, msgSetUiccSubDone);
            set3GPPDone = TRUE;
        }
        else if (set3GPP2Done == FALSE && (appType == IPhoneConstants::APPTYPE_CSIM ||
                appType == IPhoneConstants::APPTYPE_RUIM)) {
            AutoPtr<IMessage> msgSetUiccSubDone;
            mhlp->Obtain(
                    this, EVENT_SET_UICC_SUBSCRIPTION_DONE, slotId, subStatus,
                    (IMessage**)&msgSetUiccSubDone);
            (*mCi)[slotId]->SetUiccSubscription(slotId, i, slotId, subStatus, msgSetUiccSubDone);
            set3GPP2Done = TRUE;
        }

        if (set3GPPDone && set3GPP2Done) break;
    }
    return NOERROR;
}

void SubscriptionHelper::ProcessSetUiccSubscriptionDone(
    /* [in] */ IMessage* msg)
{
    AutoPtr<ISubscriptionControllerHelper> schlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&schlp);
    AutoPtr<ISubscriptionController> subCtrlr;
    schlp->GetInstance((ISubscriptionController**)&subCtrlr);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject*)obj.Get();
    Int32 slotId = 0;
    msg->GetArg1(&slotId);
    Int32 newSubState = 0;
    msg->GetArg2(&newSubState);
    AutoPtr<ArrayOf<Int64> > subId;
    subCtrlr->GetSubIdUsingSlotId(slotId, (ArrayOf<Int64>**)&subId);

    if (ar->mException != NULL) {
        // String str("Exception in SET_UICC_SUBSCRIPTION, slotId = ");
        // str += slotId;
        // str += " newSubState ";
        // str += newSubState;
        // Loge(str);
        // broadcast set uicc failure
        (*mSubStatus)[slotId] = SUB_SET_UICC_FAIL;
        BroadcastSetUiccResult(slotId, newSubState, IPhoneConstants::FAILURE);
        return;
    }

    Int32 subStatus = 0;
    IISub::Probe(subCtrlr)->GetSubState((*subId)[0], &subStatus);
    if (newSubState != subStatus) {
        Int32 res = 0;
        IISub::Probe(subCtrlr)->SetSubState((*subId)[0], newSubState, &res);
    }
    BroadcastSetUiccResult(slotId, newSubState, IPhoneConstants::SUCCESS);

    (*mSubStatus)[slotId] = newSubState;
    // After activating all subs, updated the user preferred sub values
    if (IsAllSubsAvailable()) {
        // String str("Received all subs, now update user preferred subs, slotid = ");
        // str += slotId;
        // str += " newSubState = ";
        // str += newSubState;
        // str += " sTriggerDds = ";
        // str += sTriggerDds;
        // Logd(str);
        subCtrlr->UpdateUserPrefs(sTriggerDds);
        sTriggerDds = FALSE;
    }
}

void SubscriptionHelper::BroadcastSetUiccResult(
    /* [in] */ Int32 slotId,
    /* [in] */ Int32 newSubState,
    /* [in] */ Int32 result)
{
    AutoPtr<ISubscriptionControllerHelper> schlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&schlp);
    AutoPtr<ISubscriptionController> subCtrlr;
    schlp->GetInstance((ISubscriptionController**)&subCtrlr);
    AutoPtr<ArrayOf<Int64> > subId;
    subCtrlr->GetSubIdUsingSlotId(slotId, (ArrayOf<Int64>**)&subId);
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_SUBSCRIPTION_SET_UICC_RESULT, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    sm->PutPhoneIdAndSubIdExtra(intent, slotId, (*subId)[0]);
    intent->PutExtra(ITelephonyIntents::EXTRA_RESULT, result);
    intent->PutExtra(ITelephonyIntents::EXTRA_NEW_SUB_STATE, newSubState);
    AutoPtr<IUserHandleHelper> uhhlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhhlp);
    AutoPtr<IUserHandle> uh;
    uhhlp->GetALL((IUserHandle**)&uh);
    mContext->SendStickyBroadcastAsUser(intent, uh);
}

Boolean SubscriptionHelper::IsAllSubsAvailable()
{
    Boolean allSubsAvailable = TRUE;

    for (Int32 i = 0; i < sNumPhones; i++) {
        if ((*mSubStatus)[i] == SUB_INIT_STATE) {
            allSubsAvailable = FALSE;
        }
    }
    return allSubsAvailable;
}

ECode SubscriptionHelper::IsRadioOn(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICommandsInterfaceRadioState state;
    (*mCi)[phoneId]->GetRadioState(&state);
    *result = RADIO_ON == state;
    return NOERROR;
}

ECode SubscriptionHelper::IsRadioAvailable(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICommandsInterfaceRadioState state;
    (*mCi)[phoneId]->GetRadioState(&state);
    *result = RADIO_UNAVAILABLE != state;
    return NOERROR;
}

ECode SubscriptionHelper::IsApmSIMNotPwdn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = sApmSIMNotPwdn;
    return NOERROR;
}

ECode SubscriptionHelper::ProceedToHandleIccEvent(
    /* [in] */ Int32 slotId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 apmState = 0;
    sg->GetInt32(cr,
            ISettingsGlobal::AIRPLANE_MODE_ON, 0, &apmState);

    // if SIM powers down in APM, telephony needs to send SET_UICC
    // once radio turns ON also do not handle process SIM change events
    Boolean bRadioOn = FALSE;
    IsRadioOn(slotId, &bRadioOn);
    if ((!sApmSIMNotPwdn) && (!bRadioOn || (apmState == 1))) {
        // String str(" proceedToHandleIccEvent, radio off/unavailable, slotId = ");
        // str += slotId;
        // Logi(str);
        (*mSubStatus)[slotId] = SUB_INIT_STATE;
    }

    // Do not handle if SIM powers down in APM mode
    if ((apmState == 1) && (!sApmSIMNotPwdn)) {
        // String str(" proceedToHandleIccEvent, sApmSIMNotPwdn = ");
        // str += sApmSIMNotPwdn;
        // Logd(str);
        *result = FALSE;
        return NOERROR;
    }

    // Seems SSR happenned or RILD crashed, do not handle SIM change events
    Boolean bAvailable = FALSE;
    IsRadioAvailable(slotId, &bAvailable);
    if (!bAvailable) {
        // String str(" proceedToHandleIccEvent, radio not available, slotId = ");
        // str += slotId;
        // Logi(str);
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

void SubscriptionHelper::Logd(
    /* [in] */ String message)
{
    // Rlog::D(LOGTAG,  message);
}

void SubscriptionHelper::Logi(
    /* [in] */ String msg)
{
    // Rlog::I(LOGTAG,  msg);
}

void SubscriptionHelper::Loge(
    /* [in] */ String msg)
{
    // Rlog::E(LOGTAG,  msg);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
