
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_NULLDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_NULLDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class NullDispatcher
    : public Object
    , public INullDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Create a dispatcher that does nothing when dispatched to.
     */
    NullDispatcher();

    virtual ~NullDispatcher() {}

    CARAPI constructor();

    /**
     * Do nothing; all parameters are ignored.
     */
    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_NULLDISPATCHER_H__
