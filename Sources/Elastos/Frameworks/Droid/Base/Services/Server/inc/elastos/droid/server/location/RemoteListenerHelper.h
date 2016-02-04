
#ifndef __ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * A helper class, that handles operations in remote listeners, and tracks for remote process death.
 */
class RemoteListenerHelper
    : public Object
    , public IRemoteListenerHelper
{
private:
    class LinkedListener
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        LinkedListener(
            /* [in] */ IInterface* listener,
            /* [in] */ RemoteListenerHelper* host);

        // @NonNull
        CARAPI GetUnderlyingListener(
            /* [out] */ IInterface** listener);

        // @Override
        CARAPI ProxyDied();

    private:
        AutoPtr<IInterface> mListener;
        RemoteListenerHelper* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    RemoteListenerHelper(
        /* [in] */ const String& name);

    CARAPI AddListener(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    CARAPI RemoveListener(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    virtual CARAPI IsSupported(
        /* [out] */ Boolean* isSupported) = 0;

    virtual CARAPI RegisterWithService(
        /* [out] */ Boolean* isRegistered) = 0;

    virtual CARAPI UnregisterFromService() = 0;

    CARAPI Foreach(
        /* [in] */ IListenerOperation* operation);

private:
    String mTag;
    AutoPtr<IHashMap> mListenerMap;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_REMOTELISTENERHELPER_H__