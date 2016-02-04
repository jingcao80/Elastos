
#include "elastos/droid/hardware/camera2/legacy/LegacyMetadataMapper.h"
#include "elastos/droid/hardware/camera2/legacy/CLegacySizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/legacy/LegacyRequestMapper.h"
#include "elastos/droid/hardware/camera2/utils/CUtilsSizeAreaComparator.h"
#include "elastos/droid/hardware/camera2/utils/ArrayUtils.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/params/CMeteringRectangle.h"
#include "elastos/droid/hardware/camera2/params/CStreamConfiguration.h"
#include "elastos/droid/hardware/camera2/params/CStreamConfigurationDuration.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CCameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/HardwareCamera.h"
#include "elastos/droid/hardware/CameraInfo.h"
#include "elastos/droid/graphics/CImageFormat.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/utility/CSizeF.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Legacy::ILegacySizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Legacy::CLegacySizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Legacy::ParameterUtils;
using Elastos::Droid::Hardware::Camera2::Legacy::LegacyRequestMapper;
using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::CMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::CStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Params::CStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Utils::ArrayUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ListUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Hardware::Camera2::Utils::IUtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Utils::CUtilsSizeAreaComparator;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::CCameraCharacteristics;
using Elastos::Droid::Hardware::HardwareCamera;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::CImageFormat;
using Elastos::Droid::Utility::CSizeF;
using Elastos::Core::ICharSequence;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(LegacyMetadataMapper, Object, ILegacyMetadataMapper)

const Int32 LegacyMetadataMapper::UNKNOWN_MODE = -1;

const Boolean LegacyMetadataMapper::LIE_ABOUT_AE_STATE = FALSE;
const Boolean LegacyMetadataMapper::LIE_ABOUT_AE_MAX_REGIONS = FALSE;
const Boolean LegacyMetadataMapper::LIE_ABOUT_AF = FALSE;
const Boolean LegacyMetadataMapper::LIE_ABOUT_AF_MAX_REGIONS = FALSE;
const Boolean LegacyMetadataMapper::LIE_ABOUT_AWB_STATE = FALSE;
const Boolean LegacyMetadataMapper::LIE_ABOUT_AWB = FALSE;

const String LegacyMetadataMapper::TAG("LegacyMetadataMapper");
const Boolean LegacyMetadataMapper::VERBOSE = FALSE;//Log.isLoggable(TAG, Log.VERBOSE);

const Int64 LegacyMetadataMapper::NS_PER_MS = 1000000;

const Float LegacyMetadataMapper::LENS_INFO_MINIMUM_FOCUS_DISTANCE_FIXED_FOCUS = 0.0f;

const Int32 LegacyMetadataMapper::REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_RAW = 0; // no raw support
const Int32 LegacyMetadataMapper::REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC = 3; // preview, video, cb
const Int32 LegacyMetadataMapper::REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC_STALL = 1; // 1 jpeg only
const Int32 LegacyMetadataMapper::REQUEST_MAX_NUM_INPUT_STREAMS_COUNT = 0; // no reprocessing

const Int32 LegacyMetadataMapper::REQUEST_PIPELINE_MAX_DEPTH_HAL1 = 3;
const Int32 LegacyMetadataMapper::REQUEST_PIPELINE_MAX_DEPTH_OURS = 3;

const Int64 LegacyMetadataMapper::APPROXIMATE_CAPTURE_DELAY_MS = 200; // 200 milliseconds
const Int64 LegacyMetadataMapper::APPROXIMATE_SENSOR_AREA_PX = (1 << 23); // 8 megapixels
const Int64 LegacyMetadataMapper::APPROXIMATE_JPEG_ENCODE_TIME_MS = 600; // 600 milliseconds

const Float LegacyMetadataMapper::PREVIEW_ASPECT_RATIO_TOLERANCE = 0.01f;

LegacyMetadataMapper::LegacyMetadataMapper()
{
}

