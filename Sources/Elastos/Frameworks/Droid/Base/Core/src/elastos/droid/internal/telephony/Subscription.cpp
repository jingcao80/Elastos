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

#include "elastos/droid/internal/telephony/Subscription.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Droid::Text::TextUtils;
//using Elastos::Droid::Telephony::IRlog;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Class holding all the information of a subscription from UICC Card.
 */
CAR_INTERFACE_IMPL(Subscription, Object, ISubscription)

const String LOGTAG("Subscription");

Subscription::Subscription()
{
    Clear();
    DEBUG = FALSE;
}

ECode Subscription::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder result;
    result += "Subscription = { mSlotId = ";
    result += mSlotId;
    result += ", m3gppIndex = ";
    result += m3gppIndex;
    result += ", m3gpp2Index = ";
    result += m3gpp2Index;
    result += ", mSubId = ";
    result += mSubId;
    result += ", subStatus = ";
    result += mSubStatus;
    result += ", mAppId = ";
    result += mAppId;
    result += ", mAppLabel = ";
    result += mAppLabel;
    result += ", mAppType = ";
    result += mAppType;
    result += ", mIccId = ";
    result += mIccId;
    result += " }";
    *str = result.ToString();
    return NOERROR;
}

ECode Subscription::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ISubscription> sub = ISubscription::Probe(obj);
    if (sub != NULL) {
        Int32 slotId, m3gppIndexGet, m3gpp2IndexGet, subId;
        SubscriptionStatus subStatus;
        String appId, appLabel, appType, iccId;
        sub->GetSlotId(&slotId);
        sub->Get3gppIndex(&m3gppIndexGet);
        sub->Get3gpp2Index(&m3gpp2IndexGet);
        sub->GetSubId(&subId);
        sub->GetSubscriptionStatus(&subStatus);
        sub->GetAppId(&appId);
        sub->GetAppLabel(&appLabel);
        sub->GetAppType(&appType);
        sub->GetIccId(&iccId);

        if ((mSlotId == slotId) && (m3gppIndex == m3gppIndexGet)
                && (m3gpp2Index == m3gpp2IndexGet) && (mSubId == subId)
                && (mSubStatus == subStatus)
                && ((TextUtils::IsEmpty(mAppId) && TextUtils::IsEmpty(appId))
                        || TextUtils::Equals(mAppId, appId))
                && ((TextUtils::IsEmpty(mAppLabel) && TextUtils::IsEmpty(appLabel))
                        || TextUtils::Equals(mAppLabel, appLabel))
                && ((TextUtils::IsEmpty(mAppType) && TextUtils::IsEmpty(appType))
                        || TextUtils::Equals(mAppType, appType))
                && ((TextUtils::IsEmpty(mIccId) && TextUtils::IsEmpty(mIccId))
                        || TextUtils::Equals(mIccId, iccId))) {
            *result = TRUE;
            return NOERROR;
        }
    } else {
//        Rlog->D(LOGTAG, "Subscription.equals: sub == NULL");
    }
    *result = FALSE;
    return NOERROR;
}

/**
 * Return TRUE if the appIndex, appId, appLabel and iccId are matching.
 * @param sub
 * @return
 */
ECode Subscription::IsSame(
    /* [in] */ ISubscription* sub,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    // Not checking the subId, subStatus and slotId, which are related to the
    // activated status
    if (sub != NULL) {
        Int32 slotId, m3gppIndexGet, m3gpp2IndexGet, subId;
        String appId, appLabel, appType, iccId;
        sub->GetSlotId(&slotId);
        sub->Get3gppIndex(&m3gppIndexGet);
        sub->Get3gpp2Index(&m3gpp2IndexGet);
        sub->GetSubId(&subId);
        sub->GetAppId(&appId);
        sub->GetAppLabel(&appLabel);
        sub->GetAppType(&appType);
        sub->GetIccId(&iccId);

//        if (DEBUG) {
//            Rlog->D(LOGTAG, "IsSame(): this = " + m3gppIndex
//                    + ":" + m3gpp2Index
//                    + ":" + mAppId
//                    + ":" + mAppType
//                    + ":" + mIccId);
//            Rlog->D(LOGTAG, "compare with = " + m3gppIndexGet
//                    + ":" + m3gpp2IndexGet
//                    + ":" + appId
//                    + ":" + appType
//                    + ":" + iccId);
//        }
        if ((m3gppIndex == m3gppIndexGet)
                && (m3gpp2Index == m3gpp2IndexGet)
                && ((TextUtils::IsEmpty(mAppId) && TextUtils::IsEmpty(appId))
                        || TextUtils::Equals(mAppId, appId))
                && ((TextUtils::IsEmpty(mAppType) && TextUtils::IsEmpty(appType))
                        || TextUtils::Equals(mAppType, appType))
                && ((TextUtils::IsEmpty(mIccId) && TextUtils::IsEmpty(iccId))
                        || TextUtils::Equals(mIccId, iccId))){
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

/**
 * Reset the subscription
 */
ECode Subscription::Clear()
{
    mSlotId = ISubscription::SUBSCRIPTION_INDEX_INVALID;
    m3gppIndex = ISubscription::SUBSCRIPTION_INDEX_INVALID;
    m3gpp2Index = ISubscription::SUBSCRIPTION_INDEX_INVALID;
    mSubId = ISubscription::SUBSCRIPTION_INDEX_INVALID;
    mSubStatus = SubscriptionStatus_SUB_INVALID;
    mAppId = NULL;
    mAppLabel = NULL;
    mAppType = NULL;
    mIccId = NULL;
    return NOERROR;
}

/**
 * Copies the subscription parameters
 * @param from
 * @return
 */
ECode Subscription::CopyFrom(
    /* [in] */ ISubscription* from,
    /* [out] */ ISubscription** result)
{
    if (from != NULL) {
        Int32 slotId, m3gppIndexGet, m3gpp2IndexGet, subId;
        SubscriptionStatus subStatus;
        String appId, appLabel, appType, iccId;
        from->GetSlotId(&slotId);
        from->Get3gppIndex(&m3gppIndexGet);
        from->Get3gpp2Index(&m3gpp2IndexGet);
        from->GetSubId(&subId);
        from->GetSubscriptionStatus(&subStatus);
        from->GetAppId(&appId);
        from->GetAppLabel(&appLabel);
        from->GetAppType(&appType);
        from->GetIccId(&iccId);

        mSlotId = slotId;
        m3gppIndex = m3gppIndexGet;
        m3gpp2Index = m3gpp2IndexGet;
        mSubId = subId;
        mSubStatus = subStatus;
        if (!(appId.IsNull())) {
            mAppId = appId;
        }
        if (!(appLabel.IsNull())) {
            mAppLabel = appLabel;
        }
        if (!(appType.IsNull())) {
            mAppType = appType;
        }
        if (!(iccId.IsNull())) {
            mIccId = iccId;
        }
    }
    if (result != NULL) {
        *result = this;
        REFCOUNT_ADD(*result)
    }
    return NOERROR;
}

/**
 * Return the valid app Index (either 3gpp or 3gpp2 index)
 * @return
 */
ECode Subscription::GetAppIndex(
        /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)

    if (m3gppIndex != ISubscription::SUBSCRIPTION_INDEX_INVALID) {
        *result = m3gppIndex;
        return NOERROR;
    } else {
        *result = m3gpp2Index;
        return NOERROR;
    }
}

ECode Subscription::GetSlotId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSlotId;
    return NOERROR;
}

ECode Subscription::SetSlotId(
    /* [in] */ Int32 slotId)
{
    mSlotId = slotId;
    return NOERROR;
}

ECode Subscription::Get3gppIndex(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = m3gppIndex;
    return NOERROR;
}

ECode Subscription::Set3gppIndex(
    /* [in] */ Int32 index)
{
    m3gppIndex = index;
    return NOERROR;
}

ECode Subscription::Get3gpp2Index(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = m3gpp2Index;
    return NOERROR;
}

ECode Subscription::Set3gpp2Index(
    /* [in] */ Int32 index)
{
    m3gpp2Index = index;
    return NOERROR;
}

ECode Subscription::GetSubId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubId;
    return NOERROR;
}

ECode Subscription::SetSubId(
    /* [in] */ Int32 subId)
{
    mSubId = subId;
    return NOERROR;
}

ECode Subscription::GetSubscriptionStatus(
    /* [out] */ SubscriptionStatus* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubStatus;
    return NOERROR;
}

ECode Subscription::SetSubscriptionStatus(
    /* [in] */ SubscriptionStatus status)
{
    mSubStatus = status;
    return NOERROR;
}

ECode Subscription::GetAppId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAppId;
    return NOERROR;
}

ECode Subscription::SetAppId(
    /* [in] */ const String& appId)
{
    mAppId = appId;
    return NOERROR;
}

ECode Subscription::GetAppLabel(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAppLabel;
    return NOERROR;
}

ECode Subscription::SetAppLabel(
    /* [in] */ const String& label)
{
    mAppLabel = label;
    return NOERROR;
}

ECode Subscription::GetAppType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAppType;
    return NOERROR;
}

ECode Subscription::SetAppType(
    /* [in] */ const String& type)
{
    mAppType = type;
    return NOERROR;
}

ECode Subscription::GetIccId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIccId;
    return NOERROR;
}

ECode Subscription::SetIccId(
    /* [in] */ const String& iccId)
{
    mIccId = iccId;
    return NOERROR;
}

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
