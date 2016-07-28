
#include "elastos/droid/internal/telephony/ModemBindingPolicyHandler.h"
#include "elastos/droid/internal/telephony/SubscriptionController.h"
#include "elastos/droid/telephony/CTelephonyManager.h"
#include "elastos/droid/os/CMessage.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Telephony::CTelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String ModemBindingPolicyHandler::TAG("ModemBindingPolicyHandler");

//*****Network Mode bit vector mask
//Basic Network Modes
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_GSM_ONLY =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_GSM |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_GPRS |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_EDGE;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_WCDMA_ONLY =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_UMTS |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_HSDPA |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_HSUPA |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_HSPA |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_HSPAP;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_CDMA_NO_EVDO =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_IS95A |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_IS95B |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_EVDO_NO_CDMA =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_0 |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_A |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_EVDO_B |
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_EHRPD;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_LTE_ONLY =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_LTE;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_ONLY =
        1 << IServiceState::RIL_RADIO_TECHNOLOGY_TD_SCDMA;

//Complex modes, i.e. Combinations of above basic Network Modes
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_GSM_UMTS =
        NETWORK_MASK_GSM_ONLY |
        NETWORK_MASK_WCDMA_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_CDMA =
        NETWORK_MASK_CDMA_NO_EVDO |
        NETWORK_MASK_EVDO_NO_CDMA;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_WCDMA_PREF =
        NETWORK_MASK_GSM_UMTS;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_GLOBAL =
        NETWORK_MASK_GSM_UMTS |
        NETWORK_MASK_CDMA;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_LTE_CDMA_EVDO =
        NETWORK_MASK_LTE_ONLY |
        NETWORK_MASK_CDMA;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_LTE_GSM_WCDMA =
        NETWORK_MASK_LTE_ONLY |
        NETWORK_MASK_GSM_UMTS;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA =
        NETWORK_MASK_LTE_ONLY |
        NETWORK_MASK_CDMA |
        NETWORK_MASK_GSM_UMTS;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_LTE_WCDMA =
        NETWORK_MASK_LTE_ONLY |
        NETWORK_MASK_WCDMA_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_WCDMA =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_WCDMA_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_LTE =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_LTE_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_GSM =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_GSM_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_GSM_LTE =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_GSM_ONLY |
        NETWORK_MASK_LTE_ONLY;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_GSM_WCDMA =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_GSM_UMTS;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_WCDMA_LTE =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_LTE_WCDMA;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_GSM_WCDMA_LTE =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_LTE_GSM_WCDMA;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_CDMA_EVDO_GSM_WCDMA =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_CDMA |
        NETWORK_MASK_GSM_UMTS;
const Int32 ModemBindingPolicyHandler::NETWORK_MASK_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA =
        NETWORK_MASK_TD_SCDMA_ONLY |
        NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA;

//***** Events
const Int32 ModemBindingPolicyHandler::EVENT_MODEM_RAT_CAPS_AVAILABLE = 1;
const Int32 ModemBindingPolicyHandler::EVENT_UPDATE_BINDING_DONE = 2;
const Int32 ModemBindingPolicyHandler::EVENT_SET_NW_MODE_DONE = 3;

//*****Constants
const Int32 ModemBindingPolicyHandler::SUCCESS = 1;
const Int32 ModemBindingPolicyHandler::FAILURE = 0;

//***** Class Variables
AutoPtr<ModemBindingPolicyHandler> ModemBindingPolicyHandler::sModemBindingPolicyHandler;
AutoPtr<ModemStackController> ModemBindingPolicyHandler::mModemStackController;

CAR_INTERFACE_IMPL(ModemBindingPolicyHandler, Handler, IModemBindingPolicyHandler)

//***** Class Methods
AutoPtr<ModemBindingPolicyHandler> ModemBindingPolicyHandler::Make(
    /* [in] */ IContext* context,
    /* [in] */ IUiccController* uiccMgr,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
{
    Logger::D(TAG, "getInstance");
    if (sModemBindingPolicyHandler == NULL) {
        sModemBindingPolicyHandler = new ModemBindingPolicyHandler(context, uiccMgr, ci);
    }
    else {
        // throw new RuntimeException("ModemBindingPolicyHandler.make() should be called once");
        assert(0 && "ModemBindingPolicyHandler.make() should be called once");
    }
    return sModemBindingPolicyHandler;
}

AutoPtr<ModemBindingPolicyHandler> ModemBindingPolicyHandler::GetInstance()
{
    if (sModemBindingPolicyHandler == NULL) {
        // throw new RuntimeException("ModemBindingPolicyHdlr.getInstance called before make()");
        assert(0 && "ModemBindingPolicyHdlr.getInstance called before make()");
    }
    return sModemBindingPolicyHandler;
}

//***** Constructor
ModemBindingPolicyHandler::ModemBindingPolicyHandler(
    /* [in] */ IContext* context,
    /* [in] */ IUiccController* uiccManager,
    /* [in] */ ArrayOf<ICommandsInterface*>* ci)
    : mNumOfSetPrefNwModeSuccess(0)
    , mModemRatCapabilitiesAvailable(FALSE)
    , mIsSetPrefNwModeInProgress(FALSE)
{
    Logd(String("Constructor - Enter"));
    AutoPtr<ITelephonyManager> tm;
    CTelephonyManager::GetDefault((ITelephonyManager**)&tm);
    tm->GetPhoneCount(&mNumPhones);

    mPreferredStackId = ArrayOf<Int32>::Alloc(mNumPhones);
    mCurrentStackId = ArrayOf<Int32>::Alloc(mNumPhones);
    mPrefNwMode = ArrayOf<Int32>::Alloc(mNumPhones);
    mNwModeinSubIdTable = ArrayOf<Int32>::Alloc(mNumPhones);

    mCi = ci;
    mContext = context;
    mModemStackController = ModemStackController::GetInstance();
    mModemCapInfo = ArrayOf<ModemStackController::ModemCapabilityInfo*>::Alloc(mNumPhones);

    mModemStackController->RegisterForModemRatCapsAvailable(this, EVENT_MODEM_RAT_CAPS_AVAILABLE, NULL);

    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mPreferredStackId)[i] = i;
        (*mCurrentStackId)[i] = i;
        mStoredResponse[i] = NULL;
    }

    Logd(String("Constructor - Exit"));
}

ECode ModemBindingPolicyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AsyncResult* ar = NULL;

    Int32 what = 0;
    msg->GetWhat(&what);
    switch(what) {
        case EVENT_UPDATE_BINDING_DONE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ar = (AsyncResult*)IAsyncResult::Probe(obj);
            HandleUpdateBindingDone(ar);
            break;
        }

        case EVENT_MODEM_RAT_CAPS_AVAILABLE:
            HandleModemRatCapsAvailable();
            break;

        case EVENT_SET_NW_MODE_DONE:
            HandleSetPreferredNetwork(msg);
            break;

        default:
            break;
    }
    return NOERROR;
}

void ModemBindingPolicyHandler::HandleSetPreferredNetwork(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncResult* ar = (AsyncResult*)IAsyncResult::Probe(obj);
    Int32 index = 0;
    IInteger32::Probe(ar->mUserObj)->GetValue(&index);
    if (ar->mException == NULL) {
        mNumOfSetPrefNwModeSuccess++;
        // set nw mode success for all the subs, then update value to DB
        if (mNumOfSetPrefNwModeSuccess == mNumPhones) {
            Boolean tmp = FALSE;
            for (Int32 i = 0; i < mNumPhones; i++) {
                Logd(String("Updating network mode in DB for slot[") + StringUtils::ToString(i) + "] with "
                        + StringUtils::ToString((*mNwModeinSubIdTable)[i]));

                AutoPtr<IContentResolver> cr;
                mContext->GetContentResolver((IContentResolver**)&cr);
                CTelephonyManager::PutInt32AtIndex(cr,
                        ISettingsGlobal::PREFERRED_NETWORK_MODE,
                        i, (*mNwModeinSubIdTable)[i], &tmp);
            }
            mNumOfSetPrefNwModeSuccess = 0;
        }
    }
    else {
        Logd(String("Failed to set preferred network mode for slot") + StringUtils::ToString(index));
        mNumOfSetPrefNwModeSuccess = 0;
    }
}

void ModemBindingPolicyHandler::HandleUpdateBindingDone(
    /* [in] */ AsyncResult* ar)
{
    mIsSetPrefNwModeInProgress = FALSE;

    HashMap<Int32, AutoPtr<IMessage> >::Iterator ator;
    for (Int32 i = 0; i < mNumPhones; i++) {
        Int32 errorCode = IRILConstants::SUCCESS;
        ator = mStoredResponse.Find(i);
        if (ator != mStoredResponse.End()) {
            AutoPtr<IMessage> resp = ator->mSecond;
            if (resp != NULL) {
                if (ar->mException != NULL) {
                    errorCode = IRILConstants::GENERIC_FAILURE;
                }
                SendResponseToTarget(resp, errorCode);
                mStoredResponse[i] = NULL;
            }
        }
    }
}

/*
* updatePrefNwTypeIfRequired: Method used to set pref network type if required.
*
* Description: If Network mode for a subid in simInfo table is valid and and is not
* equal to value in DB, then update the DB value and send request to RIL.
*/
ECode ModemBindingPolicyHandler::UpdatePrefNwTypeIfRequired()
{
    Boolean updateRequired = FALSE;
    SyncPreferredNwModeFromDB();
    AutoPtr<ISubscriptionController> subCtrlr;
    subCtrlr = SubscriptionController::GetInstance();
    for (Int32 i=0; i < mNumPhones; i++ ) {
        AutoPtr<ArrayOf<Int64> > subIdList;
        IISub::Probe(subCtrlr)->GetSubId(i, (ArrayOf<Int64>**)&subIdList);
        if (subIdList != NULL && (*subIdList)[0] > 0) {
            Int64 subId = (*subIdList)[0];
            subCtrlr->GetNwMode(subId, &(*mNwModeinSubIdTable)[i]);

            if ((*mNwModeinSubIdTable)[i] == ISubscriptionManager::DEFAULT_NW_MODE){
                updateRequired = FALSE;
                break;
            }
            if ((*mNwModeinSubIdTable)[i] != (*mPrefNwMode)[i]) {
                updateRequired = TRUE;
            }
        }
    }

    if (updateRequired) {
        if (FAILURE == UpdateStackBindingIfRequired(FALSE)) {
            //In case of Update Stack Binding not required or failure, send setPrefNwType to
            //RIL immediately. In case of success after stack binding completed setPrefNwType
            //request is anyways sent.
            for (Int32 i=0; i < mNumPhones; i++ ) {
                AutoPtr<IInteger32> o;
                CInteger32::New(i, (IInteger32**)&o);
                AutoPtr<IMessage> msg;
                ObtainMessage(EVENT_SET_NW_MODE_DONE, o, (IMessage**)&msg);
                (*mCi)[i]->SetPreferredNetworkType((*mNwModeinSubIdTable)[i], msg);
            }
        }
   }
   return NOERROR;
}

void ModemBindingPolicyHandler::HandleModemRatCapsAvailable()
{
    mModemRatCapabilitiesAvailable = TRUE;
    //Initialization sequence: Need to send Bind request always, so override is TRUE.
    if (SUCCESS == UpdateStackBindingIfRequired(TRUE)) mIsSetPrefNwModeInProgress = TRUE;
}

void ModemBindingPolicyHandler::SyncCurrentStackInfo()
{
    //Get Current phoneId to stack mapping.
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*mCurrentStackId)[i] = mModemStackController->GetCurrentStackIdForPhoneId(i);
        mModemCapInfo->Set((*mCurrentStackId)[i], mModemStackController->GetModemRatCapsForPhoneId(i));
        //reset Preferred to current.
        (*mPreferredStackId)[i] = ((*mCurrentStackId)[i] >= 0) ? (*mCurrentStackId)[i] : i;
    }
}

/*
* UpdateStackBindingIfRequired: Method used to update stack binding if required.
* @param: Boolean override - if TRUE send update stack binding request always
* @return: Int32 SUCCESS or FAILURE. i.e. Request acepted or not.
*   SUCCESS, if update stack binding request is sent and it returns SUCCESS
*   FAILURE, if update stack binding request is not sent or
*       if update stack binding request is sent and it returns FAILURE.
*/
Int32 ModemBindingPolicyHandler::UpdateStackBindingIfRequired(
    /* [in] */ Boolean isBootUp)
{
    Boolean isUpdateStackBindingRequired = FALSE;
    Int32 response = FAILURE;

    UpdatePreferredStackIds();

    for (Int32 i = 0; i < mNumPhones; i++) {
        if ((*mPreferredStackId)[i] != (*mCurrentStackId)[i]) {
            //if preferred stackId is different from current, bindupdate is required.
            isUpdateStackBindingRequired = TRUE;
            break;
        }
    }
    if (isBootUp || isUpdateStackBindingRequired) {
        AutoPtr<IMessage> msg = CMessage::Obtain(this, EVENT_UPDATE_BINDING_DONE, NULL);
        response = mModemStackController->UpdateStackBinding(mPreferredStackId, isBootUp, msg);
    }

    return response;
}

/*
* updatePreferredStackIds: Method used to get isCrossBindingRequired and set the Preferred
* StackIds as per Network Mode for phoneId.
* @return: Boolean TRUE/FALSE based on crossMapping is required.
*
* Description: used to determine the preferred stackIds based on Network Mode selected
* and return if CrossBindingRequired.
* Logic:
* 1. For each phoneId, check if current stack supports the present Network Mode.
* 2. If supported, continue to next phoneId. If stacks on all phoneId support their respective
*     Network Mode then crossmapping is not required so preferredStackIds will be same as
*     CurrentStackIds
* 3. If stack of any of the phoneId is not supporting their Network Mode, then check if stack of
*     other phoneIds supports present NetworkMode.
*    a. If none of the stacks support the present Network Mode, cross mapping is not required
*       and preferredStackIds will be same as CurrentStackIds.
*    b. If some other stack supports, check if network mode on other phoneId is supported by
*       stack after cross mapping.
*           i. if supported cross mapping is required and set preferredStackIds accordingly.
*           ii. if not supported cross mapping is not required and preferredStackIds will be
*              same as CurrentStackIds.
*/
void ModemBindingPolicyHandler::UpdatePreferredStackIds()
{
    if (!mModemRatCapabilitiesAvailable) {
        Loge(String("updatePreferredStackIds: Modem Capabilites are not Available. Return!!"));
        return;
    }

    //Get current prefNwMode and Stack info before updating pref Stack
    SyncPreferredNwModeFromDB();
    SyncCurrentStackInfo();

    for (Int32 curPhoneId = 0; curPhoneId < mNumPhones; curPhoneId++) {
        //Continue if current stack supports Network Mode.
        if (IsNwModeSupportedOnStack((*mPrefNwMode)[curPhoneId], (*mCurrentStackId)[curPhoneId])) {
            Logd(String("updatePreferredStackIds: current stack[")
                    + StringUtils::ToString((*mCurrentStackId)[curPhoneId]) +
                    "]supports NwMode[" + StringUtils::ToString((*mPrefNwMode)[curPhoneId])
                    + "] on phoneId[" + StringUtils::ToString(curPhoneId) + "]");
            continue;
        }

        //Current stack i do not support present Network Mode.
        //Check if any other stack supports the present Network Mode
        for (Int32 otherPhoneId = 0; otherPhoneId < mNumPhones; otherPhoneId++) {
            //continue for same stack, check only on other stacks
            if (otherPhoneId == curPhoneId) continue;

            if (IsNwModeSupportedOnStack((*mPrefNwMode)[curPhoneId],
                    (*mCurrentStackId)[otherPhoneId])) {
                //Some other stack supports present Network Mode, Check if Network Mode
                //on other PhoneId is supported on current Stack after Cross binding.
                if (IsNwModeSupportedOnStack((*mPrefNwMode)[otherPhoneId],
                        (*mCurrentStackId)[curPhoneId])) {
                    Logd(String("updatePreferredStackIds: Cross Binding is possible between phoneId[")
                            + StringUtils::ToString(curPhoneId) + "] and phoneId["
                            + StringUtils::ToString(otherPhoneId) + "]");
                    //set preferred stackid of curPhoneId as current stack id of otherPhoneId
                    //and preferred stackid of otherPhoneId as current stack id of curPhoneId
                    (*mPreferredStackId)[curPhoneId] = (*mCurrentStackId)[otherPhoneId];
                    (*mPreferredStackId)[otherPhoneId] = (*mCurrentStackId)[curPhoneId];
                }
            }
        }
    }
}

Boolean ModemBindingPolicyHandler::IsNwModeSupportedOnStack(
    /* [in] */ Int32 nwMode,
    /* [in] */ Int32 stackId)
{
    AutoPtr<ArrayOf<Int32> > numRatSupported = ArrayOf<Int32>::Alloc(mNumPhones);
    Int32 maxNumRatSupported = 0;
    Boolean isSupported = FALSE;

    //Get num of RATs supported for this NwMode on all Stacks
    for (Int32 i = 0; i < mNumPhones; i++) {
        (*numRatSupported)[i] = GetNumOfRatSupportedForNwMode(nwMode, (*mModemCapInfo)[i]);
        if (maxNumRatSupported < (*numRatSupported)[i]) maxNumRatSupported = (*numRatSupported)[i];
    }

    //if current stackId supports Max RATs of all other stacks, then return TRUE.
    if ((*numRatSupported)[stackId] == maxNumRatSupported) isSupported = TRUE;

    Logd(String("nwMode:") + StringUtils::ToString(nwMode)
            + ", on stack:" + StringUtils::ToString(stackId) + " is " +
            (isSupported ? "Supported" : "Not Supported"));

    return isSupported;
}

void ModemBindingPolicyHandler::SyncPreferredNwModeFromDB()
{
    for (Int32 i = 0; i < mNumPhones; i++) {
        // try {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        if (FAILED(CTelephonyManager::GetInt32AtIndex(cr,
                ISettingsGlobal::PREFERRED_NETWORK_MODE, i, &(*mPrefNwMode)[i]))) {
            Loge(String("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!"));
            (*mPrefNwMode)[i] = IPhone::PREFERRED_NT_MODE;
        }
         // } catch (SettingNotFoundException snfe) {
         //    Loge("getPreferredNetworkMode: Could not find PREFERRED_NETWORK_MODE!!!");
         //    mPrefNwMode[i] = Phone.PREFERRED_NT_MODE;
         // }
    }
}

ECode ModemBindingPolicyHandler::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ Int32 phoneId,
    /* [in] */ IMessage* response)
{
    //if binding is in progress return failure for this request
    if (mIsSetPrefNwModeInProgress) {
        Loge(String("setPreferredNetworkType: In Progress:"));
        SendResponseToTarget(response, IRILConstants::GENERIC_FAILURE);
        return NOERROR;
    }

    Logd(String("setPreferredNetworkType: nwMode:")
            + StringUtils::ToString(networkType) + ", on phoneId:"
            + StringUtils::ToString(phoneId));

    mIsSetPrefNwModeInProgress = TRUE;

    //If CrossBinding request is not accepted, i.e. return value is FAILURE
    //send request directly to RIL, or else store the setpref Msg for later processing.
    if (UpdateStackBindingIfRequired(FALSE) == SUCCESS) {
        mStoredResponse[phoneId] = response;
    }
    else {
        (*mCi)[phoneId]->SetPreferredNetworkType(networkType, response);
        mIsSetPrefNwModeInProgress = FALSE;
    }
    return NOERROR;
}

void ModemBindingPolicyHandler::SendResponseToTarget(
    /* [in] */ IMessage* response,
    /* [in] */ Int32 responseCode)
{
    if (response != NULL) {
        assert(0 && "TODO");
        // Exception e = CommandException.fromRilErrno(responseCode);
        // AsyncResult.forMessage(response, NULL, e);
        response->SendToTarget();
    }
}

