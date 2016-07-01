
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONCONTROLLER_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Internal.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncResult.h"

#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Internal::Telephony::DataConnection::IDctController;
using Elastos::Droid::Internal::Telephony::DataConnection::IDdsScheduler;
using Elastos::Droid::Internal::Telephony::DataConnection::IDdsSchedulerAc;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::AsyncResult;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Text::Format::ITime;

using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class SubscriptionController
    : public Object
    , public ISubscriptionController
    , public IISub
{
public:
    /**
     * Copied from android.util.LocalLog with Flush() adding flush and line number
     * TODO: Update LocalLog
     */
    class ScLocalLog
        : public Object
    {
    public:
        ScLocalLog(
            /* [in] */ Int32 maxLines);

        CARAPI_(void) Log(
            /* [in] */ String msg);

        CARAPI_(void) Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

    private:
        AutoPtr<ILinkedList> mLog;
        Int32 mMaxLines;
        AutoPtr<ITime> mNow;
    };

private:
    class DataConnectionHandler
        : public Handler
    {
    public:
        DataConnectionHandler(
            /* [in] */ SubscriptionController* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        SubscriptionController* mHost;
    };

    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ SubscriptionController* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        SubscriptionController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SubscriptionController();

    static CARAPI_(AutoPtr<ISubscriptionController>) Init(
        /* [in] */ IPhone* phone);

    static CARAPI_(AutoPtr<ISubscriptionController>) Init(
        /* [in] */ IContext* c,
        /* [in] */ ArrayOf<ICommandsInterface*>* ci);

    static CARAPI_(AutoPtr<ISubscriptionController>) GetInstance();

    CARAPI constructor(
        /* [in] */ IContext* c);

    CARAPI constructor(
        /* [in] */ IPhone* phone);

    CARAPI GetSubIdFromNetworkRequest(
        /* [in] */ INetworkRequest* n,
        /* [out] */ Int64* result);

    CARAPI StartOnDemandDataSubscriptionRequest(
        /* [in] */ INetworkRequest* n);

    CARAPI StopOnDemandDataSubscriptionRequest(
        /* [in] */ INetworkRequest* n);

    /**
     * Get the SubInfoRecord according to an index
     * @param subId The unique SubInfoRecord index in database
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    CARAPI GetSubInfoForSubscriber(
        /* [in] */ Int64 subId,
        /* [out] */ ISubInfoRecord** result);

    /**
     * Get the SubInfoRecord according to an IccId
     * @param iccId the IccId of SIM card
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    CARAPI GetSubInfoUsingIccId(
        /* [in] */ const String& iccId,
        /* [out] */ IList** result);

    /**
     * Get the SubInfoRecord according to slotId
     * @param slotId the slot which the SIM is inserted
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    CARAPI GetSubInfoUsingSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ IList** result);

    /**
     * Get all the SubInfoRecord(s) in subinfo database
     * @return Array list of all SubInfoRecords in database, include thsoe that were inserted before
     */
    //@Override
    CARAPI GetAllSubInfoList(
        /* [out] */ IList** result);

    /**
     * Get the SubInfoRecord(s) of the currently inserted SIM(s)
     * @return Array list of currently inserted SubInfoRecord(s)
     */
    //@Override
    CARAPI GetActiveSubInfoList(
        /* [out] */ IList** result);

    /**
     * Get the SUB count of active SUB(s)
     * @return active SIM count
     */
    //@Override
    CARAPI GetActiveSubInfoCount(
        /* [out] */ Int32* result);

    /**
     * Get the SUB count of all SUB(s) in subinfo database
     * @return all SIM count in database, include what was inserted before
     */
    //@Override
    CARAPI GetAllSubInfoCount(
        /* [out] */ Int32* result);

    /**
     * Add a new SubInfoRecord to subinfo database if needed
     * @param iccId the IccId of the SIM card
     * @param slotId the slot which the SIM is inserted
     * @return the URL of the newly created row or the updated row
     */
    //@Override
    CARAPI AddSubInfoRecord(
        /* [in] */ const String& iccId,
        /* [in] */ Int32 slotId,
        /* [out] */ Int32* result);

    /**
     * Set SIM color by simInfo index
     * @param color the color of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    CARAPI SetColor(
        /* [in] */ Int32 color,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set display name by simInfo index
     * @param displayName the display name of SIM card
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
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
     * @return the number of records updated
     */
    //@Override
    CARAPI SetDisplayNameUsingSrc(
        /* [in] */ const String& displayName,
        /* [in] */ Int64 subId,
        /* [in] */ Int64 nameSource,
        /* [out] */ Int32* result);

    /**
     * Set phone number by subId
     * @param number the phone number of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    CARAPI SetDisplayNumber(
        /* [in] */ const String& number,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set number display format. 0: none, 1: the first four digits, 2: the last four digits
     * @param format the display format of phone number
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    CARAPI SetDisplayNumberFormat(
        /* [in] */ Int32 format,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set data roaming by simInfo index
     * @param roaming 0:Don't allow data when roaming, 1:Allow data when roaming
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    CARAPI SetDataRoaming(
        /* [in] */ Int32 roaming,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Set MCC/MNC by subscription ID
     * @param mccMnc MCC/MNC associated with the subscription
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    CARAPI SetMccMnc(
        /* [in] */ const String& mccMnc,
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetSlotId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * Return the subId for specified slot Id.
     * @deprecated
     */
    //@Override
    //@Deprecated
    CARAPI GetSubId(
        /* [in] */ Int32 slotId,
        /* [out] */ ArrayOf<Int64>** result);

    //@Override
    CARAPI GetPhoneId(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /**
     * @return the number of records cleared
     */
    //@Override
    CARAPI ClearSubInfo(
        /* [out] */ Int32* result);

    //@Override
    //@Deprecated
    CARAPI GetDefaultSubId(
        /* [out] */ Int64* result);

    //@Override
    CARAPI SetDefaultSmsSubId(
        /* [in] */ Int64 subId);

    //@Override
    CARAPI GetDefaultSmsSubId(
        /* [out] */ Int64* result);

    //@Override
    CARAPI SetDefaultVoiceSubId(
        /* [in] */ Int64 subId);

    //@Override
    CARAPI GetDefaultVoiceSubId(
        /* [out] */ Int64* result);

    /* Returns User SMS Prompt property,  enabled or not */
    //@Override
    CARAPI IsSMSPromptEnabled(
        /* [out] */ Boolean* result);

    /*Sets User SMS Prompt property,  enable or not */
    //@Override
    CARAPI SetSMSPromptEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI GetDefaultDataSubId(
        /* [out] */ Int64* result);

    //@Override
    CARAPI SetDefaultDataSubId(
        /* [in] */ Int64 subId);

    CARAPI GetCurrentDds(
        /* [out] */ Int64* result);

    CARAPI SetDataSubId(
        /* [in] */ Int64 subId);

    /* Sets the default subscription. If only one sub is active that
     * sub is set as default subId. If two or more  sub's are active
     * the first sub is set as default subscription
     */
    // FIXME
    CARAPI SetDefaultSubId(
        /* [in] */ Int64 subId);

    //@Override
    CARAPI ClearDefaultsForInactiveSubIds();

    /* This should return Int64 and not Int64 [] since each phone has
     * exactly 1 sub id for now, it could return the 0th element
     * returned from GetSubId()
     */
    // FIXME will design a mechanism to manage the relationship between PhoneId/SlotId/SubId
    // since phoneId = SlotId is not always TRUE
    CARAPI GetSubIdUsingPhoneId(
        /* [in] */ Int32 phoneId,
        /* [out] */ Int64* result);

    CARAPI GetSubIdUsingSlotId(
        /* [in] */ Int32 slotId,
        /* [out] */ ArrayOf<Int64>** result);

    CARAPI GetSubInfoUsingSlotIdWithCheck(
        /* [in] */ Int32 slotId,
        /* [in] */ Boolean needCheck,
        /* [out] */ IList** result);

    CARAPI UpdatePhonesAvailability(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones);

    /**
     * @return the list of subId's that are active, is never NULL but the length maybe 0.
     */
    //@Override
    CARAPI GetActiveSubIdList(
        /* [out] */ ArrayOf<Int64>** result);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    //@Override
    CARAPI ActivateSubId(
        /* [in] */ Int64 subId);

    //@Override
    CARAPI DeactivateSubId(
        /* [in] */ Int64 subId);

    CARAPI SetNwMode(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 nwMode);

    CARAPI GetNwMode(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    //@Override
    CARAPI SetSubState(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 subStatus,
        /* [out] */ Int32* result);

    //@Override
    CARAPI GetSubState(
        /* [in] */ Int64 subId,
        /* [out] */ Int32* result);

    /* setDds flag is used to trigger DDS switch request during
      device powerUp and when flex map performed */
    CARAPI UpdateUserPrefs(
        /* [in] */ Boolean setDds);

    /* Returns User Voice Prompt property,  enabled or not */
    //@Override
    CARAPI IsVoicePromptEnabled(
        /* [out] */ Boolean* result);

    /*Sets User SMS Prompt property,  enable or not */
    //@Override
    CARAPI SetVoicePromptEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI GetOnDemandDataSubId(
        /* [out] */ Int64* result);

    CARAPI RegisterForOnDemandDdsLockNotification(
        /* [in] */ Int64 clientSubId,
        /* [in] */ ISubscriptionControllerOnDemandDdsLockNotifier* callback);

    /* {@hide} */
    CARAPI NotifyOnDemandDataSubIdChanged(
        /* [in] */ INetworkRequest* n);

    CARAPI RemoveStaleSubPreferences(
        /* [in] */ const String& prefKey);

    CARAPI_(Boolean) IsSubInfoReady();

    /**
     * Make sure the caller has the READ_PHONE_STATE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    CARAPI_(void) EnforceSubscriptionPermission();

    /**
     * Broadcast when subinfo settings has chanded
     * @SubId The unique SubInfoRecord index in database
     * @param columnName The column that is updated
     * @param intContent The updated integer value
     * @param stringContent The updated string value
     */
    CARAPI_(void) BroadcastSimInfoContentChanged(
        /* [in] */ Int64 subId,
        /* [in] */ String columnName,
        /* [in] */ Int32 intContent,
        /* [in] */ String stringContent);

    /**
     * New SubInfoRecord instance and fill in detail info
     * @param cursor
     * @return the query result of desired SubInfoRecord
     */
    CARAPI_(AutoPtr<ISubInfoRecord>) GetSubInfoRecord(
        /* [in] */ ICursor* cursor);

    /**
     * Query SubInfoRecord(s) from subinfo database
     * @param selection A filter declaring which rows to return
     * @param queryKey query key content
     * @return Array list of queried result from database
     */
    CARAPI_(AutoPtr<IList>) GetSubInfo(
        /* [in] */ String selection,
        /* [in] */ IInterface* queryKey);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) SetSimResource(
        /* [in] */ Int32 type);

    CARAPI_(void) Logvl(
        /* [in] */ String msg);

    CARAPI_(void) Logv(
        /* [in] */ String msg);

    CARAPI_(void) Logdl(
        /* [in] */ String msg);

    static CARAPI_(void) Slogd(
        /* [in] */ String msg);

    CARAPI_(void) Logd(
        /* [in] */ String msg);

    CARAPI_(void) Logel(
        /* [in] */ String msg);

    CARAPI_(void) Loge(
        /* [in] */ String msg);

    CARAPI_(void) BroadcastDefaultSmsSubIdChanged(
        /* [in] */ Int64 subId);

    CARAPI_(void) BroadcastDefaultVoiceSubIdChanged(
        /* [in] */ Int64 subId);

    CARAPI_(void) UpdateDataSubId(
        /* [in] */ AsyncResult* ar);

    CARAPI_(void) UpdateAllDataConnectionTrackers();

    CARAPI_(void) BroadcastDefaultDataSubIdChanged(
        /* [in] */ Int32 status);

    CARAPI_(Boolean) ShouldDefaultBeCleared(
        /* [in] */ IList* records,
        /* [in] */ Int64 subId);

    CARAPI_(void) ValidateSubId(
        /* [in] */ Int64 subId);

    static CARAPI_(void) PrintStackTrace(
        /* [in] */ String msg);

public:
    static const String LOGTAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    static const Int32 MAX_LOCAL_LOG_LINES; // TODO: Reduce to 100 when 17678050 is fixed

    Object mLock;
    Boolean mSuccess;

    /** The singleton instance. */
    static AutoPtr<ArrayOf<IPhoneProxy*> > sProxyPhones;
    AutoPtr<IContext> mContext;
    AutoPtr<ICallManager> mCM;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ScLocalLog> mLocalLog;

    static AutoPtr<ISubscriptionController> sInstance;

    static const Int32 RES_TYPE_BACKGROUND_DARK;
    static const Int32 RES_TYPE_BACKGROUND_LIGHT;

    static const AutoPtr<ArrayOf<Int32> > sSimBackgroundDarkRes;
    static const AutoPtr<ArrayOf<Int32> > sSimBackgroundLightRes;

    //FIXME this does not allow for multiple subs in a slot
    static AutoPtr<IHashMap> mSimInfo;
    static Int64 mDefaultVoiceSubId;
    static Int32 mDefaultPhoneId;

    static const Int32 EVENT_WRITE_MSISDN_DONE;

    static const Int32 EVENT_SET_DEFAULT_DATA_DONE;
    AutoPtr<DataConnectionHandler> mDataConnectionHandler;
    AutoPtr<IDctController> mDctController;

    AutoPtr<IHashMap> mOnDemandDdsLockNotificationRegistrants;

    AutoPtr<IDdsScheduler> mScheduler;
    AutoPtr<IDdsSchedulerAc> mSchedulerAc;

    // Dummy subId is used when no SIMs present on device
    // with MSIM configuration and this is corresponds
    // to phoneId 0.
    static const Int32 DUMMY_SUB_ID;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_SUBSCRIPTIONCONTROLLER_H__
