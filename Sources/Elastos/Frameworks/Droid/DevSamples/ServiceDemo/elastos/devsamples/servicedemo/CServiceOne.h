
#ifndef __Elastos_DevSamples_ServiceDemo_CSERVICEONE_H__
#define __Elastos_DevSamples_ServiceDemo_CSERVICEONE_H__

#include "_Elastos_DevSamples_ServiceDemo_CServiceOne.h"
#include <elastos/droid/app/Service.h>

using Elastos::Droid::App::Service;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CServiceOne)
    , public Service
    , public IGetSystemTimeService
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetSystemTime(
        /* [out] */ String* timeStr);

protected:
    CARAPI OnCreate();

    CARAPI OnStart(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 startId);

    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** binder);

    CARAPI OnUnbind(
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* succeeded);

private:
};

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ServiceDemo_CSERVICEONE_H__
