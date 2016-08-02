
#ifndef __ELASTOS_DROID_SERVER_TRUST_TRUSTAGENTWRAPPER_H__
#define __ELASTOS_DROID_SERVER_TRUST_TRUSTAGENTWRAPPER_H__

#include "_Elastos.Droid.Server.h"
#include <Elastos.Droid.Service.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/UserHandle.h>

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Service::Trust::IITrustAgentService;
using Elastos::Droid::Service::Trust::IITrustAgentServiceCallback;
using Elastosx::Net::Ssl::ITrustManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Trust {

class TrustManagerService;
/**
 * A wrapper around a TrustAgentService interface. Coordinates communication between
 * TrustManager and the actual TrustAgent.
 */
class TrustAgentWrapper
    : public Object
{
private:
    class InnerSub_BroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("TrustAgentWrapper::InnerSub_BroadcastReceiver")

        InnerSub_BroadcastReceiver(
            /* [in] */ TrustAgentWrapper* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        TrustAgentWrapper* mHost;
    };

    class InnerSub_Handler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TrustAgentWrapper::InnerSub_Handler")

        InnerSub_Handler(
            /* [in] */ TrustAgentWrapper* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TrustAgentWrapper* mHost;
    };

    class InnerSub_ITrustAgentServiceCallback
        : public Object
        , public IITrustAgentServiceCallback
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_ITrustAgentServiceCallback(
            /* [in] */ TrustAgentWrapper* host);

        // @Override
        CARAPI GrantTrust(
            /* [in] */ ICharSequence* userMessage,
            /* [in] */ Int64 durationMs,
            /* [in] */ Boolean initiatedByUser);

        // @Override
        CARAPI RevokeTrust();

        // @Override
        CARAPI SetManagingTrust(
            /* [in] */ Boolean managingTrust);

        // @Override
        CARAPI OnSetTrustAgentFeaturesEnabledCompleted(
            /* [in] */ Boolean result,
            /* [in] */ IBinder* token);

    private:
        TrustAgentWrapper* mHost;
    };

    class InnerSub_ServiceConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_ServiceConnection(
            /* [in] */ TrustAgentWrapper* host);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

    private:
        TrustAgentWrapper* mHost;
    };

public:
    TrustAgentWrapper();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ TrustManagerService* trustManagerService,
        /* [in] */ IIntent* intent,
        /* [in] */ IUserHandle* user);

    /**
     * @see android.service.trust.TrustAgentService#onUnlockAttempt(boolean)
     */
    CARAPI OnUnlockAttempt(
        /* [in] */ Boolean successful);

    /* package */
    CARAPI UpdateDevicePolicyFeatures(
        /* [out] */ Boolean* result);

    CARAPI IsTrusted(
        /* [out] */ Boolean* result);

    CARAPI IsManagingTrust(
        /* [out] */ Boolean* result);

    CARAPI GetMessage(
        /* [out] */ ICharSequence** result);

    CARAPI Unbind();

    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    CARAPI IsBound(
        /* [out] */ Boolean* result);

    /**
     * If not connected, returns the time at which the agent is restarted.
     *
     * @return restart time in uptime millis.
     */
    CARAPI GetScheduledRestartUptimeMillis(
        /* [out] */ Int64* result);

private:
    CARAPI OnError(
        /* [in] */ ECode ec);

    CARAPI OnTrustTimeout();

    CARAPI SetCallback(
        /* [in] */ IITrustAgentServiceCallback* callback);

    CARAPI ScheduleRestart();

private:
    static const String EXTRA_COMPONENT_NAME;

    static const String TRUST_EXPIRED_ACTION;

    static const String PERMISSION;

    static const Boolean DEBUG;

    static const String TAG;

    static const Int32 MSG_GRANT_TRUST;

    static const Int32 MSG_REVOKE_TRUST;

    static const Int32 MSG_TRUST_TIMEOUT;

    static const Int32 MSG_RESTART_TIMEOUT;

    static const Int32 MSG_SET_TRUST_AGENT_FEATURES_COMPLETED;

    static const Int32 MSG_MANAGING_TRUST;

    /**
     * Time in uptime millis that we wait for the service connection, both when starting
     * and when the service disconnects.
     */
    static const Int64 RESTART_TIMEOUT_MILLIS;

    /**
     * Long extra for {@link #MSG_GRANT_TRUST}
     */
    static const String DATA_DURATION;

    TrustManagerService* mTrustManagerService;

    Int32 mUserId;

    AutoPtr<IContext> mContext;

    AutoPtr<IComponentName> mName;

    AutoPtr<IITrustAgentService> mTrustAgentService;

    Boolean mBound;

    Int64 mScheduledRestartUptimeMillis;

    Int64 mMaximumTimeToLock;

    // Trust state
    Boolean mTrusted;

    AutoPtr<ICharSequence> mMessage;

    Boolean mTrustDisabledByDpm;

    Boolean mManagingTrust;

    AutoPtr<IBinder> mSetTrustAgentFeaturesToken;

    AutoPtr<IAlarmManager> mAlarmManager;

    AutoPtr<IIntent> mAlarmIntent;

    AutoPtr<IPendingIntent> mAlarmPendingIntent;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IITrustAgentServiceCallback> mCallback;

    AutoPtr<IServiceConnection> mConnection;
};

} // namespace Trust
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TRUST_TRUSTAGENTWRAPPER_H__
