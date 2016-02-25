
#include "elastos/droid/hardware/display/CDisplayManagerCallback.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL(CDisplayManagerCallback, Object, IIDisplayManagerCallback)

CAR_OBJECT_IMPL(CDisplayManagerCallback)

ECode CDisplayManagerCallback::constructor(
    /* [in] */ Handle32 owner)
{
    mOwner = (DisplayManagerGlobal*)owner;
    return NOERROR;
}

//@Override
ECode CDisplayManagerCallback::OnDisplayEvent(
    /* [in] */ Int32 displayId,
    /* [in] */ Int32 event)
{
    mOwner->HandleDisplayEvent(displayId, event);

    return NOERROR;
}

ECode CDisplayManagerCallback::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos