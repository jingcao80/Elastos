
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_INVOKEDISPATCHER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_INVOKEDISPATCHER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Dispatch {

class InvokeDispatcher
    : public Object
    , public IInvokeDispatcher
    , public IDispatchable
{
public:
    CAR_INTERFACE_DECL()

    InvokeDispatcher();

    virtual ~InvokeDispatcher() {}

    CARAPI constructor(
        /* [in] */ IInterface* target);

    //@Override
    CARAPI Dispatch(
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

private:
    static const String TAG;
    AutoPtr<IInterface> mTarget;
};

} // namespace Dispatch
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_DISPATCH_INVOKEDISPATCHER_H__
