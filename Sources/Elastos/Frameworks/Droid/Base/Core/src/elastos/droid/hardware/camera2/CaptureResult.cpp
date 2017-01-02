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

#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/camera2/CCaptureResultKey.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::ECLSID_CCaptureResultKey;
using Elastos::Droid::Hardware::Camera2::EIID_ICaptureResultKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CFace;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CLensShadingMap;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CTonemapCurve;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IFace;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::EIID_ILensShadingMap;
using Elastos::Droid::Hardware::Camera2::Params::EIID_ITonemapCurve;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Graphics::ECLSID_CPoint;
using Elastos::Droid::Location::ECLSID_CLocation;
using Elastos::Droid::Graphics::EIID_IRect;
using Elastos::Droid::Graphics::EIID_IPoint;
using Elastos::Droid::Location::EIID_ILocation;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Droid::Utility::ECLSID_CRational;
using Elastos::Droid::Utility::ECLSID_CPair;
using Elastos::Droid::Utility::ECLSID_CRange;
using Elastos::Droid::Utility::EIID_ISize;
using Elastos::Droid::Utility::EIID_IRational;
using Elastos::Droid::Utility::EIID_IPair;
using Elastos::Droid::Utility::EIID_IRange;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CDouble;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Core::EIID_IString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CaptureResultKey, Object, ICaptureResultKey)

CaptureResultKey::CaptureResultKey()
{
}

ECode CaptureResultKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    mKey = new CameraMetadataNativeKey(name,  classId, interfaceId);
    return NOERROR;
}

ECode CaptureResultKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ const ClassID& componentClassId,
    /* [in] */ const InterfaceID& componentInterfaceId)
{
    mKey = new CameraMetadataNativeKey(name, classId, interfaceId,
        componentClassId, componentInterfaceId);
    return NOERROR;
}

ECode CaptureResultKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNativeKey(name, typeReference);
    return NOERROR;
}

ECode CaptureResultKey::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
}

ECode CaptureResultKey::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mKey->GetName(name);
}

ECode CaptureResultKey::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    return IObject::Probe(mKey)->GetHashCode(code);
}

ECode CaptureResultKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;

    CaptureResultKey* other = (CaptureResultKey*)ICaptureResultKey::Probe(obj);
    if (other != NULL) {
        *e = Object::Equals(other->mKey, mKey);
    }
    return NOERROR;
}

ECode CaptureResultKey::GetNativeKey(
    /* [out] */ ICameraMetadataNativeKey** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

const String CaptureResult::TAG("CaptureResult");
const Boolean CaptureResult::VERBOSE = FALSE;

static AutoPtr<ICaptureResultKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    AutoPtr<ICaptureResultKey> key;
    CCaptureResultKey::New(String(name), classId, interfaceId, (ICaptureResultKey**)&key);
    return key;
}

