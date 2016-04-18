
#ifndef __Elastos_DevSamples_ServiceDemo_CSERVICECONNECTION_H__
#define __Elastos_DevSamples_ServiceDemo_CSERVICECONNECTION_H__

#include "_Elastos_DevSamples_ServiceDemo_CServiceConnection.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CServiceConnection)
    , public Object
    , public IServiceConnection
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IWeakReference* weakHost);

    CARAPI OnServiceConnected(
        /* [in] */ IComponentName* name,
        /* [in] */ IBinder* service);

    CARAPI OnServiceDisconnected(
        /* [in] */ IComponentName* name);

private:
    AutoPtr<IWeakReference> mWeakHost;
};

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ServiceDemo_CSERVICECONNECTION_H__
