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

#ifndef __ELASTOS_DROID_MEDIA_CCAMCORDERPROFILEHELPER_H__
#define __ELASTOS_DROID_MEDIA_CCAMCORDERPROFILEHELPER_H__

#include "_Elastos_Droid_Media_CCamcorderProfileHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CCamcorderProfileHelper)
    , public Singleton
    , public ICamcorderProfileHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the camcorder profile for the first back-facing camera on the
     * device at the given quality level. If the device has no back-facing
     * camera, this returns null.
     * @param quality the target quality level for the camcorder profile
     * @see #get(Int32, Int32)
     */
    CARAPI Get(
        /* [in] */ Int32 quality,
        /* [out] */ ICamcorderProfile** result);

    /**
     * Returns the camcorder profile for the given camera at the given
     * quality level.
     *
     * Quality levels QUALITY_LOW, QUALITY_HIGH are guaranteed to be supported, while
     * other levels may or may not be supported. The supported levels can be checked using
     * {@link #hasProfile(Int32, Int32)}.
     * QUALITY_LOW refers to the lowest quality available, while QUALITY_HIGH refers to
     * the highest quality available.
     * QUALITY_LOW/QUALITY_HIGH have to match one of qcif, cif, 480p, 720p, or 1080p.
     * E.g. if the device supports 480p, 720p, and 1080p, then low is 480p and high is
     * 1080p.
     *
     * The same is true for time lapse quality levels, i.e. QUALITY_TIME_LAPSE_LOW,
     * QUALITY_TIME_LAPSE_HIGH are guaranteed to be supported and have to match one of
     * qcif, cif, 480p, 720p, or 1080p.
     *
     * A camcorder recording session with higher quality level usually has higher output
     * bit rate, better video and/or audio recording quality, larger video frame
     * resolution and higher audio sampling rate, etc, than those with lower quality
     * level.
     *
     * @param cameraId the id for the camera
     * @param quality the target quality level for the camcorder profile.
     * @see #QUALITY_LOW
     * @see #QUALITY_HIGH
     * @see #QUALITY_QCIF
     * @see #QUALITY_CIF
     * @see #QUALITY_480P
     * @see #QUALITY_720P
     * @see #QUALITY_1080P
     * @see #QUALITY_TIME_LAPSE_LOW
     * @see #QUALITY_TIME_LAPSE_HIGH
     * @see #QUALITY_TIME_LAPSE_QCIF
     * @see #QUALITY_TIME_LAPSE_CIF
     * @see #QUALITY_TIME_LAPSE_480P
     * @see #QUALITY_TIME_LAPSE_720P
     * @see #QUALITY_TIME_LAPSE_1080P
     */
    CARAPI Get(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ ICamcorderProfile** result);

    /**
     * Returns true if camcorder profile exists for the first back-facing
     * camera at the given quality level.
     * @param quality the target quality level for the camcorder profile
     */
    CARAPI HasProfile(
        /* [in] */ Int32 quality,
        /* [out] */ Boolean* result);

    /**
     * Returns true if camcorder profile exists for the given camera at
     * the given quality level.
     * @param cameraId the id for the camera
     * @param quality the target quality level for the camcorder profile
     */
    CARAPI HasProfile(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 quality,
        /* [out] */ Boolean* result);

};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_CCAMCORDERPROFILEHELPER_H__
