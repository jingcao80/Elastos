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

#ifndef __ELASTOS_DROID_TELEPHONY_SUBSCRIPTIONMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_SUBSCRIPTIONMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telephony {

class SubscriptionManager
{
public:
    static CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Get the SubInfoRecord associated with the subId
     * @param subId The unique SubInfoRecord index in database
     * @return SubInfoRecord, maybe null
     * @hide - to be unhidden
     */
    static CARAPI GetSubInfoForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ ISubInfoRecord** sir);

    /**
     * Get the SubInfoRecord according to an IccId
     * @param iccId the IccId of SIM card
     * @return SubInfoRecord List, maybe empty but not null
     * @hide
     */
    static CARAPI GetSubInfoUsingIccId(
        /* [in] */ const String& iccId,
        /* [out] */ IList** ids);

    /**
     * Get the SubInfoRecord according to slotId
     * @param slotId the slot which the SIM is inserted
     * @return SubInfoRecord list, maybe empty but not null
     * @hide - to be unhidden
     */
    static CARAPI GetSubInfoUsingSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ IList** ids);

    /**
     * Get all the SubInfoRecord(s) in subInfo database
     * @return List of all SubInfoRecords in database, include those that were inserted before
     * maybe empty but not null.
     * @hide
     */
    static CARAPI GetAllSubInfoList(
        /* [out] */ IList** ids);

    /**
     * Get the SubInfoRecord(s) of the currently inserted SIM(s)
     * @return Array list of currently inserted SubInfoRecord(s) maybe empty but not null
     * @hide - to be unhidden
     */
    static CARAPI GetActiveSubInfoList(
        /* [out] */ IList** ids);

    /**
     * Get the SUB count of all SUB(s) in subinfo database
     * @return all SIM count in database, include what was inserted before
     * @hide
     */
    static CARAPI GetAllSubInfoCount(
        /* [out] */ Int32* count);

    /**
     * Get the count of active SUB(s)
     * @return active SIM count
     * @hide
     */
    static CARAPI GetActiveSubInfoCount(
        /* [out] */ Int32* count);

    /**
     * Add a new SubInfoRecord to subinfo database if needed
     * @param iccId the IccId of the SIM card
     * @param slotId the slot which the SIM is inserted
     * @return the URL of the newly created row or the updated row
     * @hide
     */
    static CARAPI AddSubInfoRecord(
        /* [in] */ const String& iccId,
        /* [in] */ Int32 slotId,
        /* [out] */ IUri** uri);

    /**
     * Set SIM color by simInfo index
     * @param color the color of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     * @hide
     */
    static CARAPI SetColor(
        /* [in] */ Int32 color,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set display name by simInfo index
     * @param displayName the display name of SIM card
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     * @hide
     */
    static CARAPI SetDisplayName(
        /* [in] */ const String& displayName,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set display name by simInfo index with name source
     * @param displayName the display name of SIM card
     * @param subId the unique SubInfoRecord index in database
     * @param nameSource 0: NAME_SOURCE_DEFAULT_SOURCE, 1: NAME_SOURCE_SIM_SOURCE,
     *                   2: NAME_SOURCE_USER_INPUT, -1 NAME_SOURCE_UNDEFINED
     * @return the number of records updated or -1 if invalid subId
     * @hide
     */
    static CARAPI SetDisplayName(
        /* [in] */ const String& displayName,
        /* [in] */ Int64 subId,
        /* [in] */ Int64 nameSource,
        /* [out] */ Int32* result);

    /**
     * Set phone number by subId
     * @param number the phone number of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     * @hide
     */
    static CARAPI SetDisplayNumber(
        /* [in] */ const String& number,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set number display format. 0: none, 1: the first four digits, 2: the last four digits
     * @param format the display format of phone number
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     * @hide
     */
    static CARAPI SetDisplayNumberFormat(
        /* [in] */ Int32 format,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set data roaming by simInfo index
     * @param roaming 0:Don't allow data when roaming, 1:Allow data when roaming
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     * @hide
     */
    static CARAPI SetDataRoaming(
        /* [in] */ Int32 roaming,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Get slotId associated with the subscription.
     * @return slotId as a positive integer or a negative value if an error either
     * SIM_NOT_INSERTED or INVALID_SLOT_ID.
     * @hide - to be unhidden
     */
    static CARAPI GetSlotId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI GetSubId(
        /* [in] */ Int32 slotId,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /** @hide */
    static CARAPI GetPhoneId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * @return the "system" defaultSubId on a voice capable device this
     * will be getDefaultVoiceSubId() and on a data only device it will be
     * getDefaultDataSubId().
     * @hide
     */
    static CARAPI GetDefaultSubId(
        /* [out] */ Int64* result);

    /** @hide */
    static CARAPI GetDefaultVoiceSubId(
        /* [out] */ Int64* result);

    /** @hide */
    static CARAPI SetDefaultVoiceSubId(
        /* [in] */ Int64 subId);

    /** @hide */
    static CARAPI GetDefaultVoiceSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    static CARAPI GetDefaultVoicePhoneId(
        /* [out] */ Int32* result);

    /**
     * @return subId of the DefaultSms subscription or the value INVALID_SUB_ID if an error.
     * @hide - to be unhidden
     */
    static CARAPI GetDefaultSmsSubId(
        /* [out] */ Int64* result);

    /** @hide */
    static CARAPI SetDefaultSmsSubId(
        /* [in] */ Int64 result);

    /** @hide */
    static CARAPI GetDefaultSmsSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    static CARAPI GetDefaultSmsPhoneId(
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI GetDefaultDataSubId(
        /* [out] */ Int64* result);

    /** @hide */
    static CARAPI SetDefaultDataSubId(
        /* [in] */ Int64 result);

    /** @hide */
    static CARAPI GetDefaultDataSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    static CARAPI GetDefaultDataPhoneId(
        /* [out] */ Int32* result);

    /** @hide */
    static CARAPI ClearSubInfo();

    //FIXME this is vulnerable to race conditions
    /** @hide */
    static CARAPI AllDefaultsSelected(
        /* [out] */ Boolean* result);

    /**
     * If a default is set to subscription which is not active, this will reset that default back to
     * INVALID_SUB_ID.
     * @hide
     */
    static CARAPI ClearDefaultsForInactiveSubIds();

    /**
     * @return true if a valid subId else false
     * @hide - to be unhidden
     */
    static CARAPI IsValidSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI IsValidSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI IsValidPhoneId(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    /** @hide */
    static CARAPI PutPhoneIdAndSubIdExtra(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 phoneId);

    /** @hide */
    static CARAPI PutPhoneIdAndSubIdExtra(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 phoneId,
        /* [in] */ Int64 subId);

    /**
     * @return the list of subId's that are active,
     *         is never null but the length maybe 0.
     * @hide
     */
    static CARAPI GetActiveSubIdList(
        /* [out, callee] */ ArrayOf<Int64>** result);

private:
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) SetSimResource(
        /* [in] */ Int32 type);

    static CARAPI_(void) Logd(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;

    static const Int32 RES_TYPE_BACKGROUND_DARK;

    static const Int32 RES_TYPE_BACKGROUND_LIGHT;

    static AutoPtr<ArrayOf<Int32> > sSimBackgroundDarkRes;
};

} // namespace Telephony
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TELEPHONY_SUBSCRIPTIONMANAGER_H__
