#ifndef __ELASTOS_DROID_SERVER_CTELEPHONY_REGISTRY_H__
#define __ELASTOS_DROID_SERVER_CTELEPHONY_REGISTRY_H__

#include "_Elastos_Droid_Server_CTelephonyRegistry.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::IVoLteServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IDisconnectCause;
using Elastos::Droid::Telephony::IPreciseCallState;
using Elastos::Droid::Telephony::IPreciseDataConnectionState;
using Elastos::Droid::Telephony::IPreciseDisconnectCause;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Telephony::IITelephonyRegistry;
using Elastos::Droid::Internal::Telephony::IIPhoneStateListener;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICalendar;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::List;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {

class LogSSC
    : public Object
{
public:
    LogSSC();

    CARAPI Set(
        /* [in] */ ITime* t,
        /* [in] */ const String& s,
        /* [in] */ Int64 subId,
        /* [in] */ Int32 phoneId,
        /* [in] */ Elastos::Droid::Telephony::IServiceState* state);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<ITime> mTime;
    String mS;
    Int64 mSubId;
    Int32 mPhoneId;
    AutoPtr<Elastos::Droid::Telephony::IServiceState> mState;
};

}// Server
}// Droid
}// Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::LogSSC, IInterface)

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * Since phone process can be restarted, this class provides a centralized place
 * that applications can register and be called back from.
 *
 * Change-Id: I450c968bda93767554b5188ee63e10c9f43c5aa4 fixes bugs 16148026
 * and 15973975 by saving the phoneId of the registrant and then using the
 * phoneId when deciding to to make a callback. This is necessary because
 * a subId changes from to a dummy value when a SIM is removed and thus won't
 * compare properly. Because SubscriptionManager.getPhoneId(Int64 subId) handles
 * the dummy value conversion we properly do the callbacks.
 *
 * Eventually we may want to remove the notion of dummy value but for now this
 * looks like the best approach.
 */
CarClass(CTelephonyRegistry)
    , public Object
    , public IITelephonyRegistry
    , public IBinder
{
private:
    class Record
        : public Object
    {
    public:
        Record();

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        String mPkgForDebug;

        AutoPtr<IBinder> mBinder;
        AutoPtr<IIPhoneStateListener> mCallback;

        Int32 mCallerUid;
        Int32 mEvents;
        Int64 mSubId;
        Int32 mPhoneId;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CTelephonyRegistry::MyHandler")

        MyHandler(
            /* [in] */ CTelephonyRegistry* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CTelephonyRegistry* mHost;
    };

    class MyBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MyBroadcastReceiver(
            /* [in] */ CTelephonyRegistry* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        CTelephonyRegistry* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CTelephonyRegistry();

    virtual ~CTelephonyRegistry();

    // we keep a copy of all of the state so we can send it out when folks
    // register for it
    //
    // In these calls we call with the lock held. This is safe becasuse remote
    // calls go through a oneway interface and local calls going through a
    // handler before they get to app code.

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI SystemRunning();

    //@Override
    CARAPI Listen(
        /* [in] */ const String& pkgForDebug,
        /* [in] */ IIPhoneStateListener* callback,
        /* [in] */ Int32 events,
        /* [in] */ Boolean NotifyNow);

    //@Override
    CARAPI ListenForSubscriber(
        /* [in] */ const String& pkgForDebug,
        /* [in] */ Int64 subId,
        /* [in] */ IIPhoneStateListener* callback,
        /* [in] */ Int32 events,
        /* [in] */ Boolean NotifyNow);

    CARAPI Listen(
        /* [in] */ const String& pkgForDebug,
        /* [in] */ IIPhoneStateListener* callback,
        /* [in] */ Int32 events,
        /* [in] */ Boolean NotifyNow,
        /* [in] */ Int64 subId);

    CARAPI Remove(
        /* [in] */ IBinder* binder);

    CARAPI NotifyCallState(
        /* [in] */ Int32 state,
        /* [in] */ const String& incomingNumber);

    CARAPI NotifyCallStateForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 state,
        /* [in] */ const String& incomingNumber);

    CARAPI NotifyServiceStateForPhoneId(
        /* [in] */ Int32 phoneId,
        /* [in] */ Int64 subId,
        /* [in] */ Elastos::Droid::Telephony::IServiceState* state);

    CARAPI NotifySignalStrength(
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI NotifySignalStrengthForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI NotifyCellInfo(
        /* [in] */ IList* cellInfo); //<CellInfo>

    CARAPI NotifyCellInfoForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ IList* cellInfo);

    CARAPI NotifyDataConnectionRealTimeInfo(
        /* [in] */ IDataConnectionRealTimeInfo* dcRtInfo);

    //@Override
    CARAPI NotifyMessageWaitingChangedForPhoneId(
        /* [in] */ Int32 phoneId,
        /* [in] */ Int64 subId,
        /* [in] */ Boolean mwi);

    CARAPI NotifyCallForwardingChanged(
        /* [in] */ Boolean cfi);

    CARAPI NotifyCallForwardingChangedForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Boolean cfi);

    CARAPI NotifyDataActivity(
        /* [in] */ Int32 state);

    CARAPI NotifyDataActivityForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 state);

    CARAPI NotifyDataConnection(
        /* [in] */ Int32 state,
        /* [in] */ Boolean isDataConnectivityPossible,
        /* [in] */ const String& reason,
        /* [in] */ const String& apn,
        /* [in] */ const String& apnType,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean roaming);

    CARAPI NotifyDataConnectionForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ Int32 state,
        /* [in] */ Boolean isDataConnectivityPossible,
        /* [in] */ const String& reason,
        /* [in] */ const String& apn,
        /* [in] */ const String& apnType,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean roaming);

    CARAPI NotifyDataConnectionFailed(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType);

    CARAPI NotifyDataConnectionFailedForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType);

    CARAPI NotifyCellLocation(
        /* [in] */ IBundle* cellLocation);

    CARAPI NotifyCellLocationForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ IBundle* cellLocation);

    CARAPI NotifyOtaspChanged(
        /* [in] */ Int32 otaspMode);

    CARAPI NotifyPreciseCallState(
        /* [in] */ Int32 ringingCallState,
        /* [in] */ Int32 foregroundCallState,
        /* [in] */ Int32 backgroundCallState);

    CARAPI NotifyDisconnectCause(
        /* [in] */ Int32 disconnectCause,
        /* [in] */ Int32 preciseDisconnectCause);

    CARAPI NotifyPreciseDataConnectionFailed(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ const String& apn,
        /* [in] */ const String& failCause);

    CARAPI NotifyVoLteServiceStateChanged(
        /* [in] */ IVoLteServiceState* lteState);

    CARAPI NotifyOemHookRawEventForSubscriber(
        /* [in] */ Int64 subId,
        /* [in] */ ArrayOf<Byte>* rawData);

    //@Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    //
    // the legacy intent Broadcasting
    //

    CARAPI BroadcastServiceStateChanged(
        /* [in] */ Elastos::Droid::Telephony::IServiceState* state,
        /* [in] */ Int64 subId);

    CARAPI BroadcastSignalStrengthChanged(
        /* [in] */ ISignalStrength* signalStrength,
        /* [in] */ Int64 subId);

    CARAPI BroadcastCallStateChanged(
        /* [in] */ Int32 state,
        /* [in] */ const String& incomingNumber,
        /* [in] */ Int64 subId);

    CARAPI BroadcastDataConnectionStateChanged(
        /* [in] */ Int32 state,
        /* [in] */ Boolean isDataConnectivityPossible,
        /* [in] */ const String& reason,
        /* [in] */ const String& apn,
        /* [in] */ const String& apnType,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ INetworkCapabilities* networkCapabilities,
        /* [in] */ Boolean roaming,
        /* [in] */ Int64 subId);

    CARAPI BroadcastDataConnectionFailed(
        /* [in] */ const String& reason,
        /* [in] */ const String& apnType,
        /* [in] */ Int64 subId);

    CARAPI BroadcastPreciseCallStateChanged(
        /* [in] */ Int32 ringingCallState,
        /* [in] */ Int32 foregroundCallState,
        /* [in] */ Int32 backgroundCallState,
        /* [in] */ Int32 disconnectCause,
        /* [in] */ Int32 preciseDisconnectCause);

    CARAPI BroadcastPreciseDataConnectionStateChanged(
        /* [in] */ Int32 state,
        /* [in] */ Int32 networkType,
        /* [in] */ const String& apnType,
        /* [in] */ const String& apn,
        /* [in] */ const String& reason,
        /* [in] */ ILinkProperties* linkProperties,
        /* [in] */ const String& failCause);

    CARAPI_(Boolean) CheckNotifyPermission(
        /* [in] */ const String& method);

    CARAPI CheckListenerPermission(
        /* [in] */ Int32 events);

    CARAPI HandleRemoveListLocked();

    CARAPI_(Boolean) ValidateEventsAndUserLocked(
        /* [in] */ Record* r,
        /* [in] */ Int32 events);

    CARAPI_(Boolean) ValidatePhoneId(
        /* [in] */ Int32 phoneId);

    static CARAPI_(void) Log(
        /* [in] */ const String& s);

    CARAPI LogServiceStateChanged(
        /* [in] */ const String& s,
        /* [in] */ Int64 subId,
        /* [in] */ Int32 phoneId,
        /* [in] */ Elastos::Droid::Telephony::IServiceState* state);

    CARAPI ToStringLogSSC(
        /* [in] */ const String& prompt);

private:
    static const String TAG;
    static const Boolean DBG;// = FALSE; // STOPSHIP if true
    static const Boolean DBG_LOC;// = FALSE; // STOPSHIP if true
    static const Boolean VDBG;// = FALSE; // STOPSHIP if true

    AutoPtr<IContext> mContext;

    // access should be inside synchronized (mRecords) for these two fields
    List<AutoPtr<IBinder> > mRemoveList;
    List<AutoPtr<Record> > mRecords;
    Object mRecordsLock;

    AutoPtr<IIBatteryStats> mBatteryStats;

    Int32 mNumPhones;

    AutoPtr<ArrayOf<Int32> > mCallState;

    AutoPtr<ArrayOf<String> > mCallIncomingNumber;

    AutoPtr<ArrayOf<Elastos::Droid::Telephony::IServiceState*> > mServiceState;

    AutoPtr<ArrayOf<ISignalStrength*> > mSignalStrength;

    AutoPtr<ArrayOf<Boolean> > mMessageWaiting;

    AutoPtr<ArrayOf<Boolean> > mCallForwarding;

    AutoPtr<ArrayOf<Int32> > mDataActivity;

    AutoPtr<ArrayOf<Int32> > mDataConnectionState;

    AutoPtr<ArrayOf<Boolean> > mDataConnectionPossible;

    AutoPtr<ArrayOf<String> > mDataConnectionReason;

    AutoPtr<ArrayOf<String> > mDataConnectionApn;

    AutoPtr<ArrayOf<IArrayList*> > mConnectedApns;

    AutoPtr<ArrayOf<ILinkProperties*> > mDataConnectionLinkProperties;

    AutoPtr<ArrayOf<INetworkCapabilities*> > mDataConnectionNetworkCapabilities;

    AutoPtr<ArrayOf<IBundle*> > mCellLocation;

    AutoPtr<ArrayOf<Int32> > mDataConnectionNetworkType;

    Int32 mOtaspMode;

    AutoPtr<IArrayList> mCellInfo;

    AutoPtr<IVoLteServiceState> mVoLteServiceState;

    AutoPtr<IDataConnectionRealTimeInfo> mDcRtInfo;

    Int32 mRingingCallState;

    Int32 mForegroundCallState;

    Int32 mBackgroundCallState;

    AutoPtr<IPreciseCallState> mPreciseCallState;

    AutoPtr<IPreciseDataConnectionState> mPreciseDataConnectionState;

    static const Int32 PHONE_STATE_PERMISSION_MASK;

    static const Int32 PRECISE_PHONE_STATE_PERMISSION_MASK;

    static const Int32 MSG_USER_SWITCHED = 1;
    static const Int32 MSG_UPDATE_DEFAULT_SUB = 2;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    AutoPtr<ArrayOf<LogSSC*> > mLogSSC;
    Int32 mNext;
};

}// Server
}// Droid
}// Elastos

#endif //__ELASTOS_DROID_SERVER_CTELEPHONY_REGISTRY_H__
