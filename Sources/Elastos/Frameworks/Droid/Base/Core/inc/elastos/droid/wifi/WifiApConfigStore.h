
#ifndef __ELASTOS_DROID_NET_WIFI_WIFIAPCONFIGSTORE_H__
#define  __ELASTOS_DROID_NET_WIFI_WIFIAPCONFIGSTORE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/AsyncChannel.h"
#include "elastos/droid/utility/State.h"
#include "elastos/droid/utility/StateMachine.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;

namespace Elastos {
namespace Droid {
namespace Wifi {

/**
 * Provides API to the WifiStateMachine for doing read/write access
 * to soft access point configuration
 */
class WifiApConfigStore : public StateMachine
{
public:
    WifiApConfigStore(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    static CARAPI_(AutoPtr<WifiApConfigStore>) MakeWifiApConfigStore(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    CARAPI_(void) LoadApConfiguration();

    CARAPI GetMessenger(
        /* [out] */ IMessenger** msg);

private:
    class DefaultState : public State
    {
    public:
        DefaultState(
            /* [in] */ WifiApConfigStore* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "DefaultState";
            return NOERROR;
        }

    private:
        WifiApConfigStore* mOwner;
    };

    class InactiveState : public State
    {
    public:
        InactiveState(
            /* [in] */ WifiApConfigStore* owner) : mOwner(owner)
        {}

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "InactiveState";
            return NOERROR;
        }

    private:
        WifiApConfigStore* mOwner;
    };

    class ActiveStateRunnable : public Runnable
    {
    public:
        ActiveStateRunnable(
            /* [in] */ WifiApConfigStore* owner) : mOwner(owner)
        {}

        CARAPI Run();

    private:
        WifiApConfigStore* mOwner;
    };

    class ActiveState : public State
    {
    public:
        ActiveState(
            /* [in] */ WifiApConfigStore* owner) : mOwner(owner)
        {}

        CARAPI Enter();

        // @Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI GetName(
            /* [out] */ String* name)
        {
            *name = "ActiveState";
            return NOERROR;
        }

    private:
        WifiApConfigStore* mOwner;
    };

private:
    CARAPI_(void) WriteApConfiguration(
        /* [in] */ IWifiConfiguration* config);

    /* Generate a default WPA2 based configuration with a random password.
       We are changing the Wifi Ap configuration storage from secure settings to a
       flat file accessible only by the system. A WPA2 based default configuration
       will keep the device secure after the update */
    CARAPI_(void) SetDefaultApConfiguration();

private:
    AutoPtr<IContext> mContext;
    static String TAG;
    static String AP_CONFIG_FILE;
    static const Int32 AP_CONFIG_FILE_VERSION;

    AutoPtr<State> mDefaultState;
    AutoPtr<State> mInactiveState;
    AutoPtr<State> mActiveState;

    AutoPtr<IWifiConfiguration> mWifiApConfig;
    AutoPtr<AsyncChannel> mReplyChannel;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_WIFIAPCONFIGSTORE_H__
