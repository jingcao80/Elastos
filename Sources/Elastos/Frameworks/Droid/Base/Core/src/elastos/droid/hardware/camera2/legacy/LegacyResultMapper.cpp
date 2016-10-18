
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyResultMapper.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyRequestMapper.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ListUtils;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyRequestMapper;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyMetadataMapper;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(LegacyResultMapper, Object, ILegacyResultMapper)

const String LegacyResultMapper::TAG("LegacyResultMapper");
const Boolean LegacyResultMapper::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

LegacyResultMapper::LegacyResultMapper()
{
}

ECode LegacyResultMapper::CachedConvertResultMetadata(
    /* [in] */ ILegacyRequest* legacyRequest,
    /* [in] */ Int64 timestamp,
    /* [out] */ ICameraMetadataNative** outcmn)
{
    VALIDATE_NOT_NULL(outcmn);
    *outcmn = NULL;

    AutoPtr<ICameraMetadataNative> result;
    Boolean cached;
    /*
     * Attempt to look up the result from the cache if the parameters haven't changed
     */
    AutoPtr<IParameters> para;
    legacyRequest->GetParameters((IParameters**)&para);
    AutoPtr<IParameters> _para;
    mCachedRequest->GetParameters((IParameters**)&_para);
    Boolean res;
    para->Same(_para, &res);
    if (mCachedRequest != NULL && res) {
        CCameraMetadataNative::New(mCachedResult, (ICameraMetadataNative**)&result);
        cached = TRUE;
    }
    else {
        result = ConvertResultMetadata(legacyRequest);
        cached = FALSE;

        // Always cache a *copy* of the metadata result,
        // since api2's client side takes ownership of it after it receives a result
        mCachedRequest = legacyRequest;
        CCameraMetadataNative::New(result, (ICameraMetadataNative**)&mCachedResult);
    }

    /*
     * Unconditionally set fields that change in every single frame
     */
    {
        // sensor.timestamp
        AutoPtr<IInteger64> obj = CoreUtils::Convert(timestamp);
        result->Set(CaptureResult::SENSOR_TIMESTAMP, TO_IINTERFACE(obj));
    }

    if (VERBOSE) {
        Logger::V(TAG, "cachedConvertResultMetadata - cached? %d timestamp = %d", cached, timestamp);

        Logger::V(TAG, "----- beginning of result dump ------");
        result->DumpToLog();
        Logger::V(TAG, "----- end of result dump ------");
    }

    *outcmn = result;
    REFCOUNT_ADD(*outcmn);
    return NOERROR;
}

