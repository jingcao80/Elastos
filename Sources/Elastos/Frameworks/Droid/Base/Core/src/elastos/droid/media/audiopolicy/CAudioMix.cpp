#include "elastos/droid/media/audiopolicy/CAudioMix.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace AudioPolicy {

CAR_INTERFACE_IMPL(CAudioMix, Object, IAudioMix)

CAR_OBJECT_IMPL(CAudioMix)

CAudioMix::CAudioMix()
    : mRouteFlags(0)
{}

CAudioMix::~CAudioMix()
{
}

ECode CAudioMix::constructor(
    /* [in] */ IAudioMixingRule * rule,
    /* [in] */ IAudioFormat * format,
    /* [in] */ Int32 routeFlags)
{
    mRule = rule;
    mFormat = format;
    mRouteFlags = routeFlags;
    return NOERROR;
}

Int32 CAudioMix::GetRouteFlags()
{
    return mRouteFlags;
}

AutoPtr<IAudioFormat> CAudioMix::GetFormat()
{
    return mFormat;
}

AutoPtr<IAudioMixingRule> CAudioMix::GetRule()
{
    return mRule;
}


} // namespace AudioPolicy
} // namespace Media
} // namepsace Droid
} // namespace Elastos
