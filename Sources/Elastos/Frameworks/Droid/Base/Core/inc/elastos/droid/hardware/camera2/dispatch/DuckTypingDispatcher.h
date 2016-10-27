
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_DUCKTYPINGDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_DUCKTYPINGDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class DuckTypingDispatcher
    : public Object
    , public IDuckTypingDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    DuckTypingDispatcher();

    virtual ~DuckTypingDispatcher() {}

    /**
     * Create a new duck typing dispatcher.
     *
     * @param target destination dispatch type, methods will be redirected to this dispatcher
     * @param targetClass destination dispatch class, methods will be converted to this class's
     */
    CARAPI constructor(
        /* [in] */ IDispatchable* target,
        /* [in] */ IInterfaceInfo* targetClass);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    AutoPtr<IMethodNameInvoker> mDuck;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_DUCKTYPINGDISPATCHER_H__