Int32 ModemBindingPolicyHandler::GetNumOfRatSupportedForNwMode(
    /* [in] */ Int32 nwMode,
    /* [in] */ ModemStackController::ModemCapabilityInfo* modemCaps)
{
    Int32 supportedRatMaskForNwMode = 0;
    if (modemCaps == NULL){
        Loge(String("GetNumOfRatSupportedForNwMode: nwMode[")
                + StringUtils::ToString(nwMode) + "] modemCaps was NULL!");
        return supportedRatMaskForNwMode;
    }
    Logd(String("getNumOfRATsSupportedForNwMode: nwMode[")
            + StringUtils::ToString(nwMode)
            +"] modemCaps = " + TO_CSTR(modemCaps));

    //send result by ANDing corresponding NETWORK MASK and Modem Caps mask.
    switch (nwMode) {
        case IRILConstants::NETWORK_MODE_WCDMA_PREF:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_WCDMA_PREF;
            break;

        case IRILConstants::NETWORK_MODE_GSM_ONLY:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_GSM_ONLY;
            break;

        case IRILConstants::NETWORK_MODE_WCDMA_ONLY:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_WCDMA_ONLY;
            break;

        case IRILConstants::NETWORK_MODE_GSM_UMTS:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_GSM_UMTS;
            break;

        case IRILConstants::NETWORK_MODE_CDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_CDMA;
            break;

        case IRILConstants::NETWORK_MODE_CDMA_NO_EVDO:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_CDMA_NO_EVDO;
            break;

        case IRILConstants::NETWORK_MODE_EVDO_NO_CDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_EVDO_NO_CDMA;
            break;

        case IRILConstants::NETWORK_MODE_GLOBAL:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_GLOBAL;
            break;

        case IRILConstants::NETWORK_MODE_LTE_CDMA_EVDO:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_LTE_CDMA_EVDO;
            break;

        case IRILConstants::NETWORK_MODE_LTE_GSM_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_LTE_GSM_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_LTE_CDMA_EVDO_GSM_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_LTE_CMDA_EVDO_GSM_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_LTE_ONLY:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_LTE_ONLY;
            break;

        case IRILConstants::NETWORK_MODE_LTE_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_LTE_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_ONLY:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_ONLY;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_LTE:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_LTE;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_GSM:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_GSM;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_GSM_LTE:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_GSM_LTE;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_GSM_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_GSM_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_WCDMA_LTE:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_WCDMA_LTE;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_GSM_WCDMA_LTE:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_GSM_WCDMA_LTE;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_CDMA_EVDO_GSM_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_CDMA_EVDO_GSM_WCDMA;
            break;

        case IRILConstants::NETWORK_MODE_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA:
            supportedRatMaskForNwMode = modemCaps->GetSupportedRatBitMask() &
                    NETWORK_MASK_TD_SCDMA_LTE_CDMA_EVDO_GSM_WCDMA;
            break;

        default:
            break;
    }

    Logd(String("getNumOfRATsSupportedForNwMode: supportedRatMaskForNwMode:") +
            StringUtils::ToString(supportedRatMaskForNwMode));

    return GetNumRatSupportedInMask(supportedRatMaskForNwMode);
}

//This method will return no. of 1's in binary representation of Int32.
Int32 ModemBindingPolicyHandler::GetNumRatSupportedInMask(
    /* [in] */ Int32 mask)
{
    Int32 noOfOnes = 0;
    /*
     * To count num of 1's in bitmask.
     * The bitwise and of a number and number - 1, will result in zeroing the least
     *  significant non-zero bit in the number. So if a number has n bits that were set to 1,
     *  then after n iterations of above operation, number will be changed to zero.
     */
    while (mask != 0) {
        mask &= mask - 1;
        noOfOnes++;
    }
    return noOfOnes;
}

void ModemBindingPolicyHandler::Logd(
    /* [in] */ const String& string)
{
    Logger::D(TAG, string);
}

void ModemBindingPolicyHandler::Loge(
    /* [in] */ const String& string)
{
    Logger::E(TAG, string);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