ECode LegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ IParameters* parameters,
    /* [in] */ IHardwareCameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)
    *outcc = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(parameters, String("parameters must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(info, String("info must not be null")))

    String paramStr;
    parameters->Flatten(&paramStr);
    AutoPtr<ICameraInfo> outerInfo = new CameraInfo();
    outerInfo->SetInfo(info);

    return CreateCharacteristics(paramStr, outerInfo, outcc);
}

ECode LegacyMetadataMapper::CreateCharacteristics(
    /* [in] */ const String& parameters,
    /* [in] */ ICameraInfo* info,
    /* [out] */ ICameraCharacteristics** outcc)
{
    VALIDATE_NOT_NULL(outcc)
    *outcc = NULL;

    FAIL_RETURN(Preconditions::CheckNotNull(parameters))
    FAIL_RETURN(Preconditions::CheckNotNull(info, String("info must not be null")))
    AutoPtr<IHardwareCameraInfo> cinfo;
    info->GetInfo((IHardwareCameraInfo**)&cinfo);
    FAIL_RETURN(Preconditions::CheckNotNull(cinfo, String("info.info must not be null")))

    AutoPtr<ICameraMetadataNative> m;
    CCameraMetadataNative::New((ICameraMetadataNative**)&m);

    MapCharacteristicsFromInfo(m, cinfo);

    AutoPtr<IParameters> params;
    HardwareCamera::GetEmptyParameters((IParameters**)&params);
    params->Unflatten(parameters);
    MapCharacteristicsFromParameters(m, params);

    if (VERBOSE) {
        Slogger::V(TAG, "createCharacteristics metadata:");
        Slogger::V(TAG, "--------------------------------------------------- (start)");
        m->DumpToLog();
        Slogger::V(TAG, "--------------------------------------------------- (end)");
    }

    return CCameraCharacteristics::New(m, outcc);
}

void LegacyMetadataMapper::MapCharacteristicsFromInfo(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IHardwareCameraInfo* i)
{
    Int32 face;
    i->GetFacing(&face);
    AutoPtr<IInteger32> intObj;
    if (face == IHardwareCameraInfo::CAMERA_FACING_BACK) {
        intObj = CoreUtils::Convert(ICameraMetadata::LENS_FACING_BACK);
    }
    else {
        intObj = CoreUtils::Convert(ICameraMetadata::LENS_FACING_FRONT);
    }
    m->Set(CameraCharacteristics::LENS_FACING, TO_IINTERFACE(intObj));
    Int32 orien;
    i->GetOrientation(&orien);
    AutoPtr<IInteger32> intObj2 = CoreUtils::Convert(orien);
    m->Set(CameraCharacteristics::SENSOR_ORIENTATION, TO_IINTERFACE(intObj2));
    return;
}

void LegacyMetadataMapper::MapCharacteristicsFromParameters(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * colorCorrection.*
     */
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
    (*array)[0] = ICameraMetadata::COLOR_CORRECTION_ABERRATION_MODE_FAST;
    AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(array);
    m->Set(CameraCharacteristics::COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES, TO_IINTERFACE(arrayObj));
    /*
     * control.ae*
     */
    MapControlAe(m, p);
    /*
     * control.af*
     */
    MapControlAf(m, p);
    /*
     * control.awb*
     */
    MapControlAwb(m, p);
    /*
     * control.*
     * - Anything that doesn't have a set of related fields
     */
    MapControlOther(m, p);
    /*
     * lens.*
     */
    MapLens(m, p);
    /*
     * flash.*
     */
    MapFlash(m, p);
    /*
     * jpeg.*
     */
    MapJpeg(m, p);

    /*
     * noiseReduction.*
     */
    AutoPtr<ArrayOf<Int32> > array2 = ArrayOf<Int32>::Alloc(1);
    (*array2)[0] = ICameraMetadata::NOISE_REDUCTION_MODE_FAST;
    AutoPtr<IArrayOf> arrayObj2 = CoreUtils::Convert(array2);
    m->Set(CameraCharacteristics::NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES, TO_IINTERFACE(arrayObj2));

    /*
     * scaler.*
     */
    MapScaler(m, p);

    /*
     * sensor.*
     */
    MapSensor(m, p);

    /*
     * statistics.*
     */
    MapStatistics(m, p);

    /*
     * sync.*
     */
    MapSync(m, p);

    /*
     * info.supportedHardwareLevel
     */
    AutoPtr<IInteger32> int32Obj = CoreUtils::Convert(ICameraMetadata::INFO_SUPPORTED_HARDWARE_LEVEL_LEGACY);
    m->Set(CameraCharacteristics::INFO_SUPPORTED_HARDWARE_LEVEL, TO_IINTERFACE(int32Obj));

    /*
     * scaler.availableStream*, scaler.available*Durations, sensor.info.maxFrameDuration
     */
    MapScalerStreamConfigs(m, p);

    // Order matters below: Put this last so that we can read the metadata set previously

    /*
     * request.*
     */
    MapRequest(m, p);
    return;
}

void LegacyMetadataMapper::MapScalerStreamConfigs(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    AutoPtr<IArrayList> availableStreamConfigs;
    CArrayList::New((IArrayList**)&availableStreamConfigs);
    /*
     * Implementation-defined (preview, recording, etc) -> use camera1 preview sizes
     * YUV_420_888 cpu callbacks -> use camera1 preview sizes
     * Other preview callbacks (CPU) -> use camera1 preview sizes
     * JPEG still capture -> use camera1 still capture sizes
     *
     * Use platform-internal format constants here, since StreamConfigurationMap does the
     * remapping to public format constants.
     */
    AutoPtr<ArrayOf<ICameraSize*> > sizes;
    p->GetSupportedPreviewSizes((ArrayOf<ICameraSize*>**)&sizes);
    AutoPtr<IList> previewSizes;
    CArrayList::New((IList**)&previewSizes);
    for (Int32 i = 0; i < sizes->GetLength(); i++) {
        previewSizes->Add(TO_IINTERFACE((*sizes)[i]));
    }

    AutoPtr<ArrayOf<ICameraSize*> > sizes2;
    p->GetSupportedPictureSizes((ArrayOf<ICameraSize*>**)&sizes2);
    AutoPtr<IList> jpegSizes;
    CArrayList::New((IList**)&jpegSizes);
    for (Int32 i = 0; i < sizes2->GetLength(); i++) {
        jpegSizes->Add(TO_IINTERFACE((*sizes2)[i]));
    }

    /*
     * Work-around for b/17589233:
     * - Some HALs's largest preview size aspect ratio does not match the largest JPEG size AR
     * - This causes a large amount of problems with focus/metering because it's relative to
     *   preview, making the difference between the JPEG and preview viewport inaccessible
     * - This boils down to metering or focusing areas being "arbitrarily" cropped
     *   in the capture result.
     * - Work-around the HAL limitations by removing all of the largest preview sizes
     *   until we get one with the same aspect ratio as the jpeg size.
     */
    {
        AutoPtr<ILegacySizeAreaComparator> areaComparator;
        CLegacySizeAreaComparator::New((ILegacySizeAreaComparator**)&areaComparator);

        // Sort preview to min->max
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        coll->Sort(previewSizes, IComparator::Probe(areaComparator));

        AutoPtr<ICameraSize> maxJpegSize;
        CLegacySizeAreaComparator::FindLargestByArea(jpegSizes, (ICameraSize**)&maxJpegSize);
        Int32 width;
        maxJpegSize->GetWidth(&width);
        Int32 height;
        maxJpegSize->GetHeight(&height);
        Float jpegAspectRatio = width * 1.0f / height;

        if (VERBOSE) {
            Slogger::V(TAG, "mapScalerStreamConfigs - largest JPEG area %dx%d, AR=%f",
                    width, height, jpegAspectRatio);
        }

        // Now remove preview sizes from the end (largest->smallest) until aspect ratio matches
        Boolean result;
        while (((previewSizes->IsEmpty(&result)), !result)) {
            Int32 _size;
            previewSizes->GetSize(&_size);
            Int32 index = _size - 1; // max is always at the end
            AutoPtr<IInterface> obj;
            previewSizes->Get(index, (IInterface**)&obj);
            AutoPtr<ICameraSize> size = ICameraSize::Probe(obj);

            Int32 width;
            size->GetWidth(&width);
            Int32 height;
            size->GetHeight(&height);
            Float previewAspectRatio = width * 1.0f / height;

            if (Elastos::Core::Math::Abs(jpegAspectRatio - previewAspectRatio) >=
                    PREVIEW_ASPECT_RATIO_TOLERANCE) {
                previewSizes->Remove(index); // Assume removing from end is O(1)

                if (VERBOSE) {
                    Slogger::V(TAG, "mapScalerStreamConfigs - removed preview size %dx%d, AR=%f"
                            "was not the same",width, height, previewAspectRatio);
                }
            }
            else {
                break;
            }
        }

        previewSizes->IsEmpty(&result);
        if (result) {
            // Fall-back to the original faulty behavior, but at least work
            Slogger::W(TAG, "mapScalerStreamConfigs - failed to find any preview size matching "
                    "JPEG aspect ratio %f", jpegAspectRatio);
            assert(0);
            //p->GetSupportedPreviewSizes(&previewSizes);
        }

        // Sort again, this time in descending order max->min
        AutoPtr<IComparator> tmp;
        coll->ReverseOrder(IComparator::Probe(areaComparator), (IComparator**)&tmp);
        coll->Sort(previewSizes, tmp);
    }

    AppendStreamConfig(availableStreamConfigs,
            HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, previewSizes);
    AppendStreamConfig(availableStreamConfigs,
            IImageFormat::YUV_420_888, previewSizes);

    AutoPtr<ArrayOf<IInteger32*> > array;
    p->GetSupportedPreviewFormats((ArrayOf<IInteger32*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInteger32> obj = (*array)[i];
        Int32 format;
        obj->GetValue(&format);

        Boolean result;
        AutoPtr<IImageFormat> helper;
        CImageFormat::AcquireSingleton((IImageFormat**)&helper);
        helper->IsPublicFormat(format, &result);
        if (result) {
            AppendStreamConfig(availableStreamConfigs, format, previewSizes);
        }
        else {
            /*
             *  Do not add any formats unknown to us
             * (since it would fail runtime checks in StreamConfigurationMap)
             */
            Slogger::W(TAG,"mapStreamConfigs - Skipping non-public format %d", format);
        }
    }

    AutoPtr<ArrayOf<ICameraSize*> > arraySize;
    p->GetSupportedPictureSizes((ArrayOf<ICameraSize*>**)&arraySize);
    AutoPtr<IList> list;
    Arrays::AsList(arraySize, (IList**)&list);
    AppendStreamConfig(availableStreamConfigs, HAL_PIXEL_FORMAT_BLOB, list);
    /*
     * scaler.availableStreamConfigurations
     */
    AutoPtr<ArrayOf<IInterface*> > tmpArray;
    availableStreamConfigs->ToArray((ArrayOf<IInterface *>**)&tmpArray);
    AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(tmpArray.Get());
    m->Set(CameraCharacteristics::SCALER_AVAILABLE_STREAM_CONFIGURATIONS, TO_IINTERFACE(arrayObj));

    /*
     * scaler.availableMinFrameDurations
     */
    // No frame durations available
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > durationArray =
            ArrayOf<IStreamConfigurationDuration*>::Alloc(0);
    AutoPtr<IArrayOf> durationArrayObj = CoreUtils::Convert(durationArray.Get());
    m->Set(CameraCharacteristics::SCALER_AVAILABLE_MIN_FRAME_DURATIONS, TO_IINTERFACE(durationArrayObj));

    Int32 jsize;
    jpegSizes->GetSize(&jsize);
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > jpegStalls =
            ArrayOf<IStreamConfigurationDuration*>::Alloc(jsize);

    Int64 longestStallDuration = -1;
    for (Int32 i = 0; i < jsize; i++) {
        AutoPtr<IInterface> obj;
        jpegSizes->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraSize> s = ICameraSize::Probe(obj);

        Int64 stallDuration = CalculateJpegStallDuration(s);
        Int32 width;
        s->GetWidth(&width);
        Int32 height;
        s->GetHeight(&height);
        AutoPtr<IStreamConfigurationDuration> duration;
        CStreamConfigurationDuration::New(HAL_PIXEL_FORMAT_BLOB, width,
                height, stallDuration, (IStreamConfigurationDuration**)&duration);
        jpegStalls->Set(i++, duration);
        if (longestStallDuration < stallDuration) {
            longestStallDuration = stallDuration;
        }
    }

    /*
     * scaler.availableStallDurations
     */
    // Set stall durations for jpeg, other formats use default stall duration
    AutoPtr<IArrayOf> jpegStallsObj = CoreUtils::Convert(jpegStalls.Get());
    m->Set(CameraCharacteristics::SCALER_AVAILABLE_STALL_DURATIONS, TO_IINTERFACE(jpegStallsObj));

    /*
     * sensor.info.maxFrameDuration
     */
    AutoPtr<IInteger64> Int64Obj = CoreUtils::Convert(longestStallDuration);
    m->Set(CameraCharacteristics::SENSOR_INFO_MAX_FRAME_DURATION, TO_IINTERFACE(Int64Obj));
    return;
}

ECode LegacyMetadataMapper::MapControlAe(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * control.aeAvailableAntiBandingModes
     */
    AutoPtr<IList> antiBandingModes;
    assert(0);
    //p->GetSupportedAntibanding((IList**)&antiBandingModes);
    if (antiBandingModes != NULL) { // antibanding is optional
        Int32 size;
        antiBandingModes->GetSize(&size);
        if (size > 0) {
            AutoPtr<ArrayOf<Int32> > modes = ArrayOf<Int32>::Alloc(size);
            Int32 j = 0;
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                antiBandingModes->Get(i, (IInterface**)&obj);
                AutoPtr<ICharSequence> sque = ICharSequence::Probe(obj);
                String mode;
                sque->ToString(&mode);

                Int32 convertedMode = ConvertAntiBandingMode(mode);
                if (convertedMode == -1) {
                    Slogger::W(TAG, "Antibanding mode %s not supported, skipping...",
                            ((mode == NULL) ? String("NULL").string() : mode.string()));
                }
                else {
                    (*modes)[j++] = convertedMode;
                }
            }
            AutoPtr<ArrayOf<Int32> > newModes;
            Arrays::CopyOf(modes, j, (ArrayOf<Int32>**)&newModes);
            AutoPtr<IArrayOf> newModesObj = CoreUtils::Convert(newModes);
            m->Set(CameraCharacteristics::CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, TO_IINTERFACE(newModesObj));
        }
    }
    else {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(0);
        AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(array);
        m->Set(CameraCharacteristics::CONTROL_AE_AVAILABLE_ANTIBANDING_MODES, TO_IINTERFACE(arrayObj));
    }

    /*
     * control.aeAvailableTargetFpsRanges
     */
    {
        AutoPtr<IList> fpsRanges;
        assert(0);
        //p->GetSupportedPreviewFpsRange((IList**)&fpsRanges);
        if (fpsRanges == NULL) {
            //throw new AssertionError("Supported FPS ranges cannot be null.");
            Slogger::E(TAG, "Supported FPS ranges cannot be null.");
            return E_ASSERTION_ERROR;
        }
        Int32 rangesSize;
        fpsRanges->GetSize(&rangesSize);
        if (rangesSize <= 0) {
            //throw new AssertionError("At least one FPS range must be supported.");
            Slogger::E(TAG, "At least one FPS range must be supported.");
            return E_ASSERTION_ERROR;
        }
        assert(0);
        // AutoPtr<ArrayOf<Range<IInteger32>*> > ranges = ArrayOf<Range<IInteger32>* >::Alloc(rangesSize);
        // for (Int32 i = 0; i < rangesSize; i++) {
        //     AutoPtr<IInterface> obj;
        //     fpsRanges->Get(i, (IInterface**)&obj);
        //     AutoPtr<IArrayOf> r = IArrayOf::Probe(obj);
        //     AutoPtr<IInterface> obj2;
        //     r->Get(IParameters::PREVIEW_FPS_MIN_INDEX, (IInterface**)&obj2);
        //     AutoPtr<IInterface> obj3;
        //     r->Get(IParameters::PREVIEW_FPS_MAX_INDEX, (IInterface**)&obj3);

        //     AutoPtr<Range<IInteger32> > tmp = Range<IInteger32>::Create(IInteger32::Probe(obj2),
        //             IInteger32::Probe(obj3));
        //     ranges->Set(i++ , tmp);

        // }

        //m->Set(CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, ranges);
    }

    /*
     * control.aeAvailableModes
     */
    {
        AutoPtr<IList> flashModes;
        assert(0);
        //p->GetSupportedFlashModes((IList**)&flashModes);

        AutoPtr<ArrayOf<String> > flashModeStrings = ArrayOf<String>::Alloc(5);
        (*flashModeStrings)[0] = IParameters::FLASH_MODE_OFF;
        (*flashModeStrings)[1] = IParameters::FLASH_MODE_AUTO;
        (*flashModeStrings)[2] = IParameters::FLASH_MODE_ON;
        (*flashModeStrings)[3] = IParameters::FLASH_MODE_RED_EYE;
        // Map these manually
        (*flashModeStrings)[4] = IParameters::FLASH_MODE_TORCH;

        AutoPtr<ArrayOf<Int32> > flashModeInts = ArrayOf<Int32>::Alloc(4);
        (*flashModeInts)[0] = ICameraMetadata::CONTROL_AE_MODE_ON;
        (*flashModeInts)[1] = ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH;
        (*flashModeInts)[2] = ICameraMetadata::CONTROL_AE_MODE_ON_ALWAYS_FLASH;
        (*flashModeInts)[3] = ICameraMetadata::CONTROL_AE_MODE_ON_AUTO_FLASH_REDEYE;

        AutoPtr<ArrayOf<Int32> > aeAvail;
        ArrayUtils::ConvertStringListToIntArray(
                flashModes, flashModeStrings, flashModeInts, (ArrayOf<Int32>**)&aeAvail);

        // No flash control -> AE is always on
        if (aeAvail == NULL || aeAvail->GetLength() == 0) {
            aeAvail = ArrayOf<Int32>::Alloc(1);
            (*aeAvail)[0] = ICameraMetadata::CONTROL_AE_MODE_ON;
        }

        // Note that AE_MODE_OFF is never available.
        AutoPtr<IArrayOf> aeAvailObj = CoreUtils::Convert(aeAvail);
        m->Set(CameraCharacteristics::CONTROL_AE_AVAILABLE_MODES, TO_IINTERFACE(aeAvailObj));
    }

    /*
     * control.aeCompensationRanges
     */
    {
        Int32 min;
        p->GetMinExposureCompensation(&min);
        Int32 max;
        p->GetMaxExposureCompensation(&max);

        //AutoPtr<Range<Int32> > array = Range<Int32>::Create(&min, &max);
        assert(0);
        //m->Set(CameraCharacteristics::CONTROL_AE_COMPENSATION_RANGE, TO_IINTERFACE(arrayObj));
    }

    /*
     * control.aeCompensationStep
     */
    {
        Float step;
        p->GetExposureCompensationStep(&step);
        AutoPtr<IRational> rational;
        ParamsUtils::CreateRational(step, (IRational**)&rational);
        m->Set(CameraCharacteristics::CONTROL_AE_COMPENSATION_STEP, TO_IINTERFACE(rational));
    }
    return NOERROR;
}

void LegacyMetadataMapper::MapControlAf(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * control.afAvailableModes
     */
    {
        AutoPtr<IList> focusModes;
        assert(0);
        //p->GetSupportedFocusModes((IList**)&focusModes);

        AutoPtr<ArrayOf<String> > focusModeStrings = ArrayOf<String>::Alloc(7);
        (*focusModeStrings)[0] = IParameters::FOCUS_MODE_AUTO;
        (*focusModeStrings)[1] = IParameters::FOCUS_MODE_CONTINUOUS_PICTURE;
        (*focusModeStrings)[2] = IParameters::FOCUS_MODE_CONTINUOUS_VIDEO;
        (*focusModeStrings)[3] = IParameters::FOCUS_MODE_EDOF;
        (*focusModeStrings)[4] = IParameters::FOCUS_MODE_INFINITY;
        (*focusModeStrings)[5] = IParameters::FOCUS_MODE_MACRO;
        (*focusModeStrings)[6] = IParameters::FOCUS_MODE_FIXED;

        AutoPtr<ArrayOf<Int32> > focusModeInts = ArrayOf<Int32>::Alloc(7);
        (*focusModeInts)[0] = ICameraMetadata::CONTROL_AF_MODE_AUTO;
        (*focusModeInts)[1] = ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE;
        (*focusModeInts)[2] = ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_VIDEO;
        (*focusModeInts)[3] = ICameraMetadata::CONTROL_AF_MODE_EDOF;
        (*focusModeInts)[4] = ICameraMetadata::CONTROL_AF_MODE_OFF;
        (*focusModeInts)[5] = ICameraMetadata::CONTROL_AF_MODE_MACRO;
        (*focusModeInts)[6] = ICameraMetadata::CONTROL_AF_MODE_OFF;

        AutoPtr<IList> afAvail;
        ArrayUtils::ConvertStringListToIntList(
                focusModes, focusModeStrings, focusModeInts, (IList**)&afAvail);

        // No AF modes supported? That's unpossible!
        Int32 size;
        afAvail->GetSize(&size);
        if (afAvail == NULL || size == 0) {
            Slogger::W(TAG, "No AF modes supported (HAL bug); defaulting to AF_MODE_OFF only");
            CArrayList::New(/*capacity*/1, (IList**)&afAvail);
            AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_MODE_OFF);

            afAvail->Add(TO_IINTERFACE(obj));
        }

        AutoPtr<ArrayOf<Int32> > outar;
        ArrayUtils::ToIntArray(afAvail, (ArrayOf<Int32>**)&outar);
        AutoPtr<IArrayOf> outarObj = CoreUtils::Convert(outar);
        m->Set(CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES, TO_IINTERFACE(outarObj));

        if (VERBOSE) {
            String str;
            ListUtils::ListToString(afAvail, &str);
            Slogger::V(TAG, "mapControlAf - control.afAvailableModes set to %s", str.string());
        }
    }
    return;
}

void LegacyMetadataMapper::MapControlAwb(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * control.awbAvailableModes
     */

    {
        AutoPtr<IList> wbModes;
        assert(0);
        //p->GetSupportedWhiteBalance((IList**)&wbModes);

        AutoPtr<ArrayOf<String> > wbModeStrings = ArrayOf<String>::Alloc(8);
        (*wbModeStrings)[0] = IParameters::WHITE_BALANCE_AUTO;
        (*wbModeStrings)[1] = IParameters::WHITE_BALANCE_INCANDESCENT;
        (*wbModeStrings)[2] = IParameters::WHITE_BALANCE_FLUORESCENT;
        (*wbModeStrings)[3] = IParameters::WHITE_BALANCE_WARM_FLUORESCENT;
        (*wbModeStrings)[4] = IParameters::WHITE_BALANCE_DAYLIGHT;
        (*wbModeStrings)[5] = IParameters::WHITE_BALANCE_CLOUDY_DAYLIGHT;
        (*wbModeStrings)[6] = IParameters::WHITE_BALANCE_TWILIGHT;
        (*wbModeStrings)[7] = IParameters::WHITE_BALANCE_SHADE;

        AutoPtr<ArrayOf<Int32> > wbModeInts = ArrayOf<Int32>::Alloc(8);
        (*wbModeInts)[0] = ICameraMetadata::CONTROL_AWB_MODE_AUTO;
        (*wbModeInts)[1] = ICameraMetadata::CONTROL_AWB_MODE_INCANDESCENT;
        (*wbModeInts)[2] = ICameraMetadata::CONTROL_AWB_MODE_FLUORESCENT;
        (*wbModeInts)[3] = ICameraMetadata::CONTROL_AWB_MODE_WARM_FLUORESCENT;
        (*wbModeInts)[4] = ICameraMetadata::CONTROL_AWB_MODE_DAYLIGHT;
        (*wbModeInts)[5] = ICameraMetadata::CONTROL_AWB_MODE_CLOUDY_DAYLIGHT;
        (*wbModeInts)[6] = ICameraMetadata::CONTROL_AWB_MODE_TWILIGHT;
        (*wbModeInts)[7] = ICameraMetadata::CONTROL_AWB_MODE_SHADE;
        // Note that CONTROL_AWB_MODE_OFF is unsupported

        AutoPtr<IList> awbAvail;
        ArrayUtils::ConvertStringListToIntList(
                    wbModes, wbModeStrings, wbModeInts, (IList**)&awbAvail);


        // No AWB modes supported? That's unpossible!
        Int32 size;
        awbAvail->GetSize(&size);
        if (awbAvail == NULL || size == 0) {
            Slogger::W(TAG, "No AWB modes supported (HAL bug); defaulting to AWB_MODE_AUTO only");
            CArrayList::New(/*capacity*/1, (IList**)&awbAvail);
            AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::CONTROL_AWB_MODE_AUTO);
            awbAvail->Add(TO_IINTERFACE(obj));
        }

        AutoPtr<ArrayOf<Int32> > outar;
        ArrayUtils::ToIntArray(awbAvail, (ArrayOf<Int32>**)&outar);
        AutoPtr<IArrayOf> outarObj = CoreUtils::Convert(outar);
        m->Set(CameraCharacteristics::CONTROL_AWB_AVAILABLE_MODES, TO_IINTERFACE(outarObj));

        if (VERBOSE) {
            String str;
            ListUtils::ListToString(awbAvail, &str);
            Slogger::V(TAG, "mapControlAwb - control.awbAvailableModes set to %s", str.string());
        }
    }
    return;
}

void LegacyMetadataMapper::MapControlOther(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * android.control.availableVideoStabilizationModes
     */
    {
        Boolean res;
        assert(0);
        //p->IsVideoStabilizationSupported(&res);
        AutoPtr<ArrayOf<Int32> > stabModes;
        if (res) {
            stabModes = ArrayOf<Int32>::Alloc(2);
            (*stabModes)[0] = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_OFF;
            (*stabModes)[1] = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_ON;
        }
        else {
            stabModes = ArrayOf<Int32>::Alloc(1);
            (*stabModes)[0] = ICameraMetadata::CONTROL_VIDEO_STABILIZATION_MODE_OFF;
        }

        AutoPtr<IArrayOf> stabModesrObj = CoreUtils::Convert(stabModes);
        m->Set(CameraCharacteristics::CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES, TO_IINTERFACE(stabModesrObj));
    }

    /*
     * android.control.maxRegions
     */
    const Int32 AE = 0;
    const Int32 AWB = 1;
    const Int32 AF = 2;

    AutoPtr<ArrayOf<Int32> > maxRegions = ArrayOf<Int32>::Alloc(3);
    p->GetMaxNumMeteringAreas(&((*maxRegions)[AE]));
    (*maxRegions)[AWB] = 0; // AWB regions not supported in API1
    p->GetMaxNumFocusAreas(&((*maxRegions)[AF]));

    if (LIE_ABOUT_AE_MAX_REGIONS) {
        (*maxRegions)[AE] = 0;
    }
    if (LIE_ABOUT_AF_MAX_REGIONS) {
        (*maxRegions)[AF] = 0;
    }

    AutoPtr<IArrayOf> maxRegionsObj = CoreUtils::Convert(maxRegions);
    m->Set(CameraCharacteristics::CONTROL_MAX_REGIONS, TO_IINTERFACE(maxRegionsObj));

    /*
     * android.control.availableEffects
     */
    AutoPtr<IList> effectModes;
    assert(0);
    //p->GetSupportedColorEffects((IList**)&effectModes);
    AutoPtr<ArrayOf<Int32> > supportedEffectModes;
    if (effectModes == NULL) {
        supportedEffectModes = ArrayOf<Int32>::Alloc(0);
    }
    else {
        ArrayUtils::ConvertStringListToIntArray(effectModes, sLegacyEffectMode,
                    sEffectModes, (ArrayOf<Int32>**)&supportedEffectModes);
    }

    AutoPtr<IArrayOf> supportedEffectModesObj = CoreUtils::Convert(supportedEffectModes);
    m->Set(CameraCharacteristics::CONTROL_AVAILABLE_EFFECTS, TO_IINTERFACE(supportedEffectModesObj));

    /*
     * android.control.availableSceneModes
     */
    AutoPtr<IList> sceneModes;
    assert(0);
    //p->GetSupportedSceneModes((IList**)&sceneModes);
    AutoPtr<IList> supportedSceneModes;
    ArrayUtils::ConvertStringListToIntList(sceneModes, sLegacySceneModes,
            sSceneModes, (IList**)&supportedSceneModes);
    if (supportedSceneModes == NULL) { // camera1 doesn't support scene mode settings
        CArrayList::New((IList**)&supportedSceneModes);
        AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_DISABLED);
        supportedSceneModes->Add(TO_IINTERFACE(obj)); // disabled is always available
    }
    Int32 max;
    p->GetMaxNumDetectedFaces(&max);
    if (max > 0) { // always supports FACE_PRIORITY when face detecting
        AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY);
        supportedSceneModes->Add(TO_IINTERFACE(obj));
    }

    AutoPtr<ArrayOf<Int32> > outar;
    ArrayUtils::ToIntArray(supportedSceneModes, (ArrayOf<Int32>**)&outar);
    AutoPtr<IArrayOf> outarObj = CoreUtils::Convert(outar);
    m->Set(CameraCharacteristics::CONTROL_AVAILABLE_SCENE_MODES, TO_IINTERFACE(outarObj));
    return;
}

void LegacyMetadataMapper::MapLens(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     *  We can tell if the lens is fixed focus;
     *  but if it's not, we can't tell the minimum focus distance, so leave it null then.
     */
    String mode;
    p->GetFocusMode(&mode);
    if (VERBOSE) {
        Slogger::V(TAG, "mapLens - focus-mode='%s'", mode.string());
    }

    if (IParameters::FOCUS_MODE_FIXED.Equals(mode)) {
        /*
         * lens.info.minimumFocusDistance
         */
        AutoPtr<IFloat> obj = CoreUtils::Convert(LENS_INFO_MINIMUM_FOCUS_DISTANCE_FIXED_FOCUS);
        m->Set(CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE, TO_IINTERFACE(obj));

        if (VERBOSE) {
            Slogger::V(TAG, "mapLens - lens.info.minimumFocusDistance = 0");
        }
    }
    else {
        if (VERBOSE) {
            Slogger::V(TAG, "mapLens - lens.info.minimumFocusDistance is unknown");
        }
    }

    Float length;
    p->GetFocalLength(&length);
    AutoPtr<ArrayOf<Float> > focalLengths = ArrayOf<Float>::Alloc(1);
    (*focalLengths)[0] = length;
    AutoPtr<IArrayOf> focalLengthsObj = CoreUtils::Convert(focalLengths);
    m->Set(CameraCharacteristics::LENS_INFO_AVAILABLE_FOCAL_LENGTHS, TO_IINTERFACE(focalLengthsObj));
    return;
}

void LegacyMetadataMapper::MapFlash(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    Boolean flashAvailable = FALSE;
    AutoPtr<IList> supportedFlashModes;
    assert(0);
    //p->GetSupportedFlashModes((IList**)&supportedFlashModes);

    if (supportedFlashModes != NULL) {
        // If only 'OFF' is available, we don't really have flash support
        Boolean result;
        AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FLASH_MODE_OFF);
        ListUtils::ListElementsEqualTo(
                supportedFlashModes, TO_IINTERFACE(obj), &result);
        flashAvailable = !result;
    }

    /*
     * flash.info.available
     */
    AutoPtr<IBoolean> focalLengthsObj = CoreUtils::Convert(flashAvailable);
    m->Set(CameraCharacteristics::FLASH_INFO_AVAILABLE, TO_IINTERFACE(focalLengthsObj));
    return;
}

void LegacyMetadataMapper::LegacyMetadataMapper::MapJpeg(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    AutoPtr<IList> thumbnailSizes;
    assert(0);
    //p->GetSupportedJpegThumbnailSizes((IList**)&thumbnailSizes);

    if (thumbnailSizes != NULL) {
        AutoPtr<ArrayOf<ISize*> > sizes;
        ParameterUtils::ConvertSizeListToArray(thumbnailSizes, (ArrayOf<ISize*>**)&sizes);
        AutoPtr<IUtilsSizeAreaComparator> comp;
        CUtilsSizeAreaComparator::New((IUtilsSizeAreaComparator**)&comp);
        Arrays::Sort(sizes, IComparator::Probe(comp));
        AutoPtr<IArrayOf> sizesObj = CoreUtils::Convert(sizes.Get());
        m->Set(CameraCharacteristics::JPEG_AVAILABLE_THUMBNAIL_SIZES, TO_IINTERFACE(sizesObj));
    }
    return;
}

void LegacyMetadataMapper::MapRequest(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * request.availableCapabilities
     */
    AutoPtr<ArrayOf<Int32> > capabilities = ArrayOf<Int32>::Alloc(1);
    (*capabilities)[0] = ICameraMetadata::REQUEST_AVAILABLE_CAPABILITIES_BACKWARD_COMPATIBLE;

    AutoPtr<IArrayOf> capabilitiesObj = CoreUtils::Convert(capabilities);
    m->Set(CameraCharacteristics::REQUEST_AVAILABLE_CAPABILITIES, TO_IINTERFACE(capabilitiesObj));

    /*
     * request.availableCharacteristicsKeys
     */
    {
        // TODO: check if the underlying key is supported before listing a key as available

        // Note: We only list public keys. Native HALs should list ALL keys regardless of visibility.

        AutoPtr<ArrayOf<ICameraCharacteristicsKey*> > availableKeys =
                ArrayOf<ICameraCharacteristicsKey*>::Alloc(33);
        availableKeys->Set(0, CameraCharacteristics::COLOR_CORRECTION_AVAILABLE_ABERRATION_MODES);
        availableKeys->Set(1, CameraCharacteristics::CONTROL_AE_AVAILABLE_ANTIBANDING_MODES);
        availableKeys->Set(2, CameraCharacteristics::CONTROL_AE_AVAILABLE_MODES);
        assert(0);
        //availableKeys->Set(3, CameraCharacteristics::CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES);
        //availableKeys->Set(4, CameraCharacteristics::CONTROL_AE_COMPENSATION_RANGE);
        availableKeys->Set(5, CameraCharacteristics::CONTROL_AE_COMPENSATION_STEP);
        availableKeys->Set(6, CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES);
        availableKeys->Set(7, CameraCharacteristics::CONTROL_AVAILABLE_EFFECTS);
        availableKeys->Set(8, CameraCharacteristics::CONTROL_AVAILABLE_SCENE_MODES);
        availableKeys->Set(9, CameraCharacteristics::CONTROL_AVAILABLE_VIDEO_STABILIZATION_MODES);
        availableKeys->Set(10, CameraCharacteristics::CONTROL_AWB_AVAILABLE_MODES);
        availableKeys->Set(11, CameraCharacteristics::CONTROL_MAX_REGIONS);
        availableKeys->Set(12, CameraCharacteristics::FLASH_INFO_AVAILABLE);
        availableKeys->Set(13, CameraCharacteristics::INFO_SUPPORTED_HARDWARE_LEVEL);
        availableKeys->Set(14, CameraCharacteristics::JPEG_AVAILABLE_THUMBNAIL_SIZES);
        availableKeys->Set(15, CameraCharacteristics::LENS_FACING);
        availableKeys->Set(16, CameraCharacteristics::LENS_INFO_AVAILABLE_FOCAL_LENGTHS);
        availableKeys->Set(17, CameraCharacteristics::NOISE_REDUCTION_AVAILABLE_NOISE_REDUCTION_MODES);
        availableKeys->Set(18, CameraCharacteristics::REQUEST_AVAILABLE_CAPABILITIES);
        availableKeys->Set(19, CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_STREAMS);
        availableKeys->Set(20, CameraCharacteristics::REQUEST_PARTIAL_RESULT_COUNT);
        availableKeys->Set(21, CameraCharacteristics::REQUEST_PIPELINE_MAX_DEPTH);
        availableKeys->Set(22, CameraCharacteristics::SCALER_AVAILABLE_MAX_DIGITAL_ZOOM);
        //                    CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP;
        availableKeys->Set(23, CameraCharacteristics::SCALER_CROPPING_TYPE);
        availableKeys->Set(24, CameraCharacteristics::SENSOR_AVAILABLE_TEST_PATTERN_MODES);
        availableKeys->Set(25, CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE);
        availableKeys->Set(26, CameraCharacteristics::SENSOR_INFO_PHYSICAL_SIZE);
        availableKeys->Set(27, CameraCharacteristics::SENSOR_INFO_PIXEL_ARRAY_SIZE);
        availableKeys->Set(28, CameraCharacteristics::SENSOR_INFO_TIMESTAMP_SOURCE);
        availableKeys->Set(29, CameraCharacteristics::SENSOR_ORIENTATION);
        availableKeys->Set(30, CameraCharacteristics::STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES);
        availableKeys->Set(31, CameraCharacteristics::STATISTICS_INFO_MAX_FACE_COUNT);
        availableKeys->Set(32, CameraCharacteristics::SYNC_MAX_LATENCY);

        AutoPtr<IList> tmp;
        Arrays::AsList(availableKeys, (IList**)&tmp);
        AutoPtr<IList> characteristicsKeys;
        CArrayList::New(ICollection::Probe(tmp), (IList**)&characteristicsKeys);

        /*
         * Add the conditional keys
         */
        AutoPtr<IInterface> obj;
        m->Get(CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE, (IInterface**)&obj);
        if (obj != NULL) {
            characteristicsKeys->Add(TO_IINTERFACE(CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE));
        }

        AutoPtr<ArrayOf<ICameraCharacteristicsKey*> > keyArray;
        characteristicsKeys->ToArray((ArrayOf<IInterface*>**)&keyArray);
        AutoPtr<ArrayOf<Int32> > tagsArray = GetTagsForKeys(keyArray);
        AutoPtr<IArrayOf> tagsArrayObj = CoreUtils::Convert(tagsArray);
        m->Set(CameraCharacteristics::REQUEST_AVAILABLE_CHARACTERISTICS_KEYS, TO_IINTERFACE(tagsArrayObj));
    }

    /*
     * request.availableRequestKeys
     */
    {
        AutoPtr<ArrayOf<ICaptureRequestKey*> > defaultAvailableKeys =
                ArrayOf<ICaptureRequestKey*>::Alloc(27);
        defaultAvailableKeys->Set(0, CaptureRequest::COLOR_CORRECTION_ABERRATION_MODE);
        defaultAvailableKeys->Set(1, CaptureRequest::CONTROL_AE_ANTIBANDING_MODE);
        defaultAvailableKeys->Set(2, CaptureRequest::CONTROL_AE_EXPOSURE_COMPENSATION);
        defaultAvailableKeys->Set(3, CaptureRequest::CONTROL_AE_LOCK);
        defaultAvailableKeys->Set(4, CaptureRequest::CONTROL_AE_MODE);
        assert(0);
        //defaultAvailableKeys->Set(5, CaptureRequest::CONTROL_AE_TARGET_FPS_RANGE);
        defaultAvailableKeys->Set(6, CaptureRequest::CONTROL_AF_MODE);
        defaultAvailableKeys->Set(7, CaptureRequest::CONTROL_AF_TRIGGER);
        defaultAvailableKeys->Set(8, CaptureRequest::CONTROL_AWB_LOCK);
        defaultAvailableKeys->Set(9, CaptureRequest::CONTROL_AWB_MODE);
        defaultAvailableKeys->Set(10, CaptureRequest::CONTROL_CAPTURE_INTENT);
        defaultAvailableKeys->Set(11, CaptureRequest::CONTROL_EFFECT_MODE);
        defaultAvailableKeys->Set(12, CaptureRequest::CONTROL_MODE);
        defaultAvailableKeys->Set(13, CaptureRequest::CONTROL_SCENE_MODE);
        defaultAvailableKeys->Set(14, CaptureRequest::CONTROL_VIDEO_STABILIZATION_MODE);
        defaultAvailableKeys->Set(15, CaptureRequest::FLASH_MODE);
        defaultAvailableKeys->Set(16, CaptureRequest::JPEG_GPS_COORDINATES);
        defaultAvailableKeys->Set(17, CaptureRequest::JPEG_GPS_PROCESSING_METHOD);
        defaultAvailableKeys->Set(18, CaptureRequest::JPEG_GPS_TIMESTAMP);
        defaultAvailableKeys->Set(19, CaptureRequest::JPEG_ORIENTATION);
        defaultAvailableKeys->Set(20, CaptureRequest::JPEG_QUALITY);
        defaultAvailableKeys->Set(21, CaptureRequest::JPEG_THUMBNAIL_QUALITY);
        defaultAvailableKeys->Set(22, CaptureRequest::JPEG_THUMBNAIL_SIZE);
        defaultAvailableKeys->Set(23, CaptureRequest::LENS_FOCAL_LENGTH);
        defaultAvailableKeys->Set(24, CaptureRequest::NOISE_REDUCTION_MODE);
        defaultAvailableKeys->Set(25, CaptureRequest::SCALER_CROP_REGION);
        defaultAvailableKeys->Set(26, CaptureRequest::STATISTICS_FACE_DETECT_MODE);

        AutoPtr<IList> tmp;
        Arrays::AsList(defaultAvailableKeys, (IList**)&tmp);
        AutoPtr<IArrayList> availableKeys;
        CArrayList::New(ICollection::Probe(tmp), (IArrayList**)&availableKeys);

        Int32 meteArea;
        p->GetMaxNumMeteringAreas(&meteArea);
        if (meteArea > 0) {
            availableKeys->Add(TO_IINTERFACE(CaptureRequest::CONTROL_AE_REGIONS));
        }
        Int32 focusArea;
        p->GetMaxNumFocusAreas(&focusArea);
        if (focusArea > 0) {
            availableKeys->Add(TO_IINTERFACE(CaptureRequest::CONTROL_AF_REGIONS));
        }

        AutoPtr<ArrayOf<ICaptureRequestKey*> > availableRequestKeys;
        IList::Probe(availableKeys)->ToArray((ArrayOf<IInterface*>**)&availableRequestKeys);
        AutoPtr<ArrayOf<Int32> > array = GetTagsForKeys(availableRequestKeys);
        AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(array);
        m->Set(CameraCharacteristics::REQUEST_AVAILABLE_REQUEST_KEYS, TO_IINTERFACE(arrayObj));
    }

    /*
     * request.availableResultKeys
     */
    {
        AutoPtr<ArrayOf<ICaptureResultKey*> > defaultAvailableKeys =
                ArrayOf<ICaptureResultKey*>::Alloc(23);
        defaultAvailableKeys->Set(0, CaptureResult::COLOR_CORRECTION_ABERRATION_MODE);
        defaultAvailableKeys->Set(1, CaptureResult::CONTROL_AE_ANTIBANDING_MODE);
        defaultAvailableKeys->Set(2, CaptureResult::CONTROL_AE_EXPOSURE_COMPENSATION);
        defaultAvailableKeys->Set(3, CaptureResult::CONTROL_AE_LOCK);
        defaultAvailableKeys->Set(4, CaptureResult::CONTROL_AE_MODE);
        defaultAvailableKeys->Set(5, CaptureResult::CONTROL_AF_MODE);
        defaultAvailableKeys->Set(6, CaptureResult::CONTROL_AF_STATE);
        defaultAvailableKeys->Set(7, CaptureResult::CONTROL_AWB_MODE);
        defaultAvailableKeys->Set(8, CaptureResult::CONTROL_AWB_LOCK);
        defaultAvailableKeys->Set(9, CaptureResult::CONTROL_MODE);
        defaultAvailableKeys->Set(10, CaptureResult::FLASH_MODE);
        defaultAvailableKeys->Set(11, CaptureResult::JPEG_GPS_COORDINATES);
        defaultAvailableKeys->Set(12, CaptureResult::JPEG_GPS_PROCESSING_METHOD);
        defaultAvailableKeys->Set(13, CaptureResult::JPEG_GPS_TIMESTAMP);
        defaultAvailableKeys->Set(14, CaptureResult::JPEG_ORIENTATION);
        defaultAvailableKeys->Set(15, CaptureResult::JPEG_QUALITY);
        defaultAvailableKeys->Set(16, CaptureResult::JPEG_THUMBNAIL_QUALITY);
        defaultAvailableKeys->Set(17, CaptureResult::LENS_FOCAL_LENGTH);
        defaultAvailableKeys->Set(18, CaptureResult::NOISE_REDUCTION_MODE);
        defaultAvailableKeys->Set(19, CaptureResult::REQUEST_PIPELINE_DEPTH);
        defaultAvailableKeys->Set(20, CaptureResult::SCALER_CROP_REGION);
        defaultAvailableKeys->Set(21, CaptureResult::SENSOR_TIMESTAMP);
        defaultAvailableKeys->Set(22, CaptureResult::STATISTICS_FACE_DETECT_MODE);
//      CaptureResult.STATISTICS_FACES;

        AutoPtr<IList> tmp;
        Arrays::AsList(defaultAvailableKeys, (IList**)&tmp);
        AutoPtr<IList> availableKeys;
        CArrayList::New(ICollection::Probe(tmp), (IList**)&availableKeys);

        Int32 meteArea;
        p->GetMaxNumMeteringAreas(&meteArea);
        if (meteArea > 0) {
            availableKeys->Add(TO_IINTERFACE(CaptureResult::CONTROL_AE_REGIONS));
        }
        Int32 focusArea;
        p->GetMaxNumFocusAreas(&focusArea);
        if (focusArea > 0) {
            availableKeys->Add(TO_IINTERFACE(CaptureResult::CONTROL_AF_REGIONS));
        }

        AutoPtr<ArrayOf<ICaptureResultKey*> > availableResultKeys;
        availableKeys->ToArray((ArrayOf<IInterface*>**)&availableResultKeys);
        AutoPtr<ArrayOf<Int32> > array = GetTagsForKeys(availableResultKeys);
        AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(array);
        m->Set(CameraCharacteristics::REQUEST_AVAILABLE_RESULT_KEYS, TO_IINTERFACE(arrayObj));
    }

    /*
     * request.maxNumOutputStreams
     */
    AutoPtr<ArrayOf<Int32> > outputStreams = ArrayOf<Int32>::Alloc(3);
    (*outputStreams)[0] = REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_RAW; /* RAW */
    (*outputStreams)[1] = REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC; /* Processed & Not-Stalling */
    (*outputStreams)[2] = REQUEST_MAX_NUM_OUTPUT_STREAMS_COUNT_PROC_STALL; /* Processed & Stalling */

    AutoPtr<IArrayOf> outputStreamsObj = CoreUtils::Convert(outputStreams);
    m->Set(CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_STREAMS, TO_IINTERFACE(outputStreamsObj));

    /*
     * request.maxNumInputStreams
     */
    AutoPtr<IInteger32> intObj = CoreUtils::Convert(REQUEST_MAX_NUM_INPUT_STREAMS_COUNT);
    m->Set(CameraCharacteristics::REQUEST_MAX_NUM_INPUT_STREAMS, TO_IINTERFACE(intObj));

    /*
     * request.partialResultCount
     */
    AutoPtr<IInteger32> intObj2 = CoreUtils::Convert(1);
    m->Set(CameraCharacteristics::REQUEST_PARTIAL_RESULT_COUNT, TO_IINTERFACE(intObj2)); // No partial results supported

    /*
     * request.pipelineMaxDepth
     */
    AutoPtr<IByte> byteObj = CoreUtils::ConvertByte((Byte)(REQUEST_PIPELINE_MAX_DEPTH_HAL1 +
            REQUEST_PIPELINE_MAX_DEPTH_OURS));
    m->Set(CameraCharacteristics::REQUEST_PIPELINE_MAX_DEPTH, TO_IINTERFACE(byteObj));
    return;
}

void LegacyMetadataMapper::MapScaler(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * scaler.availableMaxDigitalZoom
     */
    Float value;
    ParameterUtils::GetMaxZoomRatio(p, &value);
    AutoPtr<IFloat> valueObj = CoreUtils::Convert(value);
    m->Set(CameraCharacteristics::SCALER_AVAILABLE_MAX_DIGITAL_ZOOM, TO_IINTERFACE(valueObj));

    /*
     * scaler.croppingType = CENTER_ONLY
     */
    AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::SCALER_CROPPING_TYPE_CENTER_ONLY);
    m->Set(CameraCharacteristics::SCALER_CROPPING_TYPE, TO_IINTERFACE(intObj));
    return;
}

void LegacyMetadataMapper::MapSensor(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    // Use the largest jpeg size (by area) for both active array and pixel array
    AutoPtr<ISize> largestJpegSize;
    ParameterUtils::GetLargestSupportedJpegSizeByArea(p, (ISize**)&largestJpegSize);
    /*
     * sensor.info.activeArraySize
     */
    {
        AutoPtr<IRect> activeArrayRect;
        ParamsUtils::CreateRect(largestJpegSize, (IRect**)&activeArrayRect);
        m->Set(CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE, TO_IINTERFACE(activeArrayRect));
    }

    /*
     * sensor.availableTestPatternModes
     */
    {
        // Only "OFF" test pattern mode is available
        AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(1);
        (*tmp)[0] = ICameraMetadata::SENSOR_TEST_PATTERN_MODE_OFF;
        AutoPtr<IArrayOf> tmpObj = CoreUtils::Convert(tmp);
        m->Set(CameraCharacteristics::SENSOR_AVAILABLE_TEST_PATTERN_MODES, TO_IINTERFACE(tmpObj));
    }

    /*
     * sensor.info.pixelArraySize
     */
    m->Set(CameraCharacteristics::SENSOR_INFO_PIXEL_ARRAY_SIZE, TO_IINTERFACE(largestJpegSize));

    /*
     * sensor.info.physicalSize
     */
    {
        /*
         * Assume focal length is at infinity focus and that the lens is rectilinear.
         */
        Float focalLength;
        p->GetFocalLength(&focalLength); // in mm
        Float horizontal;
        p->GetHorizontalViewAngle(&horizontal);
        Double angleHor = horizontal * Elastos::Core::Math::PI / 180; // to radians
        Float vertical;
        p->GetVerticalViewAngle(&vertical);
        Double angleVer = vertical * Elastos::Core::Math::PI / 180; // to radians

        Float height = (Float)Elastos::Core::Math::Abs(2 * focalLength * Elastos::Core::Math::Tan(angleVer / 2));
        Float width = (Float)Elastos::Core::Math::Abs(2 * focalLength * Elastos::Core::Math::Tan(angleHor / 2));

        AutoPtr<ISizeF> tmp;
        CSizeF::New(width, height, (ISizeF**)&tmp);
        m->Set(CameraCharacteristics::SENSOR_INFO_PHYSICAL_SIZE, TO_IINTERFACE(tmp)); // in mm
    }

    /*
     * sensor.info.timestampSource
     */
    {
        AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::SENSOR_INFO_TIMESTAMP_SOURCE_UNKNOWN);
        m->Set(CameraCharacteristics::SENSOR_INFO_TIMESTAMP_SOURCE, TO_IINTERFACE(intObj));
    }
    return;
}

void LegacyMetadataMapper::MapStatistics(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * statistics.info.availableFaceDetectModes
     */
    AutoPtr<IArrayList> fdModes;

    Int32 maxNum;
    p->GetMaxNumDetectedFaces(&maxNum);
    if (maxNum > 0) {
        CArrayList::New(2, (IArrayList**)&fdModes);
        AutoPtr<IInteger32> obj1 = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF);
        AutoPtr<IInteger32> obj2 = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE);
        fdModes->Set(0, TO_IINTERFACE(obj1));
        fdModes->Set(1, TO_IINTERFACE(obj2));
        // FULL is never-listed, since we have no way to query it statically
    }
    else {
        CArrayList::New(1, (IArrayList**)&fdModes);
        AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF);
        fdModes->Set(0, TO_IINTERFACE(obj));
    }
    m->Set(CameraCharacteristics::STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES, TO_IINTERFACE(fdModes));

    /*
     * statistics.info.maxFaceCount
     */

    AutoPtr<IInteger32> obj = CoreUtils::Convert(maxNum);
    m->Set(CameraCharacteristics::STATISTICS_INFO_MAX_FACE_COUNT, TO_IINTERFACE(obj));
    return;
}

void LegacyMetadataMapper::MapSync(
    /* [in] */ ICameraMetadataNative* m,
    /* [in] */ IParameters* p)
{
    /*
     * sync.maxLatency
     */
    AutoPtr<IInteger32> obj = CoreUtils::Convert(ICameraMetadata::SYNC_MAX_LATENCY_UNKNOWN);
    m->Set(CameraCharacteristics::SYNC_MAX_LATENCY, TO_IINTERFACE(obj));
    return;
}

void LegacyMetadataMapper::AppendStreamConfig(
    /* [in] */ IArrayList* configs,
    /* [in] */ Int32 format,
    /* [in] */ IList* sizes)
{
    Int32 _size;
    sizes->GetSize(&_size);
    for (Int32 i = 0; i < _size; i++) {
        AutoPtr<IInterface> obj;
        sizes->Get(i, (IInterface**)&obj);
        AutoPtr<ICameraSize> size = ICameraSize::Probe(obj);

        AutoPtr<IStreamConfiguration> config;
        Int32 width;
        size->GetWidth(&width);
        Int32 height;
        size->GetHeight(&height);
        CStreamConfiguration::New(format, width, height,
                /*input*/FALSE, (IStreamConfiguration**)&config);
        configs->Add(config);
    }
    return;
}

static AutoPtr<ArrayOf<String> > initLegacySceneModes()
{
    AutoPtr<ArrayOf<String> > tmp = ArrayOf<String>::Alloc(17);

    (*tmp)[0] = IParameters::SCENE_MODE_AUTO;
    (*tmp)[1] = IParameters::SCENE_MODE_ACTION;
    (*tmp)[2] = IParameters::SCENE_MODE_PORTRAIT;
    (*tmp)[3] = IParameters::SCENE_MODE_LANDSCAPE;
    (*tmp)[4] = IParameters::SCENE_MODE_NIGHT;
    (*tmp)[5] = IParameters::SCENE_MODE_NIGHT_PORTRAIT;
    (*tmp)[6] = IParameters::SCENE_MODE_THEATRE;
    (*tmp)[7] = IParameters::SCENE_MODE_BEACH;
    (*tmp)[8] = IParameters::SCENE_MODE_SNOW;
    (*tmp)[9] = IParameters::SCENE_MODE_SUNSET;
    (*tmp)[10] = IParameters::SCENE_MODE_STEADYPHOTO;
    (*tmp)[11] = IParameters::SCENE_MODE_FIREWORKS;
    (*tmp)[12] = IParameters::SCENE_MODE_SPORTS;
    (*tmp)[13] = IParameters::SCENE_MODE_PARTY;
    (*tmp)[14] = IParameters::SCENE_MODE_CANDLELIGHT;
    (*tmp)[15] = IParameters::SCENE_MODE_BARCODE;
    (*tmp)[16] = IParameters::SCENE_MODE_HDR;
    return tmp;
}

AutoPtr<ArrayOf<String> > LegacyMetadataMapper::sLegacySceneModes = initLegacySceneModes();

static AutoPtr<ArrayOf<Int32> > initSceneModes()
{
    AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(17);

    (*tmp)[0] = ICameraMetadata::CONTROL_SCENE_MODE_DISABLED;
    (*tmp)[1] = ICameraMetadata::CONTROL_SCENE_MODE_ACTION;
    (*tmp)[2] = ICameraMetadata::CONTROL_SCENE_MODE_PORTRAIT;
    (*tmp)[3] = ICameraMetadata::CONTROL_SCENE_MODE_LANDSCAPE;
    (*tmp)[4] = ICameraMetadata::CONTROL_SCENE_MODE_NIGHT;
    (*tmp)[5] = ICameraMetadata::CONTROL_SCENE_MODE_NIGHT_PORTRAIT;
    (*tmp)[6] = ICameraMetadata::CONTROL_SCENE_MODE_THEATRE;
    (*tmp)[7] = ICameraMetadata::CONTROL_SCENE_MODE_BEACH;
    (*tmp)[8] = ICameraMetadata::CONTROL_SCENE_MODE_SNOW;
    (*tmp)[9] = ICameraMetadata::CONTROL_SCENE_MODE_SUNSET;
    (*tmp)[10] = ICameraMetadata::CONTROL_SCENE_MODE_STEADYPHOTO;
    (*tmp)[11] = ICameraMetadata::CONTROL_SCENE_MODE_FIREWORKS;
    (*tmp)[12] = ICameraMetadata::CONTROL_SCENE_MODE_SPORTS;
    (*tmp)[13] = ICameraMetadata::CONTROL_SCENE_MODE_PARTY;
    (*tmp)[14] = ICameraMetadata::CONTROL_SCENE_MODE_CANDLELIGHT;
    (*tmp)[15] = ICameraMetadata::CONTROL_SCENE_MODE_BARCODE;
    (*tmp)[16] = ICameraMetadata::CONTROL_SCENE_MODE_HDR;
    return tmp;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::sSceneModes = initSceneModes();

Int32 LegacyMetadataMapper::ConvertSceneModeFromLegacy(
    /* [in] */ const String& mode)
{
    if (mode == NULL) {
        return ICameraMetadata::CONTROL_SCENE_MODE_DISABLED;
    }
    Int32 index;
    assert(0);
    //ArrayUtils::GetArrayIndex(sLegacySceneModes, mode, &index);
    if (index < 0) {
        return UNKNOWN_MODE;
    }
    return (*sSceneModes)[index];
}

String LegacyMetadataMapper::ConvertSceneModeToLegacy(
    /* [in] */ Int32 mode)
{
    if (mode == ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY) {
        // OK: Let LegacyFaceDetectMapper handle turning face detection on/off
        return IParameters::SCENE_MODE_AUTO;
    }

    Int32 index;
    ArrayUtils::GetArrayIndex(sSceneModes, mode, &index);
    if (index < 0) {
        return String(NULL);
    }
    return (*sLegacySceneModes)[index];
}

static AutoPtr<ArrayOf<String> > initLegacyEffectMode()
{
    AutoPtr<ArrayOf<String> > tmp = ArrayOf<String>::Alloc(9);

    (*tmp)[0] = IParameters::EFFECT_NONE;
    (*tmp)[1] = IParameters::EFFECT_MONO;
    (*tmp)[2] = IParameters::EFFECT_NEGATIVE;
    (*tmp)[3] = IParameters::EFFECT_SOLARIZE;
    (*tmp)[4] = IParameters::EFFECT_SEPIA;
    (*tmp)[5] = IParameters::EFFECT_POSTERIZE;
    (*tmp)[6] = IParameters::EFFECT_WHITEBOARD;
    (*tmp)[7] = IParameters::EFFECT_BLACKBOARD;
    (*tmp)[8] = IParameters::EFFECT_AQUA;
    return tmp;
}

AutoPtr<ArrayOf<String> > LegacyMetadataMapper::sLegacyEffectMode = initLegacyEffectMode();

static AutoPtr<ArrayOf<Int32> > initEffectModes()
{
    AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(9);

    (*tmp)[0] = ICameraMetadata::CONTROL_EFFECT_MODE_OFF;
    (*tmp)[1] = ICameraMetadata::CONTROL_EFFECT_MODE_MONO;
    (*tmp)[2] = ICameraMetadata::CONTROL_EFFECT_MODE_NEGATIVE;
    (*tmp)[3] = ICameraMetadata::CONTROL_EFFECT_MODE_SOLARIZE;
    (*tmp)[4] = ICameraMetadata::CONTROL_EFFECT_MODE_SEPIA;
    (*tmp)[5] = ICameraMetadata::CONTROL_EFFECT_MODE_POSTERIZE;
    (*tmp)[6] = ICameraMetadata::CONTROL_EFFECT_MODE_WHITEBOARD;
    (*tmp)[7] = ICameraMetadata::CONTROL_EFFECT_MODE_BLACKBOARD;
    (*tmp)[8] = ICameraMetadata::CONTROL_EFFECT_MODE_AQUA;
    return tmp;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::sEffectModes = initEffectModes();

Int32 LegacyMetadataMapper::ConvertEffectModeFromLegacy(
    /* [in] */ const String& mode)
{
    if (mode == NULL) {
        return ICameraMetadata::CONTROL_EFFECT_MODE_OFF;
    }
    Int32 index;
    assert(0);
    //ArrayUtils::GetArrayIndex(sLegacyEffectMode, mode, &index);
    if (index < 0) {
        return UNKNOWN_MODE;
    }
    return (*sEffectModes)[index];
}

String LegacyMetadataMapper::ConvertEffectModeToLegacy(
    /* [in] */ Int32 mode)
{
    Int32 index;
    ArrayUtils::GetArrayIndex(sEffectModes, mode, &index);
    if (index < 0) {
        return String(NULL);
    }
    return (*sLegacyEffectMode)[index];
}

Int32 LegacyMetadataMapper::ConvertAntiBandingMode(
    /* [in] */ const String& mode)
{
    if (mode.IsNull()) {
        return -1;
    }

    if (mode == IParameters::ANTIBANDING_OFF) {
        return ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_OFF;
    }
    else if (mode == IParameters::ANTIBANDING_50HZ) {
        return ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_50HZ;
    }
    else if (mode == IParameters::ANTIBANDING_60HZ) {
        return ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_60HZ;
    }
    else if (mode == IParameters::ANTIBANDING_AUTO) {
        return ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_AUTO;
    }
    else {
        Slogger::W(TAG, "convertAntiBandingMode - Unknown antibanding mode %s", mode.string());
        return -1;
    }
}

Int32 LegacyMetadataMapper::ConvertAntiBandingModeOrDefault(
    /* [in] */ const String& mode)
{
    Int32 antiBandingMode = ConvertAntiBandingMode(mode);
    if (antiBandingMode == -1) {
        return ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_OFF;
    }

    return antiBandingMode;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::ConvertAeFpsRangeToLegacy(
    /* [in] */ Range<IInteger32>* fpsRange)
{
    AutoPtr<ArrayOf<Int32> > legacyFps = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IInteger32> obj = fpsRange->GetLower();
    Int32 lower;
    obj->GetValue(&lower);
    (*legacyFps)[IParameters::PREVIEW_FPS_MIN_INDEX] = lower;
    Int32 upper;
    obj = NULL;
    obj = fpsRange->GetUpper();
    obj->GetValue(&upper);
    (*legacyFps)[IParameters::PREVIEW_FPS_MAX_INDEX] = upper;
    return legacyFps;
}

Int64 LegacyMetadataMapper::CalculateJpegStallDuration(
    /* [in] */ ICameraSize* size)
{
    Int64 baseDuration = APPROXIMATE_CAPTURE_DELAY_MS * NS_PER_MS; // 200ms for capture
    Int32 width;
    size->GetWidth(&width);
    Int32 height;
    size->GetHeight(&height);
    Int64 area = width * (Int64)height;
    Int64 stallPerArea = APPROXIMATE_JPEG_ENCODE_TIME_MS * NS_PER_MS /
            APPROXIMATE_SENSOR_AREA_PX; // 600ms stall for 8mp
    return baseDuration + area * stallPerArea;
}

ECode LegacyMetadataMapper::ConvertRequestMetadata(
    /* [in] */ ILegacyRequest* request)
{
    return LegacyRequestMapper::ConvertRequestMetadata(request);
}

static AutoPtr<ArrayOf<Int32> > initAllowedTemplates()
{
    AutoPtr<ArrayOf<Int32> > tmp = ArrayOf<Int32>::Alloc(3);

    (*tmp)[0] = ICameraDevice::TEMPLATE_PREVIEW;
    (*tmp)[1] = ICameraDevice::TEMPLATE_STILL_CAPTURE;
    (*tmp)[2] = ICameraDevice::TEMPLATE_RECORD;
    // Disallowed templates in legacy mode:
    // CameraDevice.TEMPLATE_VIDEO_SNAPSHOT,
    // CameraDevice.TEMPLATE_ZERO_SHUTTER_LAG,
    // CameraDevice.TEMPLATE_MANUAL
    return tmp;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::sAllowedTemplates = initAllowedTemplates();

ECode LegacyMetadataMapper::CreateRequestTemplate(
    /* [in] */ ICameraCharacteristics* c,
    /* [in] */ Int32 templateId,
    /* [out] */ ICameraMetadataNative** outcm)
{
    VALIDATE_NOT_NULL(outcm)
    *outcm = NULL;

    Boolean result;
    ArrayUtils::Contains(sAllowedTemplates, templateId, &result);
    if (!result) {
        //throw new IllegalArgumentException("templateId out of range");
        Slogger::E(TAG, "templateId out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ICameraMetadataNative> m;
    CCameraMetadataNative::New((ICameraMetadataNative**)&m);

    /*
     * NOTE: If adding new code here and it needs to query the static info,
     * query the camera characteristics, so we can reuse this for api2 code later
     * to create our own templates in the framework
     */

    /*
     * control.*
     */

    // control.awbMode
    AutoPtr<IInteger32> awbModeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AWB_MODE_AUTO);
    m->Set(CaptureRequest::CONTROL_AWB_MODE, TO_IINTERFACE(awbModeObj));
    // AWB is always unconditionally available in API1 devices

    // control.aeAntibandingMode
    AutoPtr<IInteger32> aeAntibandingModeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_ANTIBANDING_MODE_AUTO);
    m->Set(CaptureRequest::CONTROL_AE_ANTIBANDING_MODE, TO_IINTERFACE(aeAntibandingModeObj));

    // control.aeExposureCompensation
    AutoPtr<IInteger32> aeExposureCompensationeObj = CoreUtils::Convert(0);
    m->Set(CaptureRequest::CONTROL_AE_EXPOSURE_COMPENSATION, TO_IINTERFACE(aeExposureCompensationeObj));

    // control.aeLock
    AutoPtr<IBoolean> aeLockObj = CoreUtils::Convert(FALSE);
    m->Set(CaptureRequest::CONTROL_AE_LOCK, TO_IINTERFACE(aeLockObj));

    // control.aePrecaptureTrigger
    AutoPtr<IInteger32> aePrecaptureTriggerObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_PRECAPTURE_TRIGGER_IDLE);
    m->Set(CaptureRequest::CONTROL_AE_PRECAPTURE_TRIGGER, TO_IINTERFACE(aePrecaptureTriggerObj));

    // control.afTrigger
    AutoPtr<IInteger32> afTriggerObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AF_TRIGGER_IDLE);
    m->Set(CaptureRequest::CONTROL_AF_TRIGGER, TO_IINTERFACE(afTriggerObj));

    // control.awbMode
    AutoPtr<IInteger32> controlawbModeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AWB_MODE_AUTO);
    m->Set(CaptureRequest::CONTROL_AWB_MODE, TO_IINTERFACE(controlawbModeObj));

    // control.awbLock
    AutoPtr<IBoolean> awbLockObj = CoreUtils::Convert(FALSE);
    m->Set(CaptureRequest::CONTROL_AWB_LOCK, TO_IINTERFACE(awbLockObj));

    // control.aeRegions, control.awbRegions, control.afRegions
    {
        AutoPtr<IInterface> obj;
        c->Get(CameraCharacteristics::SENSOR_INFO_ACTIVE_ARRAY_SIZE, (IInterface**)&obj);
        AutoPtr<IRect> activeArray = IRect::Probe(obj);
        AutoPtr<ArrayOf<IMeteringRectangle*> > activeRegions =
                ArrayOf<IMeteringRectangle*>::Alloc(1);
        AutoPtr<IMeteringRectangle> tmp;
        Int32 width;
        activeArray->GetWidth(&width);
        Int32 height;
        activeArray->GetHeight(&height);
        CMeteringRectangle::New(/*x*/0, /*y*/0, /*width*/width - 1,
                /*height*/height - 1,/*weight*/0, (IMeteringRectangle**)&tmp);
        activeRegions->Set(0, tmp);
        AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(activeRegions.Get());
        m->Set(CaptureRequest::CONTROL_AE_REGIONS, TO_IINTERFACE(arrayObj));
        m->Set(CaptureRequest::CONTROL_AWB_REGIONS, TO_IINTERFACE(arrayObj));
        m->Set(CaptureRequest::CONTROL_AF_REGIONS, TO_IINTERFACE(arrayObj));
    }

    // control.captureIntent
    {
        Int32 captureIntent;
        switch (templateId) {
            case ICameraDevice::TEMPLATE_PREVIEW:
                captureIntent = ICameraMetadata::CONTROL_CAPTURE_INTENT_PREVIEW;
                break;
            case ICameraDevice::TEMPLATE_STILL_CAPTURE:
                captureIntent = ICameraMetadata::CONTROL_CAPTURE_INTENT_STILL_CAPTURE;
                break;
            case ICameraDevice::TEMPLATE_RECORD:
                captureIntent = ICameraMetadata::CONTROL_CAPTURE_INTENT_VIDEO_RECORD;
                break;
            default:
                // Can't get anything else since it's guarded by the IAE check
                //throw new AssertionError("Impossible; keep in sync with sAllowedTemplates");
                Slogger::E(TAG, "Impossible; keep in sync with sAllowedTemplates");
                return E_ASSERTION_ERROR;
        }
        AutoPtr<IInteger32> intObj = CoreUtils::Convert(captureIntent);
        m->Set(CaptureRequest::CONTROL_CAPTURE_INTENT, TO_IINTERFACE(intObj));
    }

    // control.aeMode
    AutoPtr<IInteger32> aeModeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_AE_MODE_ON);
    m->Set(CaptureRequest::CONTROL_AE_MODE, TO_IINTERFACE(aeModeObj));
    // AE is always unconditionally available in API1 devices

    // control.mode
    AutoPtr<IInteger32> modeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_MODE_AUTO);
    m->Set(CaptureRequest::CONTROL_MODE, TO_IINTERFACE(modeObj));

    // control.afMode
    {
        AutoPtr<IInterface> tmp;
        c->Get(CameraCharacteristics::LENS_INFO_MINIMUM_FOCUS_DISTANCE, (IInterface**)&tmp);
        AutoPtr<IFloat> floatObj = IFloat::Probe(tmp);
        Float minimumFocusDistance;
        Int32 afMode = 0;
        if (floatObj != NULL) {
            floatObj->GetValue(&minimumFocusDistance);
            if (minimumFocusDistance == LENS_INFO_MINIMUM_FOCUS_DISTANCE_FIXED_FOCUS) {
                // Cannot control auto-focus with fixed-focus cameras
                afMode = ICameraMetadata::CONTROL_AF_MODE_OFF;
            }
        }
        else {
            // If a minimum focus distance is reported; the camera must have AF
            afMode = ICameraMetadata::CONTROL_AF_MODE_AUTO;

            if (templateId == ICameraDevice::TEMPLATE_RECORD ||
                    templateId == ICameraDevice::TEMPLATE_VIDEO_SNAPSHOT) {
                AutoPtr<IInterface> obj;
                c->Get(CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES, (IInterface**)&obj);
                AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
                assert(0);
                Boolean result;
                //ArrayUtils::Contains(arrayObj, ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_VIDEO, &result);
                if (result) {
                    afMode = ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_VIDEO;
                }
            }
            else if (templateId == ICameraDevice::TEMPLATE_PREVIEW ||
                    templateId == ICameraDevice::TEMPLATE_STILL_CAPTURE) {
                AutoPtr<IInterface> obj;
                c->Get(CameraCharacteristics::CONTROL_AF_AVAILABLE_MODES, (IInterface**)&obj);
                AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
                assert(0);
                Boolean result;
                //ArrayUtils::Contains(arrayObj, ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE, &result);
                if (result) {
                    afMode = ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE;
                }
            }
        }

        if (VERBOSE) {
            Slogger::V(TAG, "createRequestTemplate (templateId=%d), afMode=%d,"
                    "minimumFocusDistance=%d", templateId, afMode, minimumFocusDistance);
        }
        AutoPtr<IInteger32> afModeObj = CoreUtils::Convert(afMode);
        m->Set(CaptureRequest::CONTROL_AF_MODE, TO_IINTERFACE(afModeObj));
    }

    {
        // control.aeTargetFpsRange
        AutoPtr<IInterface> obj;
        assert(0);
        //c->Get(CameraCharacteristics::CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES, (IInterface**)&obj);
        AutoPtr<IArrayOf> availableFpsRange = IArrayOf::Probe(obj);

        // Pick FPS range with highest max value, tiebreak on higher min value
        AutoPtr<IInterface> tmp;
        availableFpsRange->Get(0, (IInterface**)&tmp);
        AutoPtr<Range<IInteger32> > bestRange = (Range<IInteger32>*)IObject::Probe(tmp);
        Int32 size;
        availableFpsRange->GetLength(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> obj;
            availableFpsRange->Get(i, (IInterface**)&obj);
            AutoPtr<Range<IInteger32> > r = (Range<IInteger32>*)IObject::Probe(obj);

            AutoPtr<IInteger32> bUpper = bestRange->GetUpper();
            Int32 bUp;
            bUpper->GetValue(&bUp);
            AutoPtr<IInteger32> rUpper = r->GetUpper();
            Int32 rUp;
            rUpper->GetValue(&rUp);
            if (bUp < rUp) {
                bestRange = r;
            }
            else {
                if (bUp == rUp) {
                    AutoPtr<IInteger32> bLower = bestRange->GetLower();
                    Int32 bLow;
                    bLower->GetValue(&bLow);
                    AutoPtr<IInteger32> rLower = r->GetLower();
                    Int32 rLow;
                    rLower->GetValue(&rLow);
                    if (bLow < rLow) {
                        bestRange = r;
                    }
                }
            }
        }
        assert(0);
        //m->Set(CaptureRequest::CONTROL_AE_TARGET_FPS_RANGE, bestRange);
    }

    // control.sceneMode -- DISABLED is always available
    AutoPtr<IInteger32> sceneModeObj = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_DISABLED);
    m->Set(CaptureRequest::CONTROL_SCENE_MODE, TO_IINTERFACE(sceneModeObj));

    /*
     * statistics.*
     */

    // statistics.faceDetectMode
    AutoPtr<IInteger32> faceDetectModeObj = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF);
    m->Set(CaptureRequest::STATISTICS_FACE_DETECT_MODE, TO_IINTERFACE(faceDetectModeObj));

    /*
     * flash.*
     */

    // flash.mode
    AutoPtr<IInteger32> flashModeObj = CoreUtils::Convert(ICameraMetadata::FLASH_MODE_OFF);
    m->Set(CaptureRequest::FLASH_MODE, TO_IINTERFACE(flashModeObj));

    /*
     * noiseReduction.*
     */
    AutoPtr<IInteger32> noiseReductionObj = CoreUtils::Convert(ICameraMetadata::NOISE_REDUCTION_MODE_FAST);
    m->Set(CaptureRequest::NOISE_REDUCTION_MODE, TO_IINTERFACE(noiseReductionObj));

    /*
     * lens.*
     */

    // lens.focalLength
    AutoPtr<IInterface> focalLengthObj;
    c->Get(CameraCharacteristics::LENS_INFO_AVAILABLE_FOCAL_LENGTHS, (IInterface**)&focalLengthObj);
    AutoPtr<IArrayOf> focalLengthArray = IArrayOf::Probe(focalLengthObj);
    AutoPtr<IInterface> floatObj;
    focalLengthArray->Get(0, (IInterface**)&floatObj);
    m->Set(CaptureRequest::LENS_FOCAL_LENGTH, floatObj);

    /*
     * jpeg.*
     */

    // jpeg.thumbnailSize - set smallest non-zero size if possible
    AutoPtr<IInterface> thumbnailSizeObj;
    c->Get(CameraCharacteristics::JPEG_AVAILABLE_THUMBNAIL_SIZES, (IInterface**)&thumbnailSizeObj);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(thumbnailSizeObj);
    Int32 length;
    array->GetLength(&length);
    AutoPtr<IInterface> tmp;

    if (length > 1) {
        array->Get(1, (IInterface**)&tmp);
    }
    else {
        array->Get(0, (IInterface**)&tmp);
    }
    m->Set(CaptureRequest::JPEG_THUMBNAIL_SIZE, tmp);

    // TODO: map other request template values
    *outcm = m;
    REFCOUNT_ADD(*outcm);
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::GetTagsForKeys(
    /* [in] */ ArrayOf<ICameraCharacteristicsKey*>* keys)
{
    AutoPtr<ArrayOf<Int32> > tags = ArrayOf<Int32>::Alloc(keys->GetLength());

    for (Int32 i = 0; i < keys->GetLength(); ++i) {
        AutoPtr<ICameraMetadataNativeKey> nativeKey;
        (*keys)[i]->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
        nativeKey->GetTag(&((*tags)[i]));
    }

    return tags;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::GetTagsForKeys(
    /* [in] */ ArrayOf<ICaptureRequestKey*>* keys)
{
    AutoPtr<ArrayOf<Int32> > tags = ArrayOf<Int32>::Alloc(keys->GetLength());

    for (Int32 i = 0; i < keys->GetLength(); ++i) {
        AutoPtr<ICameraMetadataNativeKey> nativeKey;
        (*keys)[i]->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
        nativeKey->GetTag(&((*tags)[i]));
    }

    return tags;
}

AutoPtr<ArrayOf<Int32> > LegacyMetadataMapper::GetTagsForKeys(
    /* [in] */ ArrayOf<ICaptureResultKey*>* keys)
{
    AutoPtr<ArrayOf<Int32> > tags = ArrayOf<Int32>::Alloc(keys->GetLength());

    for (Int32 i = 0; i < keys->GetLength(); ++i) {
        AutoPtr<ICameraMetadataNativeKey> nativeKey;
        (*keys)[i]->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
        nativeKey->GetTag(&((*tags)[i]));
    }

    return tags;
}

String LegacyMetadataMapper::ConvertAfModeToLegacy(
    /* [in] */ Int32 mode,
    /* [in] */ IList* supportedFocusModes)
{
    if (supportedFocusModes == NULL) {
        Slogger::W(TAG, "No focus modes supported; API1 bug");
        return String(NULL);
    }

    Boolean result;
    supportedFocusModes->IsEmpty(&result);
    if (result) {
        Slogger::W(TAG, "No focus modes supported; API1 bug");
        return String(NULL);
    }

    String param;
    switch (mode) {
        case ICameraMetadata::CONTROL_AF_MODE_AUTO:
            param = IParameters::FOCUS_MODE_AUTO;
            break;
        case ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_PICTURE:
            param = IParameters::FOCUS_MODE_CONTINUOUS_PICTURE;
            break;
        case ICameraMetadata::CONTROL_AF_MODE_CONTINUOUS_VIDEO:
            param = IParameters::FOCUS_MODE_CONTINUOUS_VIDEO;
            break;
        case ICameraMetadata::CONTROL_AF_MODE_EDOF:
            param = IParameters::FOCUS_MODE_EDOF;
            break;
        case ICameraMetadata::CONTROL_AF_MODE_MACRO:
            param = IParameters::FOCUS_MODE_MACRO;
            break;
        case ICameraMetadata::CONTROL_AF_MODE_OFF:
        {
            Boolean result;
            AutoPtr<ICharSequence> obj = CoreUtils::Convert(IParameters::FOCUS_MODE_FIXED);
            supportedFocusModes->Contains(TO_IINTERFACE(obj), &result);
            if (result) {
                param = IParameters::FOCUS_MODE_FIXED;
            }
            else {
                param = IParameters::FOCUS_MODE_INFINITY;
            }
        }
    }

    AutoPtr<ICharSequence> tmp = CoreUtils::Convert(param);
    supportedFocusModes->Contains(TO_IINTERFACE(tmp), &result);
    if (!result) {
        // Weed out bad user input by setting to the first arbitrary focus mode
        AutoPtr<IInterface> obj;
        supportedFocusModes->Get(0, (IInterface**)&obj);
        AutoPtr<ICharSequence> sque = ICharSequence::Probe(obj);
        String defaultMode;
        sque->ToString(&defaultMode);
        Slogger::W(TAG, "convertAfModeToLegacy - ignoring unsupported mode %d,"
                "defaulting to %s", mode, defaultMode.string());
        param = defaultMode;
    }

    return param;
}

} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
