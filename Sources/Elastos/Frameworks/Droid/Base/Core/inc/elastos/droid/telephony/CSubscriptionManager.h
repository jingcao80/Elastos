
#ifndef __ELASTOS_DROID_TELEPHONY_CSUBSCRIPTIONMANAGER_H__
#define __ELASTOS_DROID_TELEPHONY_CSUBSCRIPTIONMANAGER_H__

#include "_Elastos_Droid_Telephony_CSubscriptionManager.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telephony {

CarClass(CSubscriptionManager)
    , public Singleton
    , public ISubscriptionManager
    , public IBaseColumns
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * Get the SubInfoRecord associated with the subId
     * @param subId The unique SubInfoRecord index in database
     * @return SubInfoRecord, maybe null
     * @hide - to be unhidden
     */
    CARAPI GetSubInfoForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ ISubInfoRecord** sir);

    /**
     * Get the SubInfoRecord according to an IccId
     * @param iccId the IccId of SIM card
     * @return SubInfoRecord List, maybe empty but not null
     * @hide
     */
    CARAPI GetSubInfoUsingIccId(
        /* [in] */ const String& iccId,
        /* [out] */ IList** ids);

    /**
     * Get the SubInfoRecord according to slotId
     * @param slotId the slot which the SIM is inserted
     * @return SubInfoRecord list, maybe empty but not null
     * @hide - to be unhidden
     */
    CARAPI GetSubInfoUsingSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ IList** ids);

    /**
     * Get all the SubInfoRecord(s) in subInfo database
     * @return List of all SubInfoRecords in database, include those that were inserted before
     * maybe empty but not null.
     * @hide
     */
    CARAPI GetAllSubInfoList(
        /* [out] */ IList** ids);

    /**
     * Get the SubInfoRecord(s) of the currently inserted SIM(s)
     * @return Array list of currently inserted SubInfoRecord(s) maybe empty but not null
     * @hide - to be unhidden
     */
    CARAPI GetActiveSubInfoList(
        /* [out] */ IList** ids);

    /**
     * Get the SUB count of all SUB(s) in subinfo database
     * @return all SIM count in database, include what was inserted before
     * @hide
     */
    CARAPI GetAllSubInfoCount(
        /* [out] */ Int32* count);

    /**
     * Get the count of active SUB(s)
     * @return active SIM count
     * @hide
     */
    CARAPI GetActiveSubInfoCount(
        /* [out] */ Int32* count);

    /**
     * Add a new SubInfoRecord to subinfo database if needed
     * @param iccId the IccId of the SIM card
     * @param slotId the slot which the SIM is inserted
     * @return the URL of the newly created row or the updated row
     * @hide
     */
    CARAPI AddSubInfoRecord(
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
    CARAPI SetColor(
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
    CARAPI SetDisplayName(
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
    CARAPI SetDisplayName(
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
    CARAPI SetDisplayNumber(
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
    CARAPI SetDisplayNumberFormat(
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
    CARAPI SetDataRoaming(
        /* [in] */ Int32 roaming,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Get slotId associated with the subscription.
     * @return slotId as a positive integer or a negative value if an error either
     * SIM_NOT_INSERTED or INVALID_SLOT_ID.
     * @hide - to be unhidden
     */
    CARAPI GetSlotId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetSubId(
        /* [in] */ Int32 slotId,
        /* [out, callee] */ ArrayOf<Int64>** result);

    /** @hide */
    CARAPI GetPhoneId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * @return the "system" defaultSubId on a voice capable device this
     * will be getDefaultVoiceSubId() and on a data only device it will be
     * getDefaultDataSubId().
     * @hide
     */
    CARAPI GetDefaultSubId(
        /* [out] */ Int64* result);

    /** @hide */
    CARAPI GetDefaultVoiceSubId(
        /* [out] */ Int64* result);

    /** @hide */
    CARAPI SetDefaultVoiceSubId(
        /* [in] */ Int64 id);

    /** @hide */
    CARAPI GetDefaultVoiceSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    CARAPI GetDefaultVoicePhoneId(
        /* [out] */ Int32* result);

    /**
     * @return subId of the DefaultSms subscription or the value INVALID_SUB_ID if an error.
     * @hide - to be unhidden
     */
    CARAPI GetDefaultSmsSubId(
        /* [out] */ Int64* result);

    /** @hide */
    CARAPI SetDefaultSmsSubId(
        /* [in] */ Int64 result);

    /** @hide */
    CARAPI GetDefaultSmsSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    CARAPI GetDefaultSmsPhoneId(
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI GetDefaultDataSubId(
        /* [out] */ Int64* result);

    /** @hide */
    CARAPI SetDefaultDataSubId(
        /* [in] */ Int64 result);

    /** @hide */
    CARAPI GetDefaultDataSubInfo(
        /* [out] */ ISubInfoRecord** result);

    /** @hide */
    CARAPI GetDefaultDataPhoneId(
        /* [out] */ Int32* result);

    /** @hide */
    CARAPI ClearSubInfo();

    //FIXME this is vulnerable to race conditions
    /** @hide */
    CARAPI AllDefaultsSelected(
        /* [out] */ Boolean* result);

    /**
     * If a default is set to subscription which is not active, this will reset that default back to
     * INVALID_SUB_ID.
     * @hide
     */
    CARAPI ClearDefaultsForInactiveSubIds();

    /**
     * @return true if a valid subId else false
     * @hide - to be unhidden
     */
    CARAPI IsValidSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsValidSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI IsValidPhoneId(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI PutPhoneIdAndSubIdExtra(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 phoneId);

    /** @hide */
    CARAPI PutPhoneIdAndSubIdExtra(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 phoneId,
        /* [in] */ Int64 subId);

    /**
     * @return the list of subId's that are active,
     *         is never null but the length maybe 0.
     * @hide
     */
    CARAPI GetActiveSubIdList(
        /* [out, callee] */ ArrayOf<Int64>** result);
};

} //namespace Elastos
} //namespace Droid
} //namespace Telephony

#endif // __ELASTOS_DROID_TELEPHONY_CSUBSCRIPTIONMANAGER_H__
