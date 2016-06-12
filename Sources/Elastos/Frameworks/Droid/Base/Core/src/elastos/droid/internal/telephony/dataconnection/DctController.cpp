
#include "elastos/droid/internal/telephony/dataconnection/DctController.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Net.h>

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

//=============================================================================
// DctController::SubHandler
//=============================================================================
DctController::SubHandler::SubHandler(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                AsyncResult ar;
                switch(msg.what) {
                    case EVENT_PHONE1_DETACH:
                    case EVENT_PHONE2_DETACH:
                    case EVENT_PHONE3_DETACH:
                    case EVENT_PHONE4_DETACH:
                        logd("EVENT_PHONE" + msg.what +
                                "_DETACH: mRequestedDataPhone=" + mRequestedDataPhone);
                        mCurrentDataPhone = PHONE_NONE;
                        if (mRequestedDataPhone != PHONE_NONE) {
                            mCurrentDataPhone = mRequestedDataPhone;
                            mRequestedDataPhone = PHONE_NONE;
                            Iterator<String> itrType = mApnTypes.iterator();
                            while (itrType.hasNext()) {
                                mDcSwitchAsyncChannel[mCurrentDataPhone].connectSync(itrType.next());
                            }
                            mApnTypes.clear();
                        }
                    break;
                    case EVENT_PHONE1_RADIO_OFF:
                    case EVENT_PHONE2_RADIO_OFF:
                    case EVENT_PHONE3_RADIO_OFF:
                    case EVENT_PHONE4_RADIO_OFF:
                        logd("EVENT_PHONE" + (msg.what - EVENT_PHONE1_RADIO_OFF + 1) + "_RADIO_OFF.");
                        mServicePowerOffFlag[msg.what - EVENT_PHONE1_RADIO_OFF] = true;
                    break;
                    default:
                    break;
                }

#endif
}

//=============================================================================
// DctController::SubIDataStateChangedCallback
//=============================================================================
CAR_INTERFACE_IMPL(DctController::SubIDataStateChangedCallback, Object, IDataStateChangedCallback)

DctController::SubIDataStateChangedCallback::SubIDataStateChangedCallback(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::SubIDataStateChangedCallback::OnDataStateChanged(
    /* [in] */ Int64 subId,
    /* [in] */ const String& state,
    /* [in] */ const String& reason,
    /* [in] */ const String& apnName,
    /* [in] */ const String& apnType,
    /* [in] */ Boolean unavailable)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                logd("[DataStateChanged]:" + "state=" + state + ",reason=" + reason
                          + ",apnName=" + apnName + ",apnType=" + apnType + ",from subId=" + subId);
                int phoneId = SubscriptionManager.getPhoneId(subId);
                mDcSwitchState[phoneId].notifyDataConnection(phoneId, state, reason,
                        apnName, apnType, unavailable);

#endif
}

//=============================================================================
// DctController::DataStateReceiver
//=============================================================================
DctController::DataStateReceiver::DataStateReceiver(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::DataStateReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized(this) {
                    if (intent.getAction().equals(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED)) {
                        ServiceState ss = ServiceState.newFromBundle(intent.getExtras());
                        long subId = intent.getLongExtra(PhoneConstants.SUBSCRIPTION_KEY, PhoneConstants.SUB1);
                        int phoneId = SubscriptionManager.getPhoneId(subId);
                        // for the case of network out of service when bootup (ignore dummy values too)
                        if (!SubscriptionManager.isValidSubId(subId) || (subId < 0) ||
                                !isActiveSubId(subId)) {
                            // FIXME: Maybe add SM.isRealSubId(subId)??
                            logd("DataStateReceiver: ignore invalid subId=" + subId
                                    + " phoneId = " + phoneId);
                            return;
                        }
                        if (!SubscriptionManager.isValidPhoneId(phoneId)) {
                            logd("DataStateReceiver: ignore invalid phoneId=" + phoneId);
                            return;
                        }
                        boolean prevPowerOff = mServicePowerOffFlag[phoneId];
                        if (ss != null) {
                            int state = ss.getState();
                            switch (state) {
                                case ServiceState.STATE_POWER_OFF:
                                    mServicePowerOffFlag[phoneId] = true;
                                    logd("DataStateReceiver: STATE_POWER_OFF Intent from phoneId="
                                            + phoneId);
                                    break;
                                case ServiceState.STATE_IN_SERVICE:
                                    mServicePowerOffFlag[phoneId] = false;
                                    logd("DataStateReceiver: STATE_IN_SERVICE Intent from phoneId="
                                            + phoneId);
                                    break;
                                case ServiceState.STATE_OUT_OF_SERVICE:
                                    logd("DataStateReceiver: STATE_OUT_OF_SERVICE Intent from phoneId="
                                            + phoneId);
                                    if (mServicePowerOffFlag[phoneId]) {
                                        mServicePowerOffFlag[phoneId] = false;
                                    }
                                    break;
                                case ServiceState.STATE_EMERGENCY_ONLY:
                                    logd("DataStateReceiver: STATE_EMERGENCY_ONLY Intent from phoneId="
                                            + phoneId);
                                    break;
                                default:
                                    logd("DataStateReceiver: SERVICE_STATE_CHANGED invalid state");
                                    break;
                            }
                            if (prevPowerOff && mServicePowerOffFlag[phoneId] == false &&
                                    mCurrentDataPhone == PHONE_NONE &&
                                    phoneId == getDataConnectionFromSetting()) {
                                logd("DataStateReceiver: Current Phone is none and default phoneId="
                                        + phoneId + ", then enableApnType()");
                                enableApnType(subId, PhoneConstants.APN_TYPE_DEFAULT);
                            }
                        }
                    }
                }

#endif
}

//=============================================================================
// DctController::
//=============================================================================
DctController::SwitchInfo::SwitchInfo(
    /* [in] */ DctController* host)
    : mRetryCount(0)
    , mHost(host)
{}

ECode DctController::SwitchInfo::constructor(
    /* [in] */ Int32 phoneId,
    /* [in] */ INetworkRequest* n,
    /* [in] */ Boolean flag,
    /* [in] */ Boolean isAttachReq)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mPhoneId = phoneId;
                mNetworkRequest = n;
                mIsDefaultDataSwitchRequested = flag;
                mIsOnDemandPsAttachRequested = isAttachReq;

#endif
}

ECode DctController::SwitchInfo::constructor(
    /* [in] */ Int32 phoneId,
    /* [in] */ Boolean flag)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mPhoneId = phoneId;
                mNetworkRequest = null;
                mIsDefaultDataSwitchRequested = flag;

#endif
}

ECode DctController::SwitchInfo::IncRetryCount()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                mRetryCount++;

#endif
}

ECode DctController::SwitchInfo::IsRetryPossible(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return (mRetryCount < MAX_RETRY_FOR_ATTACH);

#endif
}

ECode DctController::SwitchInfo::ToString(
    /* [out] */ String* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                return "SwitchInfo[phoneId = " + mPhoneId
                    + ", NetworkRequest =" + mNetworkRequest
                    + ", isDefaultSwitchRequested = " + mIsDefaultDataSwitchRequested
                    + ", isOnDemandPsAttachRequested = " + mIsOnDemandPsAttachRequested
                    + ", RetryCount = " + mRetryCount;

#endif
}

ECode DctController::SwitchInfo::GetMPhoneId(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPhoneId;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMPhoneId(
    /* [in] */ Int32 phoneId)
{
    mPhoneId = phoneId;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMNetworkRequest(
    /* [out] */ INetworkRequest** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNetworkRequest;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMNetworkRequest(
    /* [in] */ INetworkRequest* networkRequest)
{
    mNetworkRequest = networkRequest;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMIsDefaultDataSwitchRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsDefaultDataSwitchRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMIsDefaultDataSwitchRequested(
    /* [in] */ Boolean isDefaultDataSwitchRequested)
{
    mIsDefaultDataSwitchRequested = isDefaultDataSwitchRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::GetMIsOnDemandPsAttachRequested(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIsOnDemandPsAttachRequested;
    return NOERROR;
}

ECode DctController::SwitchInfo::SetMIsOnDemandPsAttachRequested(
    /* [in] */ Boolean isOnDemandPsAttachRequested)
{
    mIsOnDemandPsAttachRequested = isOnDemandPsAttachRequested;
    return NOERROR;
}

//=============================================================================
// DctController::DdsSwitchSerializerHandler
//=============================================================================
const String DctController::DdsSwitchSerializerHandler::TAG("DdsSwitchSerializer");

DctController::DdsSwitchSerializerHandler::DdsSwitchSerializerHandler(
    /* [in] */ DctController* host)
    : mHost(host)
{}

ECode DctController::DdsSwitchSerializerHandler::constructor(
    /* [in] */ ILooper* looper)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                super(looper);

#endif
}

ECode DctController::DdsSwitchSerializerHandler::UnLock()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                Rlog.d(TAG, "unLock the DdsSwitchSerializer");
                synchronized(this) {
                    mIsDdsSwitchCompleted = true;
                    Rlog.d(TAG, "unLocked the DdsSwitchSerializer");
                    notifyAll();
                }

#endif
}

ECode DctController::DdsSwitchSerializerHandler::IsLocked(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                synchronized(this) {
                    Rlog.d(TAG, "isLocked = " + !mIsDdsSwitchCompleted);
                    return !mIsDdsSwitchCompleted;
                }

#endif
}

ECode DctController::DdsSwitchSerializerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
                switch(msg.what) {
                    case EVENT_START_DDS_SWITCH: {
                        Rlog.d(TAG, "EVENT_START_DDS_SWITCH");
                        try {
                            synchronized(this) {
                                while(!mIsDdsSwitchCompleted) {
                                    Rlog.d(TAG, "DDS switch in progress, wait");
                                    wait();
                                }
                                Rlog.d(TAG, "Locked!");
                                mIsDdsSwitchCompleted = false;
                            }
                        } catch (Exception e) {
                            Rlog.d(TAG, "Exception while serializing the DDS"
                                    + " switch request , e=" + e);
                            return;
                        }
                        NetworkRequest n = (NetworkRequest)msg.obj;
                        Rlog.d(TAG, "start the DDS switch for req " + n);
                        long subId = mSubController.getSubIdFromNetworkRequest(n);
                        if(subId == mSubController.getCurrentDds()) {
                            Rlog.d(TAG, "No change in DDS, respond back");
                            mIsDdsSwitchCompleted = true;
                            mNotifyOnDemandDataSwitchInfo.notifyRegistrants(
                                    new AsyncResult(null, n, null));
                            return;
                        }
                        int phoneId = mSubController.getPhoneId(subId);
                        int prefPhoneId = mSubController.getPhoneId(
                                mSubController.getCurrentDds());
                        Phone phone = mPhones[prefPhoneId].getActivePhone();
                        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
                        SwitchInfo s = new SwitchInfo(new Integer(phoneId), n, false, false);
                        Message dataAllowFalse = Message.obtain(DctController.this,
                                EVENT_SET_DATA_ALLOW_FALSE, s);
                        dcTracker.setDataAllowed(false, dataAllowFalse);
                        if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
                            //cleanup data from apss as there is no detach procedure for CDMA
                            dcTracker.cleanUpAllConnections("Ondemand DDS switch");
                        }
                        mPhones[prefPhoneId].registerForAllDataDisconnected(
                                sDctController, EVENT_ALL_DATA_DISCONNECTED, s);
                        break;
                    }
                }

#endif
}

//=============================================================================
// DctController
//=============================================================================
CAR_INTERFACE_IMPL(DctController, Handler, IDctController)

const String DctController::LOG__TAG("DctController");
const Boolean DctController::DBG = true;
const Int32 DctController::EVENT_PHONE1_DETACH = 1;
const Int32 DctController::EVENT_PHONE2_DETACH = 2;
const Int32 DctController::EVENT_PHONE3_DETACH = 3;
const Int32 DctController::EVENT_PHONE4_DETACH = 4;
const Int32 DctController::EVENT_PHONE1_RADIO_OFF = 5;
const Int32 DctController::EVENT_PHONE2_RADIO_OFF = 6;
const Int32 DctController::EVENT_PHONE3_RADIO_OFF = 7;
const Int32 DctController::EVENT_PHONE4_RADIO_OFF = 8;
const Int32 DctController::EVENT_START_DDS_SWITCH = 9;
const Int32 DctController::PHONE_NONE = -1;
const Int32 DctController::EVENT_ALL_DATA_DISCONNECTED = 1;
const Int32 DctController::EVENT_SET_DATA_ALLOW_DONE = 2;
const Int32 DctController::EVENT_DELAYED_RETRY = 3;
const Int32 DctController::EVENT_LEGACY_SET_DATA_SUBSCRIPTION = 4;
const Int32 DctController::EVENT_SET_DATA_ALLOW_FALSE = 5;

ECode DctController::IsActiveSubId(
    /* [in] */ Int64 subId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long[] activeSubs = mSubController.getActiveSubIdList();
        for (int i = 0; i < activeSubs.length; i++) {
            if (subId == activeSubs[i]) {
                return true;
            }
        }
        return false;

#endif
}

ECode DctController::GetDataStateChangedCallback(
    /* [out] */ IDataStateChangedCallback** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDataStateChangedCallback;

#endif
}

ECode DctController::GetInstance(
    /* [out] */ IDctController** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
       if (sDctController == null) {
        throw new RuntimeException(
            "DCTrackerController.getInstance can't be called before makeDCTController()");
        }
       return sDctController;

#endif
}

ECode DctController::MakeDctController(
    /* [in] */ ArrayOf<IPhoneProxy*>* phones,
    /* [in] */ ILooper* looper,
    /* [out] */ IDctController** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (sDctController == null) {
            sDctController = new DctController(phones, looper);
            DdsScheduler.init();
        }
        return sDctController;

#endif
}

DctController::DctController()
    : mCurrentDataPhone(PHONE_NONE)
    , mRequestedDataPhone(PHONE_NONE)
    , mIsDdsSwitchCompleted(true)
    , MAX_RETRY_FOR_ATTACH(6)
    , ATTACH_RETRY_DELAY(1000 * 10)
{}

ECode DctController::constructor(
    /* [in] */ ArrayOf<IPhoneProxy*>* phones,
    /* [in] */ ILooper* looper)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        super(looper);
        if (phones == null || phones.length == 0) {
            if (phones == null) {
                loge("DctController(phones): UNEXPECTED phones=null, ignore");
            } else {
                loge("DctController(phones): UNEXPECTED phones.length=0, ignore");
            }
            return;
        }
        mPhoneNum = phones.length;
        mServicePowerOffFlag = new boolean[mPhoneNum];
        mPhones = phones;
        mDcSwitchState = new DcSwitchState[mPhoneNum];
        mDcSwitchAsyncChannel = new DcSwitchAsyncChannel[mPhoneNum];
        mDcSwitchStateHandler = new Handler[mPhoneNum];
        mActivePhone = mPhones[0];
        for (int i = 0; i < mPhoneNum; ++i) {
            int phoneId = i;
            mServicePowerOffFlag[i] = true;
            mDcSwitchState[i] = new DcSwitchState(mPhones[i], "DcSwitchState-" + phoneId, phoneId);
            mDcSwitchState[i].start();
            mDcSwitchAsyncChannel[i] = new DcSwitchAsyncChannel(mDcSwitchState[i], phoneId);
            mDcSwitchStateHandler[i] = new Handler();
            int status = mDcSwitchAsyncChannel[i].fullyConnectSync(mPhones[i].getContext(),
                mDcSwitchStateHandler[i], mDcSwitchState[i].getHandler());
            if (status == AsyncChannel.STATUS_SUCCESSFUL) {
                logd("DctController(phones): Connect success: " + i);
            } else {
                loge("DctController(phones): Could not connect to " + i);
            }
            mDcSwitchState[i].registerForIdle(mRspHander, EVENT_PHONE1_DETACH + i, null);
            // Register for radio state change
            PhoneBase phoneBase = (PhoneBase)((PhoneProxy)mPhones[i]).getActivePhone();
            phoneBase.mCi.registerForOffOrNotAvailable(mRspHander, EVENT_PHONE1_RADIO_OFF + i, null);
        }
        mContext = mActivePhone.getContext();
        IntentFilter filter = new IntentFilter();
        filter.addAction(TelephonyIntents.ACTION_DATA_CONNECTION_FAILED);
        filter.addAction(TelephonyIntents.ACTION_SERVICE_STATE_CHANGED);
        mDataStateReceiver = new DataStateReceiver();
        Intent intent = mContext.registerReceiver(mDataStateReceiver, filter);
        HandlerThread t = new HandlerThread("DdsSwitchSerializer");
        t.start();
        mDdsSwitchSerializer = new DdsSwitchSerializerHandler(t.getLooper());

    CRegistrantList::New((IRegistrantList**)&mNotifyDefaultDataSwitchInfo);
    CRegistrantList::New((IRegistrantList**)&mNotifyOnDemandDataSwitchInfo);
    CRegistrantList::New((IRegistrantList**)&mNotifyOnDemandPsAttach);
    CHashSet::New((IHashSet**)&mApnTypes);
    mRspHander = new SubHandler(this);
    mDataStateChangedCallback = new SubIDataStateChangedCallback(this);
    AutoPtr<ISubscriptionController> mSubController = SubscriptionController.getInstance();
#endif
}

ECode DctController::GetIccCardState(
    /* [in] */ Int32 phoneId,
    /* [out] */ IccCardConstantsState* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mPhones[phoneId].getIccCard().getState();

#endif
}

ECode DctController::EnableApnType(
    /* [in] */ Int64 subId,
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int phoneId = SubscriptionManager.getPhoneId(subId);
        if (phoneId == PHONE_NONE || !isValidphoneId(phoneId)) {
            logw("enableApnType(): with PHONE_NONE or Invalid PHONE ID");
            return PhoneConstants.APN_REQUEST_FAILED;
        }
        logd("enableApnType():type=" + type + ",phoneId=" + phoneId +
                ",powerOff=" + mServicePowerOffFlag[phoneId]);
        if (!PhoneConstants.APN_TYPE_DEFAULT.equals(type)) {
            for (int peerphoneId =0; peerphoneId < mPhoneNum; peerphoneId++) {
                // check peer Phone has non default APN activated as receiving non default APN request.
                if (phoneId == peerphoneId) {
                    continue;
                }
                String[] activeApnTypes = mPhones[peerphoneId].getActiveApnTypes();
                if (activeApnTypes != null && activeApnTypes.length != 0) {
                    for (int i=0; i<activeApnTypes.length; i++) {
                        if (!PhoneConstants.APN_TYPE_DEFAULT.equals(activeApnTypes[i]) &&
                                mPhones[peerphoneId].getDataConnectionState(activeApnTypes[i]) !=
                                PhoneConstants.DataState.DISCONNECTED) {
                            logd("enableApnType:Peer Phone still have non-default active APN type:"+
                                    "activeApnTypes[" + i + "]=" + activeApnTypes[i]);
                            return PhoneConstants.APN_REQUEST_FAILED;
                        }
                    }
                }
            }
        }
        logd("enableApnType(): CurrentDataPhone=" +
                    mCurrentDataPhone + ", RequestedDataPhone=" + mRequestedDataPhone);
        if (phoneId == mCurrentDataPhone &&
               !mDcSwitchAsyncChannel[mCurrentDataPhone].isIdleOrDeactingSync()) {
           mRequestedDataPhone = PHONE_NONE;
           logd("enableApnType(): mRequestedDataPhone equals request PHONE ID.");
           return mDcSwitchAsyncChannel[phoneId].connectSync(type);
        } else {
            // Only can switch data when mCurrentDataPhone is PHONE_NONE,
            // it is set to PHONE_NONE only as receiving EVENT_PHONEX_DETACH
            if (mCurrentDataPhone == PHONE_NONE) {
                mCurrentDataPhone = phoneId;
                mRequestedDataPhone = PHONE_NONE;
                logd("enableApnType(): current PHONE is NONE or IDLE, mCurrentDataPhone=" +
                        mCurrentDataPhone);
                return mDcSwitchAsyncChannel[phoneId].connectSync(type);
            } else {
                logd("enableApnType(): current PHONE:" + mCurrentDataPhone + " is active.");
                if (phoneId != mRequestedDataPhone) {
                    mApnTypes.clear();
                }
                mApnTypes.add(type);
                mRequestedDataPhone = phoneId;
                mDcSwitchState[mCurrentDataPhone].cleanupAllConnection();
            }
        }
        return PhoneConstants.APN_REQUEST_STARTED;

#endif
}

ECode DctController::DisableApnType(
    /* [in] */ Int64 subId,
    /* [in] */ const String& type,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int phoneId = SubscriptionManager.getPhoneId(subId);
        if (phoneId == PHONE_NONE || !isValidphoneId(phoneId)) {
            logw("disableApnType(): with PHONE_NONE or Invalid PHONE ID");
            return PhoneConstants.APN_REQUEST_FAILED;
        }
        logd("disableApnType():type=" + type + ",phoneId=" + phoneId +
                ",powerOff=" + mServicePowerOffFlag[phoneId]);
        return mDcSwitchAsyncChannel[phoneId].disconnectSync(type);

#endif
}

ECode DctController::IsDataConnectivityPossible(
    /* [in] */ const String& type,
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (phoneId == PHONE_NONE || !isValidphoneId(phoneId)) {
            logw("isDataConnectivityPossible(): with PHONE_NONE or Invalid PHONE ID");
            return false;
        } else {
            return mPhones[phoneId].isDataConnectivityPossible(type);
        }

#endif
}

ECode DctController::IsIdleOrDeacting(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mDcSwitchAsyncChannel[phoneId].isIdleOrDeactingSync()) {
            return true;
        } else {
            return false;
        }

#endif
}

ECode DctController::IsValidphoneId(
    /* [in] */ Int32 phoneId,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return phoneId >= 0 && phoneId < mPhoneNum;

#endif
}

ECode DctController::IsValidApnType(
    /* [in] */ const String& apnType,
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
         if (apnType.equals(PhoneConstants.APN_TYPE_DEFAULT)
             || apnType.equals(PhoneConstants.APN_TYPE_MMS)
             || apnType.equals(PhoneConstants.APN_TYPE_SUPL)
             || apnType.equals(PhoneConstants.APN_TYPE_DUN)
             || apnType.equals(PhoneConstants.APN_TYPE_HIPRI)
             || apnType.equals(PhoneConstants.APN_TYPE_FOTA)
             || apnType.equals(PhoneConstants.APN_TYPE_IMS)
             || apnType.equals(PhoneConstants.APN_TYPE_CBS))
        {
            return true;
        } else {
            return false;
        }

#endif
}

ECode DctController::GetDataConnectionFromSetting(
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long subId = mSubController.getDefaultDataSubId();
        int phoneId = SubscriptionManager.getPhoneId(subId);
        return phoneId;

#endif
}

ECode DctController::Logv(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.v(LOG__TAG, "[DctController] " + s);

#endif
}

ECode DctController::Logd(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, "[DctController] " + s);

#endif
}

ECode DctController::Logw(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.w(LOG__TAG, "[DctController] " + s);

#endif
}

ECode DctController::Loge(
    /* [in] */ const String& s)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.e(LOG__TAG, "[DctController] " + s);

#endif
}

ECode DctController::DoDetach(
    /* [in] */ Int32 phoneId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Phone phone = mPhones[phoneId].getActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
        dcTracker.setDataAllowed(false, null);
        if (phone.getPhoneType() == PhoneConstants.PHONE_TYPE_CDMA) {
            //cleanup data from apss as there is no detach procedure for CDMA
            dcTracker.cleanUpAllConnections("DDS switch");
        }

#endif
}

ECode DctController::SetDefaultDataSubId(
    /* [in] */ Int64 reqSubId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        int reqPhoneId = mSubController.getPhoneId(reqSubId);
        long currentDds = mSubController.getCurrentDds();
        long defaultDds = mSubController.getDefaultDataSubId();
        SwitchInfo s = new SwitchInfo(new Integer(reqPhoneId), true);
        int currentDdsPhoneId = mSubController.getPhoneId(currentDds);
        if (currentDdsPhoneId < 0 || currentDdsPhoneId >= mPhoneNum) {
            // If Current dds subId is invalid set the received subId as current DDS
            // This generally happens when device power-up first time.
            logd(" setDefaultDataSubId,  reqSubId = " + reqSubId + " currentDdsPhoneId  "
                    + currentDdsPhoneId);
            mSubController.setDataSubId(reqSubId);
            defaultDds = reqSubId;
            currentDdsPhoneId = mSubController.getPhoneId(defaultDds);
        }
        Rlog.d(LOG__TAG, "setDefaultDataSubId reqSubId :" + reqSubId + " reqPhoneId = "
                + reqPhoneId);
        // Avoid sending data allow false and true on same sub .
        if ((reqSubId != defaultDds) && (reqPhoneId != currentDdsPhoneId)) {
            doDetach(currentDdsPhoneId);
        } else {
            logd("setDefaultDataSubId for default DDS, skip PS detach on DDS subs");
            sendMessage(obtainMessage(EVENT_LEGACY_SET_DATA_SUBSCRIPTION,
                        new AsyncResult(s, null, null)));
            return;
        }
        mPhones[currentDdsPhoneId].registerForAllDataDisconnected(
                this, EVENT_ALL_DATA_DISCONNECTED, s);

#endif
}

ECode DctController::InformDefaultDdsToPropServ(
    /* [in] */ Int32 defDdsPhoneId)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        if (mDdsSwitchPropService != null) {
            logd("Inform OemHookDDS service of current DDS = " + defDdsPhoneId);
            mDdsSwitchPropService.sendMessageSynchronously(1, defDdsPhoneId,
                    mPhoneNum);
            logd("OemHookDDS service finished");
        } else {
            logd("OemHookDds service not ready yet");
        }

#endif
}

ECode DctController::DoPsAttach(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, "doPsAttach for :" + n);
        long subId = mSubController.getSubIdFromNetworkRequest(n);
        int phoneId = mSubController.getPhoneId(subId);
        Phone phone = mPhones[phoneId].getActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
        //request only PS ATTACH on requested subscription.
        //No DdsSerealization lock required.
        SwitchInfo s = new SwitchInfo(new Integer(phoneId), n, false, true);
        Message psAttachDone = Message.obtain(this,
                EVENT_SET_DATA_ALLOW_DONE, s);
        int defDdsPhoneId = getDataConnectionFromSetting();
        informDefaultDdsToPropServ(defDdsPhoneId);
        dcTracker.setDataAllowed(true, psAttachDone);