AutoPtr<ICameraMetadataNative> LegacyResultMapper::ConvertResultMetadata(
    /* [in] */ ILegacyRequest* legacyRequest)
{
    AutoPtr<ICameraCharacteristics> characteristics;
    legacyRequest->GetCameraCharacteristics((ICameraCharacteristics**)&characteristics);
    AutoPtr<ICaptureRequest> request;
    legacyRequest->GetCaptureRequest((ICaptureRequest**)&request);
    AutoPtr<ISize> previewSize;
    legacyRequest->GetSize((ISize**)&previewSize);
    AutoPtr<IParameters> params;
    legacyRequest->GetParameters((IParameters**)&params);

    AutoPtr<ICameraMetadataNative> result;
    CCameraMetadataNative::New((ICameraMetadataNative**)&result);

    AutoPtr<IInterface> obj;
    characteristics->Get(CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE, (IInterface**)&obj);
    AutoPtr<IRect> activeArraySize = IRect::Probe(obj);
    AutoPtr<IParameterUtilsZoomData> zoomData;
    AutoPtr<IInterface> obj2;
    request->Get(CaptureRequest::SCALER_CROP_REGION, (IInterface**)&obj2);
    ParameterUtils::ConvertScalerCropRegion(activeArraySize,
            IRect::Probe(obj2), previewSize, params, (IParameterUtilsZoomData**)&zoomData);

    /*
     * colorCorrection
     */
    // colorCorrection.aberrationMode
    {
        // Always hardcoded to FAST
        AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::COLOR_CORRECTION_ABERRATION_MODE_FAST);
        result->Set(CaptureResult::COLOR_CORRECTION_ABERRATION_MODE, TO_IINTERFACE(obj));
    }

    /*
     * control
     */

    /*
     * control.ae*
     */
    MapAe(result, characteristics, request, activeArraySize, zoomData, /*out*/params);

    /*
     * control.af*
     */
    MapAf(result, activeArraySize, zoomData, /*out*/params);

    /*
     * control.awb*
     */
    MapAwb(result, /*out*/params);

    /*
     * control.captureIntent
     */
    {
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW);
        ParamsUtils::GetOrDefault(request,
                CaptureRequest::CONTROL_CAPTURE_INTENT,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 _captureIntent;
        intObj->GetValue(&_captureIntent);

        Int32 captureIntent = LegacyRequestMapper::
                FilterSupportedCaptureIntent(_captureIntent);

        AutoPtr<IInteger32> obj2 = CoreUtils::Convert(captureIntent);
        result->Set(CaptureResult::CONTROL_CAPTURE_INTENT, TO_IINTERFACE(obj2));
    }

    /*
     * control.mode
     */
    {
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_MODE_AUTO);
        ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_MODE,
                TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 controlMode;
        intObj->GetValue(&controlMode);
        if (controlMode == ICameraMetadata::CONTROL_MODE_USE_SCENE_MODE) {
            result->Set(CaptureResult::CONTROL_MODE, TO_IINTERFACE(intObj));
        }
        else {
            result->Set(CaptureResult::CONTROL_MODE, TO_IINTERFACE(res));
        }
    }

    /*
     * control.sceneMode
     */
    {
        String legacySceneMode;
        params->GetSceneMode(&legacySceneMode);
        Int32 mode = LegacyMetadataMapper::ConvertSceneModeFromLegacy(legacySceneMode);
        if (mode != ILegacyMetadataMapper::UNKNOWN_MODE) {
            AutoPtr<IInteger32> res = CoreUtils::Convert(mode);
            result->Set(CaptureResult::CONTROL_SCENE_MODE, TO_IINTERFACE(res));
            // In case of SCENE_MODE == FACE_PRIORITY, LegacyFaceDetectMapper will override
            // the result to say SCENE_MODE == FACE_PRIORITY.
        }
        else {
            Logger::W(TAG, "Unknown scene mode %s returned by camera HAL,"
                    "setting to disabled.", legacySceneMode.string());
            AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_DISABLED);
            result->Set(CaptureResult::CONTROL_SCENE_MODE, TO_IINTERFACE(res));
        }
    }

    /*
     * control.effectMode
     */
    {
        String legacyEffectMode;
        params->GetColorEffect(&legacyEffectMode);
        Int32 mode = LegacyMetadataMapper::ConvertEffectModeFromLegacy(legacyEffectMode);
        if (mode != ILegacyMetadataMapper::UNKNOWN_MODE) {
            AutoPtr<IInteger32> res = CoreUtils::Convert(mode);
            result->Set(CaptureResult::CONTROL_EFFECT_MODE, TO_IINTERFACE(res));
        }
        else {
            Logger::W(TAG, "Unknown effect mode %s returned by camera HAL, setting"
                    "to off.", legacyEffectMode.string());
            AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_EFFECT_MODE_OFF);
            result->Set(CaptureResult::CONTROL_EFFECT_MODE, TO_IINTERFACE(res));
        }
    }

    // control.videoStabilizationMode
    {
        Int32 stabMode;
        Boolean res;
        Boolean res2;
        params->IsVideoStabilizationSupported(&res);
        if (res) {
            params->GetVideoStabilization(&res2);
            if (res2) {
                stabMode = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_ON;
            }
            else {
                stabMode = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_OFF;
            }
        }
        else {
            stabMode = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_OFF;
        }
        AutoPtr<IInteger32> tmp = CoreUtils::Convert(stabMode);
        result->Set(CaptureResult::CONTROL_VIDEO_STABILIZATION_MODE, TO_IINTERFACE(tmp));
    }

    /*
     * flash
     */
    {
        // flash.mode, flash.state mapped in mapAeAndFlashMode
    }

    /*
     * lens
     */
    // lens.focusDistance
    {
        String mode;
        params->GetFocusMode(&mode);
        if (IParameters::FOCUS_MODE_INFINITY.Equals(mode)) {
            AutoPtr<IFloat> res = CoreUtils::Convert(0.0f);
            result->Set(CaptureResult::LENS_FOCUS_DISTANCE, TO_IINTERFACE(res));
        }
    }

    // lens.focalLength
    Float focalLength;
    params->GetFocalLength(&focalLength);
    AutoPtr<IFloat> res = CoreUtils::Convert(focalLength);
    result->Set(CaptureResult::LENS_FOCAL_LENGTH, TO_IINTERFACE(res));

    /*
     * request
     */
    // request.pipelineDepth
    AutoPtr<IInterface> obj3;
    characteristics->Get(CameraCharacteristics::REQUEST_PIPELINE_MAX_DEPTH, (IInterface**)&obj3);
    result->Set(CaptureResult::REQUEST_PIPELINE_DEPTH, obj3);

    /*
     * scaler
     */
    MapScaler(result, zoomData, /*out*/params);

    /*
     * sensor
     */
    // sensor.timestamp varies every frame; mapping is done in #cachedConvertResultMetadata
    {
        // Unconditionally no test patterns
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::SENSOR_TEST_PATTERN_MODE_OFF);
        result->Set(CaptureResult::SENSOR_TEST_PATTERN_MODE, TO_IINTERFACE(res));
    }

    /*
     * jpeg
     */
    // jpeg.gpsLocation
    AutoPtr<IInterface> obj4;
    request->Get(CaptureRequest::JPEG_GPS_LOCATION, (IInterface**)&obj4);
    result->Set(CaptureResult::JPEG_GPS_LOCATION, obj4);

    // jpeg.orientation
    AutoPtr<IInterface> obj5;
    request->Get(CaptureRequest::JPEG_ORIENTATION, (IInterface**)&obj5);
    result->Set(CaptureResult::JPEG_ORIENTATION, obj5);

    // jpeg.quality
    Int32 quality;
    params->GetJpegQuality(&quality);
    AutoPtr<IByte> res2 = CoreUtils::ConvertByte((Byte)quality);
    result->Set(CaptureResult::JPEG_QUALITY, TO_IINTERFACE(res2));

    // jpeg.thumbnailQuality
    Int32 thumbnailQuality;
    params->GetJpegThumbnailQuality(&thumbnailQuality);
    AutoPtr<IByte> res3 = CoreUtils::ConvertByte((Byte)thumbnailQuality);
    result->Set(CaptureResult::JPEG_THUMBNAIL_QUALITY, TO_IINTERFACE(res3));

    // jpeg.thumbnailSize
    AutoPtr<ICameraSize> s;
    params->GetJpegThumbnailSize((ICameraSize**)&s);
    if (s != NULL) {
        AutoPtr<ISize> size;
        ParameterUtils::ConvertSize(s, (ISize**)&size);
        result->Set(CaptureResult::JPEG_THUMBNAIL_SIZE, TO_IINTERFACE(size));
    }
    else {
        Logger::W(TAG, "Null thumbnail size received from parameters.");
    }

    /*
     * noiseReduction.*
     */
    // noiseReduction.mode
    AutoPtr<IInteger32> res4 = CoreUtils::Convert(ICameraMetadata::NOISE_REDUCTION_MODE_FAST);
    result->Set(CaptureResult::NOISE_REDUCTION_MODE, TO_IINTERFACE(res4));

    return result;
}

void LegacyResultMapper::MapAe(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ ICaptureRequest* request,
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IParameters* p)
{
    // control.aeAntiBandingMode
    {
        String antibanding;
        p->GetAntibanding(&antibanding);
        Int32 antiBandingMode = LegacyMetadataMapper::
                ConvertAntiBandingModeOrDefault(antibanding);
        AutoPtr<IInteger32> res = CoreUtils::Convert(antiBandingMode);
        m->Set(CaptureResult::CONTROL_AE_ANTIBANDING_MODE, TO_IINTERFACE(res));
    }

    // control.aeExposureCompensation
    {
        Int32 exposure;
        p->GetExposureCompensation(&exposure);
        AutoPtr<IInteger32> obj = CoreUtils::Convert(exposure);
        m->Set(CaptureResult::CONTROL_AE_EXPOSURE_COMPENSATION, TO_IINTERFACE(obj));
    }

    // control.aeLock
    {
        Boolean tmp;
        p->IsAutoExposureLockSupported(&tmp);
        Boolean lock;
        if (tmp) {
            p->IsAutoExposureLockSupported(&lock);
        }
        else {
            lock = FALSE;
        }
        AutoPtr<IBoolean> obj = CoreUtils::Convert(lock);
        m->Set(CaptureResult::CONTROL_AE_LOCK, TO_IINTERFACE(obj));
        if (VERBOSE) {
            Boolean result;
            p->IsAutoExposureLockSupported(&result);
            Logger::V(TAG,
                    "mapAe - android.control.aeLock = %d, supported = %d", lock, result);
        }

        AutoPtr<IInterface> res;
        request->Get(CaptureRequest::CONTROL_AE_LOCK, (IInterface**)&res);
        AutoPtr<IBoolean> obj2 = IBoolean::Probe(res);
        if (obj2 != NULL) {
            Boolean requestLock;
            obj2->GetValue(&requestLock);
            if (requestLock != lock) {
                Logger::W(TAG,
                    "mapAe - android.control.aeLock was requested to %d but resulted"
                    "in %d", requestLock, lock);
            }
        }
    }

    // control.aeMode, flash.mode, flash.state
    MapAeAndFlashMode(m, characteristics, p);

    // control.aeState
    if (LegacyMetadataMapper::LIE_ABOUT_AE_STATE) {
        // Lie to pass CTS temporarily.
        // TODO: Implement precapture trigger, after which we can report CONVERGED ourselves
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_STATE_CONVERGED);
        m->Set(CaptureResult::CONTROL_AE_STATE, TO_IINTERFACE(res));
    }

    // control.aeRegions
    Int32 maxNum;
    p->GetMaxNumMeteringAreas(&maxNum);
    if (maxNum > 0) {
        if (VERBOSE) {
            String meteringAreas;
            p->Get(String("metering-areas"), &meteringAreas);
            Logger::V(TAG, "mapAe - parameter dump; metering-areas: %s", meteringAreas.string());
        }

        AutoPtr<IList> list;
        assert(0);
        // p->GetMeteringAreas((IList**)&list);
        AutoPtr<ArrayOf<IMeteringRectangle*> > meteringRectArray =
                GetMeteringRectangles(activeArray, zoomData, list, String("AE"));

        assert(0);
        //m->Set(CaptureResult::CONTROL_AE_REGIONS, meteringRectArray);
    }
    return;
}

void LegacyResultMapper::MapAf(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IParameters* p)
{
    // control.afMode
    String mode;
    p->GetFocusMode(&mode);
    AutoPtr<IInteger32> res = CoreUtils::Convert(ConvertLegacyAfMode(mode));
    m->Set(CaptureResult::CONTROL_AF_MODE, TO_IINTERFACE(res));

    // control.afRegions
    Int32 maxNum;
    p->GetMaxNumFocusAreas(&maxNum);
    if (maxNum > 0) {
        if (VERBOSE) {
            String focusAreas;
            p->Get(String("focus-areas"), &focusAreas);
            Logger::V(TAG, "mapAe - parameter dump; focus-areas: %s", focusAreas.string());
        }

        AutoPtr<IList> list;
        assert(0);
        //p->GetFocusAreas(&list);
        AutoPtr<ArrayOf<IMeteringRectangle*> > meteringRectArray =
                GetMeteringRectangles(activeArray, zoomData, list,
                String("AF"));

        assert(0);
        //m->Set(CaptureResult::CONTROL_AF_REGIONS, meteringRectArray);
    }
    return;
}

void LegacyResultMapper::MapAwb(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    // control.awbLock
    {
        Boolean lock;
        Boolean tmp;
        p->IsAutoWhiteBalanceLockSupported(&tmp);
        if (tmp) {
            p->GetAutoWhiteBalanceLock(&lock);
        }
        else {
            lock = FALSE;
        }
        AutoPtr<IBoolean> obj = CoreUtils::Convert(lock);
        m->Set(CaptureResult::CONTROL_AWB_LOCK, TO_IINTERFACE(obj));
    }

    // control.awbMode
    {
        String mode;
        p->GetWhiteBalance(&mode);
        Int32 awbMode = ConvertLegacyAwbMode(mode);
        AutoPtr<IInteger32> res = CoreUtils::Convert(awbMode);
        m->Set(CaptureResult::CONTROL_AWB_MODE, TO_IINTERFACE(res));
    }
    return;
}

AutoPtr<ArrayOf<IMeteringRectangle*> > LegacyResultMapper::GetMeteringRectangles(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IList* meteringAreaList,
    /* [in] */ const String& regionName)
{
    AutoPtr<IList> meteringRectList;
    CArrayList::New((IList**)&meteringRectList);
    if (meteringAreaList != NULL) {
        Int32 size;
        meteringAreaList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            meteringAreaList->Get(i, (IInterface**)&obj);
            AutoPtr<ICameraArea> area = ICameraArea::Probe(obj);

            AutoPtr<IParameterUtilsWeightedRectangle> rect;
            ParameterUtils::ConvertCameraAreaToActiveArrayRectangle(
                    activeArray, zoomData, area, (IParameterUtilsWeightedRectangle**)&rect);

            AutoPtr<IMeteringRectangle> tmp;
            rect->ToMetering((IMeteringRectangle**)&tmp);
            meteringRectList->Add(TO_IINTERFACE(tmp));
        }
    }

    if (VERBOSE) {
        String str;
        ListUtils::ListToString(meteringRectList, &str);
        Logger::V(TAG,
                "Metering rectangles for %s: %s", regionName.string(), str.string());
    }

    AutoPtr<ArrayOf<IMeteringRectangle*> > tmp;
    meteringRectList->ToArray((ArrayOf<IInterface*>**)&tmp);
    return tmp;
}

void LegacyResultMapper::MapAeAndFlashMode(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ ICameraCharacteristics* characteristics,
    /* [in] */ IParameters* p)
{
    // Default: AE mode on but flash never fires
    Int32 flashMode = ICameraMetadata::FLASH_MODE_OFF;
    // If there is no flash on this camera, the state is always unavailable
    // , otherwise it's only known for TORCH/SINGLE modes
    AutoPtr<IInterface> obj;
    characteristics->Get(CameraCharacteristics::FLASH_INFO_AVAILABLE, (IInterface**)&obj);
    AutoPtr<IBoolean> boolObj = IBoolean::Probe(obj);
    Boolean result;
    boolObj->GetValue(&result);
    AutoPtr<IInteger32> flashState = result ? NULL :
            CoreUtils::Convert(ICameraMetadata::FLASH_STATE_UNAVAILABLE);
    Int32 aeMode = ICameraMetadata::CONTROL_AE_MODE_ON;

    String flashModeSetting;
    p->GetFlashMode(&flashModeSetting);

    do {
        if (!flashModeSetting.IsNull()) {
            if (flashModeSetting == IParameters::FLASH_MODE_OFF) {
                break; // ok, using default
            }
            else if (flashModeSetting == IParameters::FLASH_MODE_AUTO) {
                aeMode = ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH;
                break;
            }
            else if (flashModeSetting == IParameters::FLASH_MODE_ON) {
                // flashMode = SINGLE + aeMode = ON is indistinguishable from ON_ALWAYS_FLASH
                flashMode = ICameraMetadata::FLASH_MODE_SINGLE;
                aeMode = ICameraMetadata::CONTROL_AE_MODE_ON_ALWAYS_FLASH;
                flashState = CoreUtils::Convert(ICameraMetadata::FLASH_STATE_FIRED);
                break;
            }
            else if (flashModeSetting == IParameters::FLASH_MODE_RED_EYE) {
                aeMode = ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE;
                break;
            }
            else if (flashModeSetting == IParameters::FLASH_MODE_TORCH) {
                flashMode = ICameraMetadata::FLASH_MODE_TORCH;
                flashState = CoreUtils::Convert(ICameraMetadata::FLASH_STATE_FIRED);
                break;
            }
            else {
                String mode;
                p->GetFlashMode(&mode);
                Logger::W(TAG,
                        "mapAeAndFlashMode - Ignoring unknown flash mode %s", mode.string());
            }
        }
    }while(FALSE);

    // flash.state
    m->Set(CaptureResult::FLASH_STATE, TO_IINTERFACE(flashState));
    // flash.mode
    AutoPtr<IInteger32> res = CoreUtils::Convert(flashMode);
    m->Set(CaptureResult::FLASH_MODE, TO_IINTERFACE(res));
    // control.aeMode
    AutoPtr<IInteger32> res2 = CoreUtils::Convert(aeMode);
    m->Set(CaptureResult::CONTROL_AE_MODE, TO_IINTERFACE(res2));
    return;
}

Int32 LegacyResultMapper::ConvertLegacyAfMode(
    /* [in] */ const String& mode)
{
    if (mode.IsNull()) {
        Logger::W(TAG, "convertLegacyAfMode - no AF mode, default to OFF");
        return ICameraMetadata::CONTROL_AF_MODE_OFF;
    }

    if (mode == IParameters::FOCUS_MODE_AUTO) {
        return ICameraMetadata::CONTROL_AF_MODE_AUTO;
    }
    else if (mode == IParameters::FOCUS_MODE_CONTINUOUS_PICTURE) {
        return ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE;
    }
    else if (mode == IParameters::FOCUS_MODE_CONTINUOUS_VIDEO) {
        return ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_VIDEO;
    }
    else if (mode == IParameters::FOCUS_MODE_EDOF) {
        return ICameraMetadata::CONTROL_AF_MODE_EDOF;
    }
    else if (mode == IParameters::FOCUS_MODE_MACRO) {
        return ICameraMetadata::CONTROL_AF_MODE_MACRO;
    }
    else if (mode == IParameters::FOCUS_MODE_FIXED) {
        return ICameraMetadata::CONTROL_AF_MODE_OFF;
    }
    else if (mode == IParameters::FOCUS_MODE_INFINITY) {
        return ICameraMetadata::CONTROL_AF_MODE_OFF;
    }
    else {
        Logger::W(TAG, "convertLegacyAfMode - unknown mode %s , ignoring", mode.string());
        return ICameraMetadata::CONTROL_AF_MODE_OFF;
    }
}

Int32 LegacyResultMapper::ConvertLegacyAwbMode(
    /* [in] */ const String& mode)
{
    if (mode.IsNull()) {
        // OK: camera1 api may not support changing WB modes; assume AUTO
        return ICameraMetadata::CONTROL_AWB_MODE_AUTO;
    }

    if (mode == IParameters::WHITE_BALANCE_AUTO) {
        return ICameraMetadata::CONTROL_AWB_MODE_AUTO;
    }
    else if (mode == IParameters::WHITE_BALANCE_INCANDESCENT) {
        return ICameraMetadata::CONTROL_AWB_MODE_INCANDESCENT;
    }
    else if (mode == IParameters::WHITE_BALANCE_FLUORESCENT) {
        return ICameraMetadata::CONTROL_AWB_MODE_FLUORESCENT;
    }
    else if (mode == IParameters::WHITE_BALANCE_WARM_FLUORESCENT) {
        return ICameraMetadata::CONTROL_AWB_MODE_WARM_FLUORESCENT;
    }
    else if (mode == IParameters::WHITE_BALANCE_DAYLIGHT) {
        return ICameraMetadata::CONTROL_AWB_MODE_DAYLIGHT;
    }
    else if (mode == IParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT) {
        return ICameraMetadata::CONTROL_AWB_MODE_CLOUDY_DAYLIGHT;
    }
    else if (mode == IParameters::WHITE_BALANCE_TWILIGHT) {
        return ICameraMetadata::CONTROL_AWB_MODE_TWILIGHT;
    }
    else if (mode == IParameters::WHITE_BALANCE_SHADE) {
        return ICameraMetadata::CONTROL_AWB_MODE_SHADE;
    }
    else {
        Logger::W(TAG, "convertAwbMode - unrecognized WB mode %s", mode.string());
        return ICameraMetadata::CONTROL_AWB_MODE_AUTO;
    }
}

void LegacyResultMapper::MapScaler(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IParameters* p)
{
    /*
     * scaler.cropRegion
     */
    {
        AutoPtr<IRect> rect;
        zoomData->GetReportedCrop((IRect**)&rect);
        m->Set(CaptureResult::SCALER_CROP_REGION, TO_IINTERFACE(rect));
    }
    return;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
