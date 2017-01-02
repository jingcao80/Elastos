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

#include "elastos/droid/internal/telephony/UiccPhoneBookController.h"
#include "elastos/droid/internal/telephony/CSubscriptionControllerHelper.h"
#include "elastos/droid/os/CServiceManager.h"

using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

//==============================================================
//  UiccPhoneBookController::
//==============================================================
const String UiccPhoneBookController::TAG("UiccPhoneBookController");

CAR_INTERFACE_IMPL_2(UiccPhoneBookController, Object, IIIccPhoneBook, IUiccPhoneBookController)

ECode UiccPhoneBookController::constructor(
    /* [in] */ ArrayOf<IPhone*>* phone)
{
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> p;
    sm->GetService(String("simphonebook"), (IInterface**)&p);
    if (p == NULL) {
        sm->AddService(String("simphonebook"), IIIccPhoneBook::Probe(this));
    }
    mPhone = phone;
    return NOERROR;
}

ECode UiccPhoneBookController::UpdateAdnRecordsInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ const String& oldTag,
    /* [in] */ const String& oldPhoneNumber,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return UpdateAdnRecordsInEfBySearchForSubscriber(GetDefaultSubId(), efid, oldTag,
            oldPhoneNumber, newTag, newPhoneNumber, pin2, result);
}

ECode UiccPhoneBookController::UpdateAdnRecordsInEfBySearchForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 efid,
    /* [in] */ const String& oldTag,
    /* [in] */ const String& oldPhoneNumber,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                        GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->UpdateAdnRecordsInEfBySearch(efid, oldTag,
                oldPhoneNumber, newTag, newPhoneNumber, pin2, result);
    }
    else {
        String str("updateAdnRecordsInEfBySearch iccPbkIntMgrProxy is");
        str += " NULL for Subscription:";
        //str += subId;
        // Rlog::E(TAG, str);
        *result = FALSE;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::UpdateAdnRecordsInEfByIndex(
    /* [in] */ Int32 efid,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return UpdateAdnRecordsInEfByIndexForSubscriber(GetDefaultSubId(), efid, newTag,
            newPhoneNumber, index, pin2, result);
}

ECode UiccPhoneBookController::UpdateAdnRecordsInEfByIndexForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 efid,
    /* [in] */ const String& newTag,
    /* [in] */ const String& newPhoneNumber,
    /* [in] */ Int32 index,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->UpdateAdnRecordsInEfByIndex(efid, newTag,
                newPhoneNumber, index, pin2, result);
    }
    else {
        String str("updateAdnRecordsInEfByIndex iccPbkIntMgrProxy is");
        str += " NULL for Subscription:";
        //str += subId;
        // Rlog::E(TAG, str);
        *result = FALSE;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetAdnRecordsSize(
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    return GetAdnRecordsSizeForSubscriber(GetDefaultSubId(), efid, result);
}

ECode UiccPhoneBookController::GetAdnRecordsSizeForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 efid,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetAdnRecordsSize(efid, result);
    }
    else {
        // String str("getAdnRecordsSize iccPbkIntMgrProxy is");
        // str += " NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = NULL;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetAdnRecordsInEf(
    /* [in] */ Int32 efid,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    return GetAdnRecordsInEfForSubscriber(GetDefaultSubId(), efid, result);
}

ECode UiccPhoneBookController::GetAdnRecordsInEfForSubscriber(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 efid,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetAdnRecordsInEf(efid, result);
    }
    else {
        // String str("getAdnRecordsInEf iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = NULL;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::UpdateAdnRecordsWithContentValuesInEfBySearch(
    /* [in] */ Int32 efid,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
        GetDefaultSubId(), efid, values, pin2, result);
}

ECode UiccPhoneBookController::UpdateAdnRecordsWithContentValuesInEfBySearchUsingSubId(
    /* [in] */ Int64 subId,
    /* [in] */ Int32 efid,
    /* [in] */ IContentValues* values,
    /* [in] */ const String& pin2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->UpdateAdnRecordsWithContentValuesInEfBySearch(
            efid, values, pin2, result);
    }
    else {
        // String str("updateAdnRecordsWithContentValuesInEfBySearchUsingSubId ");
        // str += "iccPbkIntMgrProxy is NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = FALSE;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetAdnCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetAdnCountUsingSubId(GetDefaultSubId(), result);
}

ECode UiccPhoneBookController::GetAdnCountUsingSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetAdnCount(result);
    }
    else {
        // String str("getAdnCount iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = 0;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetAnrCountUsingSubId(GetDefaultSubId(), result);
}

ECode UiccPhoneBookController::GetAnrCountUsingSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetAnrCount(result);
    }
    else {
        // String str("getAnrCount iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = 0;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetEmailCountUsingSubId(GetDefaultSubId(), result);
}

ECode UiccPhoneBookController::GetEmailCountUsingSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetEmailCount(result);
    }
    else {
        // String str("getEmailCount iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = 0;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetSpareAnrCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetSpareAnrCountUsingSubId(GetDefaultSubId(), result);
}

ECode UiccPhoneBookController::GetSpareAnrCountUsingSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetSpareAnrCount(result);
    }
    else {
        // String str("getSpareAnrCount iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = 0;
        return NOERROR;
    }
}

ECode UiccPhoneBookController::GetSpareEmailCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return GetSpareEmailCountUsingSubId(GetDefaultSubId(), result);
}

ECode UiccPhoneBookController::GetSpareEmailCountUsingSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> iccPbkIntMgrProxy =
                         GetIccPhoneBookInterfaceManagerProxy(subId);
    if (iccPbkIntMgrProxy != NULL) {
        return iccPbkIntMgrProxy->GetSpareEmailCount(result);
    }
    else {
        // String str("getSpareEmailCount iccPbkIntMgrProxy is");
        // str += "NULL for Subscription:";
        // str += subId;
        // Rlog::E(TAG, str);
        *result = 0;
        return NOERROR;
    }
}

AutoPtr<IIccPhoneBookInterfaceManagerProxy> UiccPhoneBookController::GetIccPhoneBookInterfaceManagerProxy(
    /* [in] */ Int64 subId)
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int32 phoneId = 0;
    IISub::Probe(sc)->GetPhoneId(subId, &phoneId);
    // try {
    AutoPtr<IPhoneProxy> p = IPhoneProxy::Probe((*mPhone)[phoneId]);
    AutoPtr<IIccPhoneBookInterfaceManagerProxy> res;
    p->GetIccPhoneBookInterfaceManagerProxy((IIccPhoneBookInterfaceManagerProxy**)&res);
    return res;
    // } Catch (NullPointerException e) {
    //     Rlog->E(TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
    //     e->PrintStackTrace(); //To print stack trace
    //     return NULL;
    // } Catch (ArrayIndexOutOfBoundsException e) {
    //     Rlog->E(TAG, "Exception is :"+e->ToString()+" For subscription :"+subId );
    //     e->PrintStackTrace();
    //     return NULL;
    // }
}

Int64 UiccPhoneBookController::GetDefaultSubId()
{
    AutoPtr<ISubscriptionControllerHelper> hlp;
    CSubscriptionControllerHelper::AcquireSingleton((ISubscriptionControllerHelper**)&hlp);
    AutoPtr<ISubscriptionController> sc;
    hlp->GetInstance((ISubscriptionController**)&sc);
    Int64 res = 0;
    IISub::Probe(sc)->GetDefaultSubId(&res);
    return res;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
