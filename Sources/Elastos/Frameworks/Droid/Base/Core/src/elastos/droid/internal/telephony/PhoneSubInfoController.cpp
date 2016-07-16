
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/telephony/PhoneSubInfoController.h"
#include "elastos/droid/os/CServiceManager.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ITelephonyManagerHelper;
using Elastos::Droid::Telephony::CTelephonyManagerHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

const String PhoneSubInfoController::TAG("PhoneSubInfoController");

CAR_INTERFACE_IMPL_2(PhoneSubInfoController, Object, IIPhoneSubInfo, IPhoneSubInfoController)

ECode PhoneSubInfoController::constructor(
    /* [in] */ ArrayOf<IPhone*>* phone)
{
    mPhone = phone;
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> serv;
    sm->GetService(String("iphonesubinfo"), (IInterface**)&serv);
    if (serv == NULL) {
        sm->AddService(String("iphonesubinfo"), IIPhoneSubInfo::Probe(this));
    }
    return NOERROR;
}

ECode PhoneSubInfoController::GetDeviceId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetDeviceIdForSubscriber(GetFirstPhoneSubId(), result);
}

ECode PhoneSubInfoController::GetDeviceIdForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetDeviceId(result);
    }
    else {
        // Rlog::E(TAG,"getDeviceId phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetImeiForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return phoneSubInfoProxy->GetImei(result);
    }
    else {
        // Rlog::E(TAG, "getDeviceId phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetDeviceSvn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetFirstPhoneSubId());
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetDeviceSvn(result);
    }
    else {
        // Rlog::E(TAG, "getDeviceSvn phoneSubInfoProxy is NULL");
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetSubscriberId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetSubscriberIdForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetSubscriberIdForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetSubscriberId(result);
    }
    else {
        // Rlog::E(TAG,"getSubscriberId phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetIccSerialNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetIccSerialNumberForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetIccSerialNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIccSerialNumber(result);
    }
    else {
        // Rlog::E(TAG,"getIccSerialNumber phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetLine1Number(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetLine1NumberForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetLine1NumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetLine1Number(result);
    }
    else {
        // Rlog::E(TAG,"getLine1Number phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetLine1AlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetLine1AlphaTagForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetLine1AlphaTagForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetLine1AlphaTag(result);
    }
    else {
        // Rlog::E(TAG,"getLine1AlphaTag phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetMsisdn(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetMsisdnForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetMsisdnForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetMsisdn(result);
    }
    else {
        // Rlog::E(TAG,"getMsisdn phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetVoiceMailNumberForSubscriber(GetDefaultVoiceSubId(), result);
}

ECode PhoneSubInfoController::GetVoiceMailNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetVoiceMailNumber(result);
    }
    else {
        // Rlog::E(TAG,"getVoiceMailNumber phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetCompleteVoiceMailNumber(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetCompleteVoiceMailNumberForSubscriber(GetDefaultVoiceSubId(), result);
}

ECode PhoneSubInfoController::GetCompleteVoiceMailNumberForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetCompleteVoiceMailNumber(result);
    }
    else {
        // Rlog::E(TAG,"getCompleteVoiceMailNumber phoneSubInfoProxy is NULL for Subscription:%s",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

ECode PhoneSubInfoController::GetVoiceMailAlphaTag(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetVoiceMailAlphaTagForSubscriber(GetDefaultVoiceSubId(), result);
}

ECode PhoneSubInfoController::GetVoiceMailAlphaTagForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetVoiceMailAlphaTag(result);
    }
    else {
        // Rlog::E(TAG,"getVoiceMailAlphaTag phoneSubInfoProxy is NULL for Subscription:",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

AutoPtr<IPhoneSubInfoProxy> PhoneSubInfoController::GetPhoneSubInfoProxy(
    /* [in] */ Int64 subId)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    AutoPtr<ITelephonyManagerHelper> tmhlp;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmhlp);
    AutoPtr<ITelephonyManager> tm;
    tmhlp->GetDefault((ITelephonyManager**)&tm);
    Int32 phoneCount = 0;
    tm->GetPhoneCount(&phoneCount);
    if (phoneId < 0 || phoneId >= phoneCount) {
        phoneId = 0;
    }

    // try {
        AutoPtr<IPhoneSubInfoProxy> result;
        IPhoneProxy::Probe((*mPhone)[phoneId])->GetPhoneSubInfoProxy((IPhoneSubInfoProxy**)&result);
        return result;
    // } Catch (NullPointerException e) {
    //     Rlog->E(TAG, "Exception is :" + e->ToString() + " For subId :" + subId);
    //     e->PrintStackTrace();
    //     return NULL;
    // } Catch (ArrayIndexOutOfBoundsException e) {
    //     Rlog->E(TAG, "Exception is :" + e->ToString() + " For subId :" + subId);
    //     e->PrintStackTrace();
    //     return NULL;
    // }
}

Int64 PhoneSubInfoController::GetDefaultSubId()
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int64 id = 0;
    IISub::Probe(sc)->GetDefaultSubId(&id);
    return id;
}

Int64 PhoneSubInfoController::GetDefaultVoiceSubId()
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int64 id = 0;
    IISub::Probe(sc)->GetDefaultVoiceSubId(&id);
    return id;
}

Int64 PhoneSubInfoController::GetFirstPhoneSubId()
{
    // get subId from first Phone/slot Id(i.e 0)
    AutoPtr<ISubscriptionControllerHelper> hlp;
    assert(0 && "TODO");
    // CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    AutoPtr<ArrayOf<Int64> > subId;
    assert(0 && "TODO");
    // IISub::Probe(sc)->GetSubId(IPhoneConstants::PHONE_ID1, (ArrayOf<Int64>**)&subId);
    return (*subId)[0];
}

ECode PhoneSubInfoController::GetIsimImpi(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIsimImpi(result);
}

ECode PhoneSubInfoController::GetIsimDomain(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIsimDomain(result);
}

ECode PhoneSubInfoController::GetIsimImpu(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIsimImpu(result);
}

ECode PhoneSubInfoController::GetIsimIst(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIsimIst(result);
}

ECode PhoneSubInfoController::GetIsimPcscf(
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return IIPhoneSubInfo::Probe(phoneSubInfoProxy)->GetIsimPcscf(result);
}

ECode PhoneSubInfoController::GetIsimChallengeResponse(
    /* [in] */ const String& nonce,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(GetDefaultSubId());
    return phoneSubInfoProxy->GetIsimChallengeResponse(nonce, result);
}

ECode PhoneSubInfoController::GetIccSimChallengeResponse(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 appType,
    /* [in] */ const String& data,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    return phoneSubInfoProxy->GetIccSimChallengeResponse(subId, appType, data, result);
}

ECode PhoneSubInfoController::GetGroupIdLevel1(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return GetGroupIdLevel1ForSubscriber(GetDefaultSubId(), result);
}

ECode PhoneSubInfoController::GetGroupIdLevel1ForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IPhoneSubInfoProxy> phoneSubInfoProxy = GetPhoneSubInfoProxy(subId);
    if (phoneSubInfoProxy != NULL) {
        return phoneSubInfoProxy->GetGroupIdLevel1(result);
    }
    else {
        // Rlog::E(TAG,"getGroupIdLevel1 phoneSubInfoProxy is NULL for Subscription:%ld",
        //         subId);
        *result = String(NULL);
        return NOERROR;
    }
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
