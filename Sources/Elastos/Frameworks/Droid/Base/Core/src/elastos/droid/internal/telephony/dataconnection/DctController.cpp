/*
 * Copyright (C) 2014 MediaTek Inc.
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

package com.android.internal.telephony.dataconnection;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Net::INetworkRequest;

using Elastos::Droid::Internal::Os::ISomeArgs;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using Elastos::Droid::Internal::Telephony::IDefaultPhoneNotifier;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Telephony::Dataconnection::IDdsScheduler;

using Elastos::Droid::Utility::ILog;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IIterator;
using Elastos::Droid::Os::IRegistrant;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Droid::Telephony::IRlog;

public class DctController extends Handler {
    private static const String LOG_TAG = "DctController";
    private static const Boolean DBG = TRUE;

    private static const Int32 EVENT_PHONE1_DETACH = 1;
    private static const Int32 EVENT_PHONE2_DETACH = 2;
    private static const Int32 EVENT_PHONE3_DETACH = 3;
    private static const Int32 EVENT_PHONE4_DETACH = 4;
    private static const Int32 EVENT_PHONE1_RADIO_OFF = 5;
    private static const Int32 EVENT_PHONE2_RADIO_OFF = 6;
    private static const Int32 EVENT_PHONE3_RADIO_OFF = 7;
    private static const Int32 EVENT_PHONE4_RADIO_OFF = 8;
    private static const Int32 EVENT_START_DDS_SWITCH = 9;

    private static const Int32 PHONE_NONE = -1;

    private static DctController sDctController;

    private static const Int32 EVENT_ALL_DATA_DISCONNECTED = 1;
    private static const Int32 EVENT_SET_DATA_ALLOW_DONE = 2;
    private static const Int32 EVENT_DELAYED_RETRY = 3;
    private static const Int32 EVENT_LEGACY_SET_DATA_SUBSCRIPTION = 4;
    private static const Int32 EVENT_SET_DATA_ALLOW_FALSE = 5;

    private RegistrantList mNotifyDefaultDataSwitchInfo = new RegistrantList();
    private RegistrantList mNotifyOnDemandDataSwitchInfo = new RegistrantList();
    private RegistrantList mNotifyOnDemandPsAttach = new RegistrantList();
    private SubscriptionController mSubController = SubscriptionController->GetInstance();

    private Phone mActivePhone;
    private Int32 mPhoneNum;
    private Boolean[] mServicePowerOffFlag;
    private PhoneProxy[] mPhones;
    private DcSwitchState[] mDcSwitchState;
    private DcSwitchAsyncChannel[] mDcSwitchAsyncChannel;
    private Handler[] mDcSwitchStateHandler;

    private HashSet<String> mApnTypes = new HashSet<String>();

    private BroadcastReceiver mDataStateReceiver;
    private Context mContext;

    private AsyncChannel mDdsSwitchPropService;

    private Int32 mCurrentDataPhone = PHONE_NONE;
    private Int32 mRequestedDataPhone = PHONE_NONE;

    private DdsSwitchSerializerHandler mDdsSwitchSerializer;
    private Boolean mIsDdsSwitchCompleted = TRUE;

    private final Int32 MAX_RETRY_FOR_ATTACH = 6;
    private final Int32 ATTACH_RETRY_DELAY = 1000 * 10;

    private Handler mRspHander = new Handler() {
        CARAPI HandleMessage(Message msg){
            AsyncResult ar;
            Switch(msg.what) {
                case EVENT_PHONE1_DETACH:
                case EVENT_PHONE2_DETACH:
                case EVENT_PHONE3_DETACH:
                case EVENT_PHONE4_DETACH:
                    Logd("EVENT_PHONE" + msg.what +
                            "_DETACH: mRequestedDataPhone=" + mRequestedDataPhone);
                    mCurrentDataPhone = PHONE_NONE;
                    If (mRequestedDataPhone != PHONE_NONE) {
                        mCurrentDataPhone = mRequestedDataPhone;
                        mRequestedDataPhone = PHONE_NONE;

                        Iterator<String> itrType = mApnTypes->Iterator();
                        While (itrType->HasNext()) {
                            mDcSwitchAsyncChannel[mCurrentDataPhone].ConnectSync(itrType->Next());
                        }
                        mApnTypes->Clear();
                    }
                break;

                case EVENT_PHONE1_RADIO_OFF:
                case EVENT_PHONE2_RADIO_OFF:
                case EVENT_PHONE3_RADIO_OFF:
                case EVENT_PHONE4_RADIO_OFF:
                    Logd("EVENT_PHONE" + (msg.what - EVENT_PHONE1_RADIO_OFF + 1) + "_RADIO_OFF.");
                    mServicePowerOffFlag[msg.what - EVENT_PHONE1_RADIO_OFF] = TRUE;
                break;

                default:
                break;
            }
        }
    };

    private DefaultPhoneNotifier.IDataStateChangedCallback mDataStateChangedCallback =
            new DefaultPhoneNotifier->IDataStateChangedCallback() {
        CARAPI OnDataStateChanged(Int64 subId, String state, String reason,
                String apnName, String apnType, Boolean unavailable) {
            Logd("[DataStateChanged]:" + "state=" + state + ",reason=" + reason
                      + ",apnName=" + apnName + ",apnType=" + apnType + ",from subId=" + subId);
            Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
            mDcSwitchState[phoneId].NotifyDataConnection(phoneId, state, reason,
                    apnName, apnType, unavailable);
        }
    };

    Boolean IsActiveSubId(Int64 subId) {
        Int64[] activeSubs = mSubController->GetActiveSubIdList();
        For (Int32 i = 0; i < activeSubs.length; i++) {
            If (subId == activeSubs[i]) {
                return TRUE;
            }
        }
        return FALSE;
    }


    private class DataStateReceiver extends BroadcastReceiver {
        CARAPI OnReceive(Context context, Intent intent) {
            Synchronized(this) {
                If (intent->GetAction()->Equals(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED)) {
                    ServiceState ss = ServiceState->NewFromBundle(intent->GetExtras());

                    Int64 subId = intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY, PhoneConstants.SUB1);
                    Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
                    // for the case of network out of service when Bootup (ignore dummy values too)
                    If (!SubscriptionManager->IsValidSubId(subId) || (subId < 0) ||
                            !IsActiveSubId(subId)) {
                        // FIXME: Maybe add SM->IsRealSubId(subId)??
                        Logd("DataStateReceiver: ignore invalid subId=" + subId
                                + " phoneId = " + phoneId);
                        return;
                    }
                    If (!SubscriptionManager->IsValidPhoneId(phoneId)) {
                        Logd("DataStateReceiver: ignore invalid phoneId=" + phoneId);
                        return;
                    }

                    Boolean prevPowerOff = mServicePowerOffFlag[phoneId];
                    If (ss != NULL) {
                        Int32 state = ss->GetState();
                        Switch (state) {
                            case ServiceState.STATE_POWER_OFF:
                                mServicePowerOffFlag[phoneId] = TRUE;
                                Logd("DataStateReceiver: STATE_POWER_OFF Intent from phoneId="
                                        + phoneId);
                                break;
                            case ServiceState.STATE_IN_SERVICE:
                                mServicePowerOffFlag[phoneId] = FALSE;
                                Logd("DataStateReceiver: STATE_IN_SERVICE Intent from phoneId="
                                        + phoneId);
                                break;
                            case ServiceState.STATE_OUT_OF_SERVICE:
                                Logd("DataStateReceiver: STATE_OUT_OF_SERVICE Intent from phoneId="
                                        + phoneId);
                                If (mServicePowerOffFlag[phoneId]) {
                                    mServicePowerOffFlag[phoneId] = FALSE;
                                }
                                break;
                            case ServiceState.STATE_EMERGENCY_ONLY:
                                Logd("DataStateReceiver: STATE_EMERGENCY_ONLY Intent from phoneId="
                                        + phoneId);
                                break;
                            default:
                                Logd("DataStateReceiver: SERVICE_STATE_CHANGED invalid state");
                                break;
                        }

                        If (prevPowerOff && mServicePowerOffFlag[phoneId] == FALSE &&
                                mCurrentDataPhone == PHONE_NONE &&
                                phoneId == GetDataConnectionFromSetting()) {
                            Logd("DataStateReceiver: Current Phone is none and default phoneId="
                                    + phoneId + ", then EnableApnType()");
                            EnableApnType(subId, PhoneConstants.APN_TYPE_DEFAULT);
                        }
                    }
                }
            }
        }
    }

    public DefaultPhoneNotifier.IDataStateChangedCallback GetDataStateChangedCallback() {
        return mDataStateChangedCallback;
    }

    public static DctController GetInstance() {
       If (sDctController == NULL) {
        throw new RuntimeException(
            "DCTrackerController.getInstance can't be called before MakeDCTController()");
        }
       return sDctController;
    }

    public static DctController MakeDctController(PhoneProxy[] phones, Looper looper) {
        If (sDctController == NULL) {
            sDctController = new DctController(phones, looper);
            DdsScheduler->Init();
        }
        return sDctController;
    }

    private DctController(PhoneProxy[] phones, Looper looper) {
        Super(looper);
        If (phones == NULL || phones.length == 0) {
            If (phones == NULL) {
                Loge("DctController(phones): UNEXPECTED phones=NULL, ignore");
            } else {
                Loge("DctController(phones): UNEXPECTED phones.length=0, ignore");
            }
            return;
        }
        mPhoneNum = phones.length;
        mServicePowerOffFlag = new Boolean[mPhoneNum];
        mPhones = phones;

        mDcSwitchState = new DcSwitchState[mPhoneNum];
        mDcSwitchAsyncChannel = new DcSwitchAsyncChannel[mPhoneNum];
        mDcSwitchStateHandler = new Handler[mPhoneNum];

        mActivePhone = mPhones[0];

        For (Int32 i = 0; i < mPhoneNum; ++i) {
            Int32 phoneId = i;
            mServicePowerOffFlag[i] = TRUE;
            mDcSwitchState[i] = new DcSwitchState(mPhones[i], "DcSwitchState-" + phoneId, phoneId);
            mDcSwitchState[i].Start();
            mDcSwitchAsyncChannel[i] = new DcSwitchAsyncChannel(mDcSwitchState[i], phoneId);
            mDcSwitchStateHandler[i] = new Handler();

            Int32 status = mDcSwitchAsyncChannel[i].FullyConnectSync(mPhones[i].GetContext(),
                mDcSwitchStateHandler[i], mDcSwitchState[i].GetHandler());

            If (status == AsyncChannel.STATUS_SUCCESSFUL) {
                Logd("DctController(phones): Connect success: " + i);
            } else {
                Loge("DctController(phones): Could not connect to " + i);
            }

            mDcSwitchState[i].RegisterForIdle(mRspHander, EVENT_PHONE1_DETACH + i, NULL);

            // Register for radio state change
            PhoneBase phoneBase = (PhoneBase)((PhoneProxy)mPhones[i]).GetActivePhone();
            phoneBase.mCi->RegisterForOffOrNotAvailable(mRspHander, EVENT_PHONE1_RADIO_OFF + i, NULL);
        }

        mContext = mActivePhone->GetContext();

        IntentFilter filter = new IntentFilter();
        filter->AddAction(TelephonyIntents.ACTION_DATA_CONNECTION_FAILED);
        filter->AddAction(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED);

        mDataStateReceiver = new DataStateReceiver();
        Intent intent = mContext->RegisterReceiver(mDataStateReceiver, filter);

        HandlerThread t = new HandlerThread("DdsSwitchSerializer");
        t->Start();

        mDdsSwitchSerializer = new DdsSwitchSerializerHandler(t->GetLooper());

    }

    private IccCardConstants.State GetIccCardState(Int32 phoneId) {
        return mPhones[phoneId].GetIccCard()->GetState();
    }

    /**
     * Enable PDP interface by apn type and phone id
     *
     * @param type enable pdp interface by apn type, such as PhoneConstants.APN_TYPE_MMS, etc.
     * @param subId Indicate which sub to query
     * @return PhoneConstants.APN_REQUEST_STARTED: action is already started
     * PhoneConstants.APN_ALREADY_ACTIVE: interface has already active
     * PhoneConstants.APN_TYPE_NOT_AVAILABLE: invalid APN type
     * PhoneConstants.APN_REQUEST_FAILED: request failed
     * PhoneConstants.APN_REQUEST_FAILED_DUE_TO_RADIO_OFF: readio turn off
     * @see #DisableApnType()
     */
    public synchronized Int32 EnableApnType(Int64 subId, String type) {
        Int32 phoneId = SubscriptionManager->GetPhoneId(subId);

        If (phoneId == PHONE_NONE || !IsValidphoneId(phoneId)) {
            Logw("EnableApnType(): with PHONE_NONE or Invalid PHONE ID");
            return PhoneConstants.APN_REQUEST_FAILED;
        }

        Logd("EnableApnType():type=" + type + ",phoneId=" + phoneId +
                ",powerOff=" + mServicePowerOffFlag[phoneId]);

        If (!PhoneConstants.APN_TYPE_DEFAULT->Equals(type)) {
            For (Int32 peerphoneId =0; peerphoneId < mPhoneNum; peerphoneId++) {
                // check peer Phone has non default APN activated as receiving non default APN request.
                If (phoneId == peerphoneId) {
                    continue;
                }

                String[] activeApnTypes = mPhones[peerphoneId].GetActiveApnTypes();
                If (activeApnTypes != NULL && activeApnTypes.length != 0) {
                    For (Int32 i=0; i<activeApnTypes.length; i++) {
                        If (!PhoneConstants.APN_TYPE_DEFAULT->Equals(activeApnTypes[i]) &&
                                mPhones[peerphoneId].GetDataConnectionState(activeApnTypes[i]) !=
                                PhoneConstants.DataState.DISCONNECTED) {
                            Logd("enableApnType:Peer Phone still have non-default active APN type:"+
                                    "activeApnTypes[" + i + "]=" + activeApnTypes[i]);
                            return PhoneConstants.APN_REQUEST_FAILED;
                        }
                    }
                }
            }
        }

        Logd("EnableApnType(): CurrentDataPhone=" +
                    mCurrentDataPhone + ", RequestedDataPhone=" + mRequestedDataPhone);

        If (phoneId == mCurrentDataPhone &&
               !mDcSwitchAsyncChannel[mCurrentDataPhone].IsIdleOrDeactingSync()) {
           mRequestedDataPhone = PHONE_NONE;
           Logd("EnableApnType(): mRequestedDataPhone equals request PHONE ID.");
           return mDcSwitchAsyncChannel[phoneId].ConnectSync(type);
        } else {
            // Only can switch data when mCurrentDataPhone is PHONE_NONE,
            // it is set to PHONE_NONE only as receiving EVENT_PHONEX_DETACH
            If (mCurrentDataPhone == PHONE_NONE) {
                mCurrentDataPhone = phoneId;
                mRequestedDataPhone = PHONE_NONE;
                Logd("EnableApnType(): current PHONE is NONE or IDLE, mCurrentDataPhone=" +
                        mCurrentDataPhone);
                return mDcSwitchAsyncChannel[phoneId].ConnectSync(type);
            } else {
                Logd("EnableApnType(): current PHONE:" + mCurrentDataPhone + " is active.");
                If (phoneId != mRequestedDataPhone) {
                    mApnTypes->Clear();
                }
                mApnTypes->Add(type);
                mRequestedDataPhone = phoneId;
                mDcSwitchState[mCurrentDataPhone].CleanupAllConnection();
            }
        }
        return PhoneConstants.APN_REQUEST_STARTED;
    }

    /**
     * disable PDP interface by apn type and sub id
     *
     * @param type enable pdp interface by apn type, such as PhoneConstants.APN_TYPE_MMS, etc.
     * @param subId Indicate which sub to query
     * @return PhoneConstants.APN_REQUEST_STARTED: action is already started
     * PhoneConstants.APN_ALREADY_ACTIVE: interface has already active
     * PhoneConstants.APN_TYPE_NOT_AVAILABLE: invalid APN type
     * PhoneConstants.APN_REQUEST_FAILED: request failed
     * PhoneConstants.APN_REQUEST_FAILED_DUE_TO_RADIO_OFF: readio turn off
     * @see #EnableApnTypeGemini()
     */
    public synchronized Int32 DisableApnType(Int64 subId, String type) {

        Int32 phoneId = SubscriptionManager->GetPhoneId(subId);

        If (phoneId == PHONE_NONE || !IsValidphoneId(phoneId)) {
            Logw("DisableApnType(): with PHONE_NONE or Invalid PHONE ID");
            return PhoneConstants.APN_REQUEST_FAILED;
        }
        Logd("DisableApnType():type=" + type + ",phoneId=" + phoneId +
                ",powerOff=" + mServicePowerOffFlag[phoneId]);
        return mDcSwitchAsyncChannel[phoneId].DisconnectSync(type);
    }

    public Boolean IsDataConnectivityPossible(String type, Int32 phoneId) {
        If (phoneId == PHONE_NONE || !IsValidphoneId(phoneId)) {
            Logw("IsDataConnectivityPossible(): with PHONE_NONE or Invalid PHONE ID");
            return FALSE;
        } else {
            return mPhones[phoneId].IsDataConnectivityPossible(type);
        }
    }

    public Boolean IsIdleOrDeacting(Int32 phoneId) {
        If (mDcSwitchAsyncChannel[phoneId].IsIdleOrDeactingSync()) {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    private Boolean IsValidphoneId(Int32 phoneId) {
        return phoneId >= 0 && phoneId < mPhoneNum;
    }

    private Boolean IsValidApnType(String apnType) {
         If (apnType->Equals(PhoneConstants.APN_TYPE_DEFAULT)
             || apnType->Equals(PhoneConstants.APN_TYPE_MMS)
             || apnType->Equals(PhoneConstants.APN_TYPE_SUPL)
             || apnType->Equals(PhoneConstants.APN_TYPE_DUN)
             || apnType->Equals(PhoneConstants.APN_TYPE_HIPRI)
             || apnType->Equals(PhoneConstants.APN_TYPE_FOTA)
             || apnType->Equals(PhoneConstants.APN_TYPE_IMS)
             || apnType->Equals(PhoneConstants.APN_TYPE_CBS))
        {
            return TRUE;
        } else {
            return FALSE;
        }
    }

    private Int32 GetDataConnectionFromSetting(){
        Int64 subId = mSubController->GetDefaultDataSubId();
        Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
        return phoneId;
    }

    private static void Logv(String s) {
        Rlog->V(LOG_TAG, "[DctController] " + s);
    }

    private static void Logd(String s) {
        Rlog->D(LOG_TAG, "[DctController] " + s);
    }

    private static void Logw(String s) {
        Rlog->W(LOG_TAG, "[DctController] " + s);
    }

    private static void Loge(String s) {
        Rlog->E(LOG_TAG, "[DctController] " + s);
    }

    private class SwitchInfo {
        private Int32 mRetryCount = 0;

        public Int32 mPhoneId;
        public NetworkRequest mNetworkRequest;
        public Boolean mIsDefaultDataSwitchRequested;
        public Boolean mIsOnDemandPsAttachRequested;

        public SwitchInfo(Int32 phoneId, NetworkRequest n, Boolean flag, Boolean isAttachReq) {
            mPhoneId = phoneId;
            mNetworkRequest = n;
            mIsDefaultDataSwitchRequested = flag;
            mIsOnDemandPsAttachRequested = isAttachReq;
        }

        public SwitchInfo(Int32 phoneId,Boolean flag) {
            mPhoneId = phoneId;
            mNetworkRequest = NULL;
            mIsDefaultDataSwitchRequested = flag;
        }

        CARAPI IncRetryCount() {
            mRetryCount++;

        }

        public Boolean IsRetryPossible() {
            Return (mRetryCount < MAX_RETRY_FOR_ATTACH);
        }

        CARAPI ToString(
        /* [out] */ String* str)
    {
            return "SwitchInfo[phoneId = " + mPhoneId
                + ", NetworkRequest =" + mNetworkRequest
                + ", isDefaultSwitchRequested = " + mIsDefaultDataSwitchRequested
                + ", isOnDemandPsAttachRequested = " + mIsOnDemandPsAttachRequested
                + ", RetryCount = " + mRetryCount;
        }
    }

    private void DoDetach(Int32 phoneId) {
        Phone phone = mPhones[phoneId].GetActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
        dcTracker->SetDataAllowed(FALSE, NULL);
        If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            //cleanup data from apss as there is no detach procedure for CDMA
            dcTracker->CleanUpAllConnections("DDS switch");
        }
    }
    CARAPI SetDefaultDataSubId(Int64 reqSubId) {
        Int32 reqPhoneId = mSubController->GetPhoneId(reqSubId);
        Int64 currentDds = mSubController->GetCurrentDds();
        Int64 defaultDds = mSubController->GetDefaultDataSubId();
        SwitchInfo s = new SwitchInfo(new Integer(reqPhoneId), TRUE);
        Int32 currentDdsPhoneId = mSubController->GetPhoneId(currentDds);
        If (currentDdsPhoneId < 0 || currentDdsPhoneId >= mPhoneNum) {
            // If Current dds subId is invalid set the received subId as current DDS
            // This generally happens when device power-up first time.
            Logd(" setDefaultDataSubId,  reqSubId = " + reqSubId + " currentDdsPhoneId  "
                    + currentDdsPhoneId);
            mSubController->SetDataSubId(reqSubId);
            defaultDds = reqSubId;
            currentDdsPhoneId = mSubController->GetPhoneId(defaultDds);
        }
        Rlog->D(LOG_TAG, "setDefaultDataSubId reqSubId :" + reqSubId + " reqPhoneId = "
                + reqPhoneId);

        // Avoid sending data allow FALSE and TRUE on same sub .
        If ((reqSubId != defaultDds) && (reqPhoneId != currentDdsPhoneId)) {
            DoDetach(currentDdsPhoneId);
        } else {
            Logd("setDefaultDataSubId for default DDS, skip PS detach on DDS subs");
            SendMessage(ObtainMessage(EVENT_LEGACY_SET_DATA_SUBSCRIPTION,
                        new AsyncResult(s, NULL, NULL)));
            return;
        }

        mPhones[currentDdsPhoneId].RegisterForAllDataDisconnected(
                this, EVENT_ALL_DATA_DISCONNECTED, s);
    }

    private void InformDefaultDdsToPropServ(Int32 defDdsPhoneId) {
        If (mDdsSwitchPropService != NULL) {
            Logd("Inform OemHookDDS service of current DDS = " + defDdsPhoneId);
            mDdsSwitchPropService->SendMessageSynchronously(1, defDdsPhoneId,
                    mPhoneNum);
            Logd("OemHookDDS service finished");
        } else {
            Logd("OemHookDds service not ready yet");
        }

    }

    CARAPI DoPsAttach(NetworkRequest n) {
        Rlog->D(LOG_TAG, "doPsAttach for :" + n);

        Int64 subId = mSubController->GetSubIdFromNetworkRequest(n);

        Int32 phoneId = mSubController->GetPhoneId(subId);
        Phone phone = mPhones[phoneId].GetActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;

        //request only PS ATTACH on requested subscription.
        //No DdsSerealization lock required.
        SwitchInfo s = new SwitchInfo(new Integer(phoneId), n, FALSE, TRUE);

        Message psAttachDone = Message->Obtain(this,
                EVENT_SET_DATA_ALLOW_DONE, s);

        Int32 defDdsPhoneId = GetDataConnectionFromSetting();
        InformDefaultDdsToPropServ(defDdsPhoneId);
        dcTracker->SetDataAllowed(TRUE, psAttachDone);
    }

    /**
     * This is public API and client might call doPsDetach on DDS sub.
     * Ignore if thats the case.
     */
    CARAPI DoPsDetach() {
        Int64 currentDds = mSubController->GetCurrentDds();
        Int64 defaultDds = mSubController->GetDefaultDataSubId();

        If (currentDds == defaultDds) {
            Rlog->D(LOG_TAG, "PS DETACH on DDS sub is not allowed.");
            return;
        }
        Rlog->D(LOG_TAG, "doPsDetach for sub:" + currentDds);

        Int32 phoneId = mSubController->GetPhoneId(
                mSubController->GetCurrentDds());

        Phone phone = mPhones[phoneId].GetActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
        dcTracker->SetDataAllowed(FALSE, NULL);
    }

    CARAPI SetOnDemandDataSubId(NetworkRequest n) {
        Rlog->D(LOG_TAG, "setDataAllowed for :" + n);
        mDdsSwitchSerializer->SendMessage(mDdsSwitchSerializer
                .ObtainMessage(EVENT_START_DDS_SWITCH, n));
    }

    CARAPI RegisterForDefaultDataSwitchInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        Synchronized (mNotifyDefaultDataSwitchInfo) {
            mNotifyDefaultDataSwitchInfo->Add(r);
        }
    }

    CARAPI RegisterForOnDemandDataSwitchInfo(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        Synchronized (mNotifyOnDemandDataSwitchInfo) {
            mNotifyOnDemandDataSwitchInfo->Add(r);
        }
    }

    CARAPI RegisterForOnDemandPsAttach(Handler h, Int32 what, Object obj) {
        Registrant r = new Registrant (h, what, obj);
        Synchronized (mNotifyOnDemandPsAttach) {
            mNotifyOnDemandPsAttach->Add(r);
        }
    }

    CARAPI RegisterDdsSwitchPropService(Messenger messenger) {
        Logd("Got messenger from DDS switch service, messenger = " + messenger);
        AsyncChannel ac = new AsyncChannel();
        ac->Connect(mContext, sDctController, messenger);
    }

    //@Override
        CARAPI HandleMessage (Message msg) {
            Boolean isLegacySetDds = FALSE;
            Rlog->D(LOG_TAG, "handleMessage msg=" + msg);

            Switch (msg.what) {
                case EVENT_LEGACY_SET_DATA_SUBSCRIPTION:
                    isLegacySetDds = TRUE;
                    //intentional fall through, no break.
                case EVENT_ALL_DATA_DISCONNECTED: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;
                    Integer phoneId = s.mPhoneId;
                    Rlog->D(LOG_TAG, "EVENT_ALL_DATA_DISCONNECTED switchInfo :" + s +
                            " isLegacySetDds = " + isLegacySetDds);
                    // In this case prefPhoneId points to the newDds we are trying to
                    // set, hence we do not need to call unregister for data disconnected
                    If (!isLegacySetDds) {
                        Int32 prefPhoneId = mSubController->GetPhoneId(
                                 mSubController->GetCurrentDds());
                        mPhones[prefPhoneId].UnregisterForAllDataDisconnected(this);
                    }
                    Message allowedDataDone = Message->Obtain(this,
                            EVENT_SET_DATA_ALLOW_DONE, s);
                    Phone phone = mPhones[phoneId].GetActivePhone();

                    InformDefaultDdsToPropServ(phoneId);

                    DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
                    dcTracker->SetDataAllowed(TRUE, allowedDataDone);

                   break;
                }

                case EVENT_DELAYED_RETRY: {
                    Rlog->D(LOG_TAG, "EVENT_DELAYED_RETRY");
                    SomeArgs args = (SomeArgs)msg.obj;
                    try {
                        SwitchInfo s = (SwitchInfo)args.arg1;
                        Boolean psAttach = (Boolean)args.arg2;
                        Rlog->D(LOG_TAG, " Retry, switchInfo = " + s);

                        Integer phoneId = s.mPhoneId;
                        Int64[] subId = mSubController->GetSubId(phoneId);
                        Phone phone = mPhones[phoneId].GetActivePhone();
                        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;

                        If(psAttach) {
                            Message psAttachDone = Message->Obtain(this,
                                    EVENT_SET_DATA_ALLOW_DONE, s);
                            dcTracker->SetDataAllowed(TRUE, psAttachDone);
                        } else {
                            Message psDetachDone = Message->Obtain(this,
                                    EVENT_SET_DATA_ALLOW_FALSE, s);
                            dcTracker->SetDataAllowed(FALSE, psDetachDone);
                        }
                    } finally {
                        args->Recycle();
                    }
                    break;
                }

                case EVENT_SET_DATA_ALLOW_DONE: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;

                    Exception errorEx = NULL;

                    Integer phoneId = s.mPhoneId;
                    Int64[] subId = mSubController->GetSubId(phoneId);
                    Rlog->D(LOG_TAG, "EVENT_SET_DATA_ALLOWED_DONE  phoneId :" + subId[0]
                            + ", switchInfo = " + s);

                    If (ar.exception != NULL) {
                        Rlog->D(LOG_TAG, "Failed, switchInfo = " + s
                                + " attempt delayed retry");
                        s->IncRetryCount();
                        If ( s->IsRetryPossible()) {
                            SomeArgs args = SomeArgs->Obtain();
                            args.arg1 = s;
                            args.arg2 = TRUE;
                            SendMessageDelayed(ObtainMessage(EVENT_DELAYED_RETRY, args),
                                    ATTACH_RETRY_DELAY);
                            return;
                        } else {
                            Rlog->D(LOG_TAG, "Already did max retries, notify failure");
                            errorEx = new RuntimeException("PS ATTACH failed");
                       }
                    } else {
                        Rlog->D(LOG_TAG, "PS ATTACH success = " + s);
                    }

                    mDdsSwitchSerializer->UnLock();

                    If (s.mIsDefaultDataSwitchRequested) {
                        mNotifyDefaultDataSwitchInfo->NotifyRegistrants(
                                new AsyncResult(NULL, subId[0], errorEx));
                    } else If (s.mIsOnDemandPsAttachRequested) {
                        mNotifyOnDemandPsAttach->NotifyRegistrants(
                                new AsyncResult(NULL, s.mNetworkRequest, errorEx));
                    } else {
                        mNotifyOnDemandDataSwitchInfo->NotifyRegistrants(
                                new AsyncResult(NULL, s.mNetworkRequest, errorEx));
                    }
                    break;
                }

                case EVENT_SET_DATA_ALLOW_FALSE: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;

                    Exception errorEx = NULL;

                    Integer phoneId = s.mPhoneId;
                    Int64[] subId = mSubController->GetSubId(phoneId);
                    Rlog->D(LOG_TAG, "EVENT_SET_DATA_FALSE  phoneId :" + subId[0]
                            + ", switchInfo = " + s);

                    If (ar.exception != NULL) {
                        Rlog->D(LOG_TAG, "Failed, switchInfo = " + s
                                + " attempt delayed retry");
                        s->IncRetryCount();
                        If (s->IsRetryPossible()) {
                            SomeArgs args = SomeArgs->Obtain();
                            args.arg1 = s;
                            args.arg2 = FALSE;
                            SendMessageDelayed(ObtainMessage(EVENT_DELAYED_RETRY, args),
                                    ATTACH_RETRY_DELAY);
                            return;
                        } else {
                            Rlog->D(LOG_TAG, "Already did max retries, notify failure");
                            errorEx = new RuntimeException("PS DETACH failed");
                            mNotifyOnDemandDataSwitchInfo->NotifyRegistrants(
                                    new AsyncResult(NULL, s.mNetworkRequest, errorEx));
                       }
                    } else {
                        Rlog->D(LOG_TAG, "PS DETACH success = " + s);
                    }
                    break;
                }

                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
                    If(msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        Logd("HALF_CONNECTED: Connection successful with DDS switch"
                                + " service");
                        mDdsSwitchPropService = (AsyncChannel) msg.obj;
                    } else {
                        Logd("HALF_CONNECTED: Connection failed with"
                                +" DDS switch service, err = " + msg.arg1);
                    }
                       break;
                }

                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    Logd("Connection disconnected with DDS switch service");
                    mDdsSwitchPropService = NULL;
                    break;
                }
        }
    }

    class DdsSwitchSerializerHandler extends Handler {
        static const String TAG = "DdsSwitchSerializer";

        public DdsSwitchSerializerHandler(Looper looper) {
            Super(looper);
        }

        CARAPI UnLock() {
            Rlog->D(TAG, "unLock the DdsSwitchSerializer");
            Synchronized(this) {
                mIsDdsSwitchCompleted = TRUE;
                Rlog->D(TAG, "unLocked the DdsSwitchSerializer");
                NotifyAll();
            }

        }

        public Boolean IsLocked() {
            Synchronized(this) {
                Rlog->D(TAG, "isLocked = " + !mIsDdsSwitchCompleted);
                return !mIsDdsSwitchCompleted;
            }

        }

        //@Override
        CARAPI HandleMessage (Message msg) {
            Switch(msg.what) {
                case EVENT_START_DDS_SWITCH: {
                    Rlog->D(TAG, "EVENT_START_DDS_SWITCH");

                    try {
                        Synchronized(this) {
                            While(!mIsDdsSwitchCompleted) {
                                Rlog->D(TAG, "DDS switch in progress, wait");
                                Wait();
                            }

                            Rlog->D(TAG, "Locked!");
                            mIsDdsSwitchCompleted = FALSE;
                        }
                    } Catch (Exception e) {
                        Rlog->D(TAG, "Exception while serializing the DDS"
                                + " switch request , e=" + e);
                        return;
                    }

                    NetworkRequest n = (NetworkRequest)msg.obj;

                    Rlog->D(TAG, "start the DDS switch for req " + n);
                    Int64 subId = mSubController->GetSubIdFromNetworkRequest(n);

                    If(subId == mSubController->GetCurrentDds()) {
                        Rlog->D(TAG, "No change in DDS, respond back");
                        mIsDdsSwitchCompleted = TRUE;
                        mNotifyOnDemandDataSwitchInfo->NotifyRegistrants(
                                new AsyncResult(NULL, n, NULL));
                        return;
                    }
                    Int32 phoneId = mSubController->GetPhoneId(subId);
                    Int32 prefPhoneId = mSubController->GetPhoneId(
                            mSubController->GetCurrentDds());
                    Phone phone = mPhones[prefPhoneId].GetActivePhone();
                    DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
                    SwitchInfo s = new SwitchInfo(new Integer(phoneId), n, FALSE, FALSE);
                    Message dataAllowFalse = Message->Obtain(DctController.this,
                            EVENT_SET_DATA_ALLOW_FALSE, s);
                    dcTracker->SetDataAllowed(FALSE, dataAllowFalse);
                    If (phone->GetPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                        //cleanup data from apss as there is no detach procedure for CDMA
                        dcTracker->CleanUpAllConnections("Ondemand DDS switch");
                    }
                    mPhones[prefPhoneId].RegisterForAllDataDisconnected(
                            sDctController, EVENT_ALL_DATA_DISCONNECTED, s);
                    break;
                }
            }
        }
    }
    public Boolean IsDctControllerLocked() {
        return mDdsSwitchSerializer->IsLocked();
    }
}
