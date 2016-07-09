
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTCONTROLLER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTCONTROLLER_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Internal::Telephony::IDefaultPhoneNotifier;
using Elastos::Droid::Internal::Telephony::IIccCardConstants;
using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneProxy;
using Elastos::Droid::Internal::Telephony::ISubscriptionController;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Net::INetworkRequest;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IRegistrantList;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

class DctController
    : public Handler
    , public IDctController
{
private:
    class SubHandler
        : public Handler
    {
    public:
        SubHandler(
            /* [in] */ DctController* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        DctController* mHost;
    };

    class SubIDataStateChangedCallback
        : public Object
        , public IDataStateChangedCallback
    {
    public:
        CAR_INTERFACE_DECL()

        SubIDataStateChangedCallback(
            /* [in] */ DctController* host);

        CARAPI OnDataStateChanged(
            /* [in] */ Int64 subId,
            /* [in] */ const String& state,
            /* [in] */ const String& reason,
            /* [in] */ const String& apnName,
            /* [in] */ const String& apnType,
            /* [in] */ Boolean unavailable);

        private:
            DctController* mHost;
    };

    class DataStateReceiver
        : public BroadcastReceiver
    {
    public:
        DataStateReceiver(
            /* [in] */ DctController* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DctController* mHost;
    };

    class SwitchInfo
        : public Object
    {
        friend class DctController;

    public:
        SwitchInfo(
            /* [in] */ DctController* host);

        CARAPI constructor(
            /* [in] */ Int32 phoneId,
            /* [in] */ INetworkRequest* n,
            /* [in] */ Boolean flag,
            /* [in] */ Boolean isAttachReq);

        CARAPI constructor(
            /* [in] */ Int32 phoneId,
            /* [in] */ Boolean flag);

        CARAPI IncRetryCount();

        CARAPI IsRetryPossible(
            /* [out] */ Boolean* result);

        CARAPI ToString(
            /* [out] */ String* result);

        CARAPI GetMPhoneId(
            /* [out] */ Int32* result);

        CARAPI SetMPhoneId(
            /* [in] */ Int32 mPhoneId);

        CARAPI GetMNetworkRequest(
            /* [out] */ INetworkRequest** result);

        CARAPI SetMNetworkRequest(
            /* [in] */ INetworkRequest* mNetworkRequest);

        CARAPI GetMIsDefaultDataSwitchRequested(
            /* [out] */ Boolean* result);

        CARAPI SetMIsDefaultDataSwitchRequested(
            /* [in] */ Boolean mIsDefaultDataSwitchRequested);

        CARAPI GetMIsOnDemandPsAttachRequested(
            /* [out] */ Boolean* result);

        CARAPI SetMIsOnDemandPsAttachRequested(
            /* [in] */ Boolean mIsOnDemandPsAttachRequested);

    private:
        Int32 mRetryCount;

        Int32 mPhoneId;

        AutoPtr<INetworkRequest> mNetworkRequest;

        Boolean mIsDefaultDataSwitchRequested;

        Boolean mIsOnDemandPsAttachRequested;

        DctController* mHost;
    };

    class DdsSwitchSerializerHandler
        : public Handler
    {
    public:
        DdsSwitchSerializerHandler(
            /* [in] */ DctController* host);

        CARAPI constructor(
            /* [in] */ ILooper* looper);

        CARAPI UnLock();

        CARAPI IsLocked(
            /* [out] */ Boolean* result);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const String TAG;

        DctController* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    // [@private]
    DctController();

    CARAPI IsActiveSubId(
        /* [in] */ Int64 subId,
        /* [out] */ Boolean* result);

    CARAPI GetDataStateChangedCallback(
        /* [out] */ IDataStateChangedCallback** result);

    static CARAPI GetInstance(
        /* [out] */ IDctController** result);

    static CARAPI MakeDctController(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones,
        /* [in] */ ILooper* looper,
        /* [out] */ IDctController** result);

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
     * @see #disableApnType()
     */
    CARAPI EnableApnType(
        /* [in] */ Int64 subId,
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

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
     * @see #enableApnTypeGemini()
     */
    CARAPI DisableApnType(
        /* [in] */ Int64 subId,
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI IsDataConnectivityPossible(
        /* [in] */ const String& type,
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    CARAPI IsIdleOrDeacting(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    CARAPI SetDefaultDataSubId(
        /* [in] */ Int64 reqSubId);

    CARAPI DoPsAttach(
        /* [in] */ INetworkRequest* n);

    /**
     * This is public API and client might call doPsDetach on DDS sub.
     * Ignore if thats the case.
     */
    CARAPI DoPsDetach();

    CARAPI SetOnDemandDataSubId(
        /* [in] */ INetworkRequest* n);

    CARAPI RegisterForDefaultDataSwitchInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI RegisterForOnDemandDataSwitchInfo(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI RegisterForOnDemandPsAttach(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* obj);

    CARAPI RegisterDdsSwitchPropService(
        /* [in] */ IMessenger* messenger);

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    CARAPI IsDctControllerLocked(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ ArrayOf<IPhoneProxy*>* phones,
        /* [in] */ ILooper* looper);

private:
    CARAPI GetIccCardState(
        /* [in] */ Int32 phoneId,
        /* [out] */ IccCardConstantsState* result);

    CARAPI IsValidphoneId(
        /* [in] */ Int32 phoneId,
        /* [out] */ Boolean* result);

    CARAPI IsValidApnType(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    CARAPI GetDataConnectionFromSetting(
        /* [out] */ Int32* result);

    static CARAPI Logv(
        /* [in] */ const char *fmt, ...);

    static CARAPI Logd(
        /* [in] */ const char *fmt, ...);

    static CARAPI Logw(
        /* [in] */ const char *fmt, ...);

    static CARAPI Loge(
        /* [in] */ const char *fmt, ...);

    CARAPI DoDetach(
        /* [in] */ Int32 phoneId);

    CARAPI InformDefaultDdsToPropServ(
        /* [in] */ Int32 defDdsPhoneId);

private:
    static const String LOG__TAG;

    static const Boolean DBG;

    static const Int32 EVENT_PHONE1_DETACH;

    static const Int32 EVENT_PHONE2_DETACH;

    static const Int32 EVENT_PHONE3_DETACH;

    static const Int32 EVENT_PHONE4_DETACH;

    static const Int32 EVENT_PHONE1_RADIO_OFF;

    static const Int32 EVENT_PHONE2_RADIO_OFF;

    static const Int32 EVENT_PHONE3_RADIO_OFF;

    static const Int32 EVENT_PHONE4_RADIO_OFF;

    static const Int32 EVENT_START_DDS_SWITCH;

    static const Int32 PHONE_NONE;

    static AutoPtr<IDctController> sDctController;

    static const Int32 EVENT_ALL_DATA_DISCONNECTED;

    static const Int32 EVENT_SET_DATA_ALLOW_DONE;

    static const Int32 EVENT_DELAYED_RETRY;

    static const Int32 EVENT_LEGACY_SET_DATA_SUBSCRIPTION;

    static const Int32 EVENT_SET_DATA_ALLOW_FALSE;

    AutoPtr<IRegistrantList> mNotifyDefaultDataSwitchInfo;

    AutoPtr<IRegistrantList> mNotifyOnDemandDataSwitchInfo;

    AutoPtr<IRegistrantList> mNotifyOnDemandPsAttach;

    AutoPtr<ISubscriptionController> mSubController;

    AutoPtr<IPhone> mActivePhone;

    Int32 mPhoneNum;

    AutoPtr<ArrayOf<Boolean> > mServicePowerOffFlag;

    AutoPtr<ArrayOf<IPhoneProxy*> > mPhones;

    AutoPtr<ArrayOf<IDcSwitchState*> > mDcSwitchState;

    AutoPtr<ArrayOf<IDcSwitchAsyncChannel*> > mDcSwitchAsyncChannel;

    AutoPtr<ArrayOf<IHandler*> > mDcSwitchStateHandler;

    AutoPtr<IHashSet> mApnTypes;

    AutoPtr<IBroadcastReceiver> mDataStateReceiver;

    AutoPtr<IContext> mContext;

    AutoPtr<IAsyncChannel> mDdsSwitchPropService;

    Int32 mCurrentDataPhone;

    Int32 mRequestedDataPhone;

    AutoPtr<DdsSwitchSerializerHandler> mDdsSwitchSerializer;

    Boolean mIsDdsSwitchCompleted;

    const Int32 MAX_RETRY_FOR_ATTACH;

    const Int32 ATTACH_RETRY_DELAY;

    AutoPtr<IHandler> mRspHander;

    AutoPtr<IDataStateChangedCallback> mDataStateChangedCallback;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTCONTROLLER_H__
