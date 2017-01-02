//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
