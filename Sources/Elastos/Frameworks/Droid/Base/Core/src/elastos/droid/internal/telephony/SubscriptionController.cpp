/*
* Copyright (C) 2011-2014 MediaTek Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

package com.android.internal.telephony;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Provider::IBaseColumns;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;

using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Telephony::IISub;
using Elastos::Droid::Internal::Telephony::Uicc::ISpnOverride;

using Elastos::Droid::Internal::Telephony::Dataconnection::IDctController;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDdsScheduler;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDdsSchedulerAc;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::ISubInfoRecord;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::Format::ITime;

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILinkedList;
using Elastos::Utility::IList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::Map::IEntry;
using Elastos::Utility::ISet;

/**
 * SubscriptionController to provide an inter-process communication to
 * access Sms in Icc.
 *
 * Any setters which take subId, slotId or phoneId as a parameter will throw an exception if the
 * parameter equals the corresponding INVALID_XXX_ID or DEFAULT_XXX_ID.
 *
 * All getters will lookup the corresponding default if the parameter is DEFAULT_XXX_ID. Ie calling
 * GetPhoneId(DEFAULT_SUB_ID) will return the same as GetPhoneId(GetDefaultSubId()).
 *
 * Finally, any getters which perform the mapping between subscriptions, slots and phones will
 * return the corresponding INVALID_XXX_ID if the parameter is INVALID_XXX_ID. All other getters
 * will fail and return the appropriate error value. Ie calling GetSlotId(INVALID_SUB_ID) will
 * return INVALID_SLOT_ID and calling GetSubInfoForSubscriber(INVALID_SUB_ID) will return NULL.
 *
 */
public class SubscriptionController extends ISub.Stub {
    static const String LOG_TAG = "SubController";
    static const Boolean DBG = TRUE;
    static const Boolean VDBG = FALSE;
    static const Int32 MAX_LOCAL_LOG_LINES = 500; // TODO: Reduce to 100 when 17678050 is fixed
    private ScLocalLog mLocalLog = new ScLocalLog(MAX_LOCAL_LOG_LINES);

    /**
     * Copied from android.util.LocalLog with Flush() adding flush and line number
     * TODO: Update LocalLog
     */
    static class ScLocalLog {

        private LinkedList<String> mLog;
        private Int32 mMaxLines;
        private Time mNow;

        public ScLocalLog(Int32 maxLines) {
            mLog = new LinkedList<String>();
            mMaxLines = maxLines;
            mNow = new Time();
        }

        public synchronized void Log(String msg) {
            If (mMaxLines > 0) {
                Int32 pid = android.os.Process->MyPid();
                Int32 tid = android.os.Process->MyTid();
                mNow->SetToNow();
                mLog->Add(mNow->Format("%m-%d %H:%M:%S") + " pid=" + pid + " tid=" + tid + " " + msg);
                While (mLog->Size() > mMaxLines) mLog->Remove();
            }
        }

        public synchronized void Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
            final Int32 LOOPS_PER_FLUSH = 10; // Flush every N loops.
            Iterator<String> itr = mLog->ListIterator(0);
            Int32 i = 0;
            While (itr->HasNext()) {
                pw->Println(Integer->ToString(i++) + ": " + itr->Next());
                // Flush periodically so we don't drop lines
                If ((i % LOOPS_PER_FLUSH) == 0) pw->Flush();
            }
        }
    }

    protected final Object mLock = new Object();
    protected Boolean mSuccess;

    /** The singleton instance. */
    private static SubscriptionController sInstance = NULL;
    protected static PhoneProxy[] sProxyPhones;
    protected Context mContext;
    protected CallManager mCM;

    private static const Int32 RES_TYPE_BACKGROUND_DARK = 0;
    private static const Int32 RES_TYPE_BACKGROUND_LIGHT = 1;

    private static const Int32[] sSimBackgroundDarkRes = SetSimResource(RES_TYPE_BACKGROUND_DARK);
    private static const Int32[] sSimBackgroundLightRes = SetSimResource(RES_TYPE_BACKGROUND_LIGHT);

    //FIXME this does not allow for multiple subs in a slot
    private static HashMap<Integer, Long> mSimInfo = new HashMap<Integer, Long>();
    private static Int64 mDefaultVoiceSubId = SubscriptionManager.DEFAULT_SUB_ID;
    private static Int32 mDefaultPhoneId = 0;

    private static const Int32 EVENT_WRITE_MSISDN_DONE = 1;

    protected Handler mHandler = new Handler() {
        //@Override
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar;

            Switch (msg.what) {
                case EVENT_WRITE_MSISDN_DONE:
                    ar = (AsyncResult) msg.obj;
                    Synchronized (mLock) {
                        mSuccess = (ar.exception == NULL);
                        Logd("EVENT_WRITE_MSISDN_DONE, mSuccess = "+mSuccess);
                        mLock->NotifyAll();
                    }
                    break;
            }
        }
    };

    private static const Int32 EVENT_SET_DEFAULT_DATA_DONE = 1;
    private DataConnectionHandler mDataConnectionHandler;
    private  DctController mDctController;

    private HashMap<Long, OnDemandDdsLockNotifier> mOnDemandDdsLockNotificationRegistrants =
        new HashMap<Long, OnDemandDdsLockNotifier>();

    private DdsScheduler mScheduler;
    private DdsSchedulerAc mSchedulerAc;

    // Dummy subId is used when no SIMs present on device
    // with MSIM configuration and this is corresponds
    // to phoneId 0.
    private static const Int32 DUMMY_SUB_ID = -1;

    public static SubscriptionController Init(Phone phone) {
        Synchronized (SubscriptionController.class) {
            If (sInstance == NULL) {
                sInstance = new SubscriptionController(phone);
            } else {
                Log->Wtf(LOG_TAG, "Init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static SubscriptionController Init(Context c, CommandsInterface[] ci) {
        Synchronized (SubscriptionController.class) {
            If (sInstance == NULL) {
                sInstance = new SubscriptionController(c);
            } else {
                Log->Wtf(LOG_TAG, "Init() called multiple times!  sInstance = " + sInstance);
            }
            return sInstance;
        }
    }

    public static SubscriptionController GetInstance() {
        If (sInstance == NULL)
        {
           Log->Wtf(LOG_TAG, "getInstance NULL");
        }

        return sInstance;
    }

    private SubscriptionController(Context c) {
        Logd("SubscriptionController init by Context");
        mContext = c;
        mCM = CallManager->GetInstance();

        If(ServiceManager->GetService("isub") == NULL) {
                ServiceManager->AddService("isub", this);
        }

        Logdl("[SubscriptionController] init by Context");
        mDataConnectionHandler = new DataConnectionHandler();

        mScheduler = DdsScheduler->GetInstance();

        mSchedulerAc = new DdsSchedulerAc();
        mSchedulerAc->Connect(mContext, mDataConnectionHandler, mScheduler->GetHandler());

    }

    public Int64 GetSubIdFromNetworkRequest(NetworkRequest n) {
        Int64 subId;
        If (n == NULL) {
            return GetDefaultDataSubId();
        }

        String str = n.networkCapabilities->GetNetworkSpecifier();
        try {
            subId = Long->ParseLong(str);
        } Catch(NumberFormatException e) {
            Loge("Exception e = " + e);
            subId = GetDefaultDataSubId();
        }

        return subId;
    }

    CARAPI StartOnDemandDataSubscriptionRequest(NetworkRequest n) {
        Logd("startOnDemandDataSubscriptionRequest = " + n);
        mSchedulerAc->AllocateDds(n);
    }

    CARAPI StopOnDemandDataSubscriptionRequest(NetworkRequest n) {
        Logd("stopOnDemandDataSubscriptionRequest = " + n);
        mSchedulerAc->FreeDds(n);
    }

    private Boolean IsSubInfoReady() {
        return mSimInfo->Size() > 0;
    }

    private SubscriptionController(Phone phone) {
        mContext = phone->GetContext();
        mCM = CallManager->GetInstance();

        If(ServiceManager->GetService("isub") == NULL) {
                ServiceManager->AddService("isub", this);
        }

        Logdl("[SubscriptionController] init by Phone");
    }

    /**
     * Make sure the caller has the READ_PHONE_STATE permission.
     *
     * @throws SecurityException if the caller does not have the required permission
     */
    private void EnforceSubscriptionPermission() {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::READ_PHONE_STATE,
                "Requires READ_PHONE_STATE");
    }

    /**
     * Broadcast when subinfo settings has chanded
     * @SubId The unique SubInfoRecord index in database
     * @param columnName The column that is updated
     * @param intContent The updated integer value
     * @param stringContent The updated string value
     */
     private void BroadcastSimInfoContentChanged(Int64 subId,
            String columnName, Int32 intContent, String stringContent) {

        Intent intent = new Intent(TelephonyIntents.ACTION_SUBINFO_CONTENT_CHANGE);
        intent->PutExtra(BaseColumns._ID, subId);
        intent->PutExtra(TelephonyIntents.EXTRA_COLUMN_NAME, columnName);
        intent->PutExtra(TelephonyIntents.EXTRA_INT_CONTENT, intContent);
        intent->PutExtra(TelephonyIntents.EXTRA_STRING_CONTENT, stringContent);
        If (intContent != SubscriptionManager.DEFAULT_INT_VALUE) {
            Logd("[broadcastSimInfoContentChanged] subId" + subId
                    + " changed, " + columnName + " -> " +  intContent);
        } else {
            Logd("[broadcastSimInfoContentChanged] subId" + subId
                    + " changed, " + columnName + " -> " +  stringContent);
        }
        mContext->SendBroadcast(intent);
    }


    /**
     * New SubInfoRecord instance and fill in detail info
     * @param cursor
     * @return the query result of desired SubInfoRecord
     */
    private SubInfoRecord GetSubInfoRecord(Cursor cursor) {
            SubInfoRecord info = new SubInfoRecord();
            info.subId = cursor->GetLong(cursor->GetColumnIndexOrThrow(BaseColumns._ID));
            info.iccId = cursor->GetString(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.ICC_ID));
            info.slotId = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.SIM_ID));
            info.displayName = cursor->GetString(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.DISPLAY_NAME));
            info.nameSource = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.NAME_SOURCE));
            info.color = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.COLOR));
            info.number = cursor->GetString(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.NUMBER));
            info.displayNumberFormat = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.DISPLAY_NUMBER_FORMAT));
            info.dataRoaming = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.DATA_ROAMING));

            Int32 size = sSimBackgroundDarkRes.length;
            If (info.color >= 0 && info.color < size) {
                info.simIconRes[RES_TYPE_BACKGROUND_DARK] = sSimBackgroundDarkRes[info.color];
                info.simIconRes[RES_TYPE_BACKGROUND_LIGHT] = sSimBackgroundLightRes[info.color];
            }
            info.mcc = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.MCC));
            info.mnc = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.MNC));
            info.mStatus = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.SUB_STATE));
            info.mNwMode = cursor->GetInt(cursor->GetColumnIndexOrThrow(
                    SubscriptionManager.NETWORK_MODE));

            Logd("[getSubInfoRecord] SubId:" + info.subId + " iccid:" + info.iccId + " slotId:" +
                    info.slotId + " displayName:" + info.displayName + " color:" + info.color +
                    " mcc/mnc:" + info.mcc + "/" + info.mnc + " subStatus: " + info.mStatus
                    + " Nwmode: " + info.mNwMode);

            return info;
    }

    /**
     * Query SubInfoRecord(s) from subinfo database
     * @param selection A filter declaring which rows to return
     * @param queryKey query key content
     * @return Array list of queried result from database
     */
     private List<SubInfoRecord> GetSubInfo(String selection, Object queryKey) {
        Logd("selection:" + selection + " " + queryKey);
        String[] selectionArgs = NULL;
        If (queryKey != NULL) {
            selectionArgs = new String[] {queryKey->ToString()};
        }
        ArrayList<SubInfoRecord> subList = NULL;
        Cursor cursor = mContext->GetContentResolver()->Query(SubscriptionManager.CONTENT_URI,
                NULL, selection, selectionArgs, SubscriptionManager.SIM_ID);
        try {
            If (cursor != NULL) {
                While (cursor->MoveToNext()) {
                    SubInfoRecord subInfo = GetSubInfoRecord(cursor);
                    If (subInfo != NULL && SubscriptionManager->IsValidSlotId(subInfo.slotId))
                    {
                        If (subList == NULL)
                        {
                            subList = new ArrayList<SubInfoRecord>();
                        }
                        subList->Add(subInfo);
                }
                }
            } else {
                Logd("Query fail");
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }

        return subList;
    }



    /**
     * Get the SubInfoRecord according to an index
     * @param subId The unique SubInfoRecord index in database
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    public SubInfoRecord GetSubInfoForSubscriber(Int64 subId) {
        Logd("[getSubInfoForSubscriberx]+ subId:" + subId);
        EnforceSubscriptionPermission();

        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            subId = GetDefaultSubId();
        }
        If (!SubscriptionManager->IsValidSubId(subId) || !IsSubInfoReady()) {
            Logd("[getSubInfoForSubscriberx]- invalid subId or not ready, subId = " + subId);
            return NULL;
        }
        Cursor cursor = mContext->GetContentResolver()->Query(SubscriptionManager.CONTENT_URI,
                NULL, BaseColumns._ID + "=?", new String[] {Long->ToString(subId)}, NULL);
        try {
            If (cursor != NULL) {
                If (cursor->MoveToFirst()) {
                    Logd("[getSubInfoForSubscriberx]- Info detail:");
                    return GetSubInfoRecord(cursor);
                }
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }
        Logd("[getSubInfoForSubscriber]- NULL info return");

        return NULL;
    }

    /**
     * Get the SubInfoRecord according to an IccId
     * @param iccId the IccId of SIM card
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    public List<SubInfoRecord> GetSubInfoUsingIccId(String iccId) {
        Logd("[getSubInfoUsingIccId]+ iccId:" + iccId);
        EnforceSubscriptionPermission();

        If (iccId == NULL || !IsSubInfoReady()) {
            Logd("[getSubInfoUsingIccId]- NULL iccid or not ready");
            return NULL;
        }
        Cursor cursor = mContext->GetContentResolver()->Query(SubscriptionManager.CONTENT_URI,
                NULL, SubscriptionManager.ICC_ID + "=?", new String[] {iccId}, NULL);
        ArrayList<SubInfoRecord> subList = NULL;
        try {
            If (cursor != NULL) {
                While (cursor->MoveToNext()) {
                    SubInfoRecord subInfo = GetSubInfoRecord(cursor);
                    If (subInfo != NULL)
                    {
                        If (subList == NULL)
                        {
                            subList = new ArrayList<SubInfoRecord>();
                        }
                        subList->Add(subInfo);
                }
                }
            } else {
                Logd("Query fail");
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }

        return subList;
    }

    /**
     * Get the SubInfoRecord according to slotId
     * @param slotId the slot which the SIM is inserted
     * @return SubInfoRecord, maybe NULL
     */
    //@Override
    public List<SubInfoRecord> GetSubInfoUsingSlotId(Int32 slotId) {
        return GetSubInfoUsingSlotIdWithCheck(slotId, TRUE);
    }

    /**
     * Get all the SubInfoRecord(s) in subinfo database
     * @return Array list of all SubInfoRecords in database, include thsoe that were inserted before
     */
    //@Override
    public List<SubInfoRecord> GetAllSubInfoList() {
        Logd("[getAllSubInfoList]+");
        EnforceSubscriptionPermission();

        List<SubInfoRecord> subList = NULL;
        subList = GetSubInfo(NULL, NULL);
        If (subList != NULL) {
            Logd("[getAllSubInfoList]- " + subList->Size() + " infos return");
        } else {
            Logd("[getAllSubInfoList]- no info return");
        }

        return subList;
    }

    /**
     * Get the SubInfoRecord(s) of the currently inserted SIM(s)
     * @return Array list of currently inserted SubInfoRecord(s)
     */
    //@Override
    public List<SubInfoRecord> GetActiveSubInfoList() {
        EnforceSubscriptionPermission();
        Logdl("[getActiveSubInfoList]+");

        List<SubInfoRecord> subList = NULL;

        If (!IsSubInfoReady()) {
            Logdl("[getActiveSubInfoList] Sub Controller not ready");
            return subList;
        }

        subList = GetSubInfo(SubscriptionManager.SIM_ID
                + "!=" + SubscriptionManager.INVALID_SLOT_ID, NULL);
        If (subList != NULL) {
            Logdl("[getActiveSubInfoList]- " + subList->Size() + " infos return");
        } else {
            Logdl("[getActiveSubInfoList]- no info return");
        }

        return subList;
    }

    /**
     * Get the SUB count of active SUB(s)
     * @return active SIM count
     */
    //@Override
    public Int32 GetActiveSubInfoCount() {
        Logd("[getActiveSubInfoCount]+");
        List<SubInfoRecord> records = GetActiveSubInfoList();
        If (records == NULL) {
            Logd("[getActiveSubInfoCount] records NULL");
            return 0;
        }
        Logd("[getActiveSubInfoCount]- count: " + records->Size());
        return records->Size();
    }

    /**
     * Get the SUB count of all SUB(s) in subinfo database
     * @return all SIM count in database, include what was inserted before
     */
    //@Override
    public Int32 GetAllSubInfoCount() {
        Logd("[getAllSubInfoCount]+");
        EnforceSubscriptionPermission();

        Cursor cursor = mContext->GetContentResolver()->Query(SubscriptionManager.CONTENT_URI,
                NULL, NULL, NULL, NULL);
        try {
            If (cursor != NULL) {
                Int32 count = cursor->GetCount();
                Logd("[getAllSubInfoCount]- " + count + " SUB(s) in DB");
                return count;
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }
        Logd("[getAllSubInfoCount]- no SUB in DB");

        return 0;
    }

    /**
     * Add a new SubInfoRecord to subinfo database if needed
     * @param iccId the IccId of the SIM card
     * @param slotId the slot which the SIM is inserted
     * @return the URL of the newly created row or the updated row
     */
    //@Override
    public Int32 AddSubInfoRecord(String iccId, Int32 slotId) {
        Logdl("[addSubInfoRecord]+ iccId:" + iccId + " slotId:" + slotId);
        EnforceSubscriptionPermission();

        If (iccId == NULL) {
            Logdl("[addSubInfoRecord]- NULL iccId");
        }

        String nameToSet;
        If (IsSubInfoReady()) { // we can only get a subId if SubInfo is ready
            Int64[] subIds = GetSubId(slotId);
            If (subIds == NULL || subIds.length == 0) {
                Logdl("[addSubInfoRecord]- getSubId fail");
                return 0;
            }

            Int32 subIdsIndex = 0;
            If (subIds.length > 1) { // one SIM can have more than one subId
                For (Int32 i = 0; i < subIds.length; i++) {
                    Logdl("[addSubInfoRecord] inspecting subIds["+i+"]: " + subIds[i]);
                    If (TelephonyManager->GetDefault()->GetSimOperatorName(subIds[i]) != "") {
                        // We have a Carrier here, with a CarrierName and everything!
                        subIdsIndex = i;
                        Logdl("[addSubInfoRecord] using subIds["+i+"]: it has a Carrier");
                    }
                }
            }

            If (subIds[subIdsIndex] == -1) { // DUMMY value from getSubId
                Long currentSubId = mSimInfo->Get(slotId);
                If (currentSubId == NULL) {
                    Logdl("[addSubInfoRecord] currentSubId is NULL, proceed with dummy value");
                } else {
                    Logdl("[addSubInfoRecord] currentSubId " + currentSubId + ", proceed with it");
                    subIds[subIdsIndex] = currentSubId;
                }
            }

            String Carrier = TelephonyManager->GetDefault()->GetSimOperator(subIds[subIdsIndex]);
            Logdl("[addSubInfoRecord] Carrier = " + Carrier);
            String CarrierName = TelephonyManager->GetDefault()->GetSimOperatorName(subIds[subIdsIndex]);
            Logdl("[addSubInfoRecord] CarrierName = " + CarrierName);
            SpnOverride mSpnOverride = new SpnOverride();

            If (mSpnOverride->ContainsCarrier(Carrier)) {
                nameToSet = mSpnOverride->GetSpn(Carrier);
                Logdl("[addSubInfoRecord] Found, SpnOverride, name = " + nameToSet);
            } else If (CarrierName != "") {
                nameToSet = CarrierName;
                Logdl("[addSubInfoRecord] Found, name = " + nameToSet);
            } else {
                nameToSet = "SIM " + Integer->ToString(slotId + 1);
                Logdl("[addSubInfoRecord] Not found, name = " + nameToSet);
            }
        } else {
            nameToSet = "SIM " + Integer->ToString(slotId + 1);
            Logdl("[addSubInfoRecord] SubInfo not ready, name = " + nameToSet);
        }

        ContentResolver resolver = mContext->GetContentResolver();
        Cursor cursor = resolver->Query(SubscriptionManager.CONTENT_URI,
                new String[] {BaseColumns._ID, SubscriptionManager.SIM_ID,
                SubscriptionManager.NAME_SOURCE}, SubscriptionManager.ICC_ID + "=?",
                new String[] {iccId}, NULL);

        try {
            If (cursor == NULL || !cursor->MoveToFirst()) {
                ContentValues value = new ContentValues();
                value->Put(SubscriptionManager.ICC_ID, iccId);
                // default SIM color differs between slots
                value->Put(SubscriptionManager.COLOR, slotId);
                value->Put(SubscriptionManager.SIM_ID, slotId);
                value->Put(SubscriptionManager.DISPLAY_NAME, nameToSet);
                Uri uri = resolver->Insert(SubscriptionManager.CONTENT_URI, value);
                Logdl("[addSubInfoRecord]- New record created: " + uri);
            } else {
                Int64 subId = cursor->GetLong(0);
                Int32 oldSimInfoId = cursor->GetInt(1);
                Int32 nameSource = cursor->GetInt(2);
                ContentValues value = new ContentValues();

                If (slotId != oldSimInfoId) {
                    value->Put(SubscriptionManager.SIM_ID, slotId);
                }

                If (nameSource == SubscriptionManager.NAME_SOURCE_UNDEFINDED) {
                    value->Put(SubscriptionManager.DISPLAY_NAME, nameToSet);
                    Logdl("[addSubInfoRecord]- going to update SIM name to " + nameToSet);
                }

                If (value->Size() > 0) {
                    resolver->Update(SubscriptionManager.CONTENT_URI, value,
                            BaseColumns._ID + "=" + Long->ToString(subId), NULL);
                }

                Logdl("[addSubInfoRecord]- Record already exist");
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }

        cursor = resolver->Query(SubscriptionManager.CONTENT_URI, NULL,
                SubscriptionManager.SIM_ID + "=?", new String[] {String->ValueOf(slotId)}, NULL);

        try {
            If (cursor != NULL && cursor->MoveToFirst()) {
                do {
                    Int64 subId = cursor->GetLong(cursor->GetColumnIndexOrThrow(BaseColumns._ID));
                    // If mSimInfo already has a valid subId for a slotId/phoneId,
                    // do not add another subId for same slotId/phoneId.
                    Long currentSubId = mSimInfo->Get(slotId);
                    If (currentSubId == NULL || !SubscriptionManager->IsValidSubId(currentSubId)) {
                        // TODO While two subs active, if user deactivats first
                        // one, need to update the default subId with second one.

                        // FIXME: Currently we assume phoneId and slotId may not be TRUE
                        // when we cross map modem or when multiple subs per slot.
                        // But is TRUE at the moment.
                        mSimInfo->Put(slotId, subId);
                        Int32 simCount = TelephonyManager->GetDefault()->GetSimCount();
                        Int64 defaultSubId = GetDefaultSubId();
                        Logdl("[addSubInfoRecord] mSimInfo.size=" + mSimInfo->Size()
                                + " slotId=" + slotId + " subId=" + subId
                                + " defaultSubId=" + defaultSubId + " simCount=" + simCount);

                        // Set the default sub if not set or if single sim device
                        If (!SubscriptionManager->IsValidSubId(defaultSubId) || simCount == 1) {
                            SetDefaultSubId(subId);
                        }
                        // If single sim device, set this subscription as the default for everything
                        If (simCount == 1) {
                            Logdl("[addSubInfoRecord] one sim set defaults to subId=" + subId);
                            SetDefaultDataSubId(subId);
                            SetDataSubId(subId);
                            SetDefaultSmsSubId(subId);
                            SetDefaultVoiceSubId(subId);
                        }
                    } else {
                        Logdl("[addSubInfoRecord] currentSubId != NULL && currentSubId is valid, IGNORE");
                    }
                    Logdl("[addSubInfoRecord]- Hashmap("+slotId+","+subId+")");
                } While (cursor->MoveToNext());
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }

        Int32 size = mSimInfo->Size();
        Logdl("[addSubInfoRecord]- info size="+size);

        // Once the records are loaded, notify DcTracker
        UpdateAllDataConnectionTrackers();

        // FIXME this does not match the javadoc
        return 1;
    }

    /**
     * Set SIM color by simInfo index
     * @param color the color of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    public Int32 SetColor(Int32 color, Int64 subId) {
        Logd("[setColor]+ color:" + color + " subId:" + subId);
        EnforceSubscriptionPermission();

        ValidateSubId(subId);
        Int32 size = sSimBackgroundDarkRes.length;
        If (color < 0 || color >= size) {
            Logd("[setColor]- fail");
            return -1;
        }
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.COLOR, color);
        Logd("[setColor]- color:" + color + " set");

        Int32 result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                BaseColumns._ID + "=" + Long->ToString(subId), NULL);
        BroadcastSimInfoContentChanged(subId, SubscriptionManager.COLOR,
                color, SubscriptionManager.DEFAULT_STRING_VALUE);

        return result;
    }

    /**
     * Set display name by simInfo index
     * @param displayName the display name of SIM card
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    public Int32 SetDisplayName(String displayName, Int64 subId) {
        return SetDisplayNameUsingSrc(displayName, subId, -1);
    }

    /**
     * Set display name by simInfo index with name source
     * @param displayName the display name of SIM card
     * @param subId the unique SubInfoRecord index in database
     * @param nameSource 0: NAME_SOURCE_DEFAULT_SOURCE, 1: NAME_SOURCE_SIM_SOURCE,
     *                   2: NAME_SOURCE_USER_INPUT, -1 NAME_SOURCE_UNDEFINED
     * @return the number of records updated
     */
    //@Override
    public Int32 SetDisplayNameUsingSrc(String displayName, Int64 subId, Int64 nameSource) {
        Logd("[setDisplayName]+  displayName:" + displayName + " subId:" + subId
                + " nameSource:" + nameSource);
        EnforceSubscriptionPermission();

        ValidateSubId(subId);
        String nameToSet;
        If (displayName == NULL) {
            nameToSet = mContext->GetString(SubscriptionManager.DEFAULT_NAME_RES);
        } else {
            nameToSet = displayName;
        }
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.DISPLAY_NAME, nameToSet);
        If (nameSource >= SubscriptionManager.NAME_SOURCE_DEFAULT_SOURCE) {
            Logd("Set nameSource=" + nameSource);
            value->Put(SubscriptionManager.NAME_SOURCE, nameSource);
        }
        Logd("[setDisplayName]- mDisplayName:" + nameToSet + " set");

        Int32 result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                BaseColumns._ID + "=" + Long->ToString(subId), NULL);
        BroadcastSimInfoContentChanged(subId, SubscriptionManager.DISPLAY_NAME,
                SubscriptionManager.DEFAULT_INT_VALUE, nameToSet);

        return result;
    }

    /**
     * Set phone number by subId
     * @param number the phone number of the SIM
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    public Int32 SetDisplayNumber(String number, Int64 subId) {
        Logd("[setDisplayNumber]+ number:" + number + " subId:" + subId);
        EnforceSubscriptionPermission();

        ValidateSubId(subId);
        Int32 result = 0;
        Int32 phoneId = GetPhoneId(subId);

        If (number == NULL || phoneId < 0 ||
                phoneId >= TelephonyManager->GetDefault()->GetPhoneCount()) {
            Logd("[setDispalyNumber]- fail");
            return -1;
        }
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.NUMBER, number);
        Logd("[setDisplayNumber]- number:" + number + " set");

        Phone phone = sProxyPhones[phoneId];
        String alphaTag = TelephonyManager->GetDefault()->GetLine1AlphaTagForSubscriber(subId);

        Synchronized(mLock) {
            mSuccess = FALSE;
            Message response = mHandler->ObtainMessage(EVENT_WRITE_MSISDN_DONE);

            phone->SetLine1Number(alphaTag, number, response);

            try {
                mLock->Wait();
            } Catch (InterruptedException e) {
                Loge("interrupted while trying to write MSISDN");
            }
        }

        If (mSuccess) {
            result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                    BaseColumns._ID + "=" + Long->ToString(subId), NULL);
            Logd("[setDisplayNumber]- update result :" + result);
            BroadcastSimInfoContentChanged(subId, SubscriptionManager.NUMBER,
                    SubscriptionManager.DEFAULT_INT_VALUE, number);
        }

        return result;
    }

    /**
     * Set number display format. 0: none, 1: the first four digits, 2: the last four digits
     * @param format the display format of phone number
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    public Int32 SetDisplayNumberFormat(Int32 format, Int64 subId) {
        Logd("[setDisplayNumberFormat]+ format:" + format + " subId:" + subId);
        EnforceSubscriptionPermission();

        ValidateSubId(subId);
        If (format < 0) {
            Logd("[setDisplayNumberFormat]- fail, return -1");
            return -1;
        }
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.DISPLAY_NUMBER_FORMAT, format);
        Logd("[setDisplayNumberFormat]- format:" + format + " set");

        Int32 result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                BaseColumns._ID + "=" + Long->ToString(subId), NULL);
        BroadcastSimInfoContentChanged(subId, SubscriptionManager.DISPLAY_NUMBER_FORMAT,
                format, SubscriptionManager.DEFAULT_STRING_VALUE);

        return result;
    }

    /**
     * Set data roaming by simInfo index
     * @param roaming 0:Don't allow data when roaming, 1:Allow data when roaming
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    //@Override
    public Int32 SetDataRoaming(Int32 roaming, Int64 subId) {
        Logd("[setDataRoaming]+ roaming:" + roaming + " subId:" + subId);
        EnforceSubscriptionPermission();

        ValidateSubId(subId);
        If (roaming < 0) {
            Logd("[setDataRoaming]- fail");
            return -1;
        }
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.DATA_ROAMING, roaming);
        Logd("[setDataRoaming]- roaming:" + roaming + " set");

        Int32 result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                BaseColumns._ID + "=" + Long->ToString(subId), NULL);
        BroadcastSimInfoContentChanged(subId, SubscriptionManager.DATA_ROAMING,
                roaming, SubscriptionManager.DEFAULT_STRING_VALUE);

        return result;
    }

    /**
     * Set MCC/MNC by subscription ID
     * @param mccMnc MCC/MNC associated with the subscription
     * @param subId the unique SubInfoRecord index in database
     * @return the number of records updated
     */
    public Int32 SetMccMnc(String mccMnc, Int64 subId) {
        Int32 mcc = 0;
        Int32 mnc = 0;
        try {
            mcc = Integer->ParseInt(mccMnc->Substring(0,3));
            mnc = Integer->ParseInt(mccMnc->Substring(3));
        } Catch (NumberFormatException e) {
            Logd("[setMccMnc] - couldn't parse mcc/mnc: " + mccMnc);
        }
        Logd("[setMccMnc]+ mcc/mnc:" + mcc + "/" + mnc + " subId:" + subId);
        ContentValues value = new ContentValues(2);
        value->Put(SubscriptionManager.MCC, mcc);
        value->Put(SubscriptionManager.MNC, mnc);

        Int32 result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI, value,
                BaseColumns._ID + "=" + Long->ToString(subId), NULL);
        BroadcastSimInfoContentChanged(subId, SubscriptionManager.MCC, mcc, NULL);

        return result;
    }


    //@Override
    public Int32 GetSlotId(Int64 subId) {
        If (VDBG) PrintStackTrace("[getSlotId] subId=" + subId);

        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            subId = GetDefaultSubId();
        }
        If (!SubscriptionManager->IsValidSubId(subId)) {
            Logd("[getSlotId]- subId invalid");
            return SubscriptionManager.INVALID_SLOT_ID;
        }

        Int32 size = mSimInfo->Size();

        If (size == 0)
        {
            Logd("[getSlotId]- size == 0, return SIM_NOT_INSERTED instead");
            return SubscriptionManager.SIM_NOT_INSERTED;
        }

        For (Entry<Integer, Long> entry: mSimInfo->EntrySet()) {
            Int32 sim = entry->GetKey();
            Int64 sub = entry->GetValue();

            If (subId == sub)
            {
                If (VDBG) Logv("[getSlotId]- return = " + sim);
                return sim;
            }
        }

        Logd("[getSlotId]- return fail");
        return SubscriptionManager.INVALID_SLOT_ID;
    }

    /**
     * Return the subId for specified slot Id.
     * @deprecated
     */
    //@Override
    //@Deprecated
    public Int64[] GetSubId(Int32 slotId) {
        If (VDBG) PrintStackTrace("[getSubId] slotId=" + slotId);

        If (slotId == SubscriptionManager.DEFAULT_SLOT_ID) {
            Logd("[getSubId]- default slotId");
            slotId = GetSlotId(GetDefaultSubId());
        }

        //FIXME remove this
        final Int64[] DUMMY_VALUES = {-1 - slotId, -1 - slotId};

        If (!SubscriptionManager->IsValidSlotId(slotId)) {
            Logd("[getSubId]- invalid slotId");
            return NULL;
        }

        //FIXME remove this
        If (slotId < 0) {
            Logd("[getSubId]- slotId < 0, return dummy instead");
            return DUMMY_VALUES;
        }

        Int32 size = mSimInfo->Size();

        If (size == 0) {
            Logd("[getSubId]- size == 0, return dummy instead");
            //FIXME return NULL
            return DUMMY_VALUES;
        }

        ArrayList<Long> subIds = new ArrayList<Long>();
        For (Entry<Integer, Long> entry: mSimInfo->EntrySet()) {
            Int32 slot = entry->GetKey();
            Int64 sub = entry->GetValue();
            If (slotId == slot) {
                subIds->Add(sub);
            }
        }

        If (VDBG) Logd("[getSubId]-, subIds = " + subIds);
        Int32 numSubIds = subIds->Size();

        If (numSubIds == 0) {
            Logd("[getSubId]- numSubIds == 0, return dummy instead");
            return DUMMY_VALUES;
        }

        Int64[] subIdArr = new Int64[numSubIds];
        For (Int32 i = 0; i < numSubIds; i++) {
            subIdArr[i] = subIds->Get(i);
        }

        return subIdArr;
    }

    //@Override
    public Int32 GetPhoneId(Int64 subId) {
        If (VDBG) PrintStackTrace("[getPhoneId] subId=" + subId);
        Int32 phoneId;

        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            subId = GetDefaultSubId();
            Logdl("[getPhoneId] asked for default subId=" + subId);
        }

        If (!SubscriptionManager->IsValidSubId(subId)) {
            Logdl("[getPhoneId]- invalid subId return=" + SubscriptionManager.INVALID_PHONE_ID);
            return SubscriptionManager.INVALID_PHONE_ID;
        }

        //FIXME remove this
        If (subId < 0) {
            phoneId = (Int32) (-1 - subId);
            If (VDBG) Logdl("[getPhoneId]- map subId=" + subId + " phoneId=" + phoneId);
            return phoneId;
        }

        Int32 size = mSimInfo->Size();

        If (size == 0) {
            phoneId = mDefaultPhoneId;
            Logdl("[getPhoneId]- no sims, returning default phoneId=" + phoneId);
            return phoneId;
        }

        // FIXME: Assumes phoneId == slotId
        For (Entry<Integer, Long> entry: mSimInfo->EntrySet()) {
            Int32 sim = entry->GetKey();
            Int64 sub = entry->GetValue();

            If (subId == sub) {
                If (VDBG) Logdl("[getPhoneId]- found subId=" + subId + " phoneId=" + sim);
                return sim;
            }
        }

        phoneId = mDefaultPhoneId;
        Logdl("[getPhoneId]- subId=" + subId + " not found return default phoneId=" + phoneId);
        return phoneId;

    }

    /**
     * @return the number of records cleared
     */
    //@Override
    public Int32 ClearSubInfo() {
        EnforceSubscriptionPermission();
        Logd("[clearSubInfo]+");

        Int32 size = mSimInfo->Size();

        If (size == 0) {
            Logdl("[clearSubInfo]- no simInfo size=" + size);
            return 0;
        }

        mSimInfo->Clear();
        Logdl("[clearSubInfo]- clear size=" + size);
        return size;
    }

    private static Int32[] SetSimResource(Int32 type) {
        Int32[] simResource = NULL;

        Switch (type) {
            case RES_TYPE_BACKGROUND_DARK:
                simResource = new Int32[] {
                    R.drawable.sim_dark_blue,
                    R.drawable.sim_dark_orange,
                    R.drawable.sim_dark_green,
                    R.drawable.sim_dark_purple
                };
                break;
            case RES_TYPE_BACKGROUND_LIGHT:
                simResource = new Int32[] {
                    R.drawable.sim_light_blue,
                    R.drawable.sim_light_orange,
                    R.drawable.sim_light_green,
                    R.drawable.sim_light_purple
                };
                break;
        }

        return simResource;
    }

    private void Logvl(String msg) {
        Logv(msg);
        mLocalLog->Log(msg);
    }

    private void Logv(String msg) {
        Rlog->V(LOG_TAG, msg);
    }

    private void Logdl(String msg) {
        Logd(msg);
        mLocalLog->Log(msg);
    }

    private static void Slogd(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    private void Logd(String msg) {
        Rlog->D(LOG_TAG, msg);
    }

    private void Logel(String msg) {
        Loge(msg);
        mLocalLog->Log(msg);
    }

    private void Loge(String msg) {
        Rlog->E(LOG_TAG, msg);
    }

    //@Override
    //@Deprecated
    public Int64 GetDefaultSubId() {
        //FIXME: Make this smarter, need to handle data only and voice devices
        Int64 subId = mDefaultVoiceSubId;
        If (VDBG) Logv("[getDefaultSubId] value = " + subId);
        return subId;
    }

    //@Override
    CARAPI SetDefaultSmsSubId(Int64 subId) {
        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            throw new RuntimeException("setDefaultSmsSubId called with DEFAULT_SUB_ID");
        }
        Logdl("[setDefaultSmsSubId] subId=" + subId);
        Settings::Global::>PutLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_SMS_SUBSCRIPTION, subId);
        BroadcastDefaultSmsSubIdChanged(subId);
    }

    private void BroadcastDefaultSmsSubIdChanged(Int64 subId) {
        // Broadcast an Intent for default sms sub change
        Logdl("[broadcastDefaultSmsSubIdChanged] subId=" + subId);
        Intent intent = new Intent(TelephonyIntents.ACTION_DEFAULT_SMS_SUBSCRIPTION_CHANGED);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(PhoneConstants.SUBSCRIPTION_KEY, subId);
        mContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    //@Override
    public Int64 GetDefaultSmsSubId() {
        Int64 subId = Settings::Global::>GetLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_SMS_SUBSCRIPTION,
                SubscriptionManager.INVALID_SUB_ID);
        If (VDBG) Logd("[getDefaultSmsSubId] subId=" + subId);
        return subId;
    }

    //@Override
    CARAPI SetDefaultVoiceSubId(Int64 subId) {
        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            throw new RuntimeException("setDefaultVoiceSubId called with DEFAULT_SUB_ID");
        }
        Logdl("[setDefaultVoiceSubId] subId=" + subId);
        Settings::Global::>PutLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_VOICE_CALL_SUBSCRIPTION, subId);
        BroadcastDefaultVoiceSubIdChanged(subId);
    }

    private void BroadcastDefaultVoiceSubIdChanged(Int64 subId) {
        // Broadcast an Intent for default voice sub change
        Logdl("[broadcastDefaultVoiceSubIdChanged] subId=" + subId);
        Intent intent = new Intent(TelephonyIntents.ACTION_DEFAULT_VOICE_SUBSCRIPTION_CHANGED);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(PhoneConstants.SUBSCRIPTION_KEY, subId);
        mContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    //@Override
    public Int64 GetDefaultVoiceSubId() {
        Int64 subId = Settings::Global::>GetLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_VOICE_CALL_SUBSCRIPTION,
                SubscriptionManager.INVALID_SUB_ID);
        If (VDBG) Logd("[getDefaultVoiceSubId] subId=" + subId);
        return subId;
    }

    /* Returns User SMS Prompt property,  enabled or not */
    //@Override
    public Boolean IsSMSPromptEnabled() {
        Boolean prompt = FALSE;
        Int32 value = 0;
        try {
            value = Settings::Global::>GetInt(mContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_SMS_PROMPT);
        } Catch (SettingNotFoundException snfe) {
            Loge("Settings Exception Reading Dual Sim SMS Prompt Values");
        }
        prompt = (value == 0) ? FALSE : TRUE ;
        If (VDBG) Logd("SMS Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User SMS Prompt property,  enable or not */
    //@Override
    CARAPI SetSMSPromptEnabled(Boolean enabled) {
        Int32 value = (enabled == FALSE) ? 0 : 1;
        Settings::Global::>PutInt(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_SMS_PROMPT, value);
        Logd("setSMSPromptOption to " + enabled);
    }


    //@Override
    public Int64 GetDefaultDataSubId() {
        Int64 subId = Settings::Global::>GetLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_DATA_CALL_SUBSCRIPTION,
                SubscriptionManager.INVALID_SUB_ID);
        If (VDBG) Logd("[getDefaultDataSubId] subId= " + subId);
        return subId;
    }

    //@Override
    CARAPI SetDefaultDataSubId(Int64 subId) {
        Logdl("[setDefaultDataSubId] subId=" + subId);

        If (mDctController == NULL) {
            mDctController = DctController->GetInstance();
            mDctController->RegisterForDefaultDataSwitchInfo(mDataConnectionHandler,
                    EVENT_SET_DEFAULT_DATA_DONE, NULL);
        }
        mDctController->SetDefaultDataSubId(subId);

    }

    public Int64 GetCurrentDds() {
        return mScheduler->GetCurrentDds();
    }


    private void UpdateDataSubId(AsyncResult ar) {
        Long subId = (Long)ar.result;
        Int32 reqStatus = PhoneConstants.FAILURE;

        Logd(" updateDataSubId,  subId=" + subId + " exception " + ar.exception);
        // Update newDds in database if the DDS request succeeded.
        If (ar.exception == NULL) {
            SetDataSubId(subId);
            reqStatus = PhoneConstants.SUCCESS;
        }
        mScheduler->UpdateCurrentDds(NULL);
        BroadcastDefaultDataSubIdChanged(reqStatus);

        // FIXME is this still needed?
        UpdateAllDataConnectionTrackers();
    }
    CARAPI SetDataSubId(Int64 subId) {
        Settings::Global::>PutLong(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_DATA_CALL_SUBSCRIPTION, subId);
    }

    private void UpdateAllDataConnectionTrackers() {
        // Tell Phone Proxies to update data connection tracker
        Int32 len = sProxyPhones.length;
        Logdl("[updateAllDataConnectionTrackers] sProxyPhones.length=" + len);
        For (Int32 phoneId = 0; phoneId < len; phoneId++) {
            sProxyPhones[phoneId].UpdateDataConnectionTracker();
        }
    }

    private void BroadcastDefaultDataSubIdChanged(Int32 status) {
        // Broadcast an Intent for default data sub change
        Logdl("[broadcastDefaultDataSubIdChanged] subId = " + GetDefaultDataSubId()
                 + " status " + status);
        Intent intent = new Intent(TelephonyIntents.ACTION_DEFAULT_DATA_SUBSCRIPTION_CHANGED);
        intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        intent->PutExtra(PhoneConstants.SUBSCRIPTION_KEY, GetDefaultDataSubId());
        intent->PutExtra(TelephonyIntents.EXTRA_RESULT, status);
        mContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
    }

    /* Sets the default subscription. If only one sub is active that
     * sub is set as default subId. If two or more  sub's are active
     * the first sub is set as default subscription
     */
    // FIXME
    CARAPI SetDefaultSubId(Int64 subId) {
        If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            throw new RuntimeException("setDefaultSubId called with DEFAULT_SUB_ID");
        }
        Logdl("[setDefaultSubId] subId=" + subId);
        If (SubscriptionManager->IsValidSubId(subId)) {
            Int32 phoneId = GetPhoneId(subId);
            If (phoneId >= 0 && (phoneId < TelephonyManager->GetDefault()->GetPhoneCount()
                    || TelephonyManager->GetDefault()->GetSimCount() == 1)) {
                Logdl("[setDefaultSubId] set mDefaultVoiceSubId=" + subId);
                mDefaultVoiceSubId = subId;
                // Update MCC MNC device configuration information
                String defaultMccMnc = TelephonyManager->GetDefault()->GetSimOperator(phoneId);
                MccTable->UpdateMccMncConfiguration(mContext, defaultMccMnc, FALSE);

                // Broadcast an Intent for default sub change
                Intent intent = new Intent(TelephonyIntents.ACTION_DEFAULT_SUBSCRIPTION_CHANGED);
                intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
                SubscriptionManager->PutPhoneIdAndSubIdExtra(intent, phoneId, subId);
                If (VDBG) {
                    Logdl("[setDefaultSubId] broadcast default subId changed phoneId=" + phoneId
                            + " subId=" + subId);
                }
                mContext->SendStickyBroadcastAsUser(intent, UserHandle.ALL);
            } else {
                If (VDBG) {
                    Logdl("[setDefaultSubId] not set invalid phoneId=" + phoneId + " subId=" + subId);
                }
            }
        }
    }

    //@Override
    CARAPI ClearDefaultsForInactiveSubIds() {
        final List<SubInfoRecord> records = GetActiveSubInfoList();
        Logdl("[clearDefaultsForInactiveSubIds] records: " + records);
        If (ShouldDefaultBeCleared(records, GetDefaultDataSubId())) {
            Logd("[clearDefaultsForInactiveSubIds] clearing default data sub id");
            SetDefaultDataSubId(SubscriptionManager.INVALID_SUB_ID);
        }
        If (ShouldDefaultBeCleared(records, GetDefaultSmsSubId())) {
            Logdl("[clearDefaultsForInactiveSubIds] clearing default sms sub id");
            SetDefaultSmsSubId(SubscriptionManager.INVALID_SUB_ID);
        }
        If (ShouldDefaultBeCleared(records, GetDefaultVoiceSubId())) {
            Logdl("[clearDefaultsForInactiveSubIds] clearing default voice sub id");
            SetDefaultVoiceSubId(SubscriptionManager.INVALID_SUB_ID);
        }
    }

    private Boolean ShouldDefaultBeCleared(List<SubInfoRecord> records, Int64 subId) {
        Logdl("[shouldDefaultBeCleared: subId] " + subId);
        If (records == NULL) {
            Logdl("[shouldDefaultBeCleared] return TRUE no records subId=" + subId);
            return TRUE;
        }
        If (subId == SubscriptionManager.ASK_USER_SUB_ID && records->Size() > 1) {
            // Only allow ASK_USER_SUB_ID if there is more than 1 subscription.
            Logdl("[shouldDefaultBeCleared] return FALSE only one subId, subId=" + subId);
            return FALSE;
        }
        For (SubInfoRecord record : records) {
            Logdl("[shouldDefaultBeCleared] Record.subId: " + record.subId);
            If (record.subId == subId) {
                Logdl("[shouldDefaultBeCleared] return FALSE subId is active, subId=" + subId);
                return FALSE;
            }
        }
        Logdl("[shouldDefaultBeCleared] return TRUE not active subId=" + subId);
        return TRUE;
    }

    private class DataConnectionHandler extends Handler {
        //@Override
        CARAPI HandleMessage(Message msg) {
            Switch (msg.what) {
                case EVENT_SET_DEFAULT_DATA_DONE:{
                    AsyncResult ar = (AsyncResult) msg.obj;
                    Logd("EVENT_SET_DEFAULT_DATA_DONE subId:" + (Long)ar.result);
                    UpdateDataSubId(ar);
                    break;
                }
            }
        }
    }

    /* This should return Int64 and not Int64 [] since each phone has
     * exactly 1 sub id for now, it could return the 0th element
     * returned from GetSubId()
     */
    // FIXME will design a mechanism to manage the relationship between PhoneId/SlotId/SubId
    // since phoneId = SlotId is not always TRUE
    public Int64 GetSubIdUsingPhoneId(Int32 phoneId) {
        Int64[] subIds = GetSubId(phoneId);
        If (subIds == NULL || subIds.length == 0) {
            return SubscriptionManager.INVALID_SUB_ID;
        }
        return subIds[0];
    }

    public Int64[] GetSubIdUsingSlotId(Int32 slotId) {
        return GetSubId(slotId);
    }

    public List<SubInfoRecord> GetSubInfoUsingSlotIdWithCheck(Int32 slotId, Boolean needCheck) {
        Logd("[getSubInfoUsingSlotIdWithCheck]+ slotId:" + slotId);
        EnforceSubscriptionPermission();

        If (slotId == SubscriptionManager.DEFAULT_SLOT_ID) {
            slotId = GetSlotId(GetDefaultSubId());
        }
        If (!SubscriptionManager->IsValidSlotId(slotId)) {
            Logd("[getSubInfoUsingSlotIdWithCheck]- invalid slotId");
            return NULL;
        }

        If (needCheck && !IsSubInfoReady()) {
            Logd("[getSubInfoUsingSlotIdWithCheck]- not ready");
            return NULL;
        }

        Cursor cursor = mContext->GetContentResolver()->Query(SubscriptionManager.CONTENT_URI,
                NULL, SubscriptionManager.SIM_ID + "=?", new String[] {String->ValueOf(slotId)}, NULL);
        ArrayList<SubInfoRecord> subList = NULL;
        try {
            If (cursor != NULL && cursor->MoveToFirst()) {
                do {
                    SubInfoRecord subInfo = GetSubInfoRecord(cursor);
                    If (subInfo != NULL) {
                        If (subList == NULL) {
                            subList = new ArrayList<SubInfoRecord>();
                        }
                        subList->Add(subInfo);
                    }
                } While (cursor->MoveToNext());
            }
        } finally {
            If (cursor != NULL) {
                cursor->Close();
            }
        }
        Logd("[getSubInfoUsingSlotId]- NULL info return");

        return subList;
    }

    private void ValidateSubId(Int64 subId) {
        Logd("validateSubId subId: " + subId);
        If (!SubscriptionManager->IsValidSubId(subId)) {
            throw new RuntimeException("Invalid sub id passed as parameter");
        } else If (subId == SubscriptionManager.DEFAULT_SUB_ID) {
            throw new RuntimeException("Default sub id passed as parameter");
        }
    }

    CARAPI UpdatePhonesAvailability(PhoneProxy[] phones) {
        sProxyPhones = phones;
    }

    /**
     * @return the list of subId's that are active, is never NULL but the length maybe 0.
     */
    //@Override
    public Int64[] GetActiveSubIdList() {
        Set<Entry<Integer, Long>> simInfoSet = mSimInfo->EntrySet();
        Logdl("[getActiveSubIdList] simInfoSet=" + simInfoSet);

        Int64[] subIdArr = new Int64[simInfoSet->Size()];
        Int32 i = 0;
        For (Entry<Integer, Long> entry: simInfoSet) {
            Int64 sub = entry->GetValue();
            subIdArr[i] = sub;
            i++;
        }

        Logdl("[getActiveSubIdList] X subIdArr.length=" + subIdArr.length);
        return subIdArr;
    }

    private static void PrintStackTrace(String msg) {
        RuntimeException re = new RuntimeException();
        Slogd("StackTrace - " + msg);
        StackTraceElement[] st = re->GetStackTrace();
        Boolean first = TRUE;
        For (StackTraceElement ste : st) {
            If (first) {
                first = FALSE;
            } else {
                Slogd(ste->ToString());
            }
        }
    }

    //@Override
    CARAPI Dump(FileDescriptor fd, PrintWriter pw, String[] args) {
        mContext->EnforceCallingOrSelfPermission(Manifest::permission::DUMP,
                "Requires DUMP");
        pw->Println("SubscriptionController:");
        pw->Println(" defaultSubId=" + GetDefaultSubId());
        pw->Println(" defaultDataSubId=" + GetDefaultDataSubId());
        pw->Println(" defaultVoiceSubId=" + GetDefaultVoiceSubId());
        pw->Println(" defaultSmsSubId=" + GetDefaultSmsSubId());

        pw->Println(" defaultDataPhoneId=" + SubscriptionManager->GetDefaultDataPhoneId());
        pw->Println(" defaultVoicePhoneId=" + SubscriptionManager->GetDefaultVoicePhoneId());
        pw->Println(" defaultSmsPhoneId=" + SubscriptionManager->GetDefaultSmsPhoneId());
        pw->Flush();

        For (Entry<Integer, Long> entry : mSimInfo->EntrySet()) {
            pw->Println(" mSimInfo[" + entry->GetKey() + "]: subId=" + entry->GetValue());
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");

        List<SubInfoRecord> sirl = GetActiveSubInfoList();
        If (sirl != NULL) {
            pw->Println(" ActiveSubInfoList:");
            For (SubInfoRecord entry : sirl) {
                pw->Println("  " + entry->ToString());
            }
        } else {
            pw->Println(" ActiveSubInfoList: is NULL");
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");

        sirl = GetAllSubInfoList();
        If (sirl != NULL) {
            pw->Println(" AllSubInfoList:");
            For (SubInfoRecord entry : sirl) {
                pw->Println("  " + entry->ToString());
            }
        } else {
            pw->Println(" AllSubInfoList: is NULL");
        }
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");

        mLocalLog->Dump(fd, pw, args);
        pw->Flush();
        pw->Println("++++++++++++++++++++++++++++++++");
        pw->Flush();
    }

    //@Override
    CARAPI ActivateSubId(Int64 subId) {
        If (GetSubState(subId) == SubscriptionManager.ACTIVE) {
            Logd("activateSubId: subscription already active, subId = " + subId);
            return;
        }

        Int32 slotId = GetSlotId(subId);
        SubscriptionHelper->GetInstance()->SetUiccSubscription(slotId, SubscriptionManager.ACTIVE);
    }

    //@Override
    CARAPI DeactivateSubId(Int64 subId) {
        If (GetSubState(subId) == SubscriptionManager.INACTIVE) {
            Logd("activateSubId: subscription already deactivated, subId = " + subId);
            return;
        }

        Int32 slotId = GetSlotId(subId);
        SubscriptionHelper->GetInstance()->SetUiccSubscription(slotId, SubscriptionManager.INACTIVE);
    }

    CARAPI SetNwMode(Int64 subId, Int32 nwMode) {
        Logd("setNwMode, nwMode: " + nwMode + " subId: " + subId);
        ContentValues value = new ContentValues(1);
        value->Put(SubscriptionManager.NETWORK_MODE, nwMode);
        mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI,
                value, BaseColumns._ID + "=" + Long->ToString(subId), NULL);
    }

    public Int32 GetNwMode(Int64 subId) {
        SubInfoRecord subInfo = GetSubInfoForSubscriber(subId);
        If (subInfo != NULL)  {
            return subInfo.mNwMode;
        } else {
            Loge("getSubState: invalid subId = " + subId);
            return SubscriptionManager.DEFAULT_NW_MODE;
        }
    }

    //@Override
    public Int32 SetSubState(Int64 subId, Int32 subStatus) {
        Int32 result = 0;
        Logd("setSubState, subStatus: " + subStatus + " subId: " + subId);
        If (ModemStackController->GetInstance()->IsStackReady()) {
            ContentValues value = new ContentValues(1);
            value->Put(SubscriptionManager.SUB_STATE, subStatus);
            result = mContext->GetContentResolver()->Update(SubscriptionManager.CONTENT_URI,
                    value, BaseColumns._ID + "=" + Long->ToString(subId), NULL);

        }
        BroadcastSimInfoContentChanged(subId,
                SubscriptionManager.SUB_STATE, subStatus, SubscriptionManager.DEFAULT_STRING_VALUE);
        return result;
    }

    //@Override
    public Int32 GetSubState(Int64 subId) {
        SubInfoRecord subInfo = GetSubInfoForSubscriber(subId);
        Int32 subStatus = SubscriptionManager.INACTIVE;

        // Consider the subStatus from subInfo record only if the
        //  record is associated with a valid slot Id.
        If ((subInfo != NULL) && (subInfo.slotId >= 0)) {
            subStatus = subInfo.mStatus;
        }
        return subStatus;
    }

    /* setDds flag is used to trigger DDS switch request during
      device powerUp and when flex map performed */
    CARAPI UpdateUserPrefs(Boolean setDds) {
        List<SubInfoRecord> subInfoList = GetActiveSubInfoList();
        Int32 mActCount = 0;
        SubInfoRecord mNextActivatedSub = NULL;

        If (subInfoList == NULL) {
            Logd("updateUserPrefs: subscription are not avaiable dds = " + GetDefaultDataSubId()
                     + " voice = " + GetDefaultVoiceSubId() + " sms = " + GetDefaultSmsSubId() +
                     " setDDs = " + setDds);
            // If no SIM cards present on device, set dummy subId
            // as data/sms/voice preferred subId.
            SetDefaultSubId(DUMMY_SUB_ID);
            SetDefaultVoiceSubId(DUMMY_SUB_ID);
            SetDefaultSmsSubId(DUMMY_SUB_ID);
            SetDataSubId(DUMMY_SUB_ID);
            return;
        }

        //Get num of activated Subs and next available activated sub info.
        For (SubInfoRecord subInfo : subInfoList) {
            If (GetSubState(subInfo.subId) == SubscriptionManager.ACTIVE) {
                mActCount++;
                If (mNextActivatedSub == NULL) mNextActivatedSub = subInfo;
            }
        }

        Logd("updateUserPrefs: active sub count = " + mActCount + " dds = " + GetDefaultDataSubId()
                 + " voice = " + GetDefaultVoiceSubId() + " sms = "
                 + GetDefaultSmsSubId() + " setDDs = " + setDds);
        //if activated sub count is less than 2, disable prompt.
        If (mActCount < 2) {
            SetSMSPromptEnabled(FALSE);
            SetVoicePromptEnabled(FALSE);
        }

        //if there are no activated subs available, no need to update. EXIT.
        If (mNextActivatedSub == NULL) return;

        If (GetSubState(GetDefaultSubId()) == SubscriptionManager.INACTIVE) {
            SetDefaultSubId(mNextActivatedSub.subId);
        }

        Int64 ddsSubId = GetDefaultDataSubId();
        Int32 ddsSubState = GetSubState(ddsSubId);
        //if current data sub is not active, fallback to next active sub.
        If (setDds || (ddsSubState == SubscriptionManager.INACTIVE)) {
            If (ddsSubState == SubscriptionManager.INACTIVE) ddsSubId = mNextActivatedSub.subId;
            SetDefaultDataSubId(ddsSubId);
        }
        //if current voice sub is not active and prompt not enabled, fallback to next active sub.
        If (GetSubState(GetDefaultVoiceSubId()) == SubscriptionManager.INACTIVE &&
            !IsVoicePromptEnabled()) {
            SetDefaultVoiceSubId(mNextActivatedSub.subId);
        }
        //if current sms sub is not active and prompt not enabled, fallback to next active sub.
        If (GetSubState(GetDefaultSmsSubId()) == SubscriptionManager.INACTIVE &&
            !IsSMSPromptEnabled()) {
            SetDefaultSmsSubId(mNextActivatedSub.subId);
        }
        Logd("updateUserPrefs: after currentDds = " + GetDefaultDataSubId() + " voice = " +
                 GetDefaultVoiceSubId() + " sms = " + GetDefaultSmsSubId() +
                 " newDds = " + ddsSubId);

    }

    /* Returns User Voice Prompt property,  enabled or not */
    //@Override
    public Boolean IsVoicePromptEnabled() {
        Boolean prompt = FALSE;
        Int32 value = 0;
        try {
            value = Settings::Global::>GetInt(mContext->GetContentResolver(),
                    Settings::Global::MULTI_SIM_VOICE_PROMPT);
        } Catch (SettingNotFoundException snfe) {
            Loge("Settings Exception Reading Dual Sim Voice Prompt Values");
        }
        prompt = (value == 0) ? FALSE : TRUE ;
        If (VDBG) Logd("Voice Prompt option:" + prompt);

       return prompt;
    }

    /*Sets User SMS Prompt property,  enable or not */
    //@Override
    CARAPI SetVoicePromptEnabled(Boolean enabled) {
        Int32 value = (enabled == FALSE) ? 0 : 1;
        Settings::Global::>PutInt(mContext->GetContentResolver(),
                Settings::Global::MULTI_SIM_VOICE_PROMPT, value);
        Logd("setVoicePromptOption to " + enabled);
    }

    //@Override
    public Int64 GetOnDemandDataSubId() {
        return GetCurrentDds();
    }

    CARAPI RegisterForOnDemandDdsLockNotification(Int64 clientSubId,
            OnDemandDdsLockNotifier callback) {
        Logd("registerForOnDemandDdsLockNotification for client=" + clientSubId);
        mOnDemandDdsLockNotificationRegistrants->Put(clientSubId, callback);

    }

    /* {@hide} */
    CARAPI NotifyOnDemandDataSubIdChanged(NetworkRequest n) {
        OnDemandDdsLockNotifier notifier = mOnDemandDdsLockNotificationRegistrants->Get(
                GetSubIdFromNetworkRequest(n));
        If (notifier != NULL) {
            notifier->NotifyOnDemandDdsLockGranted(n);
        } else {
            Logd("No registrants for OnDemandDdsLockGranted event");
        }
    }

    public interface OnDemandDdsLockNotifier {
        CARAPI NotifyOnDemandDdsLockGranted(NetworkRequest n);
    }
    CARAPI RemoveStaleSubPreferences(String prefKey) {
        List<SubInfoRecord> subInfoList = GetAllSubInfoList();
        SharedPreferences sp = PreferenceManager->GetDefaultSharedPreferences(mContext);
        For (SubInfoRecord subInfo : subInfoList) {
            If (subInfo.slotId == -1) {
                sp->Edit()->Remove(prefKey+subInfo.subId).Commit();
            }
        }
    }
}
