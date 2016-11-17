
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/CCaptureRequestKey.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNativeKey;
using Elastos::Droid::Hardware::Camera2::Impl::CameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CTonemapCurve;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CFace;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Params::EIID_ITonemapCurve;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IFace;
using Elastos::Droid::Hardware::Camera2::Params::EIID_IMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Droid::Graphics::ECLSID_CRect;
using Elastos::Droid::Graphics::EIID_IRect;
using Elastos::Droid::Location::ECLSID_CLocation;
using Elastos::Droid::Location::EIID_ILocation;
using Elastos::Droid::Utility::ECLSID_CSize;
using Elastos::Droid::Utility::ECLSID_CRational;
using Elastos::Droid::Utility::ECLSID_CRange;
using Elastos::Droid::Utility::EIID_ISize;
using Elastos::Droid::Utility::EIID_IRational;
using Elastos::Droid::Utility::EIID_IRange;
using Elastos::Core::StringBuilder;
using Elastos::Core::ECLSID_CArrayOf;
using Elastos::Core::ECLSID_CString;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CDouble;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::ECLSID_CBoolean;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::EIID_IString;
using Elastos::Core::EIID_IByte;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::EIID_IInteger64;
using Elastos::Core::EIID_IBoolean;
using Elastos::Core::ICloneable;
using Elastos::Utility::CHashSet;
using Elastos::Utility::Objects;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {

CAR_INTERFACE_IMPL(CaptureRequestKey, Object, ICaptureRequestKey)

CaptureRequestKey::CaptureRequestKey()
{
}

ECode CaptureRequestKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    mKey = new CameraMetadataNativeKey(name,  classId, interfaceId);
    return NOERROR;
}

ECode CaptureRequestKey::constructor(
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

ECode CaptureRequestKey::constructor(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
{
    mKey = new CameraMetadataNativeKey(name, typeReference);
    return NOERROR;
}

ECode CaptureRequestKey::constructor(
    /* [in] */ ICameraMetadataNativeKey* nativeKey)
{
    mKey = nativeKey;
    return NOERROR;
}

ECode CaptureRequestKey::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    return mKey->GetName(name);
}

ECode CaptureRequestKey::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    return IObject::Probe(mKey)->GetHashCode(code);
}

ECode CaptureRequestKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* e)
{
    VALIDATE_NOT_NULL(e);
    *e = FALSE;

    CaptureRequestKey* other = (CaptureRequestKey*)ICaptureRequestKey::Probe(obj);
    if (other != NULL) {
        *e = Object::Equals(other->mKey, mKey);
    }
    return NOERROR;
}

ECode CaptureRequestKey::GetNativeKey(
    /* [out] */ ICameraMetadataNativeKey** key)
{
    VALIDATE_NOT_NULL(key);

    *key = mKey;
    REFCOUNT_ADD(*key);
    return NOERROR;
}

ECode CaptureRequestKey::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CaptureRequestKey:{");
    sb += TO_CSTR(mKey);
    sb += "}";
    *str = sb.ToString();
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

static AutoPtr<ICaptureRequestKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId)
{
    AutoPtr<ICaptureRequestKey> key;
    CCaptureRequestKey::New(String(name), classId, interfaceId, (ICaptureRequestKey**)&key);
    return key;
}

static AutoPtr<ICaptureRequestKey> CreateKey(
    /* [in] */ const char* name,
    /* [in] */ const ClassID& classId,
    /* [in] */ const InterfaceID& interfaceId,
    /* [in] */ const ClassID& componentClassId,
    /* [in] */ const InterfaceID& componentInterfaceId)
{
    AutoPtr<ICaptureRequestKey> key;
    CCaptureRequestKey::New(String(name), classId, interfaceId,
        componentClassId, componentInterfaceId, (ICaptureRequestKey**)&key);
    return key;
}

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_MODE =
    CreateKey("android.colorCorrection.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_TRANSFORM =
    CreateKey("android.colorCorrection.transform", ECLSID_CColorSpaceTransform, EIID_IColorSpaceTransform);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_GAINS =
    CreateKey("android.colorCorrection.gains", ECLSID_CRggbChannelVector, EIID_IRggbChannelVector);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::COLOR_CORRECTION_ABERRATION_MODE =
    CreateKey("android.colorCorrection.aberrationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_ANTIBANDING_MODE =
    CreateKey("android.control.aeAntibandingMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_EXPOSURE_COMPENSATION =
    CreateKey("android.control.aeExposureCompensation", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_LOCK =
    CreateKey("android.control.aeLock", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_MODE =
    CreateKey("android.control.aeMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_REGIONS =
    CreateKey("android.control.aeRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

//android.util.Range<Integer>
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_TARGET_FPS_RANGE =
    CreateKey("android.control.aeTargetFpsRange", ECLSID_CRange, EIID_IRange,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AE_PRECAPTURE_TRIGGER =
    CreateKey("android.control.aePrecaptureTrigger", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_MODE =
    CreateKey("android.control.afMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_REGIONS =
    CreateKey("android.control.afRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AF_TRIGGER =
    CreateKey("android.control.afTrigger", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_LOCK =
    CreateKey("android.control.awbLock", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_MODE =
    CreateKey("android.control.awbMode", ECLSID_CInteger32, EIID_IInteger32);

//android.hardware.camera2.params.MeteringRectangle[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_AWB_REGIONS =
    CreateKey("android.control.awbRegions", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CMeteringRectangle, EIID_IMeteringRectangle);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_CAPTURE_INTENT =
    CreateKey("android.control.captureIntent", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_EFFECT_MODE =
    CreateKey("android.control.effectMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_MODE =
    CreateKey("android.control.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_SCENE_MODE =
    CreateKey("android.control.sceneMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::CONTROL_VIDEO_STABILIZATION_MODE =
    CreateKey("android.control.videoStabilizationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::EDGE_MODE =
    CreateKey("android.edge.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::FLASH_MODE =
    CreateKey("android.flash.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::HOT_PIXEL_MODE =
    CreateKey("android.hotPixel.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_LOCATION =
    CreateKey("android.jpeg.gpsLocation", ECLSID_CLocation, EIID_ILocation);

//double[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_COORDINATES =
    CreateKey("android.jpeg.gpsCoordinates", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CDouble, EIID_IDouble);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_PROCESSING_METHOD =
    CreateKey("android.jpeg.gpsProcessingMethod", ECLSID_CString, EIID_IString);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_GPS_TIMESTAMP =
    CreateKey("android.jpeg.gpsTimestamp", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_ORIENTATION =
    CreateKey("android.jpeg.orientation", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_QUALITY =
    CreateKey("android.jpeg.quality", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_THUMBNAIL_QUALITY =
    CreateKey("android.jpeg.thumbnailQuality", ECLSID_CByte, EIID_IByte);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::JPEG_THUMBNAIL_SIZE =
    CreateKey("android.jpeg.thumbnailSize", ECLSID_CSize, EIID_ISize);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_APERTURE =
    CreateKey("android.lens.aperture", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FILTER_DENSITY =
    CreateKey("android.lens.filterDensity", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FOCAL_LENGTH =
    CreateKey("android.lens.focalLength", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_FOCUS_DISTANCE =
    CreateKey("android.lens.focusDistance", ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LENS_OPTICAL_STABILIZATION_MODE =
    CreateKey("android.lens.opticalStabilizationMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::NOISE_REDUCTION_MODE =
    CreateKey("android.noiseReduction.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::REQUEST_ID =
    CreateKey("android.request.id", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SCALER_CROP_REGION =
    CreateKey("android.scaler.cropRegion", ECLSID_CRect, EIID_IRect);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_EXPOSURE_TIME =
    CreateKey("android.sensor.exposureTime", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_FRAME_DURATION =
    CreateKey("android.sensor.frameDuration", ECLSID_CInteger64, EIID_IInteger64);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_SENSITIVITY =
    CreateKey("android.sensor.sensitivity", ECLSID_CInteger32, EIID_IInteger32);

//int[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_TEST_PATTERN_DATA =
    CreateKey("android.sensor.testPatternData", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SENSOR_TEST_PATTERN_MODE =
    CreateKey("android.sensor.testPatternMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::SHADING_MODE =
    CreateKey("android.shading.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_FACE_DETECT_MODE =
    CreateKey("android.statistics.faceDetectMode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_HOT_PIXEL_MAP_MODE =
    CreateKey("android.statistics.hotPixelMapMode", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::STATISTICS_LENS_SHADING_MAP_MODE =
    CreateKey("android.statistics.lensShadingMapMode", ECLSID_CInteger32, EIID_IInteger32);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_BLUE =
    CreateKey("android.tonemap.curveBlue", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_GREEN =
    CreateKey("android.tonemap.curveGreen", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

//float[].class
INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE_RED =
    CreateKey("android.tonemap.curveRed", ECLSID_CArrayOf, EIID_IArrayOf,
        ECLSID_CFloat, EIID_IFloat);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_CURVE =
    CreateKey("android.tonemap.curve", ECLSID_CTonemapCurve, EIID_ITonemapCurve);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::TONEMAP_MODE =
    CreateKey("android.tonemap.mode", ECLSID_CInteger32, EIID_IInteger32);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::LED_TRANSMIT =
    CreateKey("android.led.transmit", ECLSID_CBoolean, EIID_IBoolean);

INIT_PROI_3 const AutoPtr<ICaptureRequestKey> CaptureRequest::BLACK_LEVEL_LOCK =
    CreateKey("android.blackLevel.lock", ECLSID_CBoolean, EIID_IBoolean);

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