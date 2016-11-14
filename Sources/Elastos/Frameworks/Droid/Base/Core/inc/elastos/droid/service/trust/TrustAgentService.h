
#ifndef __ELASTOS_DROID_SERVICE_TRUST_TRUSTAGENTSERVICE_H__
#define __ELASTOS_DROID_SERVICE_TRUST_TRUSTAGENTSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Trust {

/**
 * A service that notifies the system about whether it believes the environment of the device
 * to be trusted.
 *
 * <p>Trust agents may only be provided by the platform. It is expected that there is only
 * one trust agent installed on the platform. In the event there is more than one,
 * either trust agent can enable trust.
 * </p>
 *
 * <p>To extend this class, you must declare the service in your manifest file with
 * the {@link android.Manifest.permission#BIND_TRUST_AGENT} permission
 * and include an intent filter with the {@link #SERVICE_INTERFACE} action. For example:</p>
 * <pre>
 * &lt;service android:name=".TrustAgent"
 *          android:label="&#64;string/service_name"
 *          android:permission="android.permission.BIND_TRUST_AGENT">
 *     &lt;intent-filter>
 *         &lt;action android:name="android.service.trust.TrustAgentService" />
 *     &lt;/intent-filter>
 *     &lt;meta-data android:name="android.service.trust.trustagent"
 *          android:value="&#64;xml/trust_agent" />
 * &lt;/service></pre>
 *
 * <p>The associated meta-data file can specify an activity that is accessible through Settings
 * and should allow configuring the trust agent, as defined in
 * {@link android.R.styleable#TrustAgent}. For example:</p>
 *
 * <pre>
 * &lt;trust-agent xmlns:android="http://schemas.android.com/apk/res/android"
 *          android:settingsActivity=".TrustAgentSettings" /></pre>
 *
 * @hide
 */
