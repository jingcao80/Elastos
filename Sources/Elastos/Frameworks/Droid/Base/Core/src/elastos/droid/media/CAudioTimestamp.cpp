#include "elastos/droid/media/CAudioTimestamp.h"

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL(CAudioTimestamp, Object, IAudioTimestamp)

CAR_OBJECT_IMPL(CAudioTimestamp)

CAudioTimestamp::CAudioTimestamp()
    : mFramePosition(0)
    , mNanoTime(0)
{
}

CAudioTimestamp::~CAudioTimestamp()
{
}

ECode CAudioTimestamp::constructor()
{
    return NOERROR;
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
