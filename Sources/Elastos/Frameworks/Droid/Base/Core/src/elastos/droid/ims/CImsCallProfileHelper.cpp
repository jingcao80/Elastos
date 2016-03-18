#include "elastos/droid/ims/CImsCallProfile.h"
#include "elastos/droid/ims/CImsCallProfileHelper.h"

namespace Elastos {
namespace Droid {
namespace Ims {

CAR_INTERFACE_IMPL(CImsCallProfileHelper, Singleton, IImsCallProfileHelper)

CAR_SINGLETON_IMPL(CImsCallProfileHelper)

ECode CImsCallProfileHelper::GetVideoStateFromCallType(
    /* [in] */ Int32 callType,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::GetVideoStateFromCallType(callType, result);
}

ECode CImsCallProfileHelper::GetCallTypeFromVideoState(
    /* [in] */ Int32 videoState,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::GetCallTypeFromVideoState(videoState, result);
}

ECode CImsCallProfileHelper::PresentationToOIR(
    /* [in] */ Int32 presentation,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::PresentationToOIR(presentation, result);
}

ECode CImsCallProfileHelper::OIRToPresentation(
    /* [in] */ Int32 oir,
    /* [out] */ Int32* result)
{
    return CImsCallProfile::OIRToPresentation(oir, result);
}

} //namespace Ims
} //namespace Droid
} //namespace Elastos
