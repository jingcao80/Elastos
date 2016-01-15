#include "Elastos.Droid.Media.h"
#include "elastos/droid/media/projection/MediaProjectionCallback.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Projection {

CAR_INTERFACE_IMPL(MediaProjectionCallback, Object, IMediaProjectionCallback)

ECode MediaProjectionCallback::OnStop()
{
    return NOERROR;
}

} // namespace Projection
} // namespace Media
} // namepsace Droid
} // namespace Elastos
