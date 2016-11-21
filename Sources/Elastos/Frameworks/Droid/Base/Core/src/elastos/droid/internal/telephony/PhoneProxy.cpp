
#include "elastos/droid/internal/telephony/PhoneProxy.h"
#include "elastos/droid/internal/telephony/cdma/CDMALTEPhone.h"
#include "elastos/droid/internal/telephony/CCallManagerHelper.h"
#include "elastos/droid/internal/telephony/CPhoneFactory.h"
#include "elastos/droid/internal/telephony/CPhoneSubInfoProxy.h"
#include "elastos/droid/internal/telephony/IccPhoneBookInterfaceManagerProxy.h"
#include "elastos/droid/internal/telephony/IccSmsInterfaceManager.h"
#include "elastos/droid/internal/telephony/PhoneBase.h"
#include "elastos/droid/internal/telephony/uicc/CIccCardProxy.h"
#include "elastos/droid/telephony/CSubscriptionManager.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/os/AsyncResult.h"
#include "elastos/droid/telephony/CServiceStateHelper.h"
#include "elastos/droid/R.h"
#include "elastos/core/StringUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Telephony::IServiceStateHelper;
using Elastos::Droid::Telephony::CServiceStateHelper;
using Elastos::Droid::Telephony::CSubscriptionManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Internal::Telephony::IccPhoneBookInterfaceManagerProxy;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMAPhone;
using Elastos::Droid::Internal::Telephony::Gsm::IGSMPhone;
using Elastos::Droid::Internal::Telephony::Cdma::CDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Cdma::ICDMALTEPhone;
using Elastos::Droid::Internal::Telephony::Uicc::CIccCardProxy;
using Elastos::Droid::R;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::IArrayOf;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

Object PhoneProxy::lockForRadioTechnologyChange;

const Int32 PhoneProxy::EVENT_VOICE_RADIO_TECH_CHANGED = 1;
const Int32 PhoneProxy::EVENT_RADIO_ON = 2;
const Int32 PhoneProxy::EVENT_REQUEST_VOICE_RADIO_TECH_DONE = 3;
const Int32 PhoneProxy::EVENT_RIL_CONNECTED = 4;
const Int32 PhoneProxy::EVENT_UPDATE_PHONE_OBJECT = 5;
const Int32 PhoneProxy::EVENT_SIM_RECORDS_LOADED = 6;

const String PhoneProxy::LOGTAG("PhoneProxy");

CAR_INTERFACE_IMPL_2(PhoneProxy, Handler, IPhone, IPhoneProxy)

PhoneProxy::PhoneProxy()
    : mResetModemOnRadioTechnologyChange(FALSE)
    , mRilVersion(0)
    , mPhoneId(0)
{
    Handler::constructor();
}

ECode PhoneProxy::constructor(
    /* [in] */ IPhoneBase* phone)
{
    mActivePhone = IPhone::Probe(phone);
    AutoPtr<ISystemProperties> sysprop;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysprop);
    sysprop->GetBoolean(
            ITelephonyProperties::PROPERTY_RESET_ON_RADIO_TECH_CHANGE,
            FALSE,
            &mResetModemOnRadioTechnologyChange);
    AutoPtr<IIccPhoneBookInterfaceManager> interfaceManager;
    IPhone::Probe(phone)->GetIccPhoneBookInterfaceManager(
                            (IIccPhoneBookInterfaceManager**)&interfaceManager);
    mIccPhoneBookInterfaceManagerProxy = new IccPhoneBookInterfaceManagerProxy(
                                                            interfaceManager);
    AutoPtr<IPhoneSubInfo> phoneInfo;
    IPhone::Probe(phone)->GetPhoneSubInfo((IPhoneSubInfo**)&phoneInfo);
    CPhoneSubInfoProxy::New(phoneInfo, (IPhoneSubInfoProxy**)&mPhoneSubInfoProxy);
    mCommandsInterface = ((PhoneBase*)IPhoneBase::Probe(mActivePhone))->mCi;

    mCommandsInterface->RegisterForRilConnected(this, EVENT_RIL_CONNECTED, NULL);
    mCommandsInterface->RegisterForOn(this, EVENT_RADIO_ON, NULL);
    mCommandsInterface->RegisterForVoiceRadioTechChanged(
                        this, EVENT_VOICE_RADIO_TECH_CHANGED, NULL);
    phone->GetPhoneId(&mPhoneId);
    mIccSmsInterfaceManager =
             new IccSmsInterfaceManager(IPhoneBase::Probe(this->mActivePhone));
    Int32 activePhoneId = 0;
    mActivePhone->GetPhoneId(&activePhoneId);
    AutoPtr<IContext> cxt;
    mActivePhone->GetContext((IContext**)&cxt);
    CIccCardProxy::New(cxt, mCommandsInterface, activePhoneId, (IIccCardProxy**)&mIccCardProxy);

    Int32 type = 0;
    IPhone::Probe(phone)->GetPhoneType(&type);
    if (type == IPhoneConstants::PHONE_TYPE_GSM) {
        // For the purpose of IccCardProxy we only care about the technology family
        mIccCardProxy->SetVoiceRadioTech(IServiceState::RIL_RADIO_TECHNOLOGY_UMTS);
    }
    else if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
        mIccCardProxy->SetVoiceRadioTech(IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT);
    }
    return NOERROR;
}

ECode PhoneProxy::HandleMessage(
    /* [in] */ IMessage* msg)
{
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 iwhat = 0;
    msg->GetWhat(&iwhat);
    AutoPtr<AsyncResult> ar = (AsyncResult*)(IObject::Probe(obj));
    switch(iwhat) {
    case EVENT_RADIO_ON: {
        /* Proactively query voice radio technologies */
        AutoPtr<IMessage> msg;
        ObtainMessage(EVENT_REQUEST_VOICE_RADIO_TECH_DONE, (IMessage**)&msg);
        mCommandsInterface->GetVoiceRadioTechnology(msg);
        break;
    }
    case EVENT_RIL_CONNECTED: {
        if (ar->mException == NULL && ar->mResult != NULL) {
            IInteger32::Probe(ar->mResult)->GetValue(&mRilVersion);
        }
        else {
            Logd(String("Unexpected exception on EVENT_RIL_CONNECTED"));
            mRilVersion = -1;
        }
        break;
    }
    case EVENT_VOICE_RADIO_TECH_CHANGED:
    case EVENT_REQUEST_VOICE_RADIO_TECH_DONE: {
        String what = (iwhat == EVENT_VOICE_RADIO_TECH_CHANGED) ?
                String("EVENT_VOICE_RADIO_TECH_CHANGED") : String("EVENT_REQUEST_VOICE_RADIO_TECH_DONE");
        if (ar->mException == NULL) {
            if (ar->mResult != NULL) {
                AutoPtr<IArrayOf> array = IArrayOf::Probe(ar->mResult);
                Int32 size = 0;
                if (array != NULL)
                    array->GetLength(&size);
                if (size > 0) {
                    AutoPtr<IInterface> ele;
                    array->Get(0, (IInterface**)&ele);
                    Int32 value;
                    IInteger32::Probe(ele)->GetValue(&value);
                    PhoneObjectUpdater(value);
                }
                else {
                    Logger::E("PhoneProxy", "%s: has no tech!", what.string());
                }
            }
            else {
                Logger::E("PhoneProxy", "%s: has no tech!", what.string());
            }
        }
        else {
            String str;
            str.AppendFormat("%s: exception=%p", (const char*)what, (ar->mException).Get());
            Loge(str);
        }
        break;
    }
    case EVENT_UPDATE_PHONE_OBJECT: {
        Int32 arg1 = 0;
        msg->GetArg1(&arg1);
        PhoneObjectUpdater(arg1);
        break;
    }
    case EVENT_SIM_RECORDS_LOADED: {
        // Only check for the voice radio tech if it not going to be updated by the voice
        // registration changes.
        AutoPtr<IContext> cxt;
        mActivePhone->GetContext((IContext**)&cxt);
        AutoPtr<IResources> res;
        cxt->GetResources((IResources**)&res);
        Boolean bChange = FALSE;
        res->GetBoolean(
                R::bool_::config_switch_phone_on_voice_reg_state_change, &bChange);
        if (!bChange) {
            AutoPtr<IMessage> msg;
            ObtainMessage(
                    EVENT_REQUEST_VOICE_RADIO_TECH_DONE, (IMessage**)&msg);
            mCommandsInterface->GetVoiceRadioTechnology(msg);
        }
        break;
    }
    default: {
        // String str("Error! This handler was not registered for this message type. Message: ");
        // str += iwhat;
        // Loge(str);
        break;
    }
    }
    Handler::HandleMessage(msg);
    return NOERROR;
}

void PhoneProxy::Logd(
    /* [in] */ String msg)
{
    // Rlog::D(LOG_TAG, "[PhoneProxy] %s", msg);
    Logger::D("PhoneProxy", "TODO [PhoneProxy] %s", msg.string());
}

void PhoneProxy::Loge(
    /* [in] */ String msg)
{
    // Rlog::E(LOG_TAG, "[PhoneProxy] %s", msg);
    Logger::E("PhoneProxy", "TODO [PhoneProxy] %s", msg.string());
}

void PhoneProxy::PhoneObjectUpdater(
    /* [in] */ Int32 newVoiceRadioTech)
{
    // String str("phoneObjectUpdater: newVoiceRadioTech=");
    // str += newVoiceRadioTech;
    // Logd(str);

    if (mActivePhone != NULL) {
        // Check for a voice over lte replacement
        if ((newVoiceRadioTech == IServiceState::RIL_RADIO_TECHNOLOGY_LTE) ||
                (newVoiceRadioTech == IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN)) {
            AutoPtr<IContext> cxt;
            mActivePhone->GetContext((IContext**)&cxt);
            AutoPtr<IResources> res;
            cxt->GetResources((IResources**)&res);
            Int32 volteReplacementRat = 0;
            res->GetInteger(
                    R::integer::config_volte_replacement_rat, &volteReplacementRat);
            // String str("phoneObjectUpdater: volteReplacementRat=");
            // str += volteReplacementRat;
            // Logd(str);
            if (volteReplacementRat != IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN) {
                newVoiceRadioTech = volteReplacementRat;
            }
        }

        Int32 mode = 0;
        GetLteOnCdmaMode(&mode);
        if(mRilVersion == 6 && mode == IPhoneConstants::LTE_ON_CDMA_TRUE) {
            String phoneName;
            mActivePhone->GetPhoneName(&phoneName);
            Int32 type = 0;
            mActivePhone->GetPhoneType(&type);
            /*
             * On v6 RIL, when LTE_ON_CDMA is TRUE, always create CDMALTEPhone
             * irrespective of the voice radio tech reported.
             */
            if (type == IPhoneConstants::PHONE_TYPE_CDMA) {
                String str("phoneObjectUpdater: LTE ON CDMA property is set. Use CDMA Phone newVoiceRadioTech=");
                str.AppendFormat("%d mActivePhone=%s", newVoiceRadioTech, (const char*)phoneName);
                Logd(str);
                return;
            }
            else {
                String str("phoneObjectUpdater: LTE ON CDMA property is set. Switch to CDMALTEPhone newVoiceRadioTech=");
                str.AppendFormat("%d mActivePhone=%s", newVoiceRadioTech, (const char*)phoneName);
                Logd(str);
                newVoiceRadioTech = IServiceState::RIL_RADIO_TECHNOLOGY_1xRTT;
            }
        }
        else {
            AutoPtr<IServiceStateHelper> sshlp;
            CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&sshlp);
            Boolean bCdma = FALSE, bGsm = FALSE;
            sshlp->IsCdma(newVoiceRadioTech, &bCdma);
            sshlp->IsGsm(newVoiceRadioTech, &bGsm);
            String phoneName;
            mActivePhone->GetPhoneName(&phoneName);
            Int32 type = 0;
            mActivePhone->GetPhoneType(&type);
            if ((bCdma &&
                    type == IPhoneConstants::PHONE_TYPE_CDMA) ||
                    (bGsm &&
                        type == IPhoneConstants::PHONE_TYPE_GSM)) {
                // Nothing changed. Keep phone as it is.
                String str("phoneObjectUpdater: No change ignore, newVoiceRadioTech=");
                str.AppendFormat("%d mActivePhone=%s", newVoiceRadioTech, (const char*)phoneName);
                Logd(str);
                return;
            }
        }
    }

    if (newVoiceRadioTech == IServiceState::RIL_RADIO_TECHNOLOGY_UNKNOWN) {
        String phoneName;
        mActivePhone->GetPhoneName(&phoneName);
        // We need some voice phone object to be active always, so never
        // delete the phone without anything to replace it with!
        String str("phoneObjectUpdater: Unknown rat ignore, newVoiceRadioTech=Unknown. mActivePhone=");
        str += phoneName;
        Logd(str);
        return;
    }

    Boolean oldPowerState = FALSE; // old power state to off
    if (mResetModemOnRadioTechnologyChange) {
        ICommandsInterfaceRadioState state;
        mCommandsInterface->GetRadioState(&state);
        if (state == RADIO_ON) {
            oldPowerState = TRUE;
            Logd(String("phoneObjectUpdater: Setting Radio Power to Off"));
            mCommandsInterface->SetRadioPower(FALSE, NULL);
        }
    }

    DeleteAndCreatePhone(newVoiceRadioTech);

    if (mResetModemOnRadioTechnologyChange && oldPowerState) { // restore power state
        Logd(String("phoneObjectUpdater: Resetting Radio"));
        mCommandsInterface->SetRadioPower(oldPowerState, NULL);
    }

    // Set the new interfaces in the proxy's
    // mIccSmsInterfaceManager->UpdatePhoneObject(IPhoneBase::Probe(mActivePhone));
    AutoPtr<IIccPhoneBookInterfaceManager> pbim;
    mActivePhone->GetIccPhoneBookInterfaceManager((IIccPhoneBookInterfaceManager**)&pbim);
    mIccPhoneBookInterfaceManagerProxy->SetmIccPhoneBookInterfaceManager(pbim);
    AutoPtr<IPhoneSubInfo> subinfo;
    mActivePhone->GetPhoneSubInfo((IPhoneSubInfo**)&subinfo);
    mPhoneSubInfoProxy->SetmPhoneSubInfo(subinfo);

    mCommandsInterface = ((PhoneBase*)IPhoneBase::Probe(mActivePhone))->mCi;
    mIccCardProxy->SetVoiceRadioTech(newVoiceRadioTech);

    // Send an Intent to the PhoneApp that we had a radio technology change
    AutoPtr<IIntent> intent;
    CIntent::New(ITelephonyIntents::ACTION_RADIO_TECHNOLOGY_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
    String phoneName;
    mActivePhone->GetPhoneName(&phoneName);
    intent->PutExtra(IPhoneConstants::PHONE_NAME_KEY, phoneName);
    AutoPtr<ISubscriptionManager> sm;
    CSubscriptionManager::AcquireSingleton((ISubscriptionManager**)&sm);
    sm->PutPhoneIdAndSubIdExtra(intent, mPhoneId);
    ActivityManagerNative::BroadcastStickyIntent(intent, String(NULL), IUserHandle::USER_ALL);
}

void PhoneProxy::DeleteAndCreatePhone(
    /* [in] */ Int32 newVoiceRadioTech)
{
    String outgoingPhoneName("Unknown");
    AutoPtr<IPhone> oldPhone = mActivePhone;
    AutoPtr<IImsPhone> imsPhone;

    if (oldPhone != NULL) {
        oldPhone->GetPhoneName(&outgoingPhoneName);
        oldPhone->UnregisterForSimRecordsLoaded(this);
    }

    AutoPtr<IServiceStateHelper> sshlp;
    CServiceStateHelper::AcquireSingleton((IServiceStateHelper**)&sshlp);
    Boolean bCdma = FALSE, bGsm = FALSE;
    sshlp->IsCdma(newVoiceRadioTech, &bCdma);
    sshlp->IsGsm(newVoiceRadioTech, &bGsm);

    String str("Switching Voice Phone : ");
    str += outgoingPhoneName;
    str += " >>> ";
    str += bGsm ? "GSM" : "CDMA";
    Logd(str);

    AutoPtr<IPhoneFactory> pf;
    CPhoneFactory::AcquireSingleton((IPhoneFactory**)&pf);
    if (bCdma) {
        pf->GetCdmaPhone(mPhoneId, (IPhone**)&mActivePhone);
    }
    else if (bGsm) {
        pf->GetGsmPhone(mPhoneId, (IPhone**)&mActivePhone);
    }

    if (oldPhone != NULL) {
        oldPhone->RelinquishOwnershipOfImsPhone((IImsPhone**)&imsPhone);
    }

    AutoPtr<ICallManagerHelper> cmhlp;
    CCallManagerHelper::AcquireSingleton((ICallManagerHelper**)&cmhlp);
    if (mActivePhone != NULL) {
        AutoPtr<ICallManager> cm;
        cmhlp->GetInstance((ICallManager**)&cm);
        Boolean b = FALSE;
        cm->RegisterPhone(mActivePhone, &b);
        if (imsPhone != NULL) {
            mActivePhone->AcquireOwnershipOfImsPhone(imsPhone);
        }
        mActivePhone->RegisterForSimRecordsLoaded(this, EVENT_SIM_RECORDS_LOADED, NULL);
    }

    if (oldPhone != NULL) {
        AutoPtr<ICallManager> cm;
        cmhlp->GetInstance((ICallManager**)&cm);
        cm->UnregisterPhone(oldPhone);
        Logd(String("Disposing old phone.."));
        oldPhone->Dispose();
        // Potential GC issues: however, callers may have references to old
        // phone on which they perform hierarchical funcs: phone->GetA()->GetB()
        // HENCE: do not delete references.
        oldPhone->RemoveReferences();
    }
    oldPhone = NULL;
}

ECode PhoneProxy::GetIccSmsInterfaceManager(
    /* [out] */ IIccSmsInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIccSmsInterfaceManager;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneProxy::GetPhoneSubInfoProxy(
    /* [out] */ IPhoneSubInfoProxy** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneSubInfoProxy;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneProxy::GetIccPhoneBookInterfaceManagerProxy(
    /* [out] */ IIccPhoneBookInterfaceManagerProxy** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIccPhoneBookInterfaceManagerProxy;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneProxy::GetIccFileHandler(
    /* [out] */ IIccFileHandler** result)
{
    VALIDATE_NOT_NULL(result)
    return IPhoneBase::Probe(mActivePhone)->GetIccFileHandler(result);
}

ECode PhoneProxy::IsImsVtCallPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsImsVtCallPresent(result);
}

ECode PhoneProxy::UpdatePhoneObject(
    /* [in] */ Int32 voiceRadioTech)
{
    // String str("updatePhoneObject: radioTechnology=");
    // str += voiceRadioTech;
    // Logd(str);
    AutoPtr<IMessage> msg;
    ObtainMessage(EVENT_UPDATE_PHONE_OBJECT, voiceRadioTech, 0, NULL, (IMessage**)&msg);
    Boolean bSend = FALSE;
    SendMessage(msg, &bSend);
    return NOERROR;
}

ECode PhoneProxy::GetServiceState(
    /* [out] */ IServiceState** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetServiceState(result);
}

ECode PhoneProxy::GetCellLocation(
    /* [out] */ ICellLocation** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCellLocation(result);
}

ECode PhoneProxy::GetAllCellInfo(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetAllCellInfo(result);
}

ECode PhoneProxy::SetCellInfoListRate(
    /* [in] */ Int32 rateInMillis)
{
    return mActivePhone->SetCellInfoListRate(rateInMillis);
}

ECode PhoneProxy::GetDataConnectionState(
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDataConnectionState(IPhoneConstants::APN_TYPE_DEFAULT, result);
}

ECode PhoneProxy::GetDataConnectionState(
    /* [in] */ const String& apnType,
    /* [out] */ PhoneConstantsDataState* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDataConnectionState(apnType, result);
}

ECode PhoneProxy::GetDataActivityState(
    /* [out] */ IPhoneDataActivityState* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDataActivityState(result);
}

ECode PhoneProxy::GetContext(
    /* [out] */ IContext** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetContext(result);
}

ECode PhoneProxy::DisableDnsCheck(
    /* [in] */ Boolean b)
{
    return mActivePhone->DisableDnsCheck(b);
}

ECode PhoneProxy::IsDnsCheckDisabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsDnsCheckDisabled(result);
}

ECode PhoneProxy::GetState(
    /* [out] */ PhoneConstantsState* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetState(result);
}

ECode PhoneProxy::GetPhoneName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPhoneName(result);
}

ECode PhoneProxy::GetPhoneType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPhoneType(result);
}

ECode PhoneProxy::GetActiveApnTypes(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetActiveApnTypes(result);
}

ECode PhoneProxy::GetActiveApnHost(
    /* [in] */ const String& apnType,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetActiveApnHost(apnType, result);
}

ECode PhoneProxy::GetLinkProperties(
    /* [in] */ const String& apnType,
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetLinkProperties(apnType, result);
}

ECode PhoneProxy::GetNetworkCapabilities(
    /* [in] */ const String& apnType,
    /* [out] */ INetworkCapabilities** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetNetworkCapabilities(apnType, result);
}

ECode PhoneProxy::GetSignalStrength(
    /* [out] */ ISignalStrength** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetSignalStrength(result);
}

ECode PhoneProxy::RegisterForUnknownConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForUnknownConnection(h, what, obj);
}

ECode PhoneProxy::UnregisterForUnknownConnection(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForUnknownConnection(h);
}

ECode PhoneProxy::RegisterForHandoverStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForHandoverStateChanged(h, what, obj);
}

ECode PhoneProxy::UnregisterForHandoverStateChanged(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForHandoverStateChanged(h);
}

ECode PhoneProxy::RegisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForPreciseCallStateChanged(h, what, obj);
}

ECode PhoneProxy::UnregisterForPreciseCallStateChanged(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForPreciseCallStateChanged(h);
}

ECode PhoneProxy::RegisterForNewRingingConnection(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForNewRingingConnection(h, what, obj);
}

ECode PhoneProxy::UnregisterForNewRingingConnection(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForNewRingingConnection(h);
}

ECode PhoneProxy::RegisterForVideoCapabilityChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForVideoCapabilityChanged(h, what, obj);
}

ECode PhoneProxy::UnregisterForVideoCapabilityChanged(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForVideoCapabilityChanged(h);
}

ECode PhoneProxy::RegisterForIncomingRing(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForIncomingRing(h, what, obj);
}

ECode PhoneProxy::UnregisterForIncomingRing(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForIncomingRing(h);
}

ECode PhoneProxy::RegisterForDisconnect(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForDisconnect(h, what, obj);
}

ECode PhoneProxy::UnregisterForDisconnect(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForDisconnect(h);
}

ECode PhoneProxy::RegisterForMmiInitiate(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForMmiInitiate(h, what, obj);
}

ECode PhoneProxy::UnregisterForMmiInitiate(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForMmiInitiate(h);
}

ECode PhoneProxy::RegisterForMmiComplete(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForMmiComplete(h, what, obj);
}

ECode PhoneProxy::UnregisterForMmiComplete(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForMmiComplete(h);
}

ECode PhoneProxy::GetPendingMmiCodes(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPendingMmiCodes(result);
}

ECode PhoneProxy::SendUssdResponse(
    /* [in] */ const String& ussdMessge)
{
    return mActivePhone->SendUssdResponse(ussdMessge);
}

ECode PhoneProxy::RegisterForServiceStateChanged(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForServiceStateChanged(h, what, obj);
}

ECode PhoneProxy::UnregisterForServiceStateChanged(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForServiceStateChanged(h);
}

ECode PhoneProxy::RegisterForSuppServiceNotification(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForSuppServiceNotification(h, what, obj);
}

ECode PhoneProxy::UnregisterForSuppServiceNotification(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForSuppServiceNotification(h);
}

ECode PhoneProxy::RegisterForSuppServiceFailed(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForSuppServiceFailed(h, what, obj);
}

ECode PhoneProxy::UnregisterForSuppServiceFailed(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForSuppServiceFailed(h);
}

ECode PhoneProxy::RegisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForInCallVoicePrivacyOn(h, what, obj);
}

ECode PhoneProxy::UnregisterForInCallVoicePrivacyOn(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForInCallVoicePrivacyOn(h);
}

ECode PhoneProxy::RegisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForInCallVoicePrivacyOff(h, what, obj);
}

ECode PhoneProxy::UnregisterForInCallVoicePrivacyOff(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForInCallVoicePrivacyOff(h);
}

ECode PhoneProxy::RegisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForCdmaOtaStatusChange(h, what, obj);
}

ECode PhoneProxy::UnregisterForCdmaOtaStatusChange(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForCdmaOtaStatusChange(h);
}

ECode PhoneProxy::RegisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForSubscriptionInfoReady(h, what, obj);
}

ECode PhoneProxy::UnregisterForSubscriptionInfoReady(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForSubscriptionInfoReady(h);
}

ECode PhoneProxy::RegisterForEcmTimerReset(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForEcmTimerReset(h, what, obj);
}

ECode PhoneProxy::UnregisterForEcmTimerReset(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForEcmTimerReset(h);
}

ECode PhoneProxy::RegisterForRingbackTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForRingbackTone(h, what, obj);
}

ECode PhoneProxy::UnregisterForRingbackTone(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForRingbackTone(h);
}

ECode PhoneProxy::RegisterForOnHoldTone(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForOnHoldTone(h, what, obj);
}

ECode PhoneProxy::UnregisterForOnHoldTone(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForOnHoldTone(h);
}

ECode PhoneProxy::RegisterForResendIncallMute(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForResendIncallMute(h, what, obj);
}

ECode PhoneProxy::UnregisterForResendIncallMute(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForResendIncallMute(h);
}

ECode PhoneProxy::RegisterForSimRecordsLoaded(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForSimRecordsLoaded(h, what, obj);
}

ECode PhoneProxy::UnregisterForSimRecordsLoaded(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForSimRecordsLoaded(h);
}

ECode PhoneProxy::GetIccRecordsLoaded(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mIccCardProxy->GetIccRecordsLoaded(result);
}

ECode PhoneProxy::GetIccCard(
    /* [out] */ IIccCard** result)
{
    VALIDATE_NOT_NULL(result)
    *result = IIccCard::Probe(mIccCardProxy);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneProxy::AcceptCall(
    /* [in] */ Int32 videoState)
{
    return mActivePhone->AcceptCall(videoState);
}

ECode PhoneProxy::DeflectCall(
    /* [in] */ const String& number)
{
    return mActivePhone->DeflectCall(number);
}

ECode PhoneProxy::RejectCall()
{
    return mActivePhone->RejectCall();
}

ECode PhoneProxy::SwitchHoldingAndActive()
{
    return mActivePhone->SwitchHoldingAndActive();
}

ECode PhoneProxy::CanConference(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->CanConference(result);
}

ECode PhoneProxy::Conference()
{
    return mActivePhone->Conference();
}

ECode PhoneProxy::EnableEnhancedVoicePrivacy(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->EnableEnhancedVoicePrivacy(enable, onComplete);
}

ECode PhoneProxy::GetEnhancedVoicePrivacy(
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetEnhancedVoicePrivacy(onComplete);
}

ECode PhoneProxy::CanTransfer(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->CanTransfer(result);
}

ECode PhoneProxy::ExplicitCallTransfer()
{
    return mActivePhone->ExplicitCallTransfer();
}

ECode PhoneProxy::ClearDisconnected()
{
    return mActivePhone->ClearDisconnected();
}

ECode PhoneProxy::GetForegroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetForegroundCall(result);
}

ECode PhoneProxy::GetBackgroundCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetBackgroundCall(result);
}

ECode PhoneProxy::GetRingingCall(
    /* [out] */ ICall** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetRingingCall(result);
}

ECode PhoneProxy::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->Dial(dialString, videoState, result);
}

ECode PhoneProxy::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ IUUSInfo* uusInfo,
    /* [in] */ Int32 videoState,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->Dial(dialString, uusInfo, videoState, result);
}

ECode PhoneProxy::AddParticipant(
    /* [in] */ const String& dialString)
{
    return mActivePhone->AddParticipant(dialString);
}

ECode PhoneProxy::HandlePinMmi(
    /* [in] */ const String& dialString,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->HandlePinMmi(dialString, result);
}

ECode PhoneProxy::HandleInCallMmiCommands(
    /* [in] */ const String& command,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->HandleInCallMmiCommands(command, result);
}

ECode PhoneProxy::SendDtmf(
    /* [in] */ Char32 c)
{
    return mActivePhone->SendDtmf(c);
}

ECode PhoneProxy::StartDtmf(
    /* [in] */ Char32 c)
{
    return mActivePhone->StartDtmf(c);
}

ECode PhoneProxy::StopDtmf()
{
    return mActivePhone->StopDtmf();
}

ECode PhoneProxy::SetRadioPower(
    /* [in] */ Boolean power)
{
    return mActivePhone->SetRadioPower(power);
}

ECode PhoneProxy::GetMessageWaitingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetMessageWaitingIndicator(result);
}

ECode PhoneProxy::GetCallForwardingIndicator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCallForwardingIndicator(result);
}

ECode PhoneProxy::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetLine1Number(result);
}

ECode PhoneProxy::GetCdmaMin(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCdmaMin(result);
}

ECode PhoneProxy::IsMinInfoReady(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsMinInfoReady(result);
}

ECode PhoneProxy::GetCdmaPrlVersion(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCdmaPrlVersion(result);
}

ECode PhoneProxy::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetLine1AlphaTag(result);
}

ECode PhoneProxy::SetLine1Number(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& number,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetLine1Number(alphaTag, number, onComplete);
}

ECode PhoneProxy::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetVoiceMailNumber(result);
}

ECode PhoneProxy::GetVoiceMessageCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetVoiceMessageCount(result);
}

ECode PhoneProxy::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetVoiceMailAlphaTag(result);
}

ECode PhoneProxy::SetVoiceMailNumber(
    /* [in] */ const String& alphaTag,
    /* [in] */ const String& voiceMailNumber,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetVoiceMailNumber(alphaTag, voiceMailNumber, onComplete);
}

ECode PhoneProxy::GetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetCallForwardingOption(commandInterfaceCFReason,
            onComplete);
}

ECode PhoneProxy::GetCallForwardingUncondTimerOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetCallForwardingUncondTimerOption(commandInterfaceCFReason,
            onComplete);
}

ECode PhoneProxy::SetCallForwardingOption(
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ const String& dialingNumber,
    /* [in] */ Int32 timerSeconds,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetCallForwardingOption(commandInterfaceCFReason,
        commandInterfaceCFAction, dialingNumber, timerSeconds, onComplete);
}

ECode PhoneProxy::SetCallForwardingUncondTimerOption(
    /* [in] */ Int32 startHour,
    /* [in] */ Int32 startMinute,
    /* [in] */ Int32 endHour,
    /* [in] */ Int32 endMinute,
    /* [in] */ Int32 commandInterfaceCFReason,
    /* [in] */ Int32 commandInterfaceCFAction,
    /* [in] */ const String& dialingNumber,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetCallForwardingUncondTimerOption(startHour, startMinute,
        endHour, endMinute, commandInterfaceCFReason,
        commandInterfaceCFAction, dialingNumber, onComplete);
}

ECode PhoneProxy::GetOutgoingCallerIdDisplay(
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetOutgoingCallerIdDisplay(onComplete);
}

ECode PhoneProxy::SetOutgoingCallerIdDisplay(
    /* [in] */ Int32 commandInterfaceCLIRMode,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetOutgoingCallerIdDisplay(commandInterfaceCLIRMode,
            onComplete);
}

ECode PhoneProxy::GetCallWaiting(
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetCallWaiting(onComplete);
}

ECode PhoneProxy::SetCallWaiting(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetCallWaiting(enable, onComplete);
}

ECode PhoneProxy::GetAvailableNetworks(
    /* [in] */ IMessage* response)
{
    return mActivePhone->GetAvailableNetworks(response);
}

ECode PhoneProxy::SetNetworkSelectionModeAutomatic(
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetNetworkSelectionModeAutomatic(response);
}

ECode PhoneProxy::SelectNetworkManually(
    /* [in] */ IOperatorInfo* network,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SelectNetworkManually(network, response);
}

ECode PhoneProxy::SetPreferredNetworkType(
    /* [in] */ Int32 networkType,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetPreferredNetworkType(networkType, response);
}

ECode PhoneProxy::GetPreferredNetworkType(
    /* [in] */ IMessage* response)
{
    return mActivePhone->GetPreferredNetworkType(response);
}

ECode PhoneProxy::GetNeighboringCids(
    /* [in] */ IMessage* response)
{
    return mActivePhone->GetNeighboringCids(response);
}

ECode PhoneProxy::SetOnPostDialCharacter(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->SetOnPostDialCharacter(h, what, obj);
}

ECode PhoneProxy::SetMute(
    /* [in] */ Boolean muted)
{
    return mActivePhone->SetMute(muted);
}

ECode PhoneProxy::GetMute(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetMute(result);
}

ECode PhoneProxy::SetEchoSuppressionEnabled()
{
    return mActivePhone->SetEchoSuppressionEnabled();
}

ECode PhoneProxy::InvokeOemRilRequestRaw(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ IMessage* response)
{
    return mActivePhone->InvokeOemRilRequestRaw(data, response);
}

ECode PhoneProxy::InvokeOemRilRequestStrings(
    /* [in] */ ArrayOf<String>* strings,
    /* [in] */ IMessage* response)
{
    return mActivePhone->InvokeOemRilRequestStrings(strings, response);
}

ECode PhoneProxy::GetDataCallList(
    /* [in] */ IMessage* response)
{
    return mActivePhone->GetDataCallList(response);
}

ECode PhoneProxy::UpdateServiceLocation()
{
    return mActivePhone->UpdateServiceLocation();
}

ECode PhoneProxy::EnableLocationUpdates()
{
    return mActivePhone->EnableLocationUpdates();
}

ECode PhoneProxy::DisableLocationUpdates()
{
    return mActivePhone->DisableLocationUpdates();
}

ECode PhoneProxy::SetUnitTestMode(
    /* [in] */ Boolean f)
{
    return mActivePhone->SetUnitTestMode(f);
}

ECode PhoneProxy::GetUnitTestMode(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetUnitTestMode(result);
}

ECode PhoneProxy::SetBandMode(
    /* [in] */ Int32 bandMode,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetBandMode(bandMode, response);
}

ECode PhoneProxy::QueryAvailableBandMode(
    /* [in] */ IMessage* response)
{
    return mActivePhone->QueryAvailableBandMode(response);
}

ECode PhoneProxy::GetDataRoamingEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDataRoamingEnabled(result);
}

ECode PhoneProxy::SetDataRoamingEnabled(
    /* [in] */ Boolean enable)
{
    return mActivePhone->SetDataRoamingEnabled(enable);
}

ECode PhoneProxy::GetDataEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDataEnabled(result);
}

ECode PhoneProxy::SetDataEnabled(
    /* [in] */ Boolean enable)
{
    return mActivePhone->SetDataEnabled(enable);
}

ECode PhoneProxy::QueryCdmaRoamingPreference(
    /* [in] */ IMessage* response)
{
    return mActivePhone->QueryCdmaRoamingPreference(response);
}

ECode PhoneProxy::SetCdmaRoamingPreference(
    /* [in] */ Int32 cdmaRoamingType,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetCdmaRoamingPreference(cdmaRoamingType, response);
}

ECode PhoneProxy::SetCdmaSubscription(
    /* [in] */ Int32 cdmaSubscriptionType,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetCdmaSubscription(cdmaSubscriptionType, response);
}

// ECode PhoneProxy::GetSimulatedRadioControl(
//     /* [out] */ ISimulatedRadioControl** result)
// {
//     VALIDATE_NOT_NULL(result)
//     return mActivePhone->GetSimulatedRadioControl(result);
// }

ECode PhoneProxy::IsDataConnectivityPossible(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsDataConnectivityPossible(IPhoneConstants::APN_TYPE_DEFAULT, result);
}

ECode PhoneProxy::IsOnDemandDataPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsOnDemandDataPossible(apnType, result);
}

ECode PhoneProxy::IsDataConnectivityPossible(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsDataConnectivityPossible(apnType, result);
}

ECode PhoneProxy::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDeviceId(result);
}

ECode PhoneProxy::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetDeviceSvn(result);
}

ECode PhoneProxy::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetSubscriberId(result);
}

ECode PhoneProxy::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetGroupIdLevel1(result);
}

ECode PhoneProxy::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetIccSerialNumber(result);
}

ECode PhoneProxy::GetEsn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetEsn(result);
}

ECode PhoneProxy::GetMeid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetMeid(result);
}

ECode PhoneProxy::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetMsisdn(result);
}

ECode PhoneProxy::GetImei(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetImei(result);
}

ECode PhoneProxy::GetPhoneSubInfo(
    /* [out] */ IPhoneSubInfo** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPhoneSubInfo(result);
}

ECode PhoneProxy::GetIccPhoneBookInterfaceManager(
    /* [out] */ IIccPhoneBookInterfaceManager** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetIccPhoneBookInterfaceManager(result);
}