// @SystemApi
class TrustAgentService
    : public Elastos::Droid::App::Service
    , public ITrustAgentService
{
private:
    class MHandler
        : public Handler
    {
        friend class TrustAgentService;
    public:
        TO_STRING_IMPL("TrustAgentService::MHandler")

        MHandler(
            /* [in] */ TrustAgentService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        TrustAgentService* mHost;
    };

    class GTRunnable
        : public Runnable
    {
    public:
        GTRunnable(
            /* [in] */ ICharSequence* message,
            /* [in] */ Int64 durationMs,
            /* [in] */ Boolean initiatedByUser,
            /* [in] */ TrustAgentService* host);

        //@Override
        CARAPI Run();

    private:
        AutoPtr<ICharSequence> mMessage;
        Int64 mDurationMs;
        Boolean mInitiatedByUser;
        TrustAgentService* mHost;
    };

public:
    class TrustAgentServiceWrapper
        : public Object
        , public ITrustAgentServiceWrapper
        , public IITrustAgentService
        , public IBinder
    {
        friend class TrustAgentService;
    public:
        CAR_INTERFACE_DECL()

        TrustAgentServiceWrapper();

        TrustAgentServiceWrapper(
            /* [in] */ TrustAgentService* host);

        CARAPI constructor();

        // @Override /* Binder API */
        CARAPI OnUnlockAttempt(
            /* [in] */ Boolean successful);

        // @Override /* Binder API */
        CARAPI OnTrustTimeout();

        // @Override /* Binder API */
        CARAPI SetCallback(
            /* [in] */ IITrustAgentServiceCallback* callback);

        // @Override /* Binder API */
        CARAPI SetTrustAgentFeaturesEnabled(
            /* [in] */ IBundle* features,
            /* [in] */ IBinder* token);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        TrustAgentService* mHost;
    };

    friend class TrustAgentServiceWrapper;
public:
    CAR_INTERFACE_DECL()

    TrustAgentService();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate();

    /**
     * Called after the user attempts to authenticate in keyguard with their device credentials,
     * such as pin, pattern or password.
     *
     * @param successful true if the user successfully completed the challenge.
     */
    CARAPI OnUnlockAttempt(
        /* [in] */ Boolean successful);


    /**
     * Called when the timeout provided by the agent expires.  Note that this may be called earlier
     * than requested by the agent if the trust timeout is adjusted by the system or
     * {@link DevicePolicyManager}.  The agent is expected to re-evaluate the trust state and only
     * call {@link #grantTrust(CharSequence, long, boolean)} if the trust state should be
     * continued.
     */
    CARAPI OnTrustTimeout();

    /**
     * Called when device policy wants to restrict features in the agent in response to
     * {@link DevicePolicyManager#setTrustAgentFeaturesEnabled(ComponentName, ComponentName, java.util.List) }.
     * Agents that support this feature should overload this method and return 'true'.
     *
     * The list of options can be obtained by calling
     * options.getStringArrayList({@link #KEY_FEATURES}). Presence of a feature string in the list
     * means it should be enabled ("white-listed"). Absence of the feature means it should be
     * disabled. An empty list means all features should be disabled.
     *
     * This function is only called if {@link DevicePolicyManager#KEYGUARD_DISABLE_TRUST_AGENTS} is
     * set.
     *
     * @param options Option feature bundle.
     * @return true if the {@link TrustAgentService} supports this feature.
     * @hide
     */
    CARAPI OnSetTrustAgentFeaturesEnabled(
        /* [in] */ IBundle* options,
        /* [out] */ Boolean* result);

    /**
     * Call to grant trust on the device.
     *
     * @param message describes why the device is trusted, e.g. "Trusted by location".
     * @param durationMs amount of time in milliseconds to keep the device in a trusted state.
     *    Trust for this agent will automatically be revoked when the timeout expires unless
     *    extended by a subsequent call to this function. The timeout is measured from the
     *    invocation of this function as dictated by {@link SystemClock#elapsedRealtime())}.
     *    For security reasons, the value should be no larger than necessary.
     *    The value may be adjusted by the system as necessary to comply with a policy controlled
     *    by the system or {@link DevicePolicyManager} restrictions. See {@link #onTrustTimeout()}
     *    for determining when trust expires.
     * @param initiatedByUser this is a hint to the system that trust is being granted as the
     *    direct result of user action - such as solving a security challenge. The hint is used
     *    by the system to optimize the experience. Behavior may vary by device and release, so
     *    one should only set this parameter if it meets the above criteria rather than relying on
     *    the behavior of any particular device or release.
     * @throws IllegalStateException if the agent is not currently managing trust.
     */
    CARAPI GrantTrust(
        /* [in] */ ICharSequence* message,
        /* [in] */ Int64 durationMs,
        /* [in] */ Boolean initiatedByUser);

    /**
     * Call to revoke trust on the device.
     */
    CARAPI RevokeTrust();

    /**
     * Call to notify the system if the agent is ready to manage trust.
     *
     * This property is not persistent across recreating the service and defaults to false.
     * Therefore this method is typically called when initializing the agent in {@link #onCreate}.
     *
     * @param managingTrust indicates if the agent would like to manage trust.
     */
    CARAPI SetManagingTrust(
        /* [in] */ Boolean managingTrust);

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

private:
    CARAPI OnError(
        /* [in] */ const String& msg);

private:
    const String TAG;
    const static Boolean DEBUG;
    const static Int32 MSG_UNLOCK_ATTEMPT = 1;
    const static Int32 MSG_SET_TRUST_AGENT_FEATURES_ENABLED = 2;
    const static Int32 MSG_TRUST_TIMEOUT = 3;

    AutoPtr<IITrustAgentServiceCallback> mCallback;

    AutoPtr<IRunnable> mPendingGrantTrustTask;

    Boolean mManagingTrust;

    // Lock used to access mPendingGrantTrustTask and mCallback.
    Object mLock;


    AutoPtr<IHandler> mHandler;
};

} // namespace Trust
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_TRUST_TRUSTAGENTSERVICE_H__
