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

#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CCameraCharacteristicsKey.h"
#include "elastos/droid/hardware/camera2/CameraMetadata.h"
#include "elastos/droid/hardware/camera2/impl/CameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/utils/TypeReference.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Utils::TypeReference;
using Elastos::Droid::Hardware::Camera2::Utils::ITypeReference;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CBlackLevelPattern;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CHighSpeedVideoConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IBlackLevelPattern;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IHighSpeedVideoConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IStreamConfigurationDuration;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Graphics::EIID_IRect;
using Elastos::Droid::Utility::ECLSID_CRational;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Droid::Utility::ECLSID_CSizeF;
using Elastos::Droid::Utility::ECLSID_CRange;
using Elastos::Droid::Utility::EIID_IRational;
using Elastos::Droid::Utility::EIID_ISize;
using Elastos::Droid::Utility::EIID_ISizeF;
using Elastos::Droid::Utility::EIID_IRange;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::EIID_IString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;

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
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    mKey = new CameraMetadataNativeKey(name, classId, interfaceId);
    return NOERROR;
}

ECode CameraCharacteristicsKey::constructor(
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

ECode CameraCharacteristicsKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNativeKey(name, typeReference);
    return NOERROR;
}

ECode CameraCharacteristicsKey::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
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

static AutoPtr<ICameraCharacteristicsKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    AutoPtr<ICameraCharacteristicsKey> key;
    CCameraCharacteristicsKey::New(String(name), classId, interfaceId, (ICameraCharacteristicsKey**)&key);
    return key;
}

static AutoPtr<ICameraCharacteristicsKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ const ClassID& componentClassId,
    /* [in] */ const InterfaceID& componentInterfaceId)
{
    AutoPtr<ICameraCharacteristicsKey> key;
    CCameraCharacteristicsKey::New(String(name), classId, interfaceId,
        componentClassId, componentInterfaceId, (ICameraCharacteristicsKey**)&key);
    return key;
}

//android.util.Range<Integer>[]
static AutoPtr<ICameraCharacteristicsKey> CreateCONTROL_AE_AVAILABLE_TARGET_FPS_RANGES()
{
    AutoPtr<ITypeReference> component = TypeReference::CreateSpecializedTypeReference(
        ECLSID_CRange, EIID_IRange, ECLSID_CInteger32, EIID_IInteger32);
    AutoPtr<ITypeReference> ref = TypeReference::CreateSpecializedTypeReference(
        ECLSID_CArrayOf, EIID_IArrayOf, component);

    AutoPtr<ICameraCharacteristicsKey> key;
    CCameraCharacteristicsKey::New(
        String("android.control.aeAvailableTargetFpsRanges"),
        ref, (ICameraCharacteristicsKey**)&key);
    return key;
}

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES =
    CreateKey("android.colorCorrection.availableAberrationModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_ANTIBANDING_MODES =
    CreateKey("android.control.aeAvailableAntibandingModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_MODES =
    CreateKey("android.control.aeAvailableModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//android.util.Range<Integer>[]
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES =
    CreateCONTROL_AE_AVAILABLE_TARGET_FPS_RANGES();

//android.util.Range<Integer>
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_COMPENSATION_RANGE =
    CreateKey("android.control.aeCompensationRange", ECLSID_CRange, EIID_IRange,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AE_COMPENSATION_STEP =
    CreateKey("android.control.aeCompensationStep", ECLSID_CRational, EIID_IRational);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES =
    CreateKey("android.control.afAvailableModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_EFFECTS =
    CreateKey("android.control.availableEffects", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_SCENE_MODES =
    CreateKey("android.control.availableSceneModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES =
    CreateKey("android.control.availableVideoStabilizationModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AWB_AVAILABLE_MODES =
    CreateKey("android.control.awbAvailableModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS =
    CreateKey("android.control.maxRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AE =
    CreateKey("android.control.maxRegionsAe", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AWB =
    CreateKey("android.control.maxRegionsAwb", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_MAX_REGIONS_AF =
    CreateKey("android.control.maxRegionsAf", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.HighSpeedVideoConfiguration[].class)
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS =
    CreateKey("android.control.availableHighSpeedVideoConfigurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CHighSpeedVideoConfiguration, EIID_IHighSpeedVideoConfiguration);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::EDGE_AVAILABLE_EDGE_MODES =
    CreateKey("android.edge.availableEdgeModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::FLASH_INFO_AVAILABLE =
    CreateKey("android.flash.info.available", ECLSID_CBoolean, EIID_IBoolean);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::HOT_PIXEL_AVAILABLE_HOT_PIXEL_MODES =
    CreateKey("android.hotPixel.availableHotPixelModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::JPEG_AVAILABLE_THUMBNAIL_SIZES =
    CreateKey("android.jpeg.availableThumbnailSizes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CSize, EIID_ISize);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_APERTURES =
    CreateKey("android.lens.info.availableApertures", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_FILTER_DENSITIES =
    CreateKey("android.lens.info.availableFilterDensities", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_FOCAL_LENGTHS =
    CreateKey("android.lens.info.availableFocalLengths", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_AVAILABLE_OPTICAL_STABILIZATION =
    CreateKey("android.lens.info.availableOpticalStabilization", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_HYPERFOCAL_DISTANCE =
    CreateKey("android.lens.info.hyperfocalDistance", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE =
    CreateKey("android.lens.info.minimumFocusDistance", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_SHADING_MAP_SIZE =
    CreateKey("android.lens.info.shadingMapSize", ECLSID_CSize, EIID_ISize);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_INFO_FOCUS_DISTANCE_CALIBRATION =
    CreateKey("android.lens.info.focusDistanceCalibration", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LENS_FACING =
    CreateKey("android.lens.facing", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES =
    CreateKey("android.noiseReduction.availableNoiseReductionModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::QUIRKS_USE_PARTIAL_RESULT =
    CreateKey("android.quirks.usePartialResult", ECLSID_CByte, EIID_IByte);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_STREAMS =
    CreateKey("android.request.maxNumOutputStreams", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_RAW =
    CreateKey("android.request.maxNumOutputRaw", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC =
    CreateKey("android.request.maxNumOutputProc", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC_STALLING =
    CreateKey("android.request.maxNumOutputProcStalling", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_MAX_NUM_INPUT_STREAMS =
    CreateKey("android.request.maxNumInputStreams", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_PIPELINE_MAX_DEPTH =
    CreateKey("android.request.pipelineMaxDepth", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_PARTIAL_RESULT_COUNT =
    CreateKey("android.request.partialResultCount", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_CAPABILITIES =
    CreateKey("android.request.availableCapabilities", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_REQUEST_KEYS =
    CreateKey("android.request.availableRequestKeys", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_RESULT_KEYS =
    CreateKey("android.request.availableResultKeys", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::REQUEST_AVAILABLE_CHARACTERISTICS_KEYS =
    CreateKey("android.request.availableCharacteristicsKeys", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_FORMATS =
    CreateKey("android.scaler.availableFormats", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//long[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_JPEG_MIN_DURATIONS =
    CreateKey("android.scaler.availableJpegMinDurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger64, EIID_IInteger64);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_JPEG_SIZES =
    CreateKey("android.scaler.availableJpegSizes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CSize, EIID_ISize);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_MAX_DIGITAL_ZOOM =
    CreateKey("android.scaler.availableMaxDigitalZoom", ECLSID_CFloat, EIID_IFloat);

//long[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_PROCESSED_MIN_DURATIONS =
    CreateKey("android.scaler.availableProcessedMinDurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger64, EIID_IInteger64);

//android.util.Size[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_PROCESSED_SIZES =
    CreateKey("android.scaler.availableProcessedSizes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CSize, EIID_ISize);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_INPUT_OUTPUT_FORMATS_MAP =
    CreateKey("android.scaler.availableInputOutputFormatsMap", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.StreamConfiguration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_STREAM_CONFIGURATIONS =
    CreateKey("android.scaler.availableStreamConfigurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CStreamConfiguration, EIID_IStreamConfiguration);

//android.hardware.camera2.params.StreamConfigurationDuration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_MIN_FRAME_DURATIONS =
    CreateKey("android.scaler.availableMinFrameDurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CStreamConfigurationDuration, EIID_IStreamConfigurationDuration);

//android.hardware.camera2.params.StreamConfigurationDuration[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_AVAILABLE_STALL_DURATIONS =
    CreateKey("android.scaler.availableStallDurations", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CStreamConfigurationDuration, EIID_IStreamConfigurationDuration);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP =
    CreateKey("android.scaler.streamConfigurationMap", ECLSID_CStreamConfigurationMap, EIID_IStreamConfigurationMap);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SCALER_CROPPING_TYPE =
    CreateKey("android.scaler.croppingType", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE =
    CreateKey("android.sensor.info.activeArraySize", ECLSID_CRect, EIID_IRect);

//android.util.Range<Integer>
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_SENSITIVITY_RANGE =
    CreateKey("android.sensor.info.sensitivityRange", ECLSID_CRange, EIID_IRange,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_COLOR_FILTER_ARRANGEMENT =
    CreateKey("android.sensor.info.colorFilterArrangement", ECLSID_CInteger32, EIID_IInteger32);

//android.util.Range<Long>
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_EXPOSURE_TIME_RANGE =
    CreateKey("android.sensor.info.exposureTimeRange", ECLSID_CRange, EIID_IRange,
        ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_MAX_FRAME_DURATION =
    CreateKey("android.sensor.info.maxFrameDuration", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_PHYSICAL_SIZE =
    CreateKey("android.sensor.info.physicalSize", ECLSID_CSizeF, EIID_ISizeF);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_PIXEL_ARRAY_SIZE =
    CreateKey("android.sensor.info.pixelArraySize", ECLSID_CSize, EIID_ISize);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_WHITE_LEVEL =
    CreateKey("android.sensor.info.whiteLevel", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_INFO_TIMESTAMP_SOURCE =
    CreateKey("android.sensor.info.timestampSource", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_REFERENCE_ILLUMINANT1 =
    CreateKey("android.sensor.referenceIlluminant1", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_REFERENCE_ILLUMINANT2 =
    CreateKey("android.sensor.referenceIlluminant2", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_CALIBRATION_TRANSFORM1 =
    CreateKey("android.sensor.calibrationTransform1", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_CALIBRATION_TRANSFORM2 =
    CreateKey("android.sensor.calibrationTransform2", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_COLOR_TRANSFORM1 =
    CreateKey("android.sensor.colorTransform1", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_COLOR_TRANSFORM2 =
    CreateKey("android.sensor.colorTransform2", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_FORWARD_MATRIX1 =
    CreateKey("android.sensor.forwardMatrix1", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_FORWARD_MATRIX2 =
    CreateKey("android.sensor.forwardMatrix2", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_BLACK_LEVEL_PATTERN =
    CreateKey("android.sensor.blackLevelPattern", ECLSID_CBlackLevelPattern, EIID_IBlackLevelPattern);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_MAX_ANALOG_SENSITIVITY =
    CreateKey("android.sensor.maxAnalogSensitivity", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_ORIENTATION =
    CreateKey("android.sensor.orientation", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SENSOR_AVAILABLE_TEST_PATTERN_MODES =
    CreateKey("android.sensor.availableTestPatternModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES =
    CreateKey("android.statistics.info.availableFaceDetectModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_MAX_FACE_COUNT =
    CreateKey("android.statistics.info.maxFaceCount", ECLSID_CInteger32, EIID_IInteger32);

//boolean[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::STATISTICS_INFO_AVAILABLE_HOT_PIXEL_MAP_MODES =
    CreateKey("android.statistics.info.availableHotPixelMapModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::TONEMAP_MAX_CURVE_POINTS =
    CreateKey("android.tonemap.maxCurvePoints", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::TONEMAP_AVAILABLE_TONE_MAP_MODES =
    CreateKey("android.tonemap.availableToneMapModes", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::LED_AVAILABLE_LEDS =
    CreateKey("android.led.availableLeds", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::INFO_SUPPORTED_HARDWARE_LEVEL =
    CreateKey("android.info.supportedHardwareLevel", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICameraCharacteristicsKey> CameraCharacteristics::SYNC_MAX_LATENCY =
    CreateKey("android.sync.maxLatency", ECLSID_CInteger32, EIID_IInteger32);

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
        Logger::E("CameraCharacteristics", "android.request.availableCharacteristicsKeys must be non-null"
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
            Logger::E("CameraCharacteristics", "android.request.availableRequestKeys must be non-null "
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
            Logger::E("CameraCharacteristics", "android.request.availableResultKeys must be non-null "
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
    //     Logger::E("CameraCharacteristics", "metadataClass must be a strict subclass of CameraMetadata");
    //     return E_ASSERTION_ERROR;
    // }
    // else if (!CameraMetadata.class.isAssignableFrom(metadataClass)) {
    //     // throw new AssertionError(
    //     //         "metadataClass must be a subclass of CameraMetadata");
    //     Logger::E("CameraCharacteristics", "metadataClass must be a subclass of CameraMetadata");
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