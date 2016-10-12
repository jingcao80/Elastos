
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CameraMetadata.h"
#include "elastos/droid/hardware/camera2/impl/CameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CBlackLevelPattern;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Utility::ECLSID_CRational;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Droid::Utility::ECLSID_CSizeF;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ECLSID_CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

//===================================================================================
// CameraCharacteristicsKey
//===================================================================================
CAR_INTERFACE_IMPL(CameraCharacteristicsKey, Object, ICameraCharacteristicsKey)

CameraCharacteristicsKey::CameraCharacteristicsKey()
{
}

ECode CameraCharacteristicsKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNativeKey(name,  type);
    return NOERROR;
}

ECode CameraCharacteristicsKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNativeKey(name,  typeReference);
    return NOERROR;
}

ECode CameraCharacteristicsKey::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
}

CameraCharacteristicsKey::CameraCharacteristicsKey(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNativeKey(name,  type);
}

CameraCharacteristicsKey::CameraCharacteristicsKey(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
}

ECode CameraCharacteristicsKey::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mKey->GetName(name);
}

ECode CameraCharacteristicsKey::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    *code = Object::GetHashCode(mKey);
    return NOERROR;
}

ECode CameraCharacteristicsKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;

    CameraCharacteristicsKey* other = (CameraCharacteristicsKey*)ICameraCharacteristicsKey::Probe(obj);
    if (other != NULL) {
        *e = Object::Equals(other->mKey, mKey);
    }
    return NOERROR;
}

ECode CameraCharacteristicsKey::GetNativeKey(
    /* [out] */ ICameraMetadataNativeKey** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode CameraCharacteristicsKey::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CameraCharacteristicsKey:");
    sb += StringUtils::ToHexString((Int32)this);
    sb += "{key:";
    sb += TO_CSTR(mKey);
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//===================================================================================
// CameraCharacteristics
//===================================================================================
//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES =
        new CameraCharacteristicsKey(String("android.colorCorrection.availableAberrationModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_ANTIBANDING_MODES =
        new CameraCharacteristicsKey(String("android.control.aeAvailableAntibandingModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_MODES =
        new CameraCharacteristicsKey(String("android.control.aeAvailableModes"), ECLSID_CArrayOf);

// INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES =
//         new CameraCharacteristicsKey<android.util.Range<Integer>[]>(String("android.control.aeAvailableTargetFpsRanges"),
//         new TypeReference<android.util.Range<Integer>[]>() {{ }});

// INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_COMPENSATION_RANGE =
//         new CameraCharacteristicsKey<android.util.Range<Integer>>(String("android.control.aeCompensationRange"),
//         new TypeReference<android.util.Range<Integer>>() {{ }});

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_COMPENSATION_STEP =
        new CameraCharacteristicsKey(String("android.control.aeCompensationStep"), ECLSID_CRational);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES =
        new CameraCharacteristicsKey(String("android.control.afAvailableModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_EFFECTS =
        new CameraCharacteristicsKey(String("android.control.availableEffects"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_SCENE_MODES =
        new CameraCharacteristicsKey(String("android.control.availableSceneModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES =
        new CameraCharacteristicsKey(String("android.control.availableVideoStabilizationModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AWB_AVAILABLE_MODES =
        new CameraCharacteristicsKey(String("android.control.awbAvailableModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS =
        new CameraCharacteristicsKey(String("android.control.maxRegions"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AE =
        new CameraCharacteristicsKey(String("android.control.maxRegionsAe"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AWB =
        new CameraCharacteristicsKey(String("android.control.maxRegionsAwb"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AF =
        new CameraCharacteristicsKey(String("android.control.maxRegionsAf"), ECLSID_CInteger32);

//android.hardware.camera2.params.HighSpeedVideoConfiguration[].class)
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS =
        new CameraCharacteristicsKey(String("android.control.availableHighSpeedVideoConfigurations"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::EDGE_AVAILABLE_EDGE_MODES =
        new CameraCharacteristicsKey(String("android.edge.availableEdgeModes"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::FLASH_INFO_AVAILABLE =
        new CameraCharacteristicsKey(String("android.flash.info.available"), ECLSID_CBoolean);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES =
        new CameraCharacteristicsKey(String("android.hotPixel.availableHotPixelModes"), ECLSID_CArrayOf);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::JPEG_AVAILABLE_THUMBNAIL_SIZES =
        new CameraCharacteristicsKey(String("android.jpeg.availableThumbnailSizes"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_APERTURES =
        new CameraCharacteristicsKey(String("android.lens.info.availableApertures"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_FILTER_DENSITIES =
        new CameraCharacteristicsKey(String("android.lens.info.availableFilterDensities"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_FOCAL_LENGTHS =
        new CameraCharacteristicsKey(String("android.lens.info.availableFocalLengths"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION =
        new CameraCharacteristicsKey(String("android.lens.info.availableOpticalStabilization"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_HYPERFOCAL_DISTANCE =
        new CameraCharacteristicsKey(String("android.lens.info.hyperfocalDistance"), ECLSID_CFloat);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE =
        new CameraCharacteristicsKey(String("android.lens.info.minimumFocusDistance"), ECLSID_CFloat);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_SHADING_MAP_SIZE =
        new CameraCharacteristicsKey(String("android.lens.info.shadingMapSize"), ECLSID_CSize);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_FOCUS_DISTANCE_CALIBRATION =
        new CameraCharacteristicsKey(String("android.lens.info.focusDistanceCalibration"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_FACING =
        new CameraCharacteristicsKey(String("android.lens.facing"), ECLSID_CInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES =
        new CameraCharacteristicsKey(String("android.noiseReduction.availableNoiseReductionModes"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::QUIRKS_USE_PARTIAL_RESULT =
        new CameraCharacteristicsKey(String("android.quirks.usePartialResult"), ECLSID_CByte);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_STREAMS =
        new CameraCharacteristicsKey(String("android.request.maxNumOutputStreams"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_RAW =
        new CameraCharacteristicsKey(String("android.request.maxNumOutputRaw"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC =
        new CameraCharacteristicsKey(String("android.request.maxNumOutputProc"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC_STALLING =
        new CameraCharacteristicsKey(String("android.request.maxNumOutputProcStalling"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_INPUT_STREAMS =
        new CameraCharacteristicsKey(String("android.request.maxNumInputStreams"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_PIPELINE_MAX_DEPTH =
        new CameraCharacteristicsKey(String("android.request.pipelineMaxDepth"), ECLSID_CByte);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_PARTIAL_RESULT_COUNT =
        new CameraCharacteristicsKey(String("android.request.partialResultCount"), ECLSID_CInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_CAPABILITIES =
        new CameraCharacteristicsKey(String("android.request.availableCapabilities"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_REQUEST_KEYS =
        new CameraCharacteristicsKey(String("android.request.availableRequestKeys"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_RESULT_KEYS =
        new CameraCharacteristicsKey(String("android.request.availableResultKeys"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_CHARACTERISTICS_KEYS =
        new CameraCharacteristicsKey(String("android.request.availableCharacteristicsKeys"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_FORMATS =
        new CameraCharacteristicsKey(String("android.scaler.availableFormats"), ECLSID_CArrayOf);

//long[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_JPEG_MIN_DURATIONS =
        new CameraCharacteristicsKey(String("android.scaler.availableJpegMinDurations"), ECLSID_CArrayOf);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_JPEG_SIZES =
        new CameraCharacteristicsKey(String("android.scaler.availableJpegSizes"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_MAX_DIGITAL_ZOOM =
        new CameraCharacteristicsKey(String("android.scaler.availableMaxDigitalZoom"), ECLSID_CFloat);

//long[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_PROCESSED_MIN_DURATIONS =
        new CameraCharacteristicsKey(String("android.scaler.availableProcessedMinDurations"), ECLSID_CArrayOf);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_PROCESSED_SIZES =
        new CameraCharacteristicsKey(String("android.scaler.availableProcessedSizes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_INPUT_OUTPUT_FORMATS_MAP =
        new CameraCharacteristicsKey(String("android.scaler.availableInputOutputFormatsMap"), ECLSID_CArrayOf);

//android.hardware.camera2.params.StreamConfiguration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_STREAM_CONFIGURATIONS =
        new CameraCharacteristicsKey(String("android.scaler.availableStreamConfigurations"), ECLSID_CArrayOf);

//android.hardware.camera2.params.StreamConfigurationDuration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_MIN_FRAME_DURATIONS =
        new CameraCharacteristicsKey(String("android.scaler.availableMinFrameDurations"), ECLSID_CArrayOf);

//android.hardware.camera2.params.StreamConfigurationDuration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_STALL_DURATIONS =
        new CameraCharacteristicsKey(String("android.scaler.availableStallDurations"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP =
        new CameraCharacteristicsKey(String("android.scaler.streamConfigurationMap"), ECLSID_CStreamConfigurationMap);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_CROPPING_TYPE =
        new CameraCharacteristicsKey(String("android.scaler.croppingType"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE =
        new CameraCharacteristicsKey(String("android.sensor.info.activeArraySize"), ECLSID_CRect);

// INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_SENSITIVITY_RANGE =
//         new CameraCharacteristicsKey<android.util.Range<Integer>>(String("android.sensor.info.sensitivityRange"),
//         new TypeReference<android.util.Range<Integer>>() {{ }});

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_COLOR_FILTER_ARRANGEMENT =
        new CameraCharacteristicsKey(String("android.sensor.info.colorFilterArrangement"), ECLSID_CInteger32);

// INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_EXPOSURE_TIME_RANGE =
//         new CameraCharacteristicsKey<android.util.Range<Long>>(String("android.sensor.info.exposureTimeRange"),
//         new TypeReference<android.util.Range<Long>>() {{ }});

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_MAX_FRAME_DURATION =
        new CameraCharacteristicsKey(String("android.sensor.info.maxFrameDuration"), ECLSID_CInteger64);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_PHYSICAL_SIZE =
        new CameraCharacteristicsKey(String("android.sensor.info.physicalSize"), ECLSID_CSizeF);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_PIXEL_ARRAY_SIZE =
        new CameraCharacteristicsKey(String("android.sensor.info.pixelArraySize"), ECLSID_CSize);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_WHITE_LEVEL =
        new CameraCharacteristicsKey(String("android.sensor.info.whiteLevel"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_TIMESTAMP_SOURCE =
        new CameraCharacteristicsKey(String("android.sensor.info.timestampSource"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_REFERENCE_ILLUMINANT1 =
        new CameraCharacteristicsKey(String("android.sensor.referenceIlluminant1"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_REFERENCE_ILLUMINANT2 =
        new CameraCharacteristicsKey(String("android.sensor.referenceIlluminant2"), ECLSID_CByte);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_CALIBRATION_TRANSFORM1 =
        new CameraCharacteristicsKey(String("android.sensor.calibrationTransform1"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_CALIBRATION_TRANSFORM2 =
        new CameraCharacteristicsKey(String("android.sensor.calibrationTransform2"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_COLOR_TRANSFORM1 =
        new CameraCharacteristicsKey(String("android.sensor.colorTransform1"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_COLOR_TRANSFORM2 =
        new CameraCharacteristicsKey(String("android.sensor.colorTransform2"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_FORWARD_MATRIX1 =
        new CameraCharacteristicsKey(String("android.sensor.forwardMatrix1"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_FORWARD_MATRIX2 =
        new CameraCharacteristicsKey(String("android.sensor.forwardMatrix2"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_BLACK_LEVEL_PATTERN =
        new CameraCharacteristicsKey(String("android.sensor.blackLevelPattern"), ECLSID_CBlackLevelPattern);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_MAX_ANALOG_SENSITIVITY =
        new CameraCharacteristicsKey(String("android.sensor.maxAnalogSensitivity"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_ORIENTATION =
        new CameraCharacteristicsKey(String("android.sensor.orientation"), ECLSID_CInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_AVAILABLE_TEST_PATTERN_MODES =
        new CameraCharacteristicsKey(String("android.sensor.availableTestPatternModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES =
        new CameraCharacteristicsKey(String("android.statistics.info.availableFaceDetectModes"), ECLSID_CArrayList);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_MAX_FACE_COUNT =
        new CameraCharacteristicsKey(String("android.statistics.info.maxFaceCount"), ECLSID_CInteger32);

//boolean[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_AVAILABLE_HOT_PIXEL_MAP_MODES =
        new CameraCharacteristicsKey(String("android.statistics.info.availableHotPixelMapModes"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::TONEMAP_MAX_CURVE_POINTS =
        new CameraCharacteristicsKey(String("android.tonemap.maxCurvePoints"), ECLSID_CInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::TONEMAP_AVAILABLE_TONE_MAP_MODES =
        new CameraCharacteristicsKey(String("android.tonemap.availableToneMapModes"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LED_AVAILABLE_LEDS =
        new CameraCharacteristicsKey(String("android.led.availableLeds"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::INFO_SUPPORTED_HARDWARE_LEVEL =
        new CameraCharacteristicsKey(String("android.info.supportedHardwareLevel"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SYNC_MAX_LATENCY =
        new CameraCharacteristicsKey(String("android.sync.maxLatency"), ECLSID_CInteger32);

CAR_INTERFACE_IMPL_2(CameraCharacteristics, Object, ICameraCharacteristics, ICameraMetadata)

CameraCharacteristics::CameraCharacteristics()
{
}

ECode CameraCharacteristics::constructor(
    /* [in] */ ICameraMetadataNative* properties)
{
    return CameraMetadataNative::Move(properties, (ICameraMetadataNative**)&mProperties);
}

ECode CameraCharacteristics::GetNativeCopy(
    /* [out] */ ICameraMetadataNative** outcmn)
{
    VALIDATE_NOT_NULL(outcmn);

    return CCameraMetadataNative::New(mProperties, outcmn);
}

ECode CameraCharacteristics::Get(
    /* [in] */ ICameraCharacteristicsKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    return mProperties->Get(key, outface);
}

ECode CameraCharacteristics::GetKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    // List of keys is immutable; cache the results after we calculate them
    if (mKeys != NULL) {
        *outlist = mKeys;
        REFCOUNT_ADD(*outlist);
        return NOERROR;
    }

    AutoPtr<IInterface> obj;
    Get(REQUEST_AVAILABLE_CHARACTERISTICS_KEYS, (IInterface**)&obj);
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
    if (arrayObj == NULL) {
        Slogger::E("CameraCharacteristics", "android.request.availableCharacteristicsKeys must be non-null"
                " in the characteristics");
        return E_ASSERTION_ERROR;
    }

    Int32 size;
    arrayObj->GetLength(&size);
    AutoPtr<ArrayOf<Int32> > filterTags = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i< size; i++) {
        AutoPtr<IInterface> tmp;
        arrayObj->Get(i, (IInterface**)&tmp);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
        Int32 value;
        intObj->GetValue(&value);
        (*filterTags)[i] = value;
    }

    AutoPtr<IList> list;
    ClassID leyClass;
    GetKeyClass(&leyClass);
    assert(0);
    //GetKeysStatic(getClass(), leyClass, this, filterTags, (IArrayList**)&list);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    collections->UnmodifiableList(list, (IList**)&mKeys);
    *outlist = mKeys;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode CameraCharacteristics::GetProtected(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    return mProperties->Get(ICameraCharacteristicsKey::Probe(key), outface);
}

ECode CameraCharacteristics::GetKeyClass(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id);

    *id = ECLSID_CCameraCharacteristicsKey;
    return NOERROR;
}

ECode CameraCharacteristics::GetAvailableCaptureRequestKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    if (mAvailableRequestKeys == NULL) {
        ClassID crKeyTyped = ECLSID_CCaptureRequestKey;

        AutoPtr<IInterface> obj;
        Get(REQUEST_AVAILABLE_REQUEST_KEYS, (IInterface**)&obj);
        AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
        if (arrayObj == NULL) {
            Slogger::E("CameraCharacteristics", "android.request.availableRequestKeys must be non-null "
                    "in the characteristics");
            return E_ASSERTION_ERROR;
        }

        Int32 size;
        arrayObj->GetLength(&size);
        AutoPtr<ArrayOf<Int32> > filterTags = ArrayOf<Int32>::Alloc(size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> tmp;
            arrayObj->Get(i, (IInterface**)&tmp);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
            Int32 value;
            intObj->GetValue(&value);
            (*filterTags)[i] = value;
        }

        assert(0);
        //GetAvailableKeyList(ECLSID_CCaptureRequest, crKeyTyped, filterTags, (IList**)&mAvailableRequestKeys);
    }
    *outlist = mAvailableRequestKeys;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode CameraCharacteristics::GetAvailableCaptureResultKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    if (mAvailableResultKeys == NULL) {
        ClassID crKeyTyped = ECLSID_CCaptureResultKey;

        AutoPtr<IInterface> obj;
        Get(REQUEST_AVAILABLE_RESULT_KEYS, (IInterface**)&obj);
        AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
        if (arrayObj == NULL) {
            Slogger::E("CameraCharacteristics", "android.request.availableResultKeys must be non-null "
                    "in the characteristics");
            return E_ASSERTION_ERROR;
        }

        Int32 size;
        arrayObj->GetLength(&size);
        AutoPtr<ArrayOf<Int32> > filterTags = ArrayOf<Int32>::Alloc(size);
        for (Int32 i = 0; i< size; i++) {
            AutoPtr<IInterface> tmp;
            arrayObj->Get(i, (IInterface**)&tmp);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
            Int32 value;
            intObj->GetValue(&value);
            (*filterTags)[i] = value;
        }

        GetAvailableKeyList(ECLSID_CCaptureResult, crKeyTyped, filterTags, (IList**)&mAvailableResultKeys);
    }
    *outlist = mAvailableResultKeys;
    REFCOUNT_ADD(*outlist);
    return NOERROR;
}

ECode CameraCharacteristics::GetAvailableKeyList(
    /* [in] */ ClassID metadataClass,
    /* [in] */ ClassID keyClass,
    /* [in] */ ArrayOf<Int32>* filterTags,
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist);
    *outlist = NULL;

    assert(0);
    // if (metadataClass.equals(CameraMetadata.class)) {
    //     // throw new AssertionError(
    //     //         "metadataClass must be a strict subclass of CameraMetadata");
    //     Slogger::E("CameraCharacteristics", "metadataClass must be a strict subclass of CameraMetadata");
    //     return E_ASSERTION_ERROR;
    // }
    // else if (!CameraMetadata.class.isAssignableFrom(metadataClass)) {
    //     // throw new AssertionError(
    //     //         "metadataClass must be a subclass of CameraMetadata");
    //     Slogger::E("CameraCharacteristics", "metadataClass must be a subclass of CameraMetadata");
    //     return E_ASSERTION_ERROR;
    // }

    AutoPtr<IArrayList> staticKeyList;
    CameraMetadata::GetKeysStatic(
        metadataClass, keyClass, /*instance*/NULL, filterTags, (IArrayList**)&staticKeyList);
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->UnmodifiableList(IList::Probe(staticKeyList), outlist);
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos