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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPARAMETERUTILS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPARAMETERUTILS_H__

#include "_Elastos_Droid_Hardware_Camera2_Legacy_CParameterUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

using Elastos::Droid::Hardware::Camera2::Params::IFace;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::ISize;
using Elastos::Utility::IList;

CarClass(CParameterUtils)
    , public Singleton
    , public IParameterUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNORMALIZED_RECTANGLE_DEFAULT(
        /* [out] */ IRect** rect);

    CARAPI GetCAMERA_AREA_DEFAULT(
        /* [out] */ ICameraArea** area);

    CARAPI GetRECTANGLE_EMPTY(
        /* [out] */ IRect** rect);

    /**
     * Convert a camera API1 size into a util size
     */
    CARAPI ConvertSize(
        /* [in] */ ICameraSize* size,
        /* [out] */ ISize** outsize);

    /**
     * Convert a camera API1 list of sizes into a util list of sizes
     */
    CARAPI ConvertSizeList(
        /* [in] */ IList* sizeList,
        /* [out] */ IList** outlist);

    /**
     * Convert a camera API1 list of sizes into an array of sizes
     */
    CARAPI ConvertSizeListToArray(
        /* [in] */ IList* sizeList,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Check if the camera API1 list of sizes contains a size with the given dimens.
     */
    CARAPI ContainsSize(
        /* [in] */ IList* sizeList,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* value);

    /**
     * Returns the largest supported picture size, as compared by its area.
     */
    CARAPI GetLargestSupportedJpegSizeByArea(
        /* [in] */ IParameters* params,
        /* [out] */ ISize** outsize);

    /**
     * Convert a camera area into a human-readable string.
     */
    CARAPI StringFromArea(
        /* [in] */ ICameraArea* area,
        /* [out] */ String* str);

    /**
     * Convert a camera area list into a human-readable string
     * @param areaList a list of areas (null is ok)
     */
    CARAPI StringFromAreaList(
        /* [in] */ IList* areaList,
        /* [out] */ String* str);

    /**
     * Calculate the closest zoom index for the user-requested crop region by rounding
     * up to the closest (largest or equal) possible zoom crop.
     *
     * <p>If the requested crop region exceeds the size of the active array, it is
     * shrunk to fit inside of the active array first.</p>
     *
     * <p>Since all api1 camera devices only support a discrete set of zooms, we have
     * to translate the per-pixel-granularity requested crop region into a per-zoom-index
     * granularity.</p>
     *
     * <p>Furthermore, since the zoom index and zoom levels also depends on the field-of-view
     * of the preview, the current preview {@code streamSize} is also used.</p>
     *
     * <p>The calculated crop regions are then written to in-place to {@code reportedCropRegion}
     * and {@code previewCropRegion}, in coordinates relative to the active array.</p>
     *
     * @param params non-{@code null} camera api1 parameters
     * @param activeArray active array dimensions, in sensor space
     * @param streamSize stream size dimensions, in pixels
     * @param cropRegion user-specified crop region, in active array coordinates
     * @param reportedCropRegion (out parameter) what the result for {@code cropRegion} looks like
     * @param previewCropRegion (out parameter) what the visual preview crop is
     * @return
     *          the zoom index inclusively between 0 and {@code Parameters#getMaxZoom},
     *          where 0 means the camera is not zoomed
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    CARAPI GetClosestAvailableZoomCrop(
        /* [in] */ IParameters* params,
        /* [in] */ IRect* activeArray,
        /* [in] */ ISize* streamSize,
        /* [in] */ IRect* cropRegion,
        /* [in] */ IRect* reportedCropRegion,
        /* [in] */ IRect* previewCropRegion,
        /* [out] */ Int32* value);

    /**
     * Get the available 'crop' (zoom) rectangles for this camera that will be reported
     * via a {@code CaptureResult} when a zoom is requested.
     *
     * <p>These crops ignores the underlying preview buffer size, and will always be reported
     * the same values regardless of what configuration of outputs is used.</p>
     *
     * <p>When zoom is supported, this will return a list of {@code 1 + #getMaxZoom} size,
     * where each crop rectangle corresponds to a zoom ratio (and is centered at the middle).</p>
     *
     * <p>Each crop rectangle is changed to have the same aspect ratio as {@code streamSize},
     * by shrinking the rectangle if necessary.</p>
     *
     * <p>To get the reported crop region when applying a zoom to the sensor, use {@code streamSize}
     * = {@code activeArray size}.</p>
     *
     * @param params non-{@code null} camera api1 parameters
     * @param activeArray active array dimensions, in sensor space
     * @param streamSize stream size dimensions, in pixels
     *
     * @return a list of available zoom rectangles, sorted from least zoomed to most zoomed
     */
    CARAPI GetAvailableZoomCropRectangles(
        /* [in] */ IParameters* params,
        /* [in] */ IRect* activeArray,
        /* [out] */ IList** outlist);

    /**
     * Get the available 'crop' (zoom) rectangles for this camera.
     *
     * <p>This is the effective (real) crop that is applied by the camera api1 device
     * when projecting the zoom onto the intermediate preview buffer. Use this when
     * deciding which zoom ratio to apply.</p>
     *
     * <p>When zoom is supported, this will return a list of {@code 1 + #getMaxZoom} size,
     * where each crop rectangle corresponds to a zoom ratio (and is centered at the middle).</p>
     *
     * <p>Each crop rectangle is changed to have the same aspect ratio as {@code streamSize},
     * by shrinking the rectangle if necessary.</p>
     *
     * <p>To get the reported crop region when applying a zoom to the sensor, use {@code streamSize}
     * = {@code activeArray size}.</p>
     *
     * @param params non-{@code null} camera api1 parameters
     * @param activeArray active array dimensions, in sensor space
     * @param streamSize stream size dimensions, in pixels
     *
     * @return a list of available zoom rectangles, sorted from least zoomed to most zoomed
     */
    CARAPI GetAvailablePreviewZoomCropRectangles(
        /* [in] */ IParameters* params,
        /* [in] */ IRect* activeArray,
        /* [in] */ ISize* previewSize,
        /* [out] */ IList** outlist);

    /**
     * Get the largest possible zoom ratio (normalized to {@code 1.0f} and higher)
     * that the camera can support.
     *
     * <p>If the camera does not support zoom, it always returns {@code 1.0f}.</p>
     *
     * @param params non-{@code null} camera api1 parameters
     * @return normalized max zoom ratio, at least {@code 1.0f}
     */
    CARAPI GetMaxZoomRatio(
        /* [in] */ IParameters* params,
        /* [out] */ Float* value);

    /**
     * Convert the user-specified crop region into zoom data; which can be used
     * to set the parameters to a specific zoom index, or to report back to the user what the
     * actual zoom was, or for other calculations requiring the current preview crop region.
     *
     * <p>None of the parameters are mutated.</p>
     *
     * @param activeArraySize active array size of the sensor (e.g. max jpeg size)
     * @param cropRegion the user-specified crop region
     * @param previewSize the current preview size (in pixels)
     * @param params the current camera parameters (not mutated)
     *
     * @return the zoom index, and the effective/reported crop regions (relative to active array)
     */
    CARAPI ConvertScalerCropRegion(
        /* [in] */ IRect* activeArraySize,
        /* [in] */ IRect* cropRegion,
        /* [in] */ ISize* previewSize,
        /* [in] */ IParameters* params,
        /* [out] */ IParameterUtilsZoomData** outzd);

    /**
     * Calculate the actual/effective/reported normalized rectangle data from a metering
     * rectangle.
     *
     * <p>If any of the rectangles are out-of-range of their intended bounding box,
     * the {@link #RECTANGLE_EMPTY empty rectangle} is substituted instead
     * (with a weight of {@code 0}).</p>
     *
     * <p>The metering rectangle is bound by the crop region (effective/reported respectively).
     * The metering {@link Camera.Area area} is bound by {@code [-1000, 1000]}.</p>
     *
     * <p>No parameters are mutated; returns the new metering data.</p>
     *
     * @param activeArraySize active array size of the sensor (e.g. max jpeg size)
     * @param meteringRect the user-specified metering rectangle
     * @param zoomData the calculated zoom data corresponding to this request
     *
     * @return the metering area, the reported/effective metering rectangles
     */
    CARAPI ConvertMeteringRectangleToLegacy(
        /* [in] */ IRect* activeArray,
        /* [in] */ IMeteringRectangle* meteringRect,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [out] */ IParameterUtilsMeteringData** outmd);

    /**
     * Convert the normalized camera area from [-1000, 1000] coordinate space
     * into the active array-based coordinate space.
     *
     * <p>Values out of range are clipped to be within the resulting (reported) crop
     * region. It is possible to have values larger than the preview crop.</p>
     *
     * <p>Weights out of range of [0, 1000] are clipped to be within the range.</p>
     *
     * @param activeArraySize active array size of the sensor (e.g. max jpeg size)
     * @param zoomData the calculated zoom data corresponding to this request
     * @param area the normalized camera area
     *
     * @return the weighed rectangle in active array coordinate space, with the weight
     */
    CARAPI ConvertCameraAreaToActiveArrayRectangle(
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ ICameraArea* area,
        /* [out] */ IParameterUtilsWeightedRectangle** outwr);

    /**
     * Convert an api1 face into an active-array based api2 face.
     *
     * <p>Out-of-ranges scores and ids will be clipped to be within range (with a warning).</p>
     *
     * @param face a non-{@code null} api1 face
     * @param activeArraySize active array size of the sensor (e.g. max jpeg size)
     * @param zoomData the calculated zoom data corresponding to this request
     *
     * @return a non-{@code null} api2 face
     *
     * @throws NullPointerException if the {@code face} was {@code null}
     */
    CARAPI ConvertFaceFromLegacy(
        /* [in] */ ICameraFace* face,
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [out] */ IFace** outface);
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_CPARAMETERUTILS_H__
