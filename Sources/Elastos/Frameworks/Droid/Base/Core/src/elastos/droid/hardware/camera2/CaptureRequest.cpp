
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include <elastos/utility/Objects.h>

using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CTonemapCurve;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Location::ECLSID_CLocation;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Core::ICloneable;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Objects;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CaptureRequest::Key, Object, ICaptureRequestKey)

CaptureRequest::Key::Key()
{
}

ECode CaptureRequest::Key::constructor()
{
    return NOERROR;
}

ECode CaptureRequest::Key::constructor(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNative::Key(name,  type);
    return NOERROR;
}

ECode CaptureRequest::Key::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNative::Key(name,  typeReference);
    return NOERROR;
}

ECode CaptureRequest::Key::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
}

CaptureRequest::Key::Key(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
}

CaptureRequest::Key::Key(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
{
    mKey = new CameraMetadataNative::Key(name,  type);
}

ECode CaptureRequest::Key::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mKey->GetName(name);
}

ECode CaptureRequest::Key::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    return IObject::Probe(mKey)->GetHashCode(code);
}

ECode CaptureRequest::Key::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;

    if (ICaptureRequestKey::Probe(obj) != NULL) {
        Key* k =(Key*)ICaptureRequestKey::Probe(obj);
        Boolean res;
        IObject::Probe(k->mKey)->Equals(mKey, &res);
        if (res) {
            *e = TRUE;
            return NOERROR;
        }
        else {
            *e = FALSE;
            return NOERROR;
        }
    }
    *e = FALSE;
    return NOERROR;
}

ECode CaptureRequest::Key::GetNativeKey(
    /* [out] */ ICameraMetadataNativeKey** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CaptureRequest::Builder, Object, ICaptureRequestBuilder)

CaptureRequest::Builder::Builder()
{
}

ECode CaptureRequest::Builder::constructor()
{
    return NOERROR;
}

ECode CaptureRequest::Builder::constructor(
    /* [in] */ ICameraMetadataNative* _template)
{
    mRequest = new CaptureRequest(_template);
    return NOERROR;
}

ECode CaptureRequest::Builder::AddTarget(
    /* [in] */ ISurface* outputTarget)
{
    return ((CaptureRequest*)mRequest.Get())->mSurfaceSet->Add(TO_IINTERFACE(outputTarget));
}

ECode CaptureRequest::Builder::RemoveTarget(
    /* [in] */ ISurface* outputTarget)
{
    return ((CaptureRequest*)mRequest.Get())->mSurfaceSet->Remove(TO_IINTERFACE(outputTarget));
}

ECode CaptureRequest::Builder::Set(
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* value)
{
    return ((CaptureRequest*)mRequest.Get())->mSettings->Set(key, value);
}

ECode CaptureRequest::Builder::Get(
    /* [in] */ ICaptureRequestKey* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    return ((CaptureRequest*)mRequest.Get())->mSettings->Get(key, result);
}

ECode CaptureRequest::Builder::SetTag(
    /* [in] */ IInterface* tag)
{
    ((CaptureRequest*)mRequest.Get())->mUserTag = tag;
    return NOERROR;
}

ECode CaptureRequest::Builder::Build(
    /* [out] */ ICaptureRequest** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICaptureRequest> tmp = new CaptureRequest(mRequest);
    *result = tmp;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CaptureRequest::Builder::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return ((CaptureRequest*)mRequest.Get())->mSettings->IsEmpty(result);
}

const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_MODE =
        new Key(String("android.colorCorrection.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_TRANSFORM =
        new Key(String("android.colorCorrection.transform"), ECLSID_CColorSpaceTransform);

const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_GAINS =
        new Key(String("android.colorCorrection.gains"), ECLSID_CRggbChannelVector);

const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_ABERRATION_MODE =
        new Key(String("android.colorCorrection.aberrationMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_ANTIBANDING_MODE =
        new Key(String("android.control.aeAntibandingMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_EXPOSURE_COMPENSATION =
        new Key(String("android.control.aeExposureCompensation"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_LOCK =
        new Key(String("android.control.aeLock"), ECLSID_CBoolean);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_MODE =
        new Key(String("android.control.aeMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_REGIONS =
        new Key(String("android.control.aeRegions"), ECLSID_CArrayOf);

//android.util.Range<Integer>
// const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_TARGET_FPS_RANGE =
//         new Key(String("android.control.aeTargetFpsRange"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_PRECAPTURE_TRIGGER =
        new Key(String("android.control.aePrecaptureTrigger"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_MODE =
        new Key(String("android.control.afMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_REGIONS =
        new Key(String("android.control.afRegions"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_TRIGGER =
        new Key(String("android.control.afTrigger"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_LOCK =
        new Key(String("android.control.awbLock"), ECLSID_CBoolean);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_MODE =
        new Key(String("android.control.awbMode"), ECLSID_CInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_REGIONS =
        new Key(String("android.control.awbRegions"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_CAPTURE_INTENT =
        new Key(String("android.control.captureIntent"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_EFFECT_MODE =
        new Key(String("android.control.effectMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_MODE =
        new Key(String("android.control.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_SCENE_MODE =
        new Key(String("android.control.sceneMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_VIDEO_STABILIZATION_MODE =
        new Key(String("android.control.videoStabilizationMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::EDGE_MODE =
        new Key(String("android.edge.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::FLASH_MODE =
        new Key(String("android.flash.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::HOT_PIXEL_MODE =
        new Key(String("android.hotPixel.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_LOCATION =
        new Key(String("android.jpeg.gpsLocation"), ECLSID_CLocation);

//double[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_COORDINATES =
        new Key(String("android.jpeg.gpsCoordinates"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_PROCESSING_METHOD =
        new Key(String("android.jpeg.gpsProcessingMethod"), ECLSID_CString);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_TIMESTAMP =
        new Key(String("android.jpeg.gpsTimestamp"), ECLSID_CInteger64);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_ORIENTATION =
        new Key(String("android.jpeg.orientation"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_QUALITY =
        new Key(String("android.jpeg.quality"), ECLSID_CByte);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_THUMBNAIL_QUALITY =
        new Key(String("android.jpeg.thumbnailQuality"), ECLSID_CByte);

const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_THUMBNAIL_SIZE =
        new Key(String("android.jpeg.thumbnailSize"), ECLSID_CSize);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_APERTURE =
        new Key(String("android.lens.aperture"), ECLSID_CFloat);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FILTER_DENSITY =
        new Key(String("android.lens.filterDensity"), ECLSID_CFloat);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FOCAL_LENGTH =
        new Key(String("android.lens.focalLength"), ECLSID_CFloat);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FOCUS_DISTANCE =
        new Key(String("android.lens.focusDistance"), ECLSID_CFloat);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_OPTICAL_STABILIZATION_MODE =
        new Key(String("android.lens.opticalStabilizationMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::NOISE_REDUCTION_MODE =
        new Key(String("android.noiseReduction.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::REQUEST_ID =
        new Key(String("android.request.id"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SCALER_CROP_REGION =
        new Key(String("android.scaler.cropRegion"), ECLSID_CRect);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_EXPOSURE_TIME =
        new Key(String("android.sensor.exposureTime"), ECLSID_CInteger64);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_FRAME_DURATION =
        new Key(String("android.sensor.frameDuration"), ECLSID_CInteger64);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_SENSITIVITY =
        new Key(String("android.sensor.sensitivity"), ECLSID_CInteger32);

//int[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_TEST_PATTERN_DATA =
        new Key(String("android.sensor.testPatternData"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_TEST_PATTERN_MODE =
        new Key(String("android.sensor.testPatternMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::SHADING_MODE =
        new Key(String("android.shading.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_FACE_DETECT_MODE =
        new Key(String("android.statistics.faceDetectMode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_HOT_PIXEL_MAP_MODE =
        new Key(String("android.statistics.hotPixelMapMode"), ECLSID_CBoolean);

const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_LENS_SHADING_MAP_MODE =
        new Key(String("android.statistics.lensShadingMapMode"), ECLSID_CInteger32);

//float[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_BLUE =
        new Key(String("android.tonemap.curveBlue"), ECLSID_CArrayOf);

//float[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_GREEN =
        new Key(String("android.tonemap.curveGreen"), ECLSID_CArrayOf);

//float[].class
const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_RED =
        new Key(String("android.tonemap.curveRed"), ECLSID_CArrayOf);

const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE =
        new Key(String("android.tonemap.curve"), ECLSID_CTonemapCurve);

const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_MODE =
        new Key(String("android.tonemap.mode"), ECLSID_CInteger32);

const AutoPtr<ICaptureRequestKey> CaptureRequest::LED_TRANSMIT =
        new Key(String("android.led.transmit"), ECLSID_CBoolean);

const AutoPtr<ICaptureRequestKey> CaptureRequest::BLACK_LEVEL_LOCK =
        new Key(String("android.blackLevel.lock"), ECLSID_CBoolean);

CAR_INTERFACE_IMPL_2(CaptureRequest, CameraMetadata, ICaptureRequest, IParcelable)

CaptureRequest::CaptureRequest()
{
    CCameraMetadataNative::New((ICameraMetadataNative**)&mSettings);
    CHashSet::New((IHashSet**)&mSurfaceSet);
}

CaptureRequest::CaptureRequest(
    /* [in] */ ICaptureRequest* source)
{
    CCameraMetadataNative::New(((CaptureRequest*)source)->mSettings,
            (ICameraMetadataNative**)&mSettings);
    AutoPtr<IInterface> obj;
    ICloneable::Probe(((CaptureRequest*)source)->mSurfaceSet)->Clone((IInterface**)&obj);
    mSurfaceSet = IHashSet::Probe(obj);
    mUserTag = ((CaptureRequest*)source)->mUserTag;
}

CaptureRequest::CaptureRequest(
    /* [in] */ ICameraMetadataNative* settings)
{
    CameraMetadataNative::Move(settings, (ICameraMetadataNative**)&mSettings);
    CHashSet::New((IHashSet**)&mSurfaceSet);
}

ECode CaptureRequest::Get(
    /* [in] */ ICaptureRequestKey* key,
    /* [out] */ IInterface** face)
{
    VALIDATE_NOT_NULL(face)

    return mSettings->Get(key, face);
}

ECode CaptureRequest::GetTargets(
    /* [out] */ ICollection** targets)
{
    VALIDATE_NOT_NULL(targets)

    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    return coll->UnmodifiableCollection(ICollection::Probe(mSurfaceSet), targets);
}

ECode CaptureRequest::GetProtected(
    /* [in] */ IInterface* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    return mSettings->Get(ICaptureRequestKey::Probe(key), outface);
}

ECode CaptureRequest::GetKeyClass(
    /* [out] */ ClassID* id)
{
    VALIDATE_NOT_NULL(id)

    *id = ECLSID_CCaptureRequestKey;
    return NOERROR;
}

ECode CaptureRequest::GetKeys(
    /* [out] */ IList** outlist)
{
    VALIDATE_NOT_NULL(outlist)

    return CameraMetadata::GetKeys(outlist);
}

ECode CaptureRequest::GetTag(
    /* [out] */ IInterface** tag)
{
    VALIDATE_NOT_NULL(tag)

    *tag = mUserTag;
    REFCOUNT_ADD(*tag);
    return NOERROR;
}

ECode CaptureRequest::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (ICaptureRequest::Probe(other) != NULL) {
        return Equals(ICaptureRequest::Probe(other), result);
    }
    return NOERROR;
}

ECode CaptureRequest::Equals(
    /* [in] */ ICaptureRequest* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (other != NULL) {
        CaptureRequest* tmp = (CaptureRequest*)other;
        if (Objects::Equals(mUserTag, tmp->mUserTag)) {
            if (mSurfaceSet->Equals(tmp->mSurfaceSet, result), result) {
                if (IObject::Probe(mSettings)->Equals(tmp->mSettings, result), result) {
                    return NOERROR;
                }
            }
        }
    }
    return NOERROR;
}

ECode CaptureRequest::GetHashCode(
    /* [out] */ Int32* code)
{
    return HashCodeHelpers::GetHashCode(IInterface::Probe(mSettings),
            IInterface::Probe(mSurfaceSet), mUserTag, code);
}


ECode CaptureRequest::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    IParcelable::Probe(mSettings)->WriteToParcel(dest);
    AutoPtr<ArrayOf<IInterface*> > objArray;
    mSurfaceSet->ToArray((ArrayOf<IInterface*>**)&objArray);
    return dest->WriteArrayOf((Handle32)objArray.Get());
}

ECode CaptureRequest::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    IParcelable::Probe(mSettings)->ReadFromParcel(source);

    mSurfaceSet->Clear();

    AutoPtr<ArrayOf<IInterface*> > objArray;
    source->ReadArrayOf((Handle32*)(&objArray));

    if (objArray == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < objArray->GetLength(); i++) {
        AutoPtr<ISurface> s = ISurface::Probe((*objArray)[i]);
        mSurfaceSet->Add(IInterface::Probe(s));
    }

    return NOERROR;
}

CARAPI CaptureRequest::ContainsTarget(
    /* [in] */ ISurface* surface,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    return mSurfaceSet->Contains(IInterface::Probe(surface), result);
}

} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos