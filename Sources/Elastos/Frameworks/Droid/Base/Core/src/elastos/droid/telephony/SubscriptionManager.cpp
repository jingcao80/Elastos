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

#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/telephony/SubscriptionManager.h"
#include "elastos/droid/telephony/CTelephonyManagerHelper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::R;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telephony {

const String SubscriptionManager::TAG("SubscriptionManager");
const Boolean SubscriptionManager::DBG = FALSE;
const Boolean SubscriptionManager::VDBG = FALSE;
const Int32 SubscriptionManager::RES_TYPE_BACKGROUND_DARK = 0;
const Int32 SubscriptionManager::RES_TYPE_BACKGROUND_LIGHT = 1;

AutoPtr<ArrayOf<Int32> > SubscriptionManager::sSimBackgroundDarkRes =
        SubscriptionManager::SetSimResource(SubscriptionManager::RES_TYPE_BACKGROUND_DARK);

ECode SubscriptionManager::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri)
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    return uriHelper->Parse(String("content://telephony/siminfo"), uri);
}

ECode SubscriptionManager::GetSubInfoForSubscriber(
    /* [in] */ Int64 subId,
    /* [out] */ ISubInfoRecord** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean b;
    if (IsValidSubId(subId, &b), !b) {
        Logd(String("[getSubInfoForSubscriberx]- invalid subId"));
        result = NULL;
        return NOERROR;
    }

    AutoPtr<ISubInfoRecord> subInfo;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetSubInfoForSubscriber(subId, (ISubInfoRecord**)&subInfo);
    }

    *result = subInfo;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionManager::GetSubInfoUsingIccId(
    /* [in] */ const String& iccId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    // if (VDBG) Logd(String("[getSubInfoUsingIccId]+ iccId=") + iccId);
    if (iccId == NULL) {
        Logd(String("[getSubInfoUsingIccId]- null iccid"));
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> list;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetSubInfoUsingIccId(iccId, (IList**)&list);
    }

    if (list == NULL) {
        CArrayList::New((IList**)&list);
    }
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionManager::GetSubInfoUsingSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)

    // FIXME: Consider never returning null
    Boolean b;
    if (IsValidSlotId(slotId, &b), !b) {
        Logd(String("[getSubInfoUsingSlotId]- invalid slotId"));
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> list;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetSubInfoUsingSlotId(slotId, (IList**)&list);
    }

    if (list == NULL) {
        CArrayList::New((IList**)&list);
    }
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionManager::GetAllSubInfoList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    if (VDBG) Logd(String("[getAllSubInfoList]+"));

    AutoPtr<IList> list;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetAllSubInfoList((IList**)&list);
    }

    if (list == NULL) {
        CArrayList::New((IList**)&list);
    }
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionManager::GetActiveSubInfoList(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IList> list;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetActiveSubInfoList((IList**)&list);
    }

    if (list == NULL) {
        CArrayList::New((IList**)&list);
    }
    *result = list;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SubscriptionManager::GetAllSubInfoCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (VDBG) Logd(String("[getAllSubInfoCount]+"));

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetAllSubInfoCount(result);
    }

    return NOERROR;
}

ECode SubscriptionManager::GetActiveSubInfoCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetActiveSubInfoCount(result);
    }

    return NOERROR;
}

ECode SubscriptionManager::AddSubInfoRecord(
    /* [in] */ const String& iccId,
    /* [in] */ Int32 slotId,
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (VDBG) Logd(String("[addSubInfoRecord]+ iccId:") + iccId + String(" slotId:") + StringUtils::ToString(slotId));
    if (iccId == NULL) {
        Logd(String("[addSubInfoRecord]- null iccId"));
    }
    Boolean b;
    if (IsValidSlotId(slotId, &b), !b) {
        Logd(String("[addSubInfoRecord]- invalid slotId"));
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        // FIXME: This returns 1 on success, 0 on error should should we return it?
        Int32 tmp = 0;
        iSub->AddSubInfoRecord(iccId, slotId, &tmp);
    }

    // FIXME: Always returns null?
    return NOERROR;
}

ECode SubscriptionManager::SetColor(
    /* [in] */ Int32 color,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    // if (VDBG) Logd(String("[setColor]+ color:") + color + " subId:" + subId);
    Int32 size = sSimBackgroundDarkRes->GetLength();

    Boolean b;
    if ((IsValidSubId(subId, &b), !b) || color < 0 || color >= size) {
        Logd(String("[setColor]- fail"));
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetColor(color, subId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::SetDisplayName(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    return SetDisplayName(displayName, subId,
            ISubscriptionManager::NAME_SOURCE_UNDEFINDED, result);
}

ECode SubscriptionManager::SetDisplayName(
    /* [in] */ const String& displayName,
    /* [in] */ Int64 subId,
    /* [in] */ Int64 nameSource,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (VDBG) {
        // Logd(String("[setDisplayName]+  displayName:") + displayName
        //         + " subId:" + subId
        //         + " nameSource:" + nameSource);
    }
    Boolean b;
    if (IsValidSubId(subId, &b), !b) {
        Logd(String("[setDisplayName]- fail"));
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDisplayNameUsingSrc(displayName, subId, nameSource, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::SetDisplayNumber(
    /* [in] */ const String& number,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    Boolean b;
    if (number == NULL || (IsValidSubId(subId, &b), !b)) {
        Logd(String("[setDisplayNumber]- fail"));
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDisplayNumber(number, subId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::SetDisplayNumberFormat(
    /* [in] */ Int32 format,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    // if (VDBG) Logd(String("[setDisplayNumberFormat]+ format:") + format
    //         + " subId:" + subId);

    Boolean b;
    if (format < 0 || (IsValidSubId(subId, &b), !b)) {
        Logd(String("[setDisplayNumberFormat]- fail, return -1"));
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDisplayNumberFormat(format, subId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::SetDataRoaming(
    /* [in] */ Int32 roaming,
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    // if (VDBG) Logd(String("[setDataRoaming]+ roaming:") + roaming
    //         + " subId:" + subId);

    Boolean b;
    if (roaming < 0 || (IsValidSubId(subId, &b), !b)) {
        Logd(String("[setDataRoaming]- fail"));
        *result = -1;
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDataRoaming(roaming, subId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::GetSlotId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    Boolean b;
    if (IsValidSubId(subId, &b), !b) {
        Logd(String("[getSlotId]- fail"));
    }

    *result = ISubscriptionManager::INVALID_SLOT_ID;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetSlotId(subId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::GetSubId(
    /* [in] */ Int32 slotId,
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    Boolean b;
    if (IsValidSlotId(slotId, &b), !b) {
        Logd(String("[getSubId]- fail"));
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetSubId(slotId, result);
    }

    return NOERROR;
}

ECode SubscriptionManager::GetPhoneId(
    /* [in] */ Int64 subId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISubscriptionManager::INVALID_PHONE_ID;

    Boolean b;
    if (IsValidSubId(subId, &b), !b) {
        Logd(String("[getPhoneId]- fail"));
        return NOERROR;
    }

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetPhoneId(subId, result);
    }

    if (VDBG) Logd(String("[getPhoneId]- phoneId=") + StringUtils::ToString(*result));
    return NOERROR;
}

ECode SubscriptionManager::GetDefaultSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISubscriptionManager::INVALID_SUB_ID;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetDefaultSubId(result);
    }

    if (VDBG) Logd(String("getDefaultSubId=") + StringUtils::ToString(*result));
    return NOERROR;
}

ECode SubscriptionManager::GetDefaultVoiceSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISubscriptionManager::INVALID_SUB_ID;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetDefaultVoiceSubId(result);
    }

    if (VDBG) Logd(String("getDefaultVoiceSubId, sub id = ") + StringUtils::ToString(*result));
    return NOERROR;
}

ECode SubscriptionManager::SetDefaultVoiceSubId(
    /* [in] */ Int64 subId)
{
    if (VDBG) Logd(String("setDefaultVoiceSubId sub id = ") + StringUtils::ToString(subId));
    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDefaultVoiceSubId(subId);
    }
    return NOERROR;
}

ECode SubscriptionManager::GetDefaultVoiceSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return GetSubInfoForSubscriber(id, result);
}

ECode SubscriptionManager::GetDefaultVoicePhoneId(
    /* [out] */ Int32* result)
{
    Int64 id;
    GetDefaultVoiceSubId(&id);
    return GetPhoneId(id, result);
}

ECode SubscriptionManager::GetDefaultSmsSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISubscriptionManager::INVALID_SUB_ID;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetDefaultSmsSubId(result);
    }

    if (VDBG) Logd(String("getDefaultSmsSubId, sub id = ") + StringUtils::ToString(*result));
    return NOERROR;
}

ECode SubscriptionManager::SetDefaultSmsSubId(
    /* [in] */ Int64 subId)
{
    if (VDBG) Logd(String("setDefaultSmsSubId sub id = ") + StringUtils::ToString(subId));
    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDefaultSmsSubId(subId);
    }
    return NOERROR;
}

ECode SubscriptionManager::GetDefaultSmsSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 id;
    GetDefaultSmsSubId(&id);
    return GetSubInfoForSubscriber(id, result);
}

ECode SubscriptionManager::GetDefaultSmsPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 id;
    GetDefaultSmsSubId(&id);
    return GetPhoneId(id, result);
}

ECode SubscriptionManager::GetDefaultDataSubId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ISubscriptionManager::INVALID_SUB_ID;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
       iSub->GetDefaultDataSubId(result);
    }

    if (VDBG) Logd(String("getDefaultDataSubId, sub id = ") + StringUtils::ToString(*result));
    return NOERROR;
}

ECode SubscriptionManager::SetDefaultDataSubId(
    /* [in] */ Int64 subId)
{
    if (VDBG) Logd(String("setDataSubscription sub id = ") + StringUtils::ToString(subId));
    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->SetDefaultDataSubId(subId);
    }
    return NOERROR;
}

ECode SubscriptionManager::GetDefaultDataSubInfo(
    /* [out] */ ISubInfoRecord** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 id;
    GetDefaultDataSubId(&id);
    return GetSubInfoForSubscriber(id, result);
}

ECode SubscriptionManager::GetDefaultDataPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 id;
    GetDefaultDataSubId(&id);
    return GetPhoneId(id, result);
}

