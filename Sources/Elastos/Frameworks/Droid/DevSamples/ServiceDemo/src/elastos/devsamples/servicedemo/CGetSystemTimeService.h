
#ifndef __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__
#define __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__

#include "_Elastos_DevSamples_ServiceDemo_CGetSystemTimeService.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace DevSamples {
namespace ServiceDemo {

CarClass(CGetSystemTimeService)
    , public Object
    , public IGetSystemTimeService
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetSystemTime(
        /* [out] */ String* timeStr);

    TO_STRING_IMPL("CGetSystemTimeService")
};

} // namespace ServiceDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_ServiceDemo_CGetSystemTimeService_H__