#endif
}

ECode DctController::DoPsDetach()
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long currentDds = mSubController.getCurrentDds();
        long defaultDds = mSubController.getDefaultDataSubId();
        if (currentDds == defaultDds) {
            Rlog.d(LOG__TAG, "PS DETACH on DDS sub is not allowed.");
            return;
        }
        Rlog.d(LOG__TAG, "doPsDetach for sub:" + currentDds);
        int phoneId = mSubController.getPhoneId(
                mSubController.getCurrentDds());
        Phone phone = mPhones[phoneId].getActivePhone();
        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
        dcTracker.setDataAllowed(false, null);

#endif
}

ECode DctController::SetOnDemandDataSubId(
    /* [in] */ INetworkRequest* n)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Rlog.d(LOG__TAG, "setDataAllowed for :" + n);
        mDdsSwitchSerializer.sendMessage(mDdsSwitchSerializer
                .obtainMessage(EVENT_START_DDS_SWITCH, n));

#endif
}

ECode DctController::RegisterForDefaultDataSwitchInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Registrant r = new Registrant (h, what, obj);
        synchronized (mNotifyDefaultDataSwitchInfo) {
            mNotifyDefaultDataSwitchInfo.add(r);
        }

#endif
}

ECode DctController::RegisterForOnDemandDataSwitchInfo(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Registrant r = new Registrant (h, what, obj);
        synchronized (mNotifyOnDemandDataSwitchInfo) {
            mNotifyOnDemandDataSwitchInfo.add(r);
        }

#endif
}

ECode DctController::RegisterForOnDemandPsAttach(
    /* [in] */ IHandler* h,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        Registrant r = new Registrant (h, what, obj);
        synchronized (mNotifyOnDemandPsAttach) {
            mNotifyOnDemandPsAttach.add(r);
        }

#endif
}

ECode DctController::RegisterDdsSwitchPropService(
    /* [in] */ IMessenger* messenger)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        logd("Got messenger from DDS switch service, messenger = " + messenger);
        AsyncChannel ac = new AsyncChannel();
        ac.connect(mContext, sDctController, messenger);

#endif
}

ECode DctController::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
            boolean isLegacySetDds = false;
            Rlog.d(LOG__TAG, "handleMessage msg=" + msg);
            switch (msg.what) {
                case EVENT_LEGACY_SET_DATA_SUBSCRIPTION:
                    isLegacySetDds = true;
                    //intentional fall through, no break.
                case EVENT_ALL_DATA_DISCONNECTED: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;
                    Integer phoneId = s.mPhoneId;
                    Rlog.d(LOG__TAG, "EVENT_ALL_DATA_DISCONNECTED switchInfo :" + s +
                            " isLegacySetDds = " + isLegacySetDds);
                    // In this case prefPhoneId points to the newDds we are trying to
                    // set, hence we do not need to call unregister for data disconnected
                    if (!isLegacySetDds) {
                        int prefPhoneId = mSubController.getPhoneId(
                                 mSubController.getCurrentDds());
                        mPhones[prefPhoneId].unregisterForAllDataDisconnected(this);
                    }
                    Message allowedDataDone = Message.obtain(this,
                            EVENT_SET_DATA_ALLOW_DONE, s);
                    Phone phone = mPhones[phoneId].getActivePhone();
                    informDefaultDdsToPropServ(phoneId);
                    DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
                    dcTracker.setDataAllowed(true, allowedDataDone);
                   break;
                }
                case EVENT_DELAYED_RETRY: {
                    Rlog.d(LOG__TAG, "EVENT_DELAYED_RETRY");
                    SomeArgs args = (SomeArgs)msg.obj;
                    try {
                        SwitchInfo s = (SwitchInfo)args.arg1;
                        boolean psAttach = (boolean)args.arg2;
                        Rlog.d(LOG__TAG, " Retry, switchInfo = " + s);
                        Integer phoneId = s.mPhoneId;
                        long[] subId = mSubController.getSubId(phoneId);
                        Phone phone = mPhones[phoneId].getActivePhone();
                        DcTrackerBase dcTracker =((PhoneBase)phone).mDcTracker;
                        if(psAttach) {
                            Message psAttachDone = Message.obtain(this,
                                    EVENT_SET_DATA_ALLOW_DONE, s);
                            dcTracker.setDataAllowed(true, psAttachDone);
                        } else {
                            Message psDetachDone = Message.obtain(this,
                                    EVENT_SET_DATA_ALLOW_FALSE, s);
                            dcTracker.setDataAllowed(false, psDetachDone);
                        }
                    } finally {
                        args.recycle();
                    }
                    break;
                }
                case EVENT_SET_DATA_ALLOW_DONE: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;
                    Exception errorEx = null;
                    Integer phoneId = s.mPhoneId;
                    long[] subId = mSubController.getSubId(phoneId);
                    Rlog.d(LOG__TAG, "EVENT_SET_DATA_ALLOWED_DONE  phoneId :" + subId[0]
                            + ", switchInfo = " + s);
                    if (ar.exception != null) {
                        Rlog.d(LOG__TAG, "Failed, switchInfo = " + s
                                + " attempt delayed retry");
                        s.incRetryCount();
                        if ( s.isRetryPossible()) {
                            SomeArgs args = SomeArgs.obtain();
                            args.arg1 = s;
                            args.arg2 = true;
                            sendMessageDelayed(obtainMessage(EVENT_DELAYED_RETRY, args),
                                    ATTACH_RETRY_DELAY);
                            return;
                        } else {
                            Rlog.d(LOG__TAG, "Already did max retries, notify failure");
                            errorEx = new RuntimeException("PS ATTACH failed");
                       }
                    } else {
                        Rlog.d(LOG__TAG, "PS ATTACH success = " + s);
                    }
                    mDdsSwitchSerializer.unLock();
                    if (s.mIsDefaultDataSwitchRequested) {
                        mNotifyDefaultDataSwitchInfo.notifyRegistrants(
                                new AsyncResult(null, subId[0], errorEx));
                    } else if (s.mIsOnDemandPsAttachRequested) {
                        mNotifyOnDemandPsAttach.notifyRegistrants(
                                new AsyncResult(null, s.mNetworkRequest, errorEx));
                    } else {
                        mNotifyOnDemandDataSwitchInfo.notifyRegistrants(
                                new AsyncResult(null, s.mNetworkRequest, errorEx));
                    }
                    break;
                }
                case EVENT_SET_DATA_ALLOW_FALSE: {
                    AsyncResult ar = (AsyncResult)msg.obj;
                    SwitchInfo s = (SwitchInfo)ar.userObj;
                    Exception errorEx = null;
                    Integer phoneId = s.mPhoneId;
                    long[] subId = mSubController.getSubId(phoneId);
                    Rlog.d(LOG__TAG, "EVENT_SET_DATA_FALSE  phoneId :" + subId[0]
                            + ", switchInfo = " + s);
                    if (ar.exception != null) {
                        Rlog.d(LOG__TAG, "Failed, switchInfo = " + s
                                + " attempt delayed retry");
                        s.incRetryCount();
                        if (s.isRetryPossible()) {
                            SomeArgs args = SomeArgs.obtain();
                            args.arg1 = s;
                            args.arg2 = false;
                            sendMessageDelayed(obtainMessage(EVENT_DELAYED_RETRY, args),
                                    ATTACH_RETRY_DELAY);
                            return;
                        } else {
                            Rlog.d(LOG__TAG, "Already did max retries, notify failure");
                            errorEx = new RuntimeException("PS DETACH failed");
                            mNotifyOnDemandDataSwitchInfo.notifyRegistrants(
                                    new AsyncResult(null, s.mNetworkRequest, errorEx));
                       }
                    } else {
                        Rlog.d(LOG__TAG, "PS DETACH success = " + s);
                    }
                    break;
                }
                case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED: {
                    if(msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
                        logd("HALF_CONNECTED: Connection successful with DDS switch"
                                + " service");
                        mDdsSwitchPropService = (AsyncChannel) msg.obj;
                    } else {
                        logd("HALF_CONNECTED: Connection failed with"
                                +" DDS switch service, err = " + msg.arg1);
                    }
                       break;
                }
                case AsyncChannel.CMD_CHANNEL_DISCONNECTED: {
                    logd("Connection disconnected with DDS switch service");
                    mDdsSwitchPropService = null;
                    break;
                }
        }

#endif
}

ECode DctController::IsDctControllerLocked(
    /* [out] */ Boolean* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return mDdsSwitchSerializer.isLocked();

#endif
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
