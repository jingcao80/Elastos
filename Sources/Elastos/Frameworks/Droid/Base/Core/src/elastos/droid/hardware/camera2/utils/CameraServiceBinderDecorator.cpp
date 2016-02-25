
#include "elastos/droid/hardware/camera2/utils/CameraServiceBinderDecorator.h"
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL_2(CameraServiceBinderDecorator::CameraServiceBinderDecoratorListener, Object,
        ICameraBinderDecoratorListener, IDecoratorListener)

const String CameraServiceBinderDecorator::TAG("CameraServiceBinderDecorator");

ECode CameraServiceBinderDecorator::CameraServiceBinderDecoratorListener::OnCatchException(
    /* [in] */ IMethodInfo* m,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [in] */ ECode t,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (t == (ECode)E_DEAD_OBJECT_EXCEPTION) {
        // Can sometimes happen (camera service died)
        // Pass on silently
    }
    else if (t == (ECode)E_REMOTE_EXCEPTION) {
        // Some other kind of remote exception - this is not normal, so let's at least
        // note it before moving on
        //Log.e(TAG, "Unexpected RemoteException from camera service call.", t);
        Slogger::E("CameraServiceBinderDecorator", "Unexpected RemoteException from camera service call. %d", t);
    }

    // All other exceptions also get sent onward
    *result = FALSE;
    return NOERROR;
}

ECode CameraServiceBinderDecorator::NewInstance(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    assert(0 && "TODO");
    // Decorator.<T> newInstance(obj, new CameraServiceBinderDecoratorListener());
    // REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
