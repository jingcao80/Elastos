#ifndef __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITIONPROVIDERSERVICE_H__
#define __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITIONPROVIDERSERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Service.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

/**
 * A service that provides conditions about boolean state.
 * <p>To extend this class, you must declare the service in your manifest file with
 * the {@link android.Manifest.permission#BIND_CONDITION_PROVIDER_SERVICE} permission
 * and include an intent filter with the {@link #SERVICE_INTERFACE} action. For example:</p>
 * <pre>
 * &lt;service android:name=".MyConditionProvider"
 *          android:label="&#64;string/service_name"
 *          android:permission="android.permission.BIND_CONDITION_PROVIDER_SERVICE">
 *     &lt;intent-filter>
 *         &lt;action android:name="android.service.notification.ConditionProviderService" />
 *     &lt;/intent-filter>
 * &lt;/service></pre>
 *
 * @hide
 */
// @SystemApi
class ConditionProviderService
    : public Elastos::Droid::App::Service
    , public IConditionProviderService
{
public:
    class Provider
        : public Object
        , public IIConditionProvider
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        Provider();

        Provider(
            /* [in] */ ConditionProviderService* host);

        CARAPI constructor();

        // @Override
        CARAPI OnConnected();

        // @Override
        CARAPI OnRequestConditions(
            /* [in] */ Int32 relevance);

        // @Override
        CARAPI OnSubscribe(
            /* [in] */ IUri* conditionId);

        // @Override
        CARAPI OnUnsubscribe(
            /* [in] */ IUri* conditionId);

        ///pay for someone's error,he write a ToString in Binder.car
        CARAPI ToString(
            /* [out] */ String* str)
        {
            return NOERROR;
        }
        ///pay for someone's error

    private:
        ConditionProviderService* mHost;
    };

private:
    class H
        : public Handler
    {
        friend class Provider;
    public:
        H(
            /* [in] */ Provider* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        const static Int32 ON_CONNECTED = 1;
        const static Int32 ON_REQUEST_CONDITIONS = 2;
        const static Int32 ON_SUBSCRIBE = 3;
        const static Int32 ON_UNSUBSCRIBE = 4;

        Provider* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ConditionProviderService();

    virtual CARAPI OnConnected() = 0;

    virtual CARAPI OnRequestConditions(
        /* [in] */ Int32 relevance) = 0;

    virtual CARAPI OnSubscribe(
        /* [in] */ IUri* conditionId) = 0;

    virtual CARAPI OnUnsubscribe(
        /* [in] */ IUri* conditionId) = 0;

public:
    CARAPI NotifyCondition(
        /* [in] */ ICondition* condition);

    CARAPI NotifyConditions(
        /* [in] */ ArrayOf<ICondition*>* conditions);

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

private:
    CARAPI GetNotificationInterface(
        /* [out] */ IINotificationManager** ni);

    CARAPI_(Boolean) IsBound();

private:
    const String TAG;// = ConditionProviderService.class.getSimpleName()
           // + "[" + getClass().getSimpleName() + "]";

    AutoPtr<H> mHandler;

    AutoPtr<Provider> mProvider;
    AutoPtr<IINotificationManager> mNoMan;
};

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_NOTIFICATION_CONDITIONPROVIDERSERVICE_H__
