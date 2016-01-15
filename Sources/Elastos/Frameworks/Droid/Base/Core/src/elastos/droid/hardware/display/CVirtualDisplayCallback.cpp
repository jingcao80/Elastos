
#include "elastos/droid/hardware/display/CVirtualDisplayCallback.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CVirtualDisplayCallback, Object, IVirtualDisplayCallback)

CAR_OBJECT_IMPL(CVirtualDisplayCallback)

ECode CVirtualDisplayCallback::OnPaused()
{
    return NOERROR;
}

ECode CVirtualDisplayCallback::OnResumed()
{
    return NOERROR;
}

ECode CVirtualDisplayCallback::OnStopped()
{
    return NOERROR;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos