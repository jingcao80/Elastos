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

#ifndef __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallCameraManager
    : public Object
{
public:
    /**
     * Initializes the InCall CameraManager.
     *
     * @param context The current context.
     */
    InCallCameraManager(
        /* [in] */ IContext* context);

    /**
     * Sets whether the front facing camera should be used or not.
     *
     * @param useFrontFacingCamera {@code True} if the front facing camera is to be used.
     */
    CARAPI_(void) SetUseFrontFacingCamera(
        /* [in] */ Boolean useFrontFacingCamera);

    /**
     * Determines whether the front facing camera is currently in use.
     *
     * @return {@code True} if the front facing camera is in use.
     */
    CARAPI_(Boolean) IsUsingFrontFacingCamera();

    /**
     * Determines the active camera ID.
     *
     * @return The active camera ID.
     */
    CARAPI_(String) GetActiveCameraId();

private:
    /**
     * Get the camera ID and aspect ratio for the front and rear cameras.
     *
     * @param context The context.
     */
    CARAPI_(void) InitializeCameraList(
        /* [in] */ IContext* context);

private:
    /**
     * The camera ID for the front facing camera.
     */
    String mFrontFacingCameraId;

    /**
     * The camera ID for the rear facing camera.
     */
    String mRearFacingCameraId;

    /**
     * The currently active camera.
     */
    Boolean mUseFrontFacingCamera;

    /**
     * Aspect ratio of the front facing camera.
     */
    Float mFrontFacingCameraAspectRatio;

    /**
     * Aspect ratio of the rear facing camera.
     */
    Float mRearFacingCameraAspectRatio;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__
