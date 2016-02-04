
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/hardware/camera2/legacy/LegacyFaceDetectMapper.h"
#include "elastos/droid/hardware/camera2/legacy/ParameterUtils.h"
#include "elastos/droid/hardware/camera2/utils/ListUtils.h"
#include "elastos/droid/hardware/camera2/utils/ParamsUtils.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Utils::ListUtils;
using Elastos::Droid::Hardware::Camera2::Utils::ParamsUtils;
using Elastos::Droid::Hardware::Camera2::Params::IFace;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Legacy {

CAR_INTERFACE_IMPL(LegacyFaceDetectMapper, Object, ILegacyFaceDetectMapper)

const String LegacyFaceDetectMapper::TAG("LegacyFaceDetectMapper");
const Boolean LegacyFaceDetectMapper::VERBOSE = FALSE;// = Log.isLoggable(TAG, Log.VERBOSE);

CAR_INTERFACE_IMPL(LegacyFaceDetectMapper::MyListener, Object, IFaceDetectionListener)

LegacyFaceDetectMapper::MyListener::MyListener(
    /* [in] */ LegacyFaceDetectMapper* host)
    : mHost(host)
{
}

ECode LegacyFaceDetectMapper::MyListener::OnFaceDetection(
    /* [in] */ ArrayOf<ICameraFace*>* faces,
    /* [in] */ IHardwareCamera* camera)
{
    Int32 lengthFaces = faces == NULL ? 0 : faces->GetLength();
    Object& _lock = mHost->mLock;
    synchronized (_lock) {
        if (mHost->mFaceDetectEnabled) {
            mHost->mFaces = faces;
        }
        else if (lengthFaces > 0) {
            // stopFaceDetectMode could race against the requests, print a debug log
            Slogger::D(TAG,
                    "onFaceDetection - Ignored some incoming faces since"
                    "face detection was disabled");
        }
    }

    if (mHost->VERBOSE) {
        Slogger::V(TAG, "onFaceDetection - read %d faces", lengthFaces);
    }
    return NOERROR;
}

LegacyFaceDetectMapper::LegacyFaceDetectMapper()
    : mFaceDetectSupported(FALSE)
    , mFaceDetectEnabled(FALSE)
    , mFaceDetectScenePriority(FALSE)
    , mFaceDetectReporting(FALSE)
{
}

ECode LegacyFaceDetectMapper::constructor()
{
    return NOERROR;
}

ECode LegacyFaceDetectMapper::constructor(
    /* [in] */ IHardwareCamera* camera,
    /* [in] */ ICameraCharacteristics* characteristics)
{
    FAIL_RETURN(Preconditions::CheckNotNull(camera, String("camera must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(characteristics,
            String("characteristics must not be null")))
    mCamera = camera;

    AutoPtr<IInterface> obj;
    characteristics->Get(CameraCharacteristics::STATISTICS_INFO_AVAILABLE_FACE_DETECT_MODES,
            (IInterface**)&obj);
    AutoPtr<IArrayList> list = IArrayList::Probe(obj);
    AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE);
    mFaceDetectSupported = ArrayUtils::Contains(list, TO_IINTERFACE(intObj));

    if (!mFaceDetectSupported) {
        return NOERROR;
    }

    AutoPtr<MyListener> listener = new MyListener(this);
    return mCamera->SetFaceDetectionListener(listener);
}

ECode LegacyFaceDetectMapper::ProcessFaceDetectMode(
    /* [in] */ ICaptureRequest* captureRequest,
    /* [in] */ IParameters* parameters)
{
    FAIL_RETURN(Preconditions::CheckNotNull(captureRequest, String("captureRequest must not be null")))

    /*
     * statistics.faceDetectMode
     */
    AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF);
    AutoPtr<IInterface> obj;
    ParamsUtils::GetOrDefault(captureRequest, CaptureRequest::STATISTICS_FACE_DETECT_MODE,
            TO_IINTERFACE(intObj), (IInterface**)&obj);
    AutoPtr<IInteger32> modeObj = IInteger32::Probe(obj);
    Int32 fdMode;
    modeObj->GetValue(&fdMode);

    if (fdMode != ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF && !mFaceDetectSupported) {
        Slogger::W(TAG,
                "processFaceDetectMode - Ignoring statistics.faceDetectMode;"
                "face detection is not available");
        return NOERROR;
    }

    /*
     * control.sceneMode
     */
    obj = NULL;
    AutoPtr<IInteger32> intObj2 = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_DISABLED);
    ParamsUtils::GetOrDefault(captureRequest, CaptureRequest::CONTROL_SCENE_MODE,
            TO_IINTERFACE(intObj2), (IInterface**)&obj);
    modeObj = NULL;
    modeObj = IInteger32::Probe(obj);
    Int32 sceneMode;
    modeObj->GetValue(&sceneMode);
    if (sceneMode == ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY && !mFaceDetectSupported) {
        Slogger::W(TAG, "processFaceDetectMode - ignoring control.sceneMode == FACE_PRIORITY;"
                "face detection is not available");
        return NOERROR;
    }

    // Print some warnings out in case the values were wrong
    switch (fdMode) {
        case ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF:
        case ICameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE:
            break;
        case ICameraMetadata::STATISTICS_FACE_DETECT_MODE_FULL:
            Slogger::W(TAG,
                    "processFaceDetectMode - statistics.faceDetectMode == FULL unsupported,"
                    "downgrading to SIMPLE");
            break;
        default:
            Slogger::W(TAG, "processFaceDetectMode - ignoring unknown statistics.faceDetectMode = %d"
                    ,fdMode);
            return NOERROR;
    }

    Boolean enableFaceDetect = (fdMode != ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF)
            || (sceneMode == ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY);
    synchronized(mLock) {
        // Enable/disable face detection if it's changed since last time
        if (enableFaceDetect != mFaceDetectEnabled) {
            if (enableFaceDetect) {
                mCamera->StartFaceDetection();

                if (VERBOSE) {
                    Slogger::V(TAG, "processFaceDetectMode - start face detection");
                }
            }
            else {
                mCamera->StopFaceDetection();

                if (VERBOSE) {
                    Slogger::V(TAG, "processFaceDetectMode - stop face detection");
                }

                mFaces = NULL;
            }

            mFaceDetectEnabled = enableFaceDetect;
            mFaceDetectScenePriority = sceneMode == ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY;
            mFaceDetectReporting = fdMode != ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF;
        }
    }
    return NOERROR;
}

ECode LegacyFaceDetectMapper::MapResultFaces(
    /* [in] */ ICameraMetadataNative* result,
    /* [in] */ ILegacyRequest* legacyRequest)
{
    FAIL_RETURN(Preconditions::CheckNotNull(result, String("result must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(legacyRequest, String("legacyRequest must not be null")))

    AutoPtr<ArrayOf<ICameraFace*> > faces;
    AutoPtr<ArrayOf<ICameraFace*> > previousFaces;
    Int32 fdMode = 0;
    Boolean fdScenePriority = FALSE;
    synchronized(mLock) {
        fdMode = mFaceDetectReporting ?
                ICameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE :
                ICameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF;

        if (mFaceDetectReporting) {
            faces = mFaces;
        }
        else {
            faces = NULL;
        }

        fdScenePriority = mFaceDetectScenePriority;

        previousFaces = mFacesPrev;
        mFacesPrev = faces;
    }

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
    AutoPtr<IInterface> obj2;
    request->Get(CaptureRequest::SCALER_CROP_REGION, (IInterface**)&obj2);
    AutoPtr<IParameterUtilsZoomData> zoomData;
    ParameterUtils::ConvertScalerCropRegion(activeArray, IRect::Probe(obj2),
            previewSize, params, (IParameterUtilsZoomData**)&zoomData);

    AutoPtr<IList> convertedFaces;
    CArrayList::New((IList**)&convertedFaces);
    if (faces != NULL) {
        for (Int32 i = 0; i < faces->GetLength(); i++) {
            AutoPtr<ICameraFace> face = (*faces)[i];
            if (face != NULL) {
                AutoPtr<IFace> tmp;
                ParameterUtils::ConvertFaceFromLegacy(face, activeArray, zoomData, (IFace**)&tmp);
                convertedFaces->Add(TO_IINTERFACE(tmp));
            }
            else {
                Slogger::W(TAG, "mapResultFaces - read NULL face from camera1 device");
            }
        }
    }

    if (VERBOSE && previousFaces != faces) { // Log only in verbose and IF the faces changed
        String str;
        ListUtils::ListToString(convertedFaces, &str);
        Slogger::V(TAG, "mapResultFaces - changed to %s", str.string());
    }

    AutoPtr<IInterface> res;

    AutoPtr<ArrayOf<IInterface*> > array;
    convertedFaces->ToArray((ArrayOf<IInterface*>**)&array);
    AutoPtr<IArrayOf> arrayObj = CoreUtils::Convert(array.Get());
    result->Set(CaptureResult::STATISTICS_FACES, TO_IINTERFACE(arrayObj));
    AutoPtr<IInteger32> objMode = CoreUtils::Convert(fdMode);
    result->Set(CaptureResult::STATISTICS_FACE_DETECT_MODE, TO_IINTERFACE(objMode));

    // Override scene mode with FACE_PRIORITY if the request was using FACE_PRIORITY
    if (fdScenePriority) {
        AutoPtr<IInteger32> intObj = CoreUtils::Convert(ICameraMetadata::CONTROL_SCENE_MODE_FACE_PRIORITY);
        result->Set(CaptureResult::CONTROL_SCENE_MODE, TO_IINTERFACE(intObj));
    }
    return NOERROR;
}


} // namespace Legacy
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
