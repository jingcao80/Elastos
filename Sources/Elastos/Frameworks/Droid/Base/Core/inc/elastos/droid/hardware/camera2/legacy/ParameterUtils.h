
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_PARAMETERUTILS_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_PARAMETERUTILS_H__

#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::Camera2::Params::IFace;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Legacy::IParameterUtilsZoomData;
using Elastos::Droid::Hardware::Camera2::Legacy::IParameterUtilsMeteringData;
using Elastos::Droid::Hardware::Camera2::Legacy::IParameterUtilsWeightedRectangle;
using Elastos::Droid::Hardware::ICameraArea;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Utility::IList;
using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::ISizeF;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class ParameterUtils
{
public:
    /**
     * Calculate effective/reported zoom data from a user-specified crop region.
     */
    class ZoomData
        : public Object
        , public IParameterUtilsZoomData
    {
    public:
        CAR_INTERFACE_DECL()

        ZoomData();

        virtual ~ZoomData() {}

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ Int32 zoomIndex,
            /* [in] */ IRect* previewCrop,
            /* [in] */ IRect* reportedCrop);

        CARAPI GetZoomIndex(
            /* [out] */ Int32* index);

        CARAPI GetPreviewCrop(
            /* [out] */ IRect** rect);

        CARAPI GetReportedCrop(
            /* [out] */ IRect** rect);

    public:
        /** Zoom index used by {@link Camera.Parameters#setZoom} */
        Int32 mZoomIndex;
        /** Effective crop-region given the zoom index, coordinates relative to active-array */
        AutoPtr<IRect> mPreviewCrop;
        /** Reported crop-region given the zoom index, coordinates relative to active-array */
        AutoPtr<IRect> mReportedCrop;
    };

    /**
     * Calculate effective/reported metering data from a user-specified metering region.
     */
    class MeteringData
        : public Object
        , public IParameterUtilsMeteringData
    {
    public:
        CAR_INTERFACE_DECL()

        MeteringData();

        virtual ~MeteringData() {}

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ ICameraArea* meteringArea,
            /* [in] */ IRect* previewMetering,
            /* [in] */ IRect* reportedMetering);

        CARAPI GetMeteringArea(
            /* [out] */ ICameraArea** area);

        CARAPI GetPreviewMetering(
            /* [out] */ IRect** rect);

        CARAPI GetReportedMetering(
            /* [out] */ IRect** rect);

    public:
        /**
         * The metering area scaled to the range of [-1000, 1000].
         * <p>Values outside of this range are clipped to be within the range.</p>
         */
        AutoPtr<ICameraArea> mMeteringArea;

        /**
         * Effective preview metering region, coordinates relative to active-array.
         *
         * <p>Clipped to fit inside of the (effective) preview crop region.</p>
         */
        AutoPtr<IRect> mPreviewMetering;

        /**
         * Reported metering region, coordinates relative to active-array.
         *
         * <p>Clipped to fit inside of the (reported) resulting crop region.</p>
         */
        AutoPtr<IRect> mReportedMetering;
    };

    /**
     * A weighted rectangle is an arbitrary rectangle (the coordinate system is unknown) with an
     * arbitrary weight.
     *
     * <p>The user of this class must know what the coordinate system ahead of time; it's
     * then possible to convert to a more concrete type such as a metering rectangle or a face.
     * </p>
     *
     * <p>When converting to a more concrete type, out-of-range values are clipped; this prevents
     * possible illegal argument exceptions being thrown at runtime.</p>
     */
    class WeightedRectangle
        : public Object
        , public IParameterUtilsWeightedRectangle
    {
    public:
        CAR_INTERFACE_DECL()

        WeightedRectangle();

        virtual ~WeightedRectangle() {}

        CARAPI constructor();

        /**
         * Create a new weighted-rectangle from a non-{@code null} rectangle; the {@code weight}
         * can be unbounded.
         */
        CARAPI constructor(
            /* [in] */ IRect* rect,
            /* [in] */ Int32 weight);

        /**
         * Convert to a metering rectangle, clipping any of the values to stay within range.
         *
         * <p>If values are clipped, a warning is printed to logcat.</p>
         *
         * @return a new metering rectangle
         */
        CARAPI ToMetering(
            /* [out] */ IMeteringRectangle** outmr);

        /**
         * Convert to a face; the rect is considered to be the bounds, and the weight
         * is considered to be the score.
         *
         * <p>If the score is out of range of {@value Face#SCORE_MIN}, {@value Face#SCORE_MAX},
         * the score is clipped first and a warning is printed to logcat.</p>
         *
         * <p>If the id is negative, the id is changed to 0 and a warning is printed to
         * logcat.</p>
         *
         * <p>All other parameters are passed-through as-is.</p>
         *
         * @return a new face with the optional features set
         */
        CARAPI ToFace(
            /* [in] */ Int32 id,
            /* [in] */ IPoint* leftEyePosition,
            /* [in] */ IPoint* rightEyePosition,
            /* [in] */ IPoint* mouthPosition,
            /* [out] */ IFace** outface);

        /**
         * Convert to a face; the rect is considered to be the bounds, and the weight
         * is considered to be the score.
         *
         * <p>If the score is out of range of {@value Face#SCORE_MIN}, {@value Face#SCORE_MAX},
         * the score is clipped first and a warning is printed to logcat.</p>
         *
         * <p>All other parameters are passed-through as-is.</p>
         *
         * @return a new face without the optional features
         */
        CARAPI ToFace(
            /* [out] */ IFace** outface);

        CARAPI GetRect(
            /* [out] */ IRect** rect);

        CARAPI GetWeight(
            /* [out] */ Int32* weight);

    private:
        static CARAPI_(Int32) ClipLower(
            /* [in] */ Int32 value,
            /* [in] */ Int32 lo,
            /* [in] */ IRect* rect,
            /* [in] */ const String& name);

        static CARAPI_(Int32) Clip(
            /* [in] */ Int32 value,
            /* [in] */ Int32 lo,
            /* [in] */ Int32 hi,
            /* [in] */ IRect* rect,
            /* [in] */ const String& name);

    public:
        /** Arbitrary rectangle (the range is user-defined); never {@code null}. */
        AutoPtr<IRect> mRect;

        /** Arbitrary weight (the range is user-defined). */
        Int32 mWeight;
    };