ECode PhoneProxy::SetUiTTYMode(
    /* [in] */ Int32 uiTtyMode,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetUiTTYMode(uiTtyMode, onComplete);
}

ECode PhoneProxy::SetTTYMode(
    /* [in] */ Int32 ttyMode,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetTTYMode(ttyMode, onComplete);
}

ECode PhoneProxy::QueryTTYMode(
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->QueryTTYMode(onComplete);
}

ECode PhoneProxy::ActivateCellBroadcastSms(
    /* [in] */ Int32 activate,
    /* [in] */ IMessage* response)
{
    return mActivePhone->ActivateCellBroadcastSms(activate, response);
}

ECode PhoneProxy::GetCellBroadcastSmsConfig(
    /* [in] */ IMessage* response)
{
    return mActivePhone->GetCellBroadcastSmsConfig(response);
}

ECode PhoneProxy::SetCellBroadcastSmsConfig(
    /* [in] */ ArrayOf<Int32>* configValuesArray,
    /* [in] */ IMessage* response)
{
    return mActivePhone->SetCellBroadcastSmsConfig(configValuesArray, response);
}

ECode PhoneProxy::NotifyDataActivity()
{
    return mActivePhone->NotifyDataActivity();
}

ECode PhoneProxy::GetSmscAddress(
    /* [in] */ IMessage* result)
{
    return mActivePhone->GetSmscAddress(result);
}

ECode PhoneProxy::SetSmscAddress(
    /* [in] */ const String& address,
    /* [in] */ IMessage* result)
{
    return mActivePhone->SetSmscAddress(address, result);
}

ECode PhoneProxy::GetCdmaEriIconIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCdmaEriIconIndex(result);
}

ECode PhoneProxy::GetCdmaEriText(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCdmaEriText(result);
}

ECode PhoneProxy::GetCdmaEriIconMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetCdmaEriIconMode(result);
}

ECode PhoneProxy::GetActivePhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActivePhone;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PhoneProxy::SendBurstDtmf(
    /* [in] */ const String& dtmfString,
    /* [in] */ Int32 on,
    /* [in] */ Int32 off,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SendBurstDtmf(dtmfString, on, off, onComplete);
}

ECode PhoneProxy::ExitEmergencyCallbackMode()
{
    return mActivePhone->ExitEmergencyCallbackMode();
}

ECode PhoneProxy::NeedsOtaServiceProvisioning(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->NeedsOtaServiceProvisioning(result);
}

ECode PhoneProxy::IsOtaSpNumber(
    /* [in] */ const String& dialStr,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsOtaSpNumber(dialStr, result);
}

ECode PhoneProxy::RegisterForCallWaiting(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForCallWaiting(h, what, obj);
}

ECode PhoneProxy::UnregisterForCallWaiting(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForCallWaiting(h);
}

ECode PhoneProxy::RegisterForSignalInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForSignalInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForSignalInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForSignalInfo(h);
}

ECode PhoneProxy::RegisterForDisplayInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForDisplayInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForDisplayInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForDisplayInfo(h);
}

ECode PhoneProxy::RegisterForNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForNumberInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForNumberInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForNumberInfo(h);
}

ECode PhoneProxy::RegisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForRedirectedNumberInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForRedirectedNumberInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForRedirectedNumberInfo(h);
}

ECode PhoneProxy::RegisterForLineControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForLineControlInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForLineControlInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForLineControlInfo(h);
}

ECode PhoneProxy::RegisterFoT53ClirlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterFoT53ClirlInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForT53ClirInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForT53ClirInfo(h);
}

ECode PhoneProxy::RegisterForT53AudioControlInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->RegisterForT53AudioControlInfo(h, what, obj);
}

ECode PhoneProxy::UnregisterForT53AudioControlInfo(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnregisterForT53AudioControlInfo(h);
}

ECode PhoneProxy::SetOnEcbModeExitResponse(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return mActivePhone->SetOnEcbModeExitResponse(h, what, obj);
}

ECode PhoneProxy::UnsetOnEcbModeExitResponse(
    /* [in] */ IHandler* h)
{
    return mActivePhone->UnsetOnEcbModeExitResponse(h);
}

ECode PhoneProxy::IsManualNetSelAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsManualNetSelAllowed(result);
}

ECode PhoneProxy::IsCspPlmnEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->IsCspPlmnEnabled(result);
}

ECode PhoneProxy::GetIsimRecords(
    /* [out] */ IIsimRecords** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetIsimRecords(result);
}

ECode PhoneProxy::GetLteOnCdmaMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetLteOnCdmaMode(result);
}

ECode PhoneProxy::GetLteOnGsmMode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetLteOnGsmMode(result);
}

ECode PhoneProxy::SetVoiceMessageWaiting(
    /* [in] */ Int32 line,
    /* [in] */ Int32 countWaiting)
{
    return mActivePhone->SetVoiceMessageWaiting(line, countWaiting);
}

ECode PhoneProxy::GetUsimServiceTable(
    /* [out] */ IUsimServiceTable** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetUsimServiceTable(result);
}

ECode PhoneProxy::GetUiccCard(
    /* [out] */ IUiccCard** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetUiccCard(result);
}

ECode PhoneProxy::NvReadItem(
    /* [in] */ Int32 itemID,
    /* [in] */ IMessage* response)
{
    return mActivePhone->NvReadItem(itemID, response);
}

ECode PhoneProxy::NvWriteItem(
    /* [in] */ Int32 itemID,
    /* [in] */ const String& itemValue,
    /* [in] */ IMessage* response)
{
    return mActivePhone->NvWriteItem(itemID, itemValue, response);
}

ECode PhoneProxy::NvWriteCdmaPrl(
    /* [in] */ ArrayOf<Byte>* preferredRoamingList,
    /* [in] */ IMessage* response)
{
    return mActivePhone->NvWriteCdmaPrl(preferredRoamingList, response);
}

ECode PhoneProxy::NvResetConfig(
    /* [in] */ Int32 resetType,
    /* [in] */ IMessage* response)
{
    return mActivePhone->NvResetConfig(resetType, response);
}

