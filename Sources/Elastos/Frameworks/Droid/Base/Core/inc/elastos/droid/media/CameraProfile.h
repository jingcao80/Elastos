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

#ifndef __ELASTOS_DROID_MEDIA_CAMERAPROFILE_H__
#define __ELASTOS_DROID_MEDIA_CAMERAPROFILE_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * The CameraProfile class is used to retrieve the pre-defined still image
 * capture (jpeg) quality levels (0-100) used for low, medium, and high
 * quality settings in the Camera application.
 *
 */
class CameraProfile
{
public:
    /**
     * Returns a pre-defined still image capture (jpeg) quality level
     * used for the given quality level in the Camera application for
     * the first back-facing camera on the device. If the device has no
     * back-facing camera, this returns 0.
     *
     * @param quality The target quality level
     */
    static CARAPI GetJpegEncodingQualityParameter(
        /* [in] */ Int32 quality,
        /* [out] */ Int32* result);

    /**
     * Returns a pre-defined still image capture (jpeg) quality level
     * used for the given quality level in the Camera application for
     * the specified camera.
     *
     * @param cameraId The id of the camera
     * @param quality The target quality level
     */
    static CARAPI GetJpegEncodingQualityParameter(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ Int32* result);

private:
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetImageEncodingQualityLevels(
        /* [in] */ Int32 cameraId);

    // Methods implemented by JNI
    static CARAPI_(void) NativeInit();

    static CARAPI_(Int32) NativeGetNumImageEncodingQualityLevels(
        /* [in] */ Int32 cameraId);

    static CARAPI_(Int32) NativeGetImageEncodingQualityLevel(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 index);

private:
    /*
     * Cache the Jpeg encoding quality parameters
     */
    static HashMap<Int32, AutoPtr<ArrayOf<Int32> > > sCache;
    static Object mCacheLock;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif /// __ELASTOS_DROID_MEDIA_CAMERAPROFILE_H__