static AutoPtr<ICaptureResultKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ const ClassID& componentClassId,
    /* [in] */ const InterfaceID& componentInterfaceId)
{
    AutoPtr<ICaptureResultKey> key;
    CCaptureResultKey::New(String(name), classId, interfaceId,
        componentClassId, componentInterfaceId, (ICaptureResultKey**)&key);
    return key;
}

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_MODE =
    CreateKey("android.colorCorrection.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_TRANSFORM =
    CreateKey("android.colorCorrection.transform", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_GAINS =
    CreateKey("android.colorCorrection.gains", ECLSID_CRggbChannelVector, EIID_IRggbChannelVector);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::COLOR_CORRECTION_ABERRATION_MODE =
    CreateKey("android.colorCorrection.aberrationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_ANTIBANDING_MODE =
    CreateKey("android.control.aeAntibandingMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_EXPOSURE_COMPENSATION =
    CreateKey("android.control.aeExposureCompensation", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_LOCK =
    CreateKey("android.control.aeLock", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_MODE =
    CreateKey("android.control.aeMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_REGIONS =
    CreateKey("android.control.aeRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

//android.util.Range<Integer>>
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_TARGET_FPS_RANGE =
    CreateKey("android.control.aeTargetFpsRange", ECLSID_CRange, EIID_IRange,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_PRECAPTURE_TRIGGER =
    CreateKey("android.control.aePrecaptureTrigger", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AE_STATE =
    CreateKey("android.control.aeState", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_MODE =
    CreateKey("android.control.afMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_REGIONS =
    CreateKey("android.control.afRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_TRIGGER =
    CreateKey("android.control.afTrigger", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AF_STATE =
    CreateKey("android.control.afState", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_LOCK =
    CreateKey("android.control.awbLock", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_MODE =
    CreateKey("android.control.awbMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_REGIONS =
    CreateKey("android.control.awbRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_CAPTURE_INTENT =
    CreateKey("android.control.captureIntent", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_AWB_STATE =
    CreateKey("android.control.awbState", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_EFFECT_MODE =
    CreateKey("android.control.effectMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_MODE =
    CreateKey("android.control.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_SCENE_MODE =
    CreateKey("android.control.sceneMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::CONTROL_VIDEO_STABILIZATION_MODE =
    CreateKey("android.control.videoStabilizationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::EDGE_MODE =
    CreateKey("android.edge.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::FLASH_MODE =
    CreateKey("android.flash.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::FLASH_STATE =
    CreateKey("android.flash.state", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::HOT_PIXEL_MODE =
    CreateKey("android.hotPixel.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_LOCATION =
    CreateKey("android.jpeg.gpsLocation", ECLSID_CLocation, EIID_ILocation);

//double[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_COORDINATES =
    CreateKey("android.jpeg.gpsCoordinates", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CDouble, EIID_IDouble);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_PROCESSING_METHOD =
    CreateKey("android.jpeg.gpsProcessingMethod", ECLSID_CString, EIID_IString);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_GPS_TIMESTAMP =
    CreateKey("android.jpeg.gpsTimestamp", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_ORIENTATION =
    CreateKey("android.jpeg.orientation", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_QUALITY =
    CreateKey("android.jpeg.quality", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_THUMBNAIL_QUALITY =
    CreateKey("android.jpeg.thumbnailQuality", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::JPEG_THUMBNAIL_SIZE =
    CreateKey("android.jpeg.thumbnailSize", ECLSID_CSize, EIID_ISize);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_APERTURE =
    CreateKey("android.lens.aperture", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FILTER_DENSITY =
    CreateKey("android.lens.filterDensity", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCAL_LENGTH =
    CreateKey("android.lens.focalLength", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCUS_DISTANCE =
    CreateKey("android.lens.focusDistance", ECLSID_CFloat, EIID_IFloat);

//android.util.Pair<Float,Float>>
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_FOCUS_RANGE =
    CreateKey("android.lens.focusRange", ECLSID_CPair, EIID_IPair,
        ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_OPTICAL_STABILIZATION_MODE =
    CreateKey("android.lens.opticalStabilizationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LENS_STATE =
    CreateKey("android.lens.state", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::NOISE_REDUCTION_MODE =
    CreateKey("android.noiseReduction.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::QUIRKS_PARTIAL_RESULT =
    CreateKey("android.quirks.partialResult", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_FRAME_COUNT =
    CreateKey("android.request.frameCount", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_ID =
    CreateKey("android.request.id", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::REQUEST_PIPELINE_DEPTH =
    CreateKey("android.request.pipelineDepth", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SCALER_CROP_REGION =
    CreateKey("android.scaler.cropRegion", ECLSID_CRect, EIID_IRect);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_EXPOSURE_TIME =
    CreateKey("android.sensor.exposureTime", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_FRAME_DURATION =
    CreateKey("android.sensor.frameDuration", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_SENSITIVITY =
    CreateKey("android.sensor.sensitivity", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TIMESTAMP =
    CreateKey("android.sensor.timestamp", ECLSID_CInteger64, EIID_IInteger64);

//Rational[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_NEUTRAL_COLOR_POINT =
    CreateKey("android.sensor.neutralColorPoint", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CRational, EIID_IRational);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_NOISE_PROFILE =
    CreateKey("android.sensor.noiseProfile", ECLSID_CPair, EIID_IPair,
        ECLSID_CDouble, EIID_IDouble);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_GREEN_SPLIT =
    CreateKey("android.sensor.greenSplit", ECLSID_CFloat, EIID_IFloat);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TEST_PATTERN_DATA =
    CreateKey("android.sensor.testPatternData", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_TEST_PATTERN_MODE =
    CreateKey("android.sensor.testPatternMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SENSOR_ROLLING_SHUTTER_SKEW =
    CreateKey("android.sensor.rollingShutterSkew", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SHADING_MODE =
    CreateKey("android.shading.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_DETECT_MODE =
    CreateKey("android.statistics.faceDetectMode", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_IDS =
    CreateKey("android.statistics.faceIds", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_LANDMARKS =
    CreateKey("android.statistics.faceLandmarks", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//android.graphics.Rect[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_RECTANGLES =
    CreateKey("android.statistics.faceRectangles", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CRect, EIID_IRect);

//byte[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACE_SCORES =
    CreateKey("android.statistics.faceScores", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CByte, EIID_IByte);

//android.hardware.camera2.params.Face[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_FACES =
    CreateKey("android.statistics.faces", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFace, EIID_IFace);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_CORRECTION_MAP =
    CreateKey("android.statistics.lensShadingCorrectionMap", ECLSID_CLensShadingMap, EIID_ILensShadingMap);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_MAP =
    CreateKey("android.statistics.lensShadingMap", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_PREDICTED_COLOR_GAINS =
    CreateKey("android.statistics.predictedColorGains", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//Rational[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_PREDICTED_COLOR_TRANSFORM =
    CreateKey("android.statistics.predictedColorTransform", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CRational, EIID_IRational);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_SCENE_FLICKER =
    CreateKey("android.statistics.sceneFlicker", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_HOT_PIXEL_MAP_MODE =
    CreateKey("android.statistics.hotPixelMapMode", ECLSID_CBoolean, EIID_IBoolean);

//android.graphics.Point[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_HOT_PIXEL_MAP =
    CreateKey("android.statistics.hotPixelMap", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CPoint, EIID_IPoint);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::STATISTICS_LENS_SHADING_MAP_MODE =
    CreateKey("android.statistics.lensShadingMapMode", ECLSID_CInteger32, EIID_IInteger32);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_BLUE =
    CreateKey("android.tonemap.curveBlue", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_GREEN =
    CreateKey("android.tonemap.curveGreen", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE_RED =
    CreateKey("android.tonemap.curveRed", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_CURVE =
    CreateKey("android.tonemap.curve", ECLSID_CTonemapCurve, EIID_ITonemapCurve);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::TONEMAP_MODE =
    CreateKey("android.tonemap.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::LED_TRANSMIT =
    CreateKey("android.led.transmit", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::BLACK_LEVEL_LOCK =
    CreateKey("android.blackLevel.lock", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureResultKey> CaptureResult::SYNC_FRAME_NUMBER =
    CreateKey("android.sync.frameNumber", ECLSID_CInteger64, EIID_IInteger64);


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
        Logger::E(TAG, "results was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (parent == NULL) {
        //throw new IllegalArgumentException("parent was null");
        Logger::E(TAG, "parent was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (extras == NULL) {
        //throw new IllegalArgumentException("extras was null");
        Logger::E(TAG, "extras was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CameraMetadataNative::Move(results, (ICameraMetadataNative**)&mResults);
    Boolean res;
    mResults->IsEmpty(&res);
    if (res) {
        //throw new AssertionError("Results must not be empty");
        Logger::E(TAG, "Results must not be empty");
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
        Logger::E(TAG, "results was null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    CameraMetadataNative::Move(results, (ICameraMetadataNative**)&mResults);
    Boolean res;
    mResults->IsEmpty(&res);
    if (res) {
        //throw new AssertionError("Results must not be empty");
        Logger::E(TAG, "Results must not be empty");
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
        Logger::V(TAG, "#get for Key = %s, returned value = %s",name.string(), str.string());
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