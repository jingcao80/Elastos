
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyRequestMapper.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Legacy::ParameterUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ListUtils;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Arrays;
using Elastos::Utility::Objects;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

const String LegacyRequestMapper::TAG("LegacyRequestMapper");
const Boolean LegacyRequestMapper::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

/** Default quality for android.jpeg.quality, android.jpeg.thumbnailQuality */
const Byte LegacyRequestMapper::DEFAULT_JPEG_QUALITY = 85;

ECode LegacyRequestMapper::ConvertRequestMetadata(
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

    AutoPtr<IInterface> obj;
    characteristics->Get(CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE, (IInterface**)&obj);
    AutoPtr<IRect> activeArray = IRect::Probe(obj);

    /*
     * scaler.cropRegion
     */
    AutoPtr<IParameterUtilsZoomData> zoomData;
    {
        AutoPtr<IInterface> obj;
        request->Get(CaptureRequest::SCALER_CROP_REGION, (IInterface**)&obj);
        AutoPtr<IRect> rect = IRect::Probe(obj);
        ParameterUtils::ConvertScalerCropRegion(activeArray, rect,
                previewSize, params, (IParameterUtilsZoomData**)&zoomData);

        Boolean result;
        params->IsZoomSupported(&result);
        if (result) {
            Int32 index;
            zoomData->GetZoomIndex(&index);
            params->SetZoom(index);
        }
        else if (VERBOSE) {
            Logger::V(TAG, "convertRequestToMetadata - zoom is not supported");
        }
    }

    /*
     * colorCorrection.*
     */
    // colorCorrection.aberrationMode
    {
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::COLOR_CORRECTION_ABERRATION_MODE_FAST);
        ParamsUtils::GetOrDefault(request,
                CaptureRequest::COLOR_CORRECTION_ABERRATION_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 aberrationMode;
        intObj->GetValue(&aberrationMode);

        if (aberrationMode != ICameraMetadata::COLOR_CORRECTION_ABERRATION_MODE_FAST) {
            Logger::W(TAG, "convertRequestToMetadata - Ignoring unsupported "
                    "colorCorrection.aberrationMode = %d", aberrationMode);
        }
    }

    /*
     * control.ae*
     */
    // control.aeAntibandingMode
    {
        AutoPtr<IInterface> obj;
        request->Get(CaptureRequest::CONTROL_AE_ANTIBANDING_MODE, (IInterface**)&obj);
        AutoPtr<IInteger32> antiBandingMode = IInteger32::Probe(obj);
        Int32 value;
        antiBandingMode->GetValue(&value);
        String legacyMode;
        if (antiBandingMode != NULL) {
            legacyMode = ConvertAeAntiBandingModeToLegacy(value);
        }
        else {
            AutoPtr<ArrayOf<String> > values;
            params->GetSupportedAntibanding((ArrayOf<String>**)&values);
            AutoPtr<IList> list;
            Arrays::AsList(values, (IList**)&list);
            AutoPtr<ArrayOf<IInterface*> > values2 = ArrayOf<IInterface*>::Alloc(4);
            AutoPtr<ICharSequence> charObj1 = CoreUtils::Convert(IParameters::ANTIBANDING_AUTO);
            AutoPtr<ICharSequence> charObj2 = CoreUtils::Convert(IParameters::ANTIBANDING_OFF);
            AutoPtr<ICharSequence> charObj3 = CoreUtils::Convert(IParameters::ANTIBANDING_50HZ);
            AutoPtr<ICharSequence> charObj4 = CoreUtils::Convert(IParameters::ANTIBANDING_60HZ);
            values2->Set(0, TO_IINTERFACE(charObj1));
            values2->Set(1, TO_IINTERFACE(charObj2));
            values2->Set(2, TO_IINTERFACE(charObj3));
            values2->Set(3, TO_IINTERFACE(charObj4));
            AutoPtr<IInterface> tmp;
            ListUtils::ListSelectFirstFrom(list, values2, (IInterface**)&tmp);
            AutoPtr<ICharSequence> strObj = ICharSequence::Probe(tmp);
            strObj->ToString(&legacyMode);
        }

        if (!legacyMode.IsNull()) {
            params->SetAntibanding(legacyMode);
        }
    }

    /*
     * control.aeRegions, afRegions
     */
    {
        // aeRegions
        {
            // Use aeRegions if available, fall back to using awbRegions if present
            AutoPtr<IInterface> obj;
            request->Get(CaptureRequest::CONTROL_AE_REGIONS, (IInterface**)&obj);
            AutoPtr<IArrayOf> aeRegions = IArrayOf::Probe(obj);
            AutoPtr<IInterface> obj2;
            request->Get(CaptureRequest::CONTROL_AWB_REGIONS, (IInterface**)&obj2);
            if (obj2 != NULL) {
                Logger::W(TAG, "convertRequestMetadata - control.awbRegions setting is not"
                        "supported, ignoring value");
            }
            Int32 maxNumMeteringAreas;
            params->GetMaxNumMeteringAreas(&maxNumMeteringAreas);
            AutoPtr<IList> meteringAreaList;
            meteringAreaList = ConvertMeteringRegionsToLegacy(activeArray, zoomData, aeRegions,
                maxNumMeteringAreas, /*regionName*/String("AE"));

            // WAR: for b/17252693, some devices can't handle params.setFocusAreas(null).
            assert(0);
            // AutoPtr<ArrayOf<IInterface*> > array;
            // meteringAreaList->ToArray((ArrayOf<IInterface*>**)&array);
            // if (maxNumMeteringAreas > 0) {
            //     params->SetMeteringAreas(array);
            // }
        }

        // afRegions
        {
            AutoPtr<IInterface> obj;
            request->Get(CaptureRequest::CONTROL_AF_REGIONS, (IInterface**)&obj);
            AutoPtr<IArrayOf> afRegions = IArrayOf::Probe(obj);
            Int32 maxNumFocusAreas;
            params->GetMaxNumFocusAreas(&maxNumFocusAreas);
            AutoPtr<IList> focusAreaList;
            focusAreaList = ConvertMeteringRegionsToLegacy(activeArray, zoomData,
                afRegions, maxNumFocusAreas, /*regionName*/String("AF"));

            // WAR: for b/17252693, some devices can't handle params.setFocusAreas(null).
            if (maxNumFocusAreas > 0) {
                assert(0);
                // /params->SetFocusAreas(focusAreaList);
            }
        }
    }

    // control.aeTargetFpsRange
    AutoPtr<Range<IInteger32> > aeFpsRange;
    AutoPtr<IInterface> tmp;
    assert(0);
    //request->Get(CaptureRequest::CONTROL_AE_TARGET_FPS_RANGE, (IInterface**)&tmp);
    if (tmp != NULL) {
        aeFpsRange = (Range<IInteger32>*)IObject::Probe(tmp);
        AutoPtr<ArrayOf<Int32> > legacyFps = ConvertAeFpsRangeToLegacy(aeFpsRange);

        // TODO - Should we enforce that all HAL1 devices must include (30, 30) FPS range?
        Boolean supported = FALSE;

        AutoPtr<IList> rangeList;
        params->GetSupportedPreviewFpsRange((IList**)&rangeList);


        Int32 size;
        rangeList->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> tmp;
            rangeList->Get(i, (IInterface**)&tmp);
            AutoPtr<IArrayOf> range = IArrayOf::Probe(tmp);

            AutoPtr<IInterface> tmp2;
            range->Get(0, (IInterface**)&tmp2);
            AutoPtr<IInteger32> obj = IInteger32::Probe(tmp2);
            Int32 num;
            obj->GetValue(&num);
            tmp2 = NULL;
            obj = NULL;
            range->Get(1, (IInterface**)&tmp2);
            obj = IInteger32::Probe(tmp2);
            Int32 num2;
            obj->GetValue(&num2);

            if ((*legacyFps)[0] == num && (*legacyFps)[1] == num2) {
                supported = TRUE;
                break;
            }
        }

        if (supported) {
            params->SetPreviewFpsRange((*legacyFps)[IParameters::PREVIEW_FPS_MIN_INDEX],
                    (*legacyFps)[IParameters::PREVIEW_FPS_MAX_INDEX]);
        }
        else {
            Logger::W(TAG, "Unsupported FPS range set [%d,%d]", (*legacyFps)[0], (*legacyFps)[1]);
        }
    }

    /*
     * control
     */

    // control.aeExposureCompensation
    {
        AutoPtr<IInterface> obj;
        assert(0);
        //characteristics->Get(CameraCharacteristics::CONTROL_AE_COMPENSATION_RANGE, (IInterface**)&obj);
        AutoPtr<Range<IInteger32> > compensationRange = (Range<IInteger32>*)IObject::Probe(obj);

        AutoPtr<IInterface> obj2;
        AutoPtr<IInteger32> res = CoreUtils::Convert(0);
        ParamsUtils::GetOrDefault(request,
                CaptureRequest::CONTROL_AE_EXPOSURE_COMPENSATION,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj2);
        AutoPtr<IInteger32> obj3 = IInteger32::Probe(obj2);
        Int32 compensation;
        obj3->GetValue(&compensation);

        Boolean result = compensationRange->Contains(obj3);
        if (!result) {
            Logger::W(TAG,
                    "convertRequestMetadata - control.aeExposureCompensation"
                    "is out of range, ignoring value");
            compensation = 0;
        }

        params->SetExposureCompensation(compensation);
    }

    // control.aeLock
    {
        Boolean supported;
        params->IsAutoExposureLockSupported(&supported);
        AutoPtr<IBoolean> tmp = CoreUtils::Convert(FALSE);
        AutoPtr<IInterface> obj = GetIfSupported(request, CaptureRequest::CONTROL_AE_LOCK,
                /*defaultValue*/TO_IINTERFACE(tmp), supported, /*allowedValue*/TO_IINTERFACE(tmp));
        AutoPtr<IBoolean> boolObj = IBoolean::Probe(obj);
        Boolean aeLock = FALSE;
        if (boolObj != NULL) {
            boolObj->GetValue(&aeLock);
            params->SetAutoExposureLock(aeLock);
        }

        if (VERBOSE) {
            Logger::V(TAG, "convertRequestToMetadata - control.aeLock set to %d", aeLock);
        }

        // TODO: Don't add control.aeLock to availableRequestKeys if it's not supported
    }

    // control.aeMode, flash.mode
    MapAeAndFlashMode(request, /*out*/params);

    // control.afMode
    {
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_MODE_OFF);
        AutoPtr<IInterface> obj;
        ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_AF_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 afMode;
        intObj->GetValue(&afMode);

        AutoPtr<ArrayOf<String> > modes;
        params->GetSupportedFocusModes((ArrayOf<String>**)&modes);
        assert(0);
        String focusMode;// = LegacyMetadataMapper::ConvertAfModeToLegacy(afMode, modes);

        if (!focusMode.IsNull()) {
            params->SetFocusMode(focusMode);
        }

        if (VERBOSE) {
            Logger::V(TAG, "convertRequestToMetadata - control.afMode %d mapped to %d",
                    afMode, focusMode.string());
        }
    }

    // control.awbMode
    {
        AutoPtr<ArrayOf<String> > balance;
        params->GetSupportedWhiteBalance((ArrayOf<String>**)&balance);
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_AWB_MODE_AUTO);
        AutoPtr<IInterface> obj = GetIfSupported(request, CaptureRequest::CONTROL_AWB_MODE,
                /*defaultValue*/TO_IINTERFACE(res),balance != NULL,
                /*allowedValue*/TO_IINTERFACE(res));
        AutoPtr<IInteger32> awbMode = IInteger32::Probe(obj);

        String whiteBalanceMode(NULL);
        if (awbMode != NULL) { // null iff AWB is not supported by camera1 api
            Int32 value;
            awbMode->GetValue(&value);
            whiteBalanceMode = ConvertAwbModeToLegacy(value);
            params->SetWhiteBalance(whiteBalanceMode);
        }

        if (VERBOSE) {
            Int32 tmp;
            awbMode->GetValue(&tmp);
            Logger::V(TAG, "convertRequestToMetadata - control.awbMode %d mapped to %s",
                    tmp , whiteBalanceMode.string());
        }
    }

    // control.awbLock
    {
        Boolean supported;
        params->IsAutoWhiteBalanceLockSupported(&supported);
        AutoPtr<IBoolean> tmp = CoreUtils::Convert(FALSE);
        AutoPtr<IInterface> obj = GetIfSupported(request, CaptureRequest::CONTROL_AWB_LOCK,
                /*defaultValue*/TO_IINTERFACE(tmp), supported,
                /*allowedValue*/TO_IINTERFACE(tmp));
        AutoPtr<IBoolean> boolObj = IBoolean::Probe(obj);
        Boolean awbLock = FALSE;
        if (boolObj != NULL) {
            boolObj->GetValue(&awbLock);
            params->SetAutoWhiteBalanceLock(awbLock);
        }

     // TODO: Don't add control.awbLock to availableRequestKeys if it's not supported
    }

    // control.captureIntent
    {
        AutoPtr<IInterface> obj;
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW);
        ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_CAPTURE_INTENT,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 captureIntent;
        intObj->GetValue(&captureIntent);
        captureIntent = FilterSupportedCaptureIntent(captureIntent);

        params->SetRecordingHint(
                captureIntent == ICameraMetadata::CONTROL_CAPTURE_INTENT_VIDEO_RECORD ||
                captureIntent == ICameraMetadata::CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT);
    }

    // control.videoStabilizationMode
    {
        Boolean result;
        params->IsVideoStabilizationSupported(&result);

        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_OFF);
        AutoPtr<IInterface> obj = GetIfSupported(request, CaptureRequest::CONTROL_VIDEO_STABILIZATION_MODE,
                /*defaultValue*/TO_IINTERFACE(res), result, /*allowedValue*/TO_IINTERFACE(res));
        AutoPtr<IInteger32> stabMode = IInteger32::Probe(obj);

        if (stabMode != NULL) {
            Int32 mode;
            stabMode->GetValue(&mode);
            params->SetVideoStabilization(mode == ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_ON);
        }
    }

    // lens.focusDistance
    {
        Boolean infinityFocusSupported;
        AutoPtr<ArrayOf<String> > modes;
        params->GetSupportedFocusModes((ArrayOf<String>**)&modes);
        assert(0);
        // ListUtils::ListContains(modes,
        //         IParameters::FOCUS_MODE_INFINITY, &infinityFocusSupported);

        AutoPtr<IFloat> res = CoreUtils::Convert(0.0f);
        AutoPtr<IInterface> obj = GetIfSupported(request, CaptureRequest::LENS_FOCUS_DISTANCE,
                /*defaultValue*/TO_IINTERFACE(res), infinityFocusSupported, /*allowedValue*/TO_IINTERFACE(res));
        AutoPtr<IFloat> obj2 = IFloat::Probe(obj);
        Float focusDistance = 0.0f;
        if (obj2 == NULL) {
            Logger::W(TAG,
                    "convertRequestToMetadata - Ignoring android.lens.focusDistance %d,"
                    "only 0.0f is supported", infinityFocusSupported);
        }
        else {
            obj2->GetValue(&focusDistance);
            if (focusDistance != 0.0f) {
                Logger::W(TAG,
                    "convertRequestToMetadata - Ignoring android.lens.focusDistance %d,"
                    "only 0.0f is supported", infinityFocusSupported);
            }
        }
    }

    // control.sceneMode, control.mode
    {
        // TODO: Map FACE_PRIORITY scene mode to face detection.
        AutoPtr<ArrayOf<String> > modes;
        params->GetSupportedSceneModes((ArrayOf<String>**)&modes);
        if (modes != NULL) {
            AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_MODE_AUTO);
            AutoPtr<IInterface> obj;
            ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
            Int32 controlMode;
            intObj->GetValue(&controlMode);
            String modeToSet;

            switch (controlMode) {
                case ICameraMetadata::CONTROL_MODE_USE_SCENE_MODE:
                {
                    AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_DISABLED);
                    AutoPtr<IInterface> obj;
                    ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_SCENE_MODE,
                            /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
                    AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
                    Int32 sceneMode;
                    intObj->GetValue(&sceneMode);

                    String legacySceneMode = LegacyMetadataMapper::ConvertSceneModeToLegacy(sceneMode);
                    if (legacySceneMode != NULL) {
                        modeToSet = legacySceneMode;
                    }
                    else {
                        modeToSet = IParameters::SCENE_MODE_AUTO;
                        Logger::W(TAG, "Skipping unknown requested scene mode: %d", sceneMode);
                    }
                    break;
                }
                case ICameraMetadata::CONTROL_MODE_AUTO:
                {
                    modeToSet = IParameters::SCENE_MODE_AUTO;
                    break;
                }
                default: {
                    Logger::W(TAG, "Control mode %d is unsupported, defaulting to AUTO", controlMode);
                    modeToSet = IParameters::SCENE_MODE_AUTO;
                }
            }
            params->SetSceneMode(modeToSet);
        }
    }

    // control.effectMode
    {
        AutoPtr<ArrayOf<String> > modes;
        params->GetSupportedColorEffects((ArrayOf<String>**)&modes);
        if (modes != NULL) {
            AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::CONTROL_EFFECT_MODE_OFF);
            AutoPtr<IInterface> obj;
            ParamsUtils::GetOrDefault(request, CaptureRequest::CONTROL_EFFECT_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
            Int32 effectMode;
            intObj->GetValue(&effectMode);

            String legacyEffectMode = LegacyMetadataMapper::ConvertEffectModeToLegacy(effectMode);
            if (!legacyEffectMode.IsNull()) {
                params->SetColorEffect(legacyEffectMode);
            }
            else {
                params->SetColorEffect(IParameters::EFFECT_NONE);
                Logger::W(TAG, "Skipping unknown requested effect mode: %d", effectMode);
            }
        }
    }

    /*
     * sensor
     */

    // sensor.testPattern
    {
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::SENSOR_TEST_PATTERN_MODE_OFF);
        AutoPtr<IInterface> obj;
        ParamsUtils::GetOrDefault(request, CaptureRequest::SENSOR_TEST_PATTERN_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 testPatternMode;
        intObj->GetValue(&testPatternMode);

        if (testPatternMode != ICameraMetadata::SENSOR_TEST_PATTERN_MODE_OFF) {
            Logger::W(TAG, "convertRequestToMetadata - ignoring sensor.testPatternMode %d;"
                    "only OFF is supported", testPatternMode);
        }
    }

    /*
     * jpeg.*
     */

    // jpeg.gpsLocation
    {
        AutoPtr<IInterface> obj;
        request->Get(CaptureRequest::JPEG_GPS_LOCATION, (IInterface**)&obj);
        AutoPtr<ILocation> location = ILocation::Probe(obj);

        if (location != NULL) {
            if (CheckForCompleteGpsData(location)) {
                Double altitude;
                location->GetAltitude(&altitude);
                params->SetGpsAltitude(altitude);
                Double latitude;
                location->GetLatitude(&latitude);
                params->SetGpsLatitude(latitude);
                Double longitude;
                location->GetLongitude(&longitude);
                params->SetGpsLongitude(longitude);
                String provider;
                location->GetProvider(&provider);
                params->SetGpsProcessingMethod(provider.ToUpperCase());
                Int64 _time;
                location->GetTime(&_time);
                params->SetGpsTimestamp(_time);
            }
            else {
                String str;
                IObject::Probe(location)->ToString(&str);
                Logger::W(TAG, "Incomplete GPS parameters provided in location %s", str.string());
            }
        }
        else {
            params->RemoveGpsData();
        }
    }

    // jpeg.orientation
    {
        AutoPtr<IInterface> obj;
        request->Get(CaptureRequest::JPEG_ORIENTATION, (IInterface**)&obj);
        AutoPtr<IInteger32> orientation = IInteger32::Probe(obj);

        AutoPtr<IInteger32> res;
        if (orientation == NULL) {
            res = CoreUtils::Convert(0);
        }
        else {
            res = orientation;
        }
        AutoPtr<IInterface> obj2;
        ParamsUtils::GetOrDefault(request, CaptureRequest::JPEG_ORIENTATION,
                TO_IINTERFACE(res), (IInterface**)&obj2);
        AutoPtr<IInteger32> obj3 = IInteger32::Probe(obj2);
        Int32 rotation;
        obj3->GetValue(&rotation);
        params->SetRotation(rotation);
    }

    // jpeg.quality
    {
        AutoPtr<IByte> res = CoreUtils::ConvertByte(DEFAULT_JPEG_QUALITY);
        AutoPtr<IInterface> obj;
        ParamsUtils::GetOrDefault(request, CaptureRequest::JPEG_QUALITY,
                TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> orientation = IInteger32::Probe(obj);
        Int32 value;
        orientation->GetValue(&value);
        params->SetJpegQuality(0xFF & value);
    }

    // jpeg.thumbnailQuality
    {
        AutoPtr<IByte> res = CoreUtils::ConvertByte(DEFAULT_JPEG_QUALITY);
        AutoPtr<IInterface> obj;
        ParamsUtils::GetOrDefault(request, CaptureRequest::JPEG_THUMBNAIL_QUALITY,
                TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> orientation = IInteger32::Probe(obj);
        Int32 value;
        orientation->GetValue(&value);
        params->SetJpegThumbnailQuality(0xFF & value);
    }

    // jpeg.thumbnailSize
    {
        AutoPtr<ArrayOf<ICameraSize*> > sizes;
        params->GetSupportedJpegThumbnailSizes((ArrayOf<ICameraSize*>**)&sizes);

        if (sizes != NULL) {
            if (sizes->GetLength() > 0) {
                AutoPtr<IInterface> obj;
                request->Get(CaptureRequest::JPEG_THUMBNAIL_SIZE, (IInterface**)&obj);
                AutoPtr<ISize> s = ISize::Probe(obj);
                Boolean invalidSize;
                if (s == NULL) {
                    invalidSize = FALSE;
                }
                else {
                    Boolean result;
                    Int32 width;
                    s->GetWidth(&width);
                    Int32 height;
                    s->GetHeight(&height);
                    assert(0);
                    //ParameterUtils::ContainsSize(sizes, width, height, &result);
                    invalidSize = !result;
                }
                if (invalidSize) {
                    String str;
                    IObject::Probe(s)->ToString(&str);
                    Logger::W(TAG, "Invalid JPEG thumbnail size set %s, skipping thumbnail...", str.string());
                }
                if (s == NULL || invalidSize) {
                    // (0,0) = "no thumbnail" in Camera API 1
                    params->SetJpegThumbnailSize(/*width*/0, /*height*/0);
                }
                else {
                    Int32 width;
                    s->GetWidth(&width);
                    Int32 height;
                    s->GetHeight(&height);
                    params->SetJpegThumbnailSize(width, height);
                }
            }
        }
    }

    /*
     * noiseReduction.*
     */
    // noiseReduction.mode
    {
        AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::NOISE_REDUCTION_MODE_FAST);
        AutoPtr<IInterface> obj;
        ParamsUtils::GetOrDefault(request, CaptureRequest::NOISE_REDUCTION_MODE,
                /*defaultValue*/TO_IINTERFACE(res), (IInterface**)&obj);
        AutoPtr<IInteger32> orientation = IInteger32::Probe(obj);
        Int32 mode;
        orientation->GetValue(&mode);

        if (mode != ICameraMetadata::NOISE_REDUCTION_MODE_FAST) {
            Logger::W(TAG, "convertRequestToMetadata - Ignoring unsupported "
                    "noiseReduction.mode = %d", mode);
        }
    }
    return NOERROR;
}

Boolean LegacyRequestMapper::CheckForCompleteGpsData(
    /* [in] */ ILocation* location)
{
    if (location != NULL) {
        String provider;
        location->GetProvider(&provider);
        if (!provider.IsNull()) {
            Int64 _time;
            location->GetTime(&_time);
            if (_time != 0) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Int32 LegacyRequestMapper::FilterSupportedCaptureIntent(
    /* [in] */ Int32 captureIntent)
{
    Int32 res;
    switch (captureIntent) {
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_CUSTOM:
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW:
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_STILL_CAPTURE:
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_VIDEO_RECORD:
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_VIDEO_SNAPSHOT:
            break;
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_ZERO_SHUTTER_LAG:
        case ICameraMetadata::CONTROL_CAPTURE_INTENT_MANUAL:
            res = ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW;
            Logger::W(TAG, "Unsupported control.captureIntent value %s; default to PREVIEW", res);
        default:
            res = ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW;
            Logger::W(TAG, "Unknown control.captureIntent value %d; default to PREVIEW", res);
    }

    return res;
}

AutoPtr<IList> LegacyRequestMapper::ConvertMeteringRegionsToLegacy(
    /* [in] */ IRect* activeArray,
    /* [in] */ IParameterUtilsZoomData* zoomData,
    /* [in] */ IArrayOf* meteringRegions,
    /* [in] */ Int32 maxNumMeteringAreas,
    /* [in] */ const String& regionName)
{
    if (meteringRegions == NULL || maxNumMeteringAreas <= 0) {
        if (maxNumMeteringAreas > 0) {
            AutoPtr<IList> res;
            assert(0);
            //Arrays::AsList(IParameterUtils::CAMERA_AREA_DEFAULT, (IList**)&res);
            return res;
        }
        else {
            return NULL;
        }
    }

    // Add all non-zero weight regions to the list
    AutoPtr<IList> meteringRectangleList;
    CArrayList::New((IList**)&meteringRectangleList);
    Int32 len;
    meteringRegions->GetLength(&len);
    for (Int32 i = 0; i< len; i++) {
        AutoPtr<IInterface> obj;
        meteringRegions->Get(i, (IInterface**)&obj);
        AutoPtr<IMeteringRectangle> rect = IMeteringRectangle::Probe(obj);
        Int32 value;
        rect->GetMeteringWeight(&value);
        if (value != IMeteringRectangle::METERING_WEIGHT_DONT_CARE) {
            meteringRectangleList->Add(TO_IINTERFACE(rect));
        }
    }

    Int32 size;
    meteringRectangleList->GetSize(&size);
    if (size == 0) {
        Logger::W(TAG, "Only received metering rectangles with weight 0.");
        AutoPtr<IList> res;
        assert(0);
        //Arrays::AsList(ParameterUtils::CAMERA_AREA_DEFAULT, (IList**)&res);
        return res;
    }

    // Ignore any regions beyond our maximum supported count
    Int32 countMeteringAreas = Elastos::Core::Math::Min(maxNumMeteringAreas, size);
    AutoPtr<IList> meteringAreaList;
    CArrayList::New(countMeteringAreas, (IList**)&meteringAreaList);

    for (Int32 i = 0; i < countMeteringAreas; ++i) {
        AutoPtr<IInterface> obj;
        meteringRectangleList->Get(i, (IInterface**)&obj);
        AutoPtr<IMeteringRectangle> rect = IMeteringRectangle::Probe(obj);

        AutoPtr<IParameterUtilsMeteringData> meteringData;
        ParameterUtils::ConvertMeteringRectangleToLegacy(activeArray, rect,
                zoomData, (IParameterUtilsMeteringData**)&meteringData);
        AutoPtr<ICameraArea> area;
        meteringData->GetMeteringArea((ICameraArea**)&area);
        meteringAreaList->Add(TO_IINTERFACE(area));
    }

    if (maxNumMeteringAreas < size) {
        Logger::W(TAG,
                "convertMeteringRegionsToLegacy - Too many requested %s regions,"
                "ignoring all beyond the first %d", regionName.string(), maxNumMeteringAreas);
    }

    if (VERBOSE) {
        String str;
        ParameterUtils::StringFromAreaList(meteringAreaList, &str);
        Logger::V(TAG, "convertMeteringRegionsToLegacy - %s areas = %s",
                regionName.string(), str.string());
    }

    return meteringAreaList;
}

void LegacyRequestMapper::MapAeAndFlashMode(
    /* [in] */ ICaptureRequest* r,
    /* [in] */ IParameters* p)
{
    AutoPtr<IInteger32> res = CoreUtils::Convert(ICameraMetadata::FLASH_MODE_OFF);
    AutoPtr<IInterface> obj;
    ParamsUtils::GetOrDefault(r, CaptureRequest::FLASH_MODE, TO_IINTERFACE(res), (IInterface**)&obj);
    AutoPtr<IInteger32> orientation = IInteger32::Probe(obj);
    Int32 flashMode;
    orientation->GetValue(&flashMode);

    AutoPtr<IInteger32> res2 = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_MODE_ON);
    AutoPtr<IInterface> obj2;
    ParamsUtils::GetOrDefault(r, CaptureRequest::CONTROL_AE_MODE, TO_IINTERFACE(res2), (IInterface**)&obj);
    AutoPtr<IInteger32> orientation2 = IInteger32::Probe(obj);
    Int32 aeMode;
    orientation2->GetValue(&aeMode);

    AutoPtr<IList> supportedFlashModes;
    assert(0);
    //p->GetSupportedFlashModes((IList**)&supportedFlashModes);

    String flashModeSetting;

    // Flash is OFF by default, on cameras that support flash
    Boolean result;
    AutoPtr<ICharSequence> obj3 = CoreUtils::Convert(IParameters::FLASH_MODE_OFF);
    ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj3), &result);
    if (result) {
        flashModeSetting = IParameters::FLASH_MODE_OFF;
    }

    /*
     * Map all of the control.aeMode* enums, but ignore AE_MODE_OFF since we never support it
     */

    // Ignore flash.mode controls unless aeMode == ON
    if (aeMode == ICameraMetadata::CONTROL_AE_MODE_ON) {
        if (flashMode == ICameraMetadata::FLASH_MODE_TORCH) {
                Boolean res;
                AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_TORCH);
                ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj), &res);
                if (res) {
                    flashModeSetting = IParameters::FLASH_MODE_TORCH;
                }
                else {
                    Logger::W(TAG, "mapAeAndFlashMode - Ignore flash.mode == TORCH;"
                            "camera does not support it");
                }
        }
        else if (flashMode == ICameraMetadata::FLASH_MODE_SINGLE) {
            Boolean res;
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_ON);
            ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj), &res);
            if (res) {
                flashModeSetting = IParameters::FLASH_MODE_ON;
            }
            else {
                Logger::W(TAG, "mapAeAndFlashMode - Ignore flash.mode == SINGLE;"
                        "camera does not support it");
            }
        }
        else {
            // Use the default FLASH_MODE_OFF
        }
    }
    else if (aeMode == ICameraMetadata::CONTROL_AE_MODE_ON_ALWAYS_FLASH) {
        Boolean res;
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_ON);
        ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj), &res);
        if (res) {
            flashModeSetting = IParameters::FLASH_MODE_ON;
        }
        else {
            Logger::W(TAG, "mapAeAndFlashMode - Ignore control.aeMode == ON_ALWAYS_FLASH;"
                    "camera does not support it");
        }
    }
    else if (aeMode == ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH) {
        Boolean res;
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_AUTO);
        ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj), &res);
        if (res) {
            flashModeSetting = IParameters::FLASH_MODE_AUTO;
        }
        else {
            Logger::W(TAG, "mapAeAndFlashMode - Ignore control.aeMode == ON_AUTO_FLASH;"
                    "camera does not support it");
        }
    }
    else if (aeMode == ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE) {
        Boolean res;
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_RED_EYE);
        ListUtils::ListContains(supportedFlashModes, TO_IINTERFACE(obj), &res);
        if (res) {
            flashModeSetting = IParameters::FLASH_MODE_RED_EYE;
        }
        else {
            Logger::W(TAG, "mapAeAndFlashMode - Ignore control.aeMode == ON_AUTO_FLASH_REDEYE;"
                    "camera does not support it");
        }
    }
    else {
        // Default to aeMode == ON, flash = OFF
    }

    if (flashModeSetting != NULL) {
        p->SetFlashMode(flashModeSetting);
    }

    if (VERBOSE) {
        String str;
        ListUtils::ListToString(supportedFlashModes, &str);
        Logger::V(TAG,
                "mapAeAndFlashMode - set flash.mode (api1) to %s, requested (api2) %d,"
                "supported (api1) %s",flashModeSetting.string(), flashMode, str.string());
    }
    return;
}

String LegacyRequestMapper::ConvertAeAntiBandingModeToLegacy(
    /* [in] */ Int32 mode)
{
    switch (mode) {
        case ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_OFF: {
            return IParameters::ANTIBANDING_OFF;
        }
        case ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_50HZ: {
            return IParameters::ANTIBANDING_50HZ;
        }
        case ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_60HZ: {
            return IParameters::ANTIBANDING_60HZ;
        }
        case ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_AUTO: {
            return IParameters::ANTIBANDING_AUTO;
        }
        default: {
            return String(NULL);
        }
    }
}

AutoPtr<ArrayOf<Int32> > LegacyRequestMapper::ConvertAeFpsRangeToLegacy(
    /* [in] */ Range<IInteger32>* fpsRange)
{
    AutoPtr<ArrayOf<Int32> > legacyFps = ArrayOf<Int32>::Alloc(2);

    AutoPtr<IInteger32> obj = fpsRange->GetLower();
    Int32 low;
    obj->GetValue(&low);
    obj = NULL;
    obj = fpsRange->GetUpper();
    Int32 up;
    obj->GetValue(&up);
    (*legacyFps)[IParameters::PREVIEW_FPS_MIN_INDEX] = low;
    (*legacyFps)[IParameters::PREVIEW_FPS_MAX_INDEX] = up;
    return legacyFps;
}

String LegacyRequestMapper::ConvertAwbModeToLegacy(
    /* [in] */ Int32 mode)
{
    switch (mode) {
        case ICameraMetadata::CONTROL_AWB_MODE_AUTO:
            return IParameters::WHITE_BALANCE_AUTO;
        case ICameraMetadata::CONTROL_AWB_MODE_INCANDESCENT:
            return IParameters::WHITE_BALANCE_INCANDESCENT;
        case ICameraMetadata::CONTROL_AWB_MODE_FLUORESCENT:
            return IParameters::WHITE_BALANCE_FLUORESCENT;
        case ICameraMetadata::CONTROL_AWB_MODE_WARM_FLUORESCENT:
            return IParameters::WHITE_BALANCE_WARM_FLUORESCENT;
        case ICameraMetadata::CONTROL_AWB_MODE_DAYLIGHT:
            return IParameters::WHITE_BALANCE_DAYLIGHT;
        case ICameraMetadata::CONTROL_AWB_MODE_CLOUDY_DAYLIGHT:
            return IParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT;
        case ICameraMetadata::CONTROL_AWB_MODE_TWILIGHT:
            return IParameters::WHITE_BALANCE_TWILIGHT;
        case ICameraMetadata::CONTROL_AWB_MODE_SHADE:
            return IParameters::WHITE_BALANCE_SHADE;
        default:
            Logger::W(TAG, "convertAwbModeToLegacy - unrecognized control.awbMode %d", mode);
            return IParameters::WHITE_BALANCE_AUTO;
    }
}

AutoPtr<IInterface> LegacyRequestMapper::GetIfSupported(
    /* [in] */ ICaptureRequest* r,
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* defaultValue,
    /* [in] */ Boolean isSupported,
    /* [in] */ IInterface* allowedValue)
{
    AutoPtr<IInterface> val;
    ParamsUtils::GetOrDefault(r, key, defaultValue, (IInterface**)&val);

    if (!isSupported) {
        if (!Objects::Equals(val.Get(), allowedValue)) {
            String name;
            key->GetName(&name);
            String valStr;
            IObject::Probe(val)->ToString(&valStr);
            Logger::W(TAG, "%s is not supported; ignoring requested value %s",
                    name.string(), valStr.string());
        }
        return NULL;
    }

    return val;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