public:
    static CARAPI GetNORMALIZED_RECTANGLE_DEFAULT(
        /* [out] */ IRect** rect);

    static CARAPI GetCAMERA_AREA_DEFAULT(
        /* [out] */ ICameraArea** area);

    static CARAPI GetRECTANGLE_EMPTY(
        /* [out] */ IRect** rect);

    /**
     * Convert a camera API1 size into a util size
     */
    static CARAPI ConvertSize(
        /* [in] */ ICameraSize* size,
        /* [out] */ ISize** outsize);

    /**
     * Convert a camera API1 list of sizes into a util list of sizes
     */
    static CARAPI ConvertSizeList(
        /* [in] */ IList* sizeList,
        /* [out] */ IList** outlist);

    /**
     * Convert a camera API1 list of sizes into an array of sizes
     */
    static CARAPI ConvertSizeListToArray(
        /* [in] */ IList* sizeList,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    static CARAPI ConvertSizeArrayToArray(
        /* [in] */ ArrayOf<ICameraSize*>* sizeList,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Check if the camera API1 list of sizes contains a size with the given dimens.
     */
    static CARAPI ContainsSize(
        /* [in] */ IList* sizeList,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* value);

    static CARAPI ContainsSize(
        /* [in] */ ArrayOf<ICameraSize*>* sizeList,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [out] */ Boolean* value);

    /**
     * Returns the largest supported picture size, as compared by its area.
     */
    static CARAPI GetLargestSupportedJpegSizeByArea(
        /* [in] */ IParameters* params,
        /* [out] */ ISize** outsize);

    /**
     * Convert a camera area into a human-readable string.
     */
    static CARAPI StringFromArea(
        /* [in] */ ICameraArea* area,
        /* [out] */ String* str);

    /**
     * Convert a camera area list into a human-readable string
     * @param areaList a list of areas (null is ok)
     */
    static CARAPI StringFromAreaList(
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
    static CARAPI GetClosestAvailableZoomCrop(
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
    static CARAPI GetAvailableZoomCropRectangles(
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
    static CARAPI GetAvailablePreviewZoomCropRectangles(
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
    static CARAPI GetMaxZoomRatio(
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
    static CARAPI ConvertScalerCropRegion(
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
    static CARAPI ConvertMeteringRectangleToLegacy(
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
    static CARAPI ConvertCameraAreaToActiveArrayRectangle(
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
    static CARAPI ConvertFaceFromLegacy(
        /* [in] */ ICameraFace* face,
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [out] */ IFace** outface);

private:
    ParameterUtils();

    /**
     * Calculate the effective crop rectangle for this preview viewport;
     * assumes the preview is centered to the sensor and scaled to fit across one of the dimensions
     * without skewing.
     *
     * <p>The preview size must be a subset of the active array size; the resulting
     * rectangle will also be a subset of the active array rectangle.</p>
     *
     * <p>The unzoomed crop rectangle is calculated only.</p>
     *
     * @param activeArray active array dimensions, in sensor space
     * @param previewSize size of the preview buffer render target, in pixels (not in sensor space)
     * @return a rectangle which serves as the preview stream's effective crop region (unzoomed),
     *         in sensor space
     *
     * @throws NullPointerException
     *          if any of the args were {@code null}
     * @throws IllegalArgumentException
     *          if {@code previewSize} is wider or taller than {@code activeArray}
     */
    static CARAPI GetPreviewCropRectangleUnzoomed(
        /* [in] */ IRect* activeArray,
        /* [in] */ ISize* previewSize,
        /* [out] */ IRect** rect);

    /**
     * Shrink the {@code shrinkTarget} rectangle to snugly fit inside of {@code reference};
     * the aspect ratio of {@code shrinkTarget} will change to be the same aspect ratio as
     * {@code reference}.
     *
     * <p>At most a single dimension will scale (down). Both dimensions will never be scaled.</p>
     *
     * @param reference the rectangle whose aspect ratio will be used as the new aspect ratio
     * @param shrinkTarget the rectangle which will be scaled down to have a new aspect ratio
     *
     * @return a new rectangle, a subset of {@code shrinkTarget},
     *          whose aspect ratio will match that of {@code reference}
     */
    static CARAPI ShrinkToSameAspectRatioCentered(
        /* [in] */ IRect* reference,
        /* [in] */ IRect* shrinkTarget,
        /* [out] */ IRect** rect);

    /**
     * Get the available 'crop' (zoom) rectangles for this camera.
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
    static CARAPI GetAvailableCropRectangles(
        /* [in] */ IParameters* params,
        /* [in] */ IRect* activeArray,
        /* [in] */ ISize* streamSize,
        /* [out] */ IList** list);

    /**
     * Returns the component-wise zoom ratio (each greater or equal than {@code 1.0});
     * largest values means more zoom.
     *
     * @param activeArraySize active array size of the sensor (e.g. max jpeg size)
     * @param cropSize size of the crop/zoom
     *
     * @return {@link SizeF} with width/height being the component-wise zoom ratio
     *
     * @throws NullPointerException if any of the args were {@code null}
     * @throws IllegalArgumentException if any component of {@code cropSize} was {@code 0}
     */
    static CARAPI GetZoomRatio(
        /* [in] */ ISize* activeArraySize,
        /* [in] */ ISize* cropSize,
        /* [out] */ ISizeF** sizef);

    static CARAPI_(AutoPtr<IPoint>) ConvertCameraPointToActiveArrayPoint(
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ IPoint* point,
        /* [in] */ Boolean usePreviewCrop);

    static CARAPI ConvertCameraAreaToActiveArrayRectangle(
        /* [in] */ IRect* activeArray,
        /* [in] */ IParameterUtilsZoomData* zoomData,
        /* [in] */ ICameraArea* area,
        /* [in] */ Boolean usePreviewCrop,
        /* [out] */ IParameterUtilsWeightedRectangle** rectangle);

public:
    /** The default normalized rectangle spans the entire size of the preview viewport */
    static const AutoPtr<IRect> NORMALIZED_RECTANGLE_DEFAULT;

    /** The default normalized area uses the default normalized rectangle with a weight=1 */
    static const AutoPtr<ICameraArea> CAMERA_AREA_DEFAULT;

    /** Empty rectangle {@code 0x0+0,0} */
    static const AutoPtr<IRect> RECTANGLE_EMPTY;

private:
    static const String TAG;
    static const Boolean VERBOSE;

    /** getZoomRatios stores zoom ratios in 1/100 increments, e.x. a zoom of 3.2 is 320 */
    static const Int32 ZOOM_RATIO_MULTIPLIER;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_PARAMETERUTILS_H__
