
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Dispatch::IDispatchable;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class BroadcastDispatcher
    : public Object
    , public IBroadcastDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    BroadcastDispatcher();

    virtual ~BroadcastDispatcher() {}

    /**
     * Create a broadcast dispatcher from the supplied dispatch targets.
     *
     * @param dispatchTargets one or more targets to dispatch to
     */
    //@SafeVarargs
    CARAPI constructor(
        /* [in] */ ArrayOf<IDispatchable*>* dispatchTargets);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    AutoPtr< ArrayOf<IDispatchable*> > mDispatchTargets;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_BROADCASTDISPATCHER_H__