ECode PhoneProxy::Dispose()
{
    if (mActivePhone != NULL) {
        mActivePhone->UnregisterForSimRecordsLoaded(this);
    }
    mCommandsInterface->UnregisterForOn(this);
    mCommandsInterface->UnregisterForVoiceRadioTechChanged(this);
    mCommandsInterface->UnregisterForRilConnected(this);
    return NOERROR;
}

ECode PhoneProxy::RemoveReferences()
{
    mActivePhone = NULL;
    mCommandsInterface = NULL;
    return NOERROR;
}

ECode PhoneProxy::UpdateCurrentCarrierInProvider(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        return ((CDMALTEPhone*)pPhone.Get())->UpdateCurrentCarrierInProvider(result);
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        return pPhone->UpdateCurrentCarrierInProvider(result);
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
        *result = FALSE;
        return NOERROR;
    }
}

ECode PhoneProxy::UpdateDataConnectionTracker()
{
    Logd(String("Updating Data Connection Tracker"));
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        pPhone->UpdateDataConnectionTracker();
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        pPhone->UpdateDataConnectionTracker();
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
    }
    return NOERROR;
}

ECode PhoneProxy::SetInternalDataEnabled(
    /* [in] */ Boolean enable)
{
    return SetInternalDataEnabled(enable, NULL);
}

ECode PhoneProxy::SetInternalDataEnabledFlag(
    /* [in] */ Boolean enable,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        pPhone->SetInternalDataEnabledFlag(enable, result);
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        pPhone->SetInternalDataEnabledFlag(enable, result);
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
    }
    return NOERROR;
}

ECode PhoneProxy::SetInternalDataEnabled(
    /* [in] */ Boolean enable,
    /* [in] */ IMessage* onCompleteMsg)
{
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        pPhone->SetInternalDataEnabled(enable, onCompleteMsg);
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        pPhone->SetInternalDataEnabled(enable, onCompleteMsg);
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
    }
    return NOERROR;
}

ECode PhoneProxy::RegisterForAllDataDisconnected(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        pPhone->RegisterForAllDataDisconnected(h, what, obj);
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        pPhone->RegisterForAllDataDisconnected(h, what, obj);
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
    }
    return NOERROR;
}

ECode PhoneProxy::UnregisterForAllDataDisconnected(
    /* [in] */ IHandler* h)
{
    if (ICDMALTEPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<ICDMALTEPhone> pPhone = ICDMALTEPhone::Probe(mActivePhone);
        pPhone->UnregisterForAllDataDisconnected(h);
    }
    else if (IGSMPhone::Probe(mActivePhone) != NULL) {
        AutoPtr<IGSMPhone> pPhone = IGSMPhone::Probe(mActivePhone);
        pPhone->UnregisterForAllDataDisconnected(h);
    }
    else {
        Loge(String("Phone object is not MultiSim. This should not hit!!!!"));
    }
    return NOERROR;
}

ECode PhoneProxy::GetSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetSubId(result);
}

ECode PhoneProxy::GetPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPhoneId(result);
}

ECode PhoneProxy::GetPcscfAddress(
    /* [in] */ const String& apnType,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetPcscfAddress(apnType, result);
}

ECode PhoneProxy::SetImsRegistrationState(
    /* [in] */ Boolean registered)
{
    // String str("setImsRegistrationState - registered: ");
    // str += registered;
    // Logd(str);

    mActivePhone->SetImsRegistrationState(registered);

    String phoneName;
    mActivePhone->GetPhoneName(&phoneName);
    if (phoneName.Equals("GSM")) {
        AutoPtr<IGSMPhone> GP = IGSMPhone::Probe(mActivePhone);
        AutoPtr<IServiceStateTracker> sst;
        IPhoneBase::Probe(GP)->GetServiceStateTracker((IServiceStateTracker**)&sst);
        sst->SetImsRegistrationState(registered);
    }
    else if (phoneName.Equals("CDMA")) {
        AutoPtr<ICDMAPhone> CP = ICDMAPhone::Probe(mActivePhone);
        AutoPtr<IServiceStateTracker> sst;
        IPhoneBase::Probe(CP)->GetServiceStateTracker((IServiceStateTracker**)&sst);
        sst->SetImsRegistrationState(registered);
    }
    return NOERROR;
}

ECode PhoneProxy::GetImsPhone(
    /* [out] */ IPhone** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetImsPhone(result);
}

ECode PhoneProxy::RelinquishOwnershipOfImsPhone(
    /* [out] */ IImsPhone** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode PhoneProxy::AcquireOwnershipOfImsPhone(
    /* [in] */ IImsPhone* imsPhone)
{
    return NOERROR;
}

ECode PhoneProxy::GetVoicePhoneServiceState(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->GetVoicePhoneServiceState(result);
}

ECode PhoneProxy::SetOperatorBrandOverride(
    /* [in] */ const String& brand,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->SetOperatorBrandOverride(brand, result);
}

ECode PhoneProxy::IsRadioAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    ICommandsInterfaceRadioState cirs;
    mCommandsInterface->GetRadioState(&cirs);
    *result = cirs != RADIO_UNAVAILABLE;
    return NOERROR;
}

ECode PhoneProxy::ShutdownRadio()
{
    return mActivePhone->ShutdownRadio();
}

ECode PhoneProxy::GetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->GetCallBarringOption(facility, password, onComplete);
}

ECode PhoneProxy::SetCallBarringOption(
    /* [in] */ const String& facility,
    /* [in] */ Boolean lockState,
    /* [in] */ const String& password,
    /* [in] */ IMessage* onComplete)
{
    return mActivePhone->SetCallBarringOption(facility, lockState, password, onComplete);
}

ECode PhoneProxy::RequestChangeCbPsw(
    /* [in] */ const String& facility,
    /* [in] */ const String& oldPwd,
    /* [in] */ const String& newPwd,
    /* [in] */ IMessage* result)
{
    return mActivePhone->RequestChangeCbPsw(facility, oldPwd, newPwd, result);
}

ECode PhoneProxy::SetLocalCallHold(
    /* [in] */ Int32 lchStatus)
{
    return mActivePhone->SetLocalCallHold(lchStatus);
}

ECode PhoneProxy::Dial(
    /* [in] */ const String& dialString,
    /* [in] */ Int32 videoState,
    /* [in] */ IBundle* extras,
    /* [out] */ IConnection** result)
{
    VALIDATE_NOT_NULL(result)
    return mActivePhone->Dial(dialString, videoState, extras, result);
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
