#ifndef __ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__
#define __ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__

#include "_Elastos_Droid_Telecom_CRemoteServiceCallback.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Internal::Telecom::IRemoteServiceCallback;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Telecom {

CarClass(CRemoteServiceCallback)
    , public Object
    , public IRemoteServiceCallback
{
public:
    class ResultRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ResultRunnable(
            /* [in] */ IList* componentNames,
            /* [in] */ IList* services,
            /* [in] */ CRemoteServiceCallback* host);

        CARAPI Run();

    public:
        AutoPtr<IList> mComponentNames;
        AutoPtr<IList> mServices;
        CRemoteServiceCallback* mHost;
    };

    class ErrorRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        ErrorRunnable(
            /* [in] */ CRemoteServiceCallback* host);

        CARAPI Run();

    public:
        CRemoteServiceCallback* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI SetHost(
        /* [in] */ IConnectionService* host);

    CARAPI OnResult(
        /* [in] */ IList* componentNames,
        /* [in] */ IList* services);

    CARAPI OnError();

public:
    AutoPtr<IConnectionService> mHost;
};

} // namespace Telecom
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TELECOM_CREMOTESERVICECALLBACK_H__