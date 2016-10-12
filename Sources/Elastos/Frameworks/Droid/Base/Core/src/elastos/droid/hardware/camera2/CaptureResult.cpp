
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CLensShadingMap;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CTonemapCurve;
using Elastos::Droid::Hardware::Camera2::ECLSID_CCaptureResultKey;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Location::ECLSID_CLocation;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CaptureResult::Key, Object, ICaptureResultKey)

CaptureResult::Key::Key()
{
}

CaptureResult::Key::Key(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNativeKey(name,  type);
}

ECode CaptureResult::Key::constructor()
{
    return NOERROR;
}

ECode CaptureResult::Key::constructor(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNativeKey(name,  type);
    return NOERROR;
}

ECode CaptureResult::Key::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNativeKey(name,  typeReference);
    return NOERROR;
}

ECode CaptureResult::Key::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
}

ECode CaptureResult::Key::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mKey->GetName(name);
}

ECode CaptureResult::Key::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    return IObject::Probe(mKey)->GetHashCode(code);
}

ECode CaptureResult::Key::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;

    Key* other = (Key*)ICaptureResultKey::Probe(obj);
    if (other != NULL) {
        *e = Object::Equals(other->mKey, mKey);
    }
    return NOERROR;
}

ECode CaptureResult::Key::GetNativeKey(
    /* [out] */ ICameraMetadataNativeKey** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

const String CaptureResult::TAG("CaptureResult");
const Boolean CaptureResult::VERBOSE = FALSE;

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_MODE =
        new Key(String("android.colorCorrection.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_TRANSFORM =
        new Key(String("android.colorCorrection.transform"), ECLSID_CColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_GAINS =
        new Key(String("android.colorCorrection.gains"), ECLSID_CRggbChannelVector);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_ABERRATION_MODE =
        new Key(String("android.colorCorrection.aberrationMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_ANTIBANDING_MODE =
        new Key(String("android.control.aeAntibandingMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_EXPOSURE_COMPENSATION =
        new Key(String("android.control.aeExposureCompensation"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_LOCK =
        new Key(String("android.control.aeLock"), ECLSID_CBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_MODE =
        new Key(String("android.control.aeMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_REGIONS =
        new Key(String("android.control.aeRegions"), ECLSID_CArrayOf);

// INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_TARGET_FPS_RANGE =
//         new Key<android.util.Range<Integer>>("android.control.aeTargetFpsRange",
//         new TypeReference<android.util.Range<Integer>>() {{ }});

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_PRECAPTURE_TRIGGER =
        new Key(String("android.control.aePrecaptureTrigger"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_STATE =
        new Key(String("android.control.aeState"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_MODE =
        new Key(String("android.control.afMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_REGIONS =
        new Key(String("android.control.afRegions"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_TRIGGER =
        new Key(String("android.control.afTrigger"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_STATE =
        new Key(String("android.control.afState"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_LOCK =
        new Key(String("android.control.awbLock"), ECLSID_CBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_MODE =
        new Key(String("android.control.awbMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_REGIONS =
        new Key(String("android.control.awbRegions"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_CAPTURE_INTENT =
        new Key(String("android.control.captureIntent"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_STATE =
        new Key(String("android.control.awbState"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_EFFECT_MODE =
        new Key(String("android.control.effectMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_MODE =
        new Key(String("android.control.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_SCENE_MODE =
        new Key(String("android.control.sceneMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_VIDEO_STABILIZATION_MODE =
        new Key(String("android.control.videoStabilizationMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::EDGE_MODE =
        new Key(String("android.edge.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::FLASH_MODE =
        new Key(String("android.flash.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::FLASH_STATE =
        new Key(String("android.flash.state"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::HOT_PIXEL_MODE =
        new Key(String("android.hotPixel.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_LOCATION =
        new Key(String("android.jpeg.gpsLocation"), ECLSID_CLocation);

//double[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_COORDINATES =
        new Key(String("android.jpeg.gpsCoordinates"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_PROCESSING_METHOD =
        new Key(String("android.jpeg.gpsProcessingMethod"), ECLSID_CString);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_TIMESTAMP =
        new Key(String("android.jpeg.gpsTimestamp"), ECLSID_CInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_ORIENTATION =
        new Key(String("android.jpeg.orientation"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_QUALITY =
        new Key(String("android.jpeg.quality"), ECLSID_CByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_THUMBNAIL_QUALITY =
        new Key(String("android.jpeg.thumbnailQuality"), ECLSID_CByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_THUMBNAIL_SIZE =
        new Key(String("android.jpeg.thumbnailSize"), ECLSID_CSize);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_APERTURE =
        new Key(String("android.lens.aperture"), ECLSID_CFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FILTER_DENSITY =
        new Key(String("android.lens.filterDensity"), ECLSID_CFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCAL_LENGTH =
        new Key(String("android.lens.focalLength"), ECLSID_CFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCUS_DISTANCE =
        new Key(String("android.lens.focusDistance"), ECLSID_CFloat);

// INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCUS_RANGE =
//         new Key<android.util.Pair<Float,Float>>("android.lens.focusRange", new TypeReference<android.util.Pair<Float,Float>>() {{ }});

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_OPTICAL_STABILIZATION_MODE =
        new Key(String("android.lens.opticalStabilizationMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_STATE =
        new Key(String("android.lens.state"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::NOISE_REDUCTION_MODE =
        new Key(String("android.noiseReduction.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::QUIRKS_PARTIAL_RESULT =
        new Key(String("android.quirks.partialResult"), ECLSID_CBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_FRAME_COUNT =
        new Key(String("android.request.frameCount"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_ID =
        new Key(String("android.request.id"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_PIPELINE_DEPTH =
        new Key(String("android.request.pipelineDepth"), ECLSID_CByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SCALER_CROP_REGION =
        new Key(String("android.scaler.cropRegion"), ECLSID_CRect);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_EXPOSURE_TIME =
        new Key(String("android.sensor.exposureTime"), ECLSID_CInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_FRAME_DURATION =
        new Key(String("android.sensor.frameDuration"), ECLSID_CInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_SENSITIVITY =
        new Key(String("android.sensor.sensitivity"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TIMESTAMP =
        new Key(String("android.sensor.timestamp"), ECLSID_CInteger64);

//Rational[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_NEUTRAL_COLOR_POINT =
        new Key(String("android.sensor.neutralColorPoint"), ECLSID_CArrayOf);

// INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_NOISE_PROFILE =
//         new Key<android.util.Pair<Double,Double>[]>("android.sensor.noiseProfile",
//         new TypeReference<android.util.Pair<Double,Double>[]>() {{ }});

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_GREEN_SPLIT =
        new Key(String("android.sensor.greenSplit"), ECLSID_CFloat);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TEST_PATTERN_DATA =
        new Key(String("android.sensor.testPatternData"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TEST_PATTERN_MODE =
        new Key(String("android.sensor.testPatternMode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_ROLLING_SHUTTER_SKEW =
        new Key(String("android.sensor.rollingShutterSkew"), ECLSID_CInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SHADING_MODE =
        new Key(String("android.shading.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_DETECT_MODE =
        new Key(String("android.statistics.faceDetectMode"), ECLSID_CInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_IDS =
        new Key(String("android.statistics.faceIds"), ECLSID_CArrayOf);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_LANDMARKS =
        new Key(String("android.statistics.faceLandmarks"), ECLSID_CArrayOf);

//android.graphics.Rect[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_RECTANGLES =
        new Key(String("android.statistics.faceRectangles"), ECLSID_CArrayOf);

//byte[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_SCORES =
        new Key(String("android.statistics.faceScores"), ECLSID_CArrayOf);

//android.hardware.camera2.params.Face[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACES =
        new Key(String("android.statistics.faces"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_CORRECTION_MAP =
        new Key(String("android.statistics.lensShadingCorrectionMap"), ECLSID_CLensShadingMap);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_MAP =
        new Key(String("android.statistics.lensShadingMap"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_PREDICTED_COLOR_GAINS =
        new Key(String("android.statistics.predictedColorGains"), ECLSID_CArrayOf);

//Rational[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_PREDICTED_COLOR_TRANSFORM =
        new Key(String("android.statistics.predictedColorTransform"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_SCENE_FLICKER =
        new Key(String("android.statistics.sceneFlicker"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_HOT_PIXEL_MAP_MODE =
        new Key(String("android.statistics.hotPixelMapMode"), ECLSID_CBoolean);

//android.graphics.Point[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_HOT_PIXEL_MAP =
        new Key(String("android.statistics.hotPixelMap"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_MAP_MODE =
        new Key(String("android.statistics.lensShadingMapMode"), ECLSID_CInteger32);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_BLUE =
        new Key(String("android.tonemap.curveBlue"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_GREEN =
        new Key(String("android.tonemap.curveGreen"), ECLSID_CArrayOf);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_RED =
        new Key(String("android.tonemap.curveRed"), ECLSID_CArrayOf);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE =
        new Key(String("android.tonemap.curve"), ECLSID_CTonemapCurve);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_MODE =
        new Key(String("android.tonemap.mode"), ECLSID_CInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LED_TRANSMIT =
        new Key(String("android.led.transmit"), ECLSID_CBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::BLACK_LEVEL_LOCK =
        new Key(String("android.blackLevel.lock"), ECLSID_CBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SYNC_FRAME_NUMBER =
        new Key(String("android.sync.frameNumber"), ECLSID_CInteger64);


CAR_INTERFACE_IMPL(CaptureResult, CameraMetadata, ICaptureResult)

CaptureResult::CaptureResult()
    : mSequenceId(0)
    , mFrameNumber(0)
{
}

ECode CaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ ICaptureRequest* parent,
    /* [in] */ ICaptureResultExtras* extras)
{
    if (results == NULL) {
        //throw new IllegalArgumentException("results was null");
        Slogger::E(TAG, "results was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parent == NULL) {
        //throw new IllegalArgumentException("parent was null");
        Slogger::E(TAG, "parent was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (extras == NULL) {
        //throw new IllegalArgumentException("extras was null");
        Slogger::E(TAG, "extras was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CameraMetadataNative::Move(results, (ICameraMetadataNative**)&mResults);
    Boolean res;
    mResults->IsEmpty(&res);
    if (res) {
        //throw new AssertionError("Results must not be empty");
        Slogger::E(TAG, "Results must not be empty");
        return E_ASSERTION_ERROR;
    }
    mRequest = parent;
    extras->GetRequestId(&mSequenceId);
    return extras->GetFrameNumber(&mFrameNumber);
}

ECode CaptureResult::constructor(
    /* [in] */ ICameraMetadataNative* results,
    /* [in] */ Int32 sequenceId)
{
    if (results == NULL) {
        //throw new IllegalArgumentException("results was null");
        Slogger::E(TAG, "results was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CameraMetadataNative::Move(results, (ICameraMetadataNative**)&mResults);
    Boolean res;
    mResults->IsEmpty(&res);
    if (res) {
        //throw new AssertionError("Results must not be empty");
        Slogger::E(TAG, "Results must not be empty");
        return E_ASSERTION_ERROR;
    }

    mRequest = NULL;
    mSequenceId = sequenceId;
    mFrameNumber = -1;
    return NOERROR;
}

ECode CaptureResult::GetNativeCopy(
    /* [out] */ ICameraMetadataNative** native)
{
    VALIDATE_NOT_NULL(native)

    return CCameraMetadataNative::New(mResults, native);
}

ECode CaptureResult::Get(
    /* [in] */ ICaptureResultKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    AutoPtr<IInterface> value;
    mResults->Get(key, (IInterface**)&value);
    if (VERBOSE) {
        String name;
        key->GetName(&name);
        String str;
        IObject::Probe(value)->ToString(&str);
        Slogger::V(TAG, "#get for Key = %s, returned value = %s",name.string(), str.string());
    }
    *outface = value;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode CaptureResult::GetProtected(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    return mResults->Get(ICaptureResultKey::Probe(key), outface);
}

ECode CaptureResult::GetKeyClass(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)

    *id = ECLSID_CCaptureResultKey;
    return NOERROR;
}

ECode CaptureResult::DumpToLog()
{
    return mResults->DumpToLog();
}

ECode CaptureResult::GetKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)

    // Force the javadoc for this function to show up on the CaptureResult page
    return CameraMetadata::GetKeys(outlist);
}

ECode CaptureResult::GetRequest(
    /* [out] */ ICaptureRequest** outcr)
{
    VALIDATE_NOT_NULL(outcr)

    *outcr = mRequest;
    REFCOUNT_ADD(*outcr);
    return NOERROR;
}

ECode CaptureResult::GetFrameNumber(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mFrameNumber;
    return NOERROR;
}

ECode CaptureResult::GetSequenceId(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSequenceId;
    return NOERROR;
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos