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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYREQUESTMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYREQUESTMAPPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Legacy::IParameterUtilsZoomData;
using Elastos::Droid::Hardware::Camera2::Legacy::ILegacyRequest;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Utility::IRange;
using Elastos::Core::Object;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyRequestMapper
{
public:
    /**
     * Set the legacy parameters using the {@link LegacyRequest legacy request}.
     *
     * <p>The legacy request's parameters are changed as a side effect of calling this
     * method.</p>
     *
     * @param legacyRequest a non-{@code null} legacy request
     */
    static CARAPI ConvertRequestMetadata(
        /* [in] */ ILegacyRequest* legacyRequest);

private:
    friend class LegacyResultMapper;

    static CARAPI_(Boolean) CheckForCompleteGpsData(
        /* [in] */ ILocation* location);

    static CARAPI_(Int32) FilterSupportedCaptureIntent(
        /* [in] */ Int32 captureIntent);

    static CARAPI_(AutoPtr<IList>) ConvertMeteringRegionsToLegacy(
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ IArrayOf* meteringRegions,
        /* [in] */ Int32 maxNumMeteringAreas,
        /* [in] */ const String& regionName);

    static CARAPI_(void) MapAeAndFlashMode(
        /* [in] */ ICaptureRequest* r,
        /* [in] */ IParameters* p);

    /**
     * Returns null if the anti-banding mode enum is not supported.
     */
    static CARAPI_(String) ConvertAeAntiBandingModeToLegacy(
        /* [in] */ Int32 mode);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) ConvertAeFpsRangeToLegacy(
        /* [in] */ IRange* fpsRange);

    static CARAPI_(String) ConvertAwbModeToLegacy(
        /* [in] */ Int32 mode);

    /**
     * Return {@code null} if the value is not supported, otherwise return the retrieved key's
     * value from the request (or the default value if it wasn't set).
     *
     * <p>If the fetched value in the request is equivalent to {@code allowedValue},
     * then omit the warning (e.g. turning off AF lock on a camera
     * that always has the AF lock turned off is a silent no-op), but still return {@code null}.</p>
     *
     * <p>Logs a warning to logcat if the key is not supported by api1 camera device.</p.
     */
    static CARAPI_(AutoPtr<IInterface>) GetIfSupported(
        /* [in] */ ICaptureRequest* r,
        /* [in] */ ICaptureRequestKey* key,
        /* [in] */ IInterface* defaultValue,
        /* [in] */ Boolean isSupported,
        /* [in] */ IInterface* allowedValue);

private:
    static const String TAG;
    static const Boolean VERBOSE;

    /** Default quality for android.jpeg.quality, android.jpeg.thumbnailQuality */
    static const Byte DEFAULT_JPEG_QUALITY;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYREQUESTMAPPER_H__
