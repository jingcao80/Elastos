
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYMATADAMAPPER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYMATADAMAPPER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/Range.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Hardware::IParameters;
using Elastos::Droid::Hardware::ICameraInfo;
using Elastos::Droid::Hardware::IHardwareCameraInfo;
using Elastos::Droid::Hardware::ICameraSize;
using Elastos::Droid::Hardware::Camera2::ICaptureResultKey;
using Elastos::Droid::Hardware::Camera2::ICaptureRequestKey;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristicsKey;
using Elastos::Droid::Hardware::Camera2::ICameraCharacteristics;
using Elastos::Droid::Hardware::Camera2::Legacy::ILegacyRequest;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::Range;
using Elastos::Core::Object;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

class LegacyMetadataMapper
    : public Object
    , public ILegacyMetadataMapper
{
public:
    CAR_INTERFACE_DECL()

    LegacyMetadataMapper();

    virtual ~LegacyMetadataMapper() {}

    /**
     * Create characteristics for a legacy device by mapping the {@code parameters}
     * and {@code info}
     *
     * @param parameters A non-{@code null} parameters set
     * @param info Camera info with camera facing direction and angle of orientation
     *
     * @return static camera characteristics for a camera device
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    static CARAPI CreateCharacteristics(
        /* [in] */ IParameters* parameters,
        /* [in] */ IHardwareCameraInfo* info,
        /* [out] */ ICameraCharacteristics** outcc);

    /**
     * Create characteristics for a legacy device by mapping the {@code parameters}
     * and {@code info}
     *
     * @param parameters A string parseable by {@link Camera.Parameters#unflatten}
     * @param info Camera info with camera facing direction and angle of orientation
     * @return static camera characteristics for a camera device
     *
     * @throws NullPointerException if any of the args were {@code null}
     */
    static CARAPI CreateCharacteristics(
        /* [in] */ const String& parameters,
        /* [in] */ ICameraInfo* info,
        /* [out] */ ICameraCharacteristics** outcc);

    /**
     * Set the legacy parameters using the {@link LegacyRequest legacy request}.
     *
     * <p>The legacy request's parameters are changed as a side effect of calling this
     * method.</p>
     *
     * @param request a non-{@code null} legacy request
     */
    static CARAPI ConvertRequestMetadata(
        /* [in] */ ILegacyRequest* request);

    /**
     * Create a request template
     *
     * @param c a non-{@code null} camera characteristics for this camera
     * @param templateId a non-negative template ID
     *
     * @return a non-{@code null} request template
     *
     * @throws IllegalArgumentException if {@code templateId} was invalid
     *
     * @see android.hardware.camera2.CameraDevice#TEMPLATE_MANUAL
     */
    static CARAPI CreateRequestTemplate(
        /* [in] */ ICameraCharacteristics* c,
        /* [in] */ Int32 templateId,
        /* [out] */ ICameraMetadataNative** outcm);

private:
    friend class LegacyRequestMapper;

    static CARAPI_(void) MapCharacteristicsFromInfo(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IHardwareCameraInfo* i);

    static CARAPI_(void) MapCharacteristicsFromParameters(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapScalerStreamConfigs(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    //@SuppressWarnings({"unchecked"})
    static CARAPI MapControlAe(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    //@SuppressWarnings({"unchecked"})
    static CARAPI_(void) MapControlAf(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapControlAwb(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapControlOther(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapLens(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapFlash(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapJpeg(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapRequest(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapScaler(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapSensor(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapStatistics(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) MapSync(
        /* [in] */ ICameraMetadataNative* m,
        /* [in] */ IParameters* p);

    static CARAPI_(void) AppendStreamConfig(
        /* [in] */ IArrayList* configs,
        /* [in] */ Int32 format,
        /* [in] */ IList* sizes);

    static CARAPI_(Int32) ConvertSceneModeFromLegacy(
        /* [in] */ const String& mode);

    static CARAPI_(String) ConvertSceneModeToLegacy(
        /* [in] */ Int32 mode);

    static CARAPI_(Int32) ConvertEffectModeFromLegacy(
        /* [in] */ const String& mode);

    static CARAPI_(String) ConvertEffectModeToLegacy(
        /* [in] */ Int32 mode);

    /**
     * Convert the ae antibanding mode from api1 into api2.
     *
     * @param mode the api1 mode, {@code null} is allowed and will return {@code -1}.
     *
     * @return The api2 value, or {@code -1} by default if conversion failed
     */
    static CARAPI_(Int32) ConvertAntiBandingMode(
        /* [in] */ const String& mode);

    /**
     * Convert the ae antibanding mode from api1 into api2.
     *
     * @param mode the api1 mode, {@code null} is allowed and will return {@code MODE_OFF}.
     *
     * @return The api2 value, or {@code MODE_OFF} by default if conversion failed
     */
    static CARAPI_(Int32) ConvertAntiBandingModeOrDefault(
        /* [in] */ const String& mode);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) ConvertAeFpsRangeToLegacy(
        /* [in] */ Range<IInteger32>* fpsRange);

    /**
     * Return the stall duration for a given output jpeg size in nanoseconds.
     *
     * <p>An 8mp image is chosen to have a stall duration of 0.8 seconds.</p>
     */
    static CARAPI_(Int64) CalculateJpegStallDuration(
        /* [in] */ ICameraSize* size);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetTagsForKeys(
        /* [in] */ ArrayOf<ICameraCharacteristicsKey*>* keys);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetTagsForKeys(
        /* [in] */ ArrayOf<ICaptureRequestKey*>* keys);

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) GetTagsForKeys(
        /* [in] */ ArrayOf<ICaptureResultKey*>* keys);

    /**
     * Convert the requested AF mode into its equivalent supported parameter.
     *
     * @param mode {@code CONTROL_AF_MODE}
     * @param supportedFocusModes list of camera1's supported focus modes
     * @return the stringified af mode, or {@code null} if its not supported
     */
    static CARAPI_(String) ConvertAfModeToLegacy(
        /* [in] */ Int32 mode,
        /* [in] */ ArrayOf<String>* supportedFocusModes);

protected:
    friend class LegacyResultMapper;

    static const Int32 UNKNOWN_MODE;

    /*
     * Development hijinks: Lie about not supporting certain capabilities
     *
     * - Unblock some CTS tests from running whose main intent is not the metadata itself
     *
     * TODO: Remove these constants and strip out any code that previously relied on them
     * being set to true.
     */
    static const Boolean LIE_ABOUT_AE_STATE;
    static const Boolean LIE_ABOUT_AE_MAX_REGIONS;
    static const Boolean LIE_ABOUT_AF;
    static const Boolean LIE_ABOUT_AF_MAX_REGIONS;
    static const Boolean LIE_ABOUT_AWB_STATE;
    static const Boolean LIE_ABOUT_AWB;

private:
    static const String TAG;
    static const Boolean VERBOSE;

    static const Int64 NS_PER_MS;

    // for metadata
    static const Float LENS_INFO_MINIMUM_FOCUS_DISTANCE_FIXED_FOCUS;

    static const Int32 REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_RAW; // no raw support
    static const Int32 REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC; // preview, video, cb
    static const Int32 REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC_STALL; // 1 jpeg only
    static const Int32 REQUEST_MAX_NUM_INPUT_STREAMS_COUNT; // no reprocessing

    /** Assume 3 HAL1 stages: Exposure, Read-out, Post-Processing */
    static const Int32 REQUEST_PIPELINE_MAX_DEPTH_HAL1;
    /** Assume 3 shim stages: Preview input, Split output, Format conversion for output */
    static const Int32 REQUEST_PIPELINE_MAX_DEPTH_OURS;
    /* TODO: Update above maxDepth values once we do more performance measurements */

    // For approximating JPEG stall durations
    static const Int64 APPROXIMATE_CAPTURE_DELAY_MS; // 200 milliseconds
    static const Int64 APPROXIMATE_SENSOR_AREA_PX; // 8 megapixels
    static const Int64 APPROXIMATE_JPEG_ENCODE_TIME_MS; // 600 milliseconds

    // Maximum difference between a preview size aspect ratio and a jpeg size aspect ratio
    static const Float PREVIEW_ASPECT_RATIO_TOLERANCE;

    static AutoPtr<ArrayOf<String> > sLegacySceneModes;

    static AutoPtr<ArrayOf<Int32> > sSceneModes;

    static AutoPtr<ArrayOf<String> > sLegacyEffectMode;

    static AutoPtr<ArrayOf<Int32> > sEffectModes;

    static AutoPtr<ArrayOf<Int32> > sAllowedTemplates;
};

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_LEGECY_LEGACYMATADAMAPPER_H__