ECode SubscriptionManager::ClearSubInfo()
{
    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        Int32 tmp = 0;
        iSub->ClearSubInfo(&tmp);
    }

    return NOERROR;
}

ECode SubscriptionManager::AllDefaultsSelected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 id;
    GetDefaultDataSubId(&id);
    if (id == ISubscriptionManager::INVALID_SUB_ID) {
        *result = FALSE;
        return NOERROR;
    }

    GetDefaultSmsSubId(&id);
    if (id == ISubscriptionManager::INVALID_SUB_ID) {
        *result = FALSE;
        return NOERROR;
    }

    GetDefaultVoiceSubId(&id);
    if (id == ISubscriptionManager::INVALID_SUB_ID) {
        *result = FALSE;
        return NOERROR;
    }
    *result = TRUE;
    return NOERROR;
}

ECode SubscriptionManager::ClearDefaultsForInactiveSubIds()
{
    if (VDBG) Logd(String("clearDefaultsForInactiveSubIds"));
    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->ClearDefaultsForInactiveSubIds();
    }
    return NOERROR;
}

ECode SubscriptionManager::IsValidSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = subId > ISubscriptionManager::INVALID_SUB_ID ;
    return NOERROR;
}

ECode SubscriptionManager::IsValidSlotId(
    /* [in] */ Int32 slotId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // We are testing INVALID_SLOT_ID and slotId >= 0 independently because we should
    // not assume that INVALID_SLOT_ID will always be a negative value.  Any negative
    // value is invalid.
    AutoPtr<ITelephonyManager> tm;
    AutoPtr<ITelephonyManagerHelper> tmHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmHelper);
    tmHelper->GetDefault((ITelephonyManager**)&tm);
    Int32 count;
    tm->GetSimCount(&count);
    *result = slotId != ISubscriptionManager::INVALID_SLOT_ID && slotId >= 0 &&
            slotId < count;
    return NOERROR;
}

ECode SubscriptionManager::IsValidPhoneId(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    // We are testing INVALID_PHONE_ID and phoneId >= 0 independently because we should
    // not assume that INVALID_PHONE_ID will always be a negative value.  Any negative
    // value is invalid.
    AutoPtr<ITelephonyManager> tm;
    AutoPtr<ITelephonyManagerHelper> tmHelper;
    CTelephonyManagerHelper::AcquireSingleton((ITelephonyManagerHelper**)&tmHelper);
    tmHelper->GetDefault((ITelephonyManager**)&tm);
    Int32 count;
    tm->GetPhoneCount(&count);
    *result = phoneId != ISubscriptionManager::INVALID_PHONE_ID && phoneId >= 0 &&
            phoneId < count;
    return NOERROR;
}

ECode SubscriptionManager::PutPhoneIdAndSubIdExtra(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 phoneId)
{
    AutoPtr<ArrayOf<Int64> > subIds;
    SubscriptionManager::GetSubId(phoneId, (ArrayOf<Int64>**)&subIds);

    if (subIds != NULL && subIds->GetLength() > 0) {
        PutPhoneIdAndSubIdExtra(intent, phoneId, (*subIds)[0]);
    }
    else {
        Logd(String("putPhoneIdAndSubIdExtra: no valid subs"));
    }
    return NOERROR;
}

ECode SubscriptionManager::PutPhoneIdAndSubIdExtra(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 phoneId,
    /* [in] */ Int64 subId)
{
    if (VDBG) Logd(String("putPhoneIdAndSubIdExtra: phoneId=") + StringUtils::ToString(phoneId) +
            String(" subId=") + StringUtils::ToString(subId));
    intent->PutExtra(IPhoneConstants::SUBSCRIPTION_KEY, subId);
    intent->PutExtra(IPhoneConstants::PHONE_KEY, phoneId);
    //FIXME this is using phoneId and slotId interchangeably
    //Eventually, this should be removed as it is not the slot id
    intent->PutExtra(IPhoneConstants::SLOT_KEY, phoneId);
    return NOERROR;
}

ECode SubscriptionManager::GetActiveSubIdList(
    /* [out, callee] */ ArrayOf<Int64>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int64> > subId;

    AutoPtr<IISub> iSub = IISub::Probe(ServiceManager::GetService(String("isub")));
    if (iSub != NULL) {
        iSub->GetActiveSubIdList((ArrayOf<Int64>**)&subId);
    }

    if (subId == NULL) {
        subId = ArrayOf<Int64>::Alloc(0);
    }

    *result = subId;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > SubscriptionManager::SetSimResource(
    /* [in] */ Int32 type)
{
    AutoPtr<ArrayOf<Int32> > simResource;

    switch (type) {
        case RES_TYPE_BACKGROUND_DARK:
            simResource = ArrayOf<Int32>::Alloc(4);
            simResource->Set(0, R::drawable::sim_dark_blue);
            simResource->Set(1, R::drawable::sim_dark_orange);
            simResource->Set(2, R::drawable::sim_dark_green);
            simResource->Set(3, R::drawable::sim_dark_purple);
            break;
        case RES_TYPE_BACKGROUND_LIGHT:
            simResource = ArrayOf<Int32>::Alloc(4);
            simResource->Set(0, R::drawable::sim_light_blue);
            simResource->Set(1, R::drawable::sim_light_orange);
            simResource->Set(2, R::drawable::sim_light_green);
            simResource->Set(3, R::drawable::sim_light_purple);
            break;
    }

    return simResource;
}

void SubscriptionManager::Logd(
    /* [in] */ const String& msg)
{
    Logger::D(TAG, "[SubscriptionManager] %s", msg.string());
}

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

