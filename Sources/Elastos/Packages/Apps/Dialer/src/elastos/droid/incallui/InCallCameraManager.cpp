
#include "elastos/droid/incallui/InCallCameraManager.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

InCallCameraManager::InCallCameraManager(
    /* [in] */ IContext* context)
    : mUseFrontFacingCamera(FALSE)
    , mFrontFacingCameraAspectRatio(0)
    , mRearFacingCameraAspectRatio(0)
{
    assert(0 && "TODO");
}

void InCallCameraManager::SetUseFrontFacingCamera(
    /* [in] */ Boolean useFrontFacingCamera)
{
    assert(0 && "TODO");
}

Boolean InCallCameraManager::IsUsingFrontFacingCamera()
{
    assert(0 && "TODO");
    return FALSE;
}

String InCallCameraManager::GetActiveCameraId()
{
    assert(0 && "TODO");
    return String(NULL);
}

void InCallCameraManager::InitializeCameraList(
    /* [in] */ IContext* context)
{
    assert(0 && "TODO");
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
