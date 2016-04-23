#ifndef __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__
#define __ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Telecomm.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Telecomm::Internal::IIConnectionServiceAdapter;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

/**
 * A component that provides an RPC servant implementation of {@link IConnectionServiceAdapter},
 * posting incoming messages on the main thread on a client-supplied delegate object.
 *
 * TODO: Generate this and similar classes using a compiler starting from AIDL interfaces.
 *
 * @hide
 */
class ConnectionServiceAdapterServant
    : public Object
    , public IConnectionServiceAdapterServant
{
public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("ConnectionServiceAdapterServant::MyHandler")

        MyHandler(
            /* [in] */ ConnectionServiceAdapterServant* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        // Internal method defined to centralize handling of RemoteException
        void InternalHandleMessage(
            /* [in] */ IMessage* msg);

    public:
        ConnectionServiceAdapterServant* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    ConnectionServiceAdapterServant();

    CARAPI constructor(
        /* [in] */ IIConnectionServiceAdapter* delegate);

    CARAPI GetStub(
        /* [out] */ IIConnectionServiceAdapter** result);

private:
    AutoPtr<IIConnectionServiceAdapter> mDelegate;

    AutoPtr<MyHandler> mHandler;

    AutoPtr<IIConnectionServiceAdapter> mStub;
};

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOMM_TELECOM_CONNECTIONSERVICEADAPTERSERVANT_H__