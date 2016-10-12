
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/impl/CameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/impl/CCameraMetadataNative.h"
#include "elastos/droid/hardware/camera2/utils/TypeReference.h"
#include "elastos/droid/hardware/camera2/params/CFace.h"
#include "elastos/droid/hardware/camera2/params/CLensShadingMap.h"
#include "elastos/droid/hardware/camera2/params/CStreamConfigurationMap.h"
#include "elastos/droid/hardware/camera2/params/CTonemapCurve.h"
#include "elastos/droid/hardware/camera2/CameraCharacteristics.h"
#include "elastos/droid/hardware/camera2/CaptureResult.h"
#include "elastos/droid/hardware/camera2/CaptureRequest.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalRegistry.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryablePrimitive.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableEnum.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableArray.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableBoolean.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableNativeByteToInteger.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableRect.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableSize.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableSizeF.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableString.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableReprocessFormatsMap.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableRange.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryablePair.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableMeteringRectangle.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableColorSpaceTransform.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableStreamConfiguration.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableStreamConfigurationDuration.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableRggbChannelVector.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableBlackLevelPattern.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableHighSpeedVideoConfiguration.h"
#include "elastos/droid/hardware/camera2/marshal/impl/CMarshalQueryableParcelable.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/location/CLocation.h"
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <binder/IServiceManager.h>
#include <camera/ICameraService.h>
#include <camera/VendorTagDescriptor.h>
#include <sys/types.h> // for socketpair
#include <sys/socket.h> // for socketpair
#include <utils/String8.h>
#include <utils/String16.h>
#include <utils/SortedVector.h>

using Elastos::Droid::Hardware::Camera2::Utils::TypeReference;
using Elastos::Droid::Hardware::Camera2::Impl::CCameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::CFace;
using Elastos::Droid::Hardware::Camera2::Params::CLensShadingMap;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::IStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Params::IHighSpeedVideoConfiguration;
using Elastos::Droid::Hardware::Camera2::Params::CStreamConfigurationMap;
using Elastos::Droid::Hardware::Camera2::Params::ITonemapCurve;
using Elastos::Droid::Hardware::Camera2::Params::CTonemapCurve;
using Elastos::Droid::Hardware::Camera2::Marshal::IMarshalQueryable;
using Elastos::Droid::Hardware::Camera2::Marshal::MarshalRegistry;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryablePrimitive;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableEnum;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableArray;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableBoolean;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableNativeByteToInteger;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableRect;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableSize;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableSizeF;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableString;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableReprocessFormatsMap;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableRange;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryablePair;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableMeteringRectangle;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableColorSpaceTransform;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableStreamConfiguration;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableStreamConfigurationDuration;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableRggbChannelVector;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableBlackLevelPattern;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableHighSpeedVideoConfiguration;
using Elastos::Droid::Hardware::Camera2::Marshal::Impl::CMarshalQueryableParcelable;

using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::ISize;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::AutoLock;
using Elastos::Core::IArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::Math;
using Elastos::Core::EIID_IDouble;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Impl {

const String TAG("CameraMetadataNative");
const Boolean VERBOSE = TRUE;

//==============================================================================
// GetCommand
//==============================================================================
CAR_INTERFACE_IMPL(GetCommand_AvailableFormats, Object, IGetCommand)

ECode GetCommand_AvailableFormats::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ArrayOf<Int32> > array = metadataNative->GetAvailableFormats();
    AutoPtr<IArrayOf> _outface = CoreUtils::Convert(array.Get());
    *outface = _outface;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_Faces, Object, IGetCommand)

ECode GetCommand_Faces::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ArrayOf<IFace*> > array = metadataNative->GetFaces();
    AutoPtr<IArrayOf> _outface = CoreUtils::Convert(array.Get());
    *outface = _outface;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_FaceRectangles, Object, IGetCommand)

ECode GetCommand_FaceRectangles::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ArrayOf<IRect*> > array = metadataNative->GetFaceRectangles();
    AutoPtr<IArrayOf> _outface = CoreUtils::Convert(array.Get());
    *outface = _outface;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_StreamConfigurationMap, Object, IGetCommand)

ECode GetCommand_StreamConfigurationMap::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<IStreamConfigurationMap> map = metadataNative->GetStreamConfigurationMap();
    *outface = map;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_MaxRegions, Object, IGetCommand)

ECode GetCommand_MaxRegions::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    Int32 regions;
    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->GetMaxRegions(key, &regions);
    AutoPtr<IInteger32> _outface = CoreUtils::Convert(regions);
    *outface = _outface;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_MaxNumOutputs, Object, IGetCommand)

ECode GetCommand_MaxNumOutputs::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    Int32 num;
    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->GetMaxNumOutputs(key, &num);
    AutoPtr<IInteger32> _outface = CoreUtils::Convert(num);
    *outface = _outface;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_TonemapCurve, Object, IGetCommand)

ECode GetCommand_TonemapCurve::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ITonemapCurve> curve = metadataNative->GetTonemapCurve();
    *outface = curve;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_GpsLocation, Object, IGetCommand)

ECode GetCommand_GpsLocation::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ILocation> location = metadataNative->GetGpsLocation();
    *outface = location;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

CAR_INTERFACE_IMPL(GetCommand_LensShadingMap, Object, IGetCommand)

ECode GetCommand_LensShadingMap::GetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    AutoPtr<ILensShadingMap> map = metadataNative->GetLensShadingMap();
    *outface = map;
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

//==============================================================================
// SetCommand
//==============================================================================

CAR_INTERFACE_IMPL(SetCommand_AvailableFormats, Object, ISetCommand)

ECode SetCommand_AvailableFormats::SetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ IInterface* value)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);
        Int32 _value;
        intObj->GetValue(&_value);
        (*values)[i] = _value;
    }

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->SetAvailableFormats(values);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SetCommand_FaceRectangles, Object, ISetCommand)

ECode SetCommand_FaceRectangles::SetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ IInterface* value)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<IRect*> > values = ArrayOf<IRect*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        AutoPtr<IRect> tmp = IRect::Probe(obj);
        values->Set(i, tmp);
    }

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->SetFaceRectangles(values);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SetCommand_Faces, Object, ISetCommand)

ECode SetCommand_Faces::SetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ IInterface* value)
{
    AutoPtr<IArrayOf> array = IArrayOf::Probe(value);
    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<IFace*> > values = ArrayOf<IFace*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        array->Get(i, (IInterface**)&obj);
        AutoPtr<IFace> tmp = IFace::Probe(obj);
        values->Set(i, tmp);
    }

    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->SetFaces(values);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SetCommand_TonemapCurve, Object, ISetCommand)

ECode SetCommand_TonemapCurve::SetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ IInterface* value)
{

    AutoPtr<ITonemapCurve> _value = ITonemapCurve::Probe(value);
    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->SetTonemapCurve(_value);
    return NOERROR;
}

CAR_INTERFACE_IMPL(SetCommand_GpsLocation, Object, ISetCommand)

ECode SetCommand_GpsLocation::SetValue(
    /* [in] */ ICameraMetadataNative* metadata,
    /* [in] */ IInterface* value)
{
    AutoPtr<ILocation> _value = ILocation::Probe(value);
    CameraMetadataNative* metadataNative = (CameraMetadataNative*)metadata;
    metadataNative->SetGpsLocation(_value);
    return NOERROR;
}

//==============================================================================
// CameraMetadataNativeKey
//==============================================================================
CAR_INTERFACE_IMPL(CameraMetadataNativeKey, Object, ICaptureRequestKey)

CameraMetadataNativeKey::CameraMetadataNativeKey(
    /* [in] */ const String& name,
    /* [in] */ ClassID type)
    : mHasTag(FALSE)
    , mTag(0)
    , mHash(0)
{
    if (name.IsNull()) {
        Logger::E("CameraMetadataNativeKey", "Key needs a valid name");
        assert(0 && "Key needs a valid name");
    }

    mName = name;
    mType = type;
    mTypeReference = TypeReference::CreateSpecializedTypeReference(type);
    mHash = mName.GetHashCode() ^ Object::GetHashCode(mTypeReference);
}

CameraMetadataNativeKey::CameraMetadataNativeKey(
    /* [in] */ const String& name,
    /* [in] */ ITypeReference* typeReference)
    : mHasTag(FALSE)
    , mTag(0)
    , mHash(0)
{
    if (name.IsNull()) {
        Logger::E("CameraMetadataNativeKey", "Key needs a valid name");
        assert(0 && "Key needs a valid name");
    }
    else if (typeReference == NULL) {
        Logger::E("CameraMetadataNativeKey", "TypeReference needs to be non-null");
        assert(0 && "TypeReference needs to be non-null");
    }

    mName = name;
    typeReference->GetType(&mType);
    mTypeReference = typeReference;
    mHash = mName.GetHashCode() ^ Object::GetHashCode(mTypeReference);
}

ECode CameraMetadataNativeKey::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mName;
    return NOERROR;
}

ECode CameraMetadataNativeKey::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    *code = mHash;
    return NOERROR;
}

ECode CameraMetadataNativeKey::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);
    *equal = FALSE;

    if (obj == NULL) {
        return NOERROR;
    }

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }

    Int32 code;
    GetHashCode(&code);
    Int32 code2 = Object::GetHashCode(obj);
    if (code != code2) {
        return NOERROR;
    }

    AutoPtr<ICameraMetadataNativeKey> lhs;
    if (ICaptureResultKey::Probe(obj) != NULL) {
        ICaptureResultKey::Probe(obj)->GetNativeKey((ICameraMetadataNativeKey**)&lhs);
    }
    else if (ICaptureRequestKey::Probe(obj) != NULL) {
        ICaptureRequestKey::Probe(obj)->GetNativeKey((ICameraMetadataNativeKey**)&lhs);
    }
    else if (ICameraCharacteristicsKey::Probe(obj) != NULL) {
        ICameraCharacteristicsKey::Probe(obj)->GetNativeKey((ICameraMetadataNativeKey**)&lhs);
    }
    else if (ICameraMetadataNativeKey::Probe(obj) != NULL) {
        lhs = ICameraMetadataNativeKey::Probe(obj);
    }
    else {
        return NOERROR;
    }

    String name;
    lhs->GetName(&name);
    AutoPtr<ITypeReference> ref;
    lhs->GetTypeReference((ITypeReference**)&ref);
    *equal = mName.Equals(name) && Object::Equals(mTypeReference, ref);
    return NOERROR;
}

ECode CameraMetadataNativeKey::GetTag(
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag);

    if (!mHasTag) {
        CameraMetadataNative::GetTag(mName, &mTag);
        mHasTag = TRUE;
    }
    *tag = mTag;
    return NOERROR;
}

ECode CameraMetadataNativeKey::GetType(
    /* [out] */ ClassID* type)
{
    VALIDATE_NOT_NULL(type);
    // TODO: remove this; other places should use #getTypeReference() instead
    *type = mType;
    return NOERROR;
}

ECode CameraMetadataNativeKey::GetTypeReference(
    /* [out] */ ITypeReference** ref)
{
    VALIDATE_NOT_NULL(ref);

    *ref = mTypeReference;
    REFCOUNT_ADD(*ref);
    return NOERROR;
}

ECode CameraMetadataNativeKey::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("CameraMetadataNativeKey:");
    sb += StringUtils::ToHexString((Int32)this);
    sb += "{name:";
    sb += mName;
    sb += ", hasTag:";
    sb += mHasTag;
    sb += ", tag:";
    sb += mTag;
    sb += ", hash:";
    sb += mHash;
    sb += ", classType:";
    String clsid;
    // clsid.AppendFormat("{%p, %p, %p, {%p, %p, %p, %p, %p, %p, %p, %p} }",
    clsid.AppendFormat("{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
        mType.mClsid.mData1, mType.mClsid.mData2, mType.mClsid.mData3,
        mType.mClsid.mData4[0], mType.mClsid.mData4[1],
        mType.mClsid.mData4[2], mType.mClsid.mData4[3],
        mType.mClsid.mData4[4], mType.mClsid.mData4[5],
        mType.mClsid.mData4[6], mType.mClsid.mData4[7]);
    sb += clsid;
    sb += ", module:";
    sb += mType.mUunm;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

//==============================================================================
// CameraMetadataNative
//==============================================================================
static AutoPtr<IHashMap> InitSetCommandMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    AutoPtr<ICameraMetadataNativeKey> key;
    AutoPtr<ISetCommand> setCmd;

    key = NULL;
    CameraCharacteristics::SCALER_AVAILABLE_FORMATS->GetNativeKey((ICameraMetadataNativeKey**)&key);
    setCmd = new SetCommand_AvailableFormats();
    map->Put(key, setCmd);

    key = NULL;
    CaptureResult::STATISTICS_FACE_RECTANGLES->GetNativeKey((ICameraMetadataNativeKey**)&key);
    setCmd = new SetCommand_FaceRectangles();
    map->Put(key, setCmd);

    key = NULL;
    CaptureResult::STATISTICS_FACES->GetNativeKey((ICameraMetadataNativeKey**)&key);
    setCmd = new SetCommand_Faces();
    map->Put(key, setCmd);

    key = NULL;
    CaptureRequest::TONEMAP_CURVE->GetNativeKey((ICameraMetadataNativeKey**)&key);
    setCmd = new SetCommand_TonemapCurve();
    map->Put(key, setCmd);

    key = NULL;
    CaptureResult::JPEG_GPS_LOCATION->GetNativeKey((ICameraMetadataNativeKey**)&key);
    setCmd = new SetCommand_GpsLocation();
    map->Put(key, setCmd);

    return map;
}

static AutoPtr<IHashMap> InitGetCommandMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);

    AutoPtr<ICameraMetadataNativeKey> key;
    AutoPtr<IGetCommand> getCmd;

    key = NULL;
    CameraCharacteristics::SCALER_AVAILABLE_FORMATS->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_AvailableFormats();
    map->Put(key, getCmd);

    key = NULL;
    CaptureResult::STATISTICS_FACES->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_Faces();
    map->Put(key, getCmd);

    key = NULL;
    CaptureResult::STATISTICS_FACE_RECTANGLES->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_FaceRectangles();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::SCALER_STREAM_CONFIGURATION_MAP->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_StreamConfigurationMap();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::CONTROL_MAX_REGIONS_AE->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxRegions();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::CONTROL_MAX_REGIONS_AWB->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxRegions();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::CONTROL_MAX_REGIONS_AF->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxRegions();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_RAW->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxNumOutputs();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxNumOutputs();
    map->Put(key, getCmd);

    key = NULL;
    CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC_STALLING->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_MaxNumOutputs();
    map->Put(key, getCmd);

    key = NULL;
    CaptureRequest::TONEMAP_CURVE->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_TonemapCurve();
    map->Put(key, getCmd);

    key = NULL;
    CaptureResult::JPEG_GPS_LOCATION->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_GpsLocation();
    map->Put(key, getCmd);

    key = NULL;
    CaptureResult::STATISTICS_LENS_SHADING_CORRECTION_MAP->GetNativeKey((ICameraMetadataNativeKey**)&key);
    getCmd = new GetCommand_LensShadingMap();
    map->Put(key, getCmd);

    return map;
}

AutoPtr<IHashMap> CameraMetadataNative::sGetCommandMap = InitGetCommandMap();
AutoPtr<IHashMap> CameraMetadataNative::sSetCommandMap = InitSetCommandMap();


static Boolean RegisterAllMarshalers()
{
    if (VERBOSE) {
        Logger::V(TAG, "Shall register metadata marshalers");
    }

    AutoPtr<ArrayOf<IMarshalQueryable*> > queryList = ArrayOf<IMarshalQueryable*>::Alloc(20);

    AutoPtr<IMarshalQueryable> query;

    // marshalers for standard types
    query = NULL;
    CMarshalQueryablePrimitive::New((IMarshalQueryable**)&query);
    queryList->Set(0, query);

    query = NULL;
    CMarshalQueryableEnum::New((IMarshalQueryable**)&query);
    queryList->Set(1, query);

    query = NULL;
    CMarshalQueryableArray::New((IMarshalQueryable**)&query);
    queryList->Set(2, query);

    // pseudo standard types, that expand/narrow the native type into a managed type
    query = NULL;
    CMarshalQueryableBoolean::New((IMarshalQueryable**)&query);
    queryList->Set(3, query);

    query = NULL;
    CMarshalQueryableNativeByteToInteger::New((IMarshalQueryable**)&query);
    queryList->Set(4, query);

    // marshalers for custom types
    query = NULL;
    CMarshalQueryableRect::New((IMarshalQueryable**)&query);
    queryList->Set(5, query);

    query = NULL;
    CMarshalQueryableSize::New((IMarshalQueryable**)&query);
    queryList->Set(6, query);

    query = NULL;
    CMarshalQueryableSizeF::New((IMarshalQueryable**)&query);
    queryList->Set(7, query);

    query = NULL;
    CMarshalQueryableString::New((IMarshalQueryable**)&query);
    queryList->Set(8, query);

    query = NULL;
    CMarshalQueryableReprocessFormatsMap::New((IMarshalQueryable**)&query);
    queryList->Set(9, query);

    query = NULL;
    CMarshalQueryableRange::New((IMarshalQueryable**)&query);
    queryList->Set(10, query);

    query = NULL;
    CMarshalQueryablePair::New((IMarshalQueryable**)&query);
    queryList->Set(11, query);

    query = NULL;
    CMarshalQueryableMeteringRectangle::New((IMarshalQueryable**)&query);
    queryList->Set(12, query);

    query = NULL;
    CMarshalQueryableColorSpaceTransform::New((IMarshalQueryable**)&query);
    queryList->Set(13, query);

    query = NULL;
    CMarshalQueryableStreamConfiguration::New((IMarshalQueryable**)&query);
    queryList->Set(14, query);

    query = NULL;
    CMarshalQueryableStreamConfigurationDuration::New((IMarshalQueryable**)&query);
    queryList->Set(15, query);

    query = NULL;
    CMarshalQueryableRggbChannelVector::New((IMarshalQueryable**)&query);
    queryList->Set(16, query);

    query = NULL;
    CMarshalQueryableBlackLevelPattern::New((IMarshalQueryable**)&query);
    queryList->Set(17, query);

    query = NULL;
    CMarshalQueryableHighSpeedVideoConfiguration::New((IMarshalQueryable**)&query);
    queryList->Set(18, query);

    // generic parcelable marshaler (MUST BE LAST since it has lowest priority)
    query = NULL;
    CMarshalQueryableParcelable::New((IMarshalQueryable**)&query);
    queryList->Set(19, query);

    for (Int32 i = 0; i < queryList->GetLength(); ++i) {
        query = (*queryList)[i];
        MarshalRegistry::RegisterMarshalQueryable(query);
    }

    if (VERBOSE) {
        Logger::V(TAG, "Registered metadata marshalers");
    }
    return TRUE;
}

const String CameraMetadataNative::CELLID_PROCESS("CELLID");
const String CameraMetadataNative::GPS_PROCESS("GPS");
const Int32 CameraMetadataNative::FACE_LANDMARK_SIZE = 6;

Boolean CameraMetadataNative::initStaticBlock = RegisterAllMarshalers();

CAR_INTERFACE_IMPL_2(CameraMetadataNative, Object, ICameraMetadataNative, IParcelable)

CameraMetadataNative::CameraMetadataNative()
    : mMetadataPtr(0)
{
}

CameraMetadataNative::~CameraMetadataNative()
{
    //try {
    Close();
    //} finally {
    //    super.finalize();
    //}
}

ECode CameraMetadataNative::constructor()
{
    //super();
    mMetadataPtr = NativeAllocate();
    if (mMetadataPtr == 0) {
        //throw new OutOfMemoryError("Failed to allocate native CameraMetadata");
        Logger::E(TAG, "Failed to allocate native CameraMetadata");
        return E_OUT_OF_MEMORY_ERROR;
    }
    return NOERROR;
}

ECode CameraMetadataNative::constructor(
    /* [in] */ ICameraMetadataNative* other)
{
    //super();
    FAIL_RETURN(NativeAllocateCopy(other, &mMetadataPtr))
    if (mMetadataPtr == 0) {
        //throw new OutOfMemoryError("Failed to allocate native CameraMetadata");
        Logger::E(TAG, "Failed to allocate native CameraMetadata");
        return E_OUT_OF_MEMORY_ERROR;
    }
    return NOERROR;
}

String CameraMetadataNative::TranslateLocationProviderToProcess(
    /* [in] */ const String& provider)
{
    if (provider.IsNull()) {
        return String(NULL);
    }

    if (provider == ILocationManager::GPS_PROVIDER) {
        return GPS_PROCESS;
    }
    else if (provider == ILocationManager::NETWORK_PROVIDER) {
        return CELLID_PROCESS;
    }
    else {
        return String(NULL);
    }

    return String(NULL);
}

String CameraMetadataNative::TranslateProcessToLocationProvider(
    /* [in] */ const String& process)
{
    if (process.IsNull()) {
        return String(NULL);
    }

    if (process == GPS_PROCESS) {
        return ILocationManager::GPS_PROVIDER;
    }
    else if (process == CELLID_PROCESS) {
        return ILocationManager::NETWORK_PROVIDER;
    }
    else {
        return String(NULL);
    }

    return String(NULL);
}

ECode CameraMetadataNative::Move(
    /* [in] */ ICameraMetadataNative* other,
    /* [out] */ ICameraMetadataNative** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICameraMetadataNative> newObject;
    CCameraMetadataNative::New((ICameraMetadataNative**)&newObject);
    newObject->Swap(other);
    *result = newObject;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CameraMetadataNative::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    NativeWriteToParcel(dest);
    return NOERROR;
}

ECode CameraMetadataNative::Get(
    /* [in] */ ICameraCharacteristicsKey* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Get(_key, result);
}

ECode CameraMetadataNative::Get(
    /* [in] */ ICaptureResultKey* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Get(_key, result);
}

ECode CameraMetadataNative::Get(
    /* [in] */ ICaptureRequestKey* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Get(_key, result);
}

ECode CameraMetadataNative::Get(
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (VERBOSE) Logger::I(TAG, "CameraMetadataNative Get %s", TO_CSTR(key));

    FAIL_RETURN(Preconditions::CheckNotNull(key, String("key must not be null")))

    // Check if key has been overridden to use a wrapper class on the java side.
    AutoPtr<IInterface> obj;
    sGetCommandMap->Get(key, (IInterface**)&obj);
    AutoPtr<IGetCommand> g = IGetCommand::Probe(obj);
    if (g != NULL) {
        return g->GetValue(this, key, result);
    }

    if (VERBOSE) Logger::I(TAG, "CameraMetadataNative Get %s not in command map.", TO_CSTR(key));

    obj = GetBase(key);
    *result = obj;
    REFCOUNT_ADD(*result);

    if (VERBOSE) Logger::I(TAG, "CameraMetadataNative Get %s result: %s.", TO_CSTR(key), TO_CSTR(obj));
    return NOERROR;
}

ECode CameraMetadataNative::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    NativeReadFromParcel(source);
    return NOERROR;
}

ECode CameraMetadataNative::NativeSetupGlobalVendorTagDescriptor(
    /* [out] */ Int32* descriptor)
{
    VALIDATE_NOT_NULL(descriptor);

    const android::String16 NAME("media.camera");
    android::sp<android::ICameraService> cameraService;
    android::status_t err = getService(NAME, /*out*/&cameraService);

    if (err != android::OK) {
        Logger::E(TAG, "%s: Failed to get camera service, received error %s (%d)", __FUNCTION__,
                strerror(-err), err);
        *descriptor = err;
        return NOERROR;
    }

    android::sp<android::VendorTagDescriptor> desc;
    err = cameraService->getCameraVendorTagDescriptor(/*out*/desc);

    if (err == -EOPNOTSUPP) {
        Logger::W(TAG, "%s: Camera HAL too old; does not support vendor tags", __FUNCTION__);
        android::VendorTagDescriptor::clearGlobalVendorTagDescriptor();

        *descriptor = android::OK;
        return NOERROR;
    }
    else if (err != android::OK) {
        Logger::E(TAG, "%s: Failed to setup vendor tag descriptors, received error %s (%d)",
                __FUNCTION__, strerror(-err), err);
        *descriptor = err;
        return NOERROR;
    }

    err = android::VendorTagDescriptor::setAsGlobalVendorTagDescriptor(desc);

    *descriptor = err;
    return NOERROR;
}

ECode CameraMetadataNative::Set(
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<IInterface> obj;
    sSetCommandMap->Get(key, (IInterface**)&obj);
    AutoPtr<ISetCommand> s = ISetCommand::Probe(obj);
    if (s != NULL) {
        return s->SetValue(this, value);
    }

    return SetBase(key, value);
}

ECode CameraMetadataNative::Set(
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Set(_key, value);
}

ECode CameraMetadataNative::Set(
    /* [in] */ ICaptureResultKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Set(_key, value);
}

ECode CameraMetadataNative::Set(
    /* [in] */ ICameraCharacteristicsKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> _key;
    key->GetNativeKey((ICameraMetadataNativeKey**)&_key);
    return Set(_key, value);
}

void CameraMetadataNative::Close()
{
    // this sets mMetadataPtr to 0
    NativeClose();
    mMetadataPtr = 0; // set it to 0 again to prevent eclipse from making this field final
    return;
}

AutoPtr<IInterface> CameraMetadataNative::GetBase(
    /* [in] */ ICameraCharacteristicsKey* key)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return GetBase(nativeKey);
}

AutoPtr<IInterface> CameraMetadataNative::GetBase(
    /* [in] */ ICaptureResultKey* key)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return GetBase(nativeKey);
}

AutoPtr<IInterface> CameraMetadataNative::GetBase(
    /* [in] */ ICaptureRequestKey* key)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return GetBase(nativeKey);
}

AutoPtr<IInterface> CameraMetadataNative::GetBase(
    /* [in] */ ICameraMetadataNativeKey* key)
{
    if (VERBOSE) Logger::V(TAG, " >> GetBase %s", TO_CSTR(key));
    Int32 tag;
    key->GetTag(&tag);

    Logger::V(TAG, " >> GetTag: %d", tag);

    AutoPtr<ArrayOf<Byte> > values;
    ReadValues(tag, (ArrayOf<Byte>**)&values);
    if (values == NULL) {
        if (VERBOSE) Logger::W(TAG, "%p read null value for tag %d in %s", this, tag, TO_CSTR(key));
        return NULL;
    }

    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder bOrder;
    orderHelper->GetNativeOrder(&bOrder);

    AutoPtr<IByteBuffer> buffer;
    AutoPtr<IByteBufferHelper> bufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferHelper);
    bufferHelper->Wrap(values, (IByteBuffer**)&buffer);
    buffer->SetOrder(bOrder);

    AutoPtr<IInterface> outface;
    AutoPtr<IMarshaler> marshaler = GetMarshalerForKey(key);
    Logger::V(TAG, " >> GetMarshalerForKey: %s", TO_CSTR(marshaler));
    marshaler->Unmarshal(buffer, (IInterface**)&outface);
    return outface;
}

AutoPtr<ArrayOf<Int32> > CameraMetadataNative::GetAvailableFormats()
{
    AutoPtr<IInterface> obj = GetBase(CameraCharacteristics::SCALER_AVAILABLE_FORMATS);
    AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);

    Int32 size;
    arrayObj->GetLength(&size);
    AutoPtr<ArrayOf<Int32> > availableFormats = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> element;
        arrayObj->Get(i, (IInterface**)&element);
        AutoPtr<IInteger32> intObj = IInteger32::Probe(element);
        Int32 value;
        intObj->GetValue(&value);

        (*availableFormats)[i] = value;
    }

    if (availableFormats != NULL) {
        for (Int32 i = 0; i < availableFormats->GetLength(); i++) {
            // JPEG has different value between native and managed side, need override.
            if ((*availableFormats)[i] == NATIVE_JPEG_FORMAT) {
                (*availableFormats)[i] = IImageFormat::JPEG;
            }
        }
    }

    return availableFormats;
}

Boolean CameraMetadataNative::SetFaces(
    /* [in] */ ArrayOf<IFace*>* faces)
{
    if (faces == NULL) {
        return FALSE;
    }

    Int32 numFaces = faces->GetLength();

    // Detect if all faces are SIMPLE or not; count # of valid faces
    Boolean fullMode = TRUE;

    for (Int32 i = 0; i < numFaces; i++) {
        AutoPtr<IFace> face = (*faces)[i];

        if (face == NULL) {
            numFaces--;
            Logger::W(TAG, "setFaces - null face detected, skipping");
            continue;
        }

        Int32 id;
        face->GetId(&id);
        if (id == IFace::ID_UNSUPPORTED) {
            fullMode = FALSE;
        }
    }

    AutoPtr<ArrayOf<IRect*> > faceRectangles = ArrayOf<IRect*>::Alloc(numFaces);
    AutoPtr<ArrayOf<Byte> > faceScores = ArrayOf<Byte>::Alloc(numFaces);
    AutoPtr<ArrayOf<Int32> > faceIds;
    AutoPtr<ArrayOf<Int32> > faceLandmarks;

    if (fullMode) {
        faceIds = ArrayOf<Int32>::Alloc(numFaces);
        faceLandmarks = ArrayOf<Int32>::Alloc(numFaces * FACE_LANDMARK_SIZE);
    }

    for (Int32 i = 0; i < numFaces; i++) {
        AutoPtr<IFace> face = (*faces)[i];

        if (face == NULL) {
            continue;
        }

        AutoPtr<IRect> outrect;
        face->GetBounds((IRect**)&outrect);
        faceRectangles->Set(i, outrect);

        Int32 value;
        face->GetScore(&value);
        (*faceScores)[i] = (Byte)value;

        if (fullMode) {
            face->GetId(&((*faceIds)[i]));

            Int32 j = 0;

            AutoPtr<IPoint> leftEyepoint;
            face->GetLeftEyePosition((IPoint**)&leftEyepoint);
            Int32 leftEyeX;
            leftEyepoint->GetX(&leftEyeX);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = leftEyeX;

            Int32 leftEyeY;
            leftEyepoint->GetY(&leftEyeY);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = leftEyeY;

            AutoPtr<IPoint> rightEyepoint;
            face->GetRightEyePosition((IPoint**)&rightEyepoint);
            Int32 rightEyeX;
            rightEyepoint->GetX(&rightEyeX);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = rightEyeX;

            Int32 rightEyeY;
            rightEyepoint->GetY(&rightEyeY);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = rightEyeY;


            AutoPtr<IPoint> mouthpoint;
            face->GetMouthPosition((IPoint**)&mouthpoint);
            Int32 mouthEyeX;
            mouthpoint->GetX(&mouthEyeX);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = mouthEyeX;

            Int32 mouthEyeY;
            mouthpoint->GetY(&mouthEyeY);
            (*faceLandmarks)[i * FACE_LANDMARK_SIZE + j++] = mouthEyeY;
        }

        i++;
    }

    AutoPtr<IArrayOf> array = CoreUtils::Convert(faceRectangles.Get());
    Set(CaptureResult::STATISTICS_FACE_RECTANGLES, array);

    AutoPtr<IArrayOf> array2 = CoreUtils::Convert(faceIds);
    Set(CaptureResult::STATISTICS_FACE_IDS, array2);

    AutoPtr<IArrayOf> array3 = CoreUtils::Convert(faceLandmarks);
    Set(CaptureResult::STATISTICS_FACE_LANDMARKS, array3);

    AutoPtr<IArrayOf> array4 = CoreUtils::ConvertByteArray(faceScores);
    Set(CaptureResult::STATISTICS_FACE_SCORES, array4);

    return TRUE;
}

AutoPtr<ArrayOf<IFace*> > CameraMetadataNative::GetFaces()
{
    AutoPtr<IInterface> obj;
    Get(CaptureResult::STATISTICS_FACE_DETECT_MODE, (IInterface**)&obj);
    AutoPtr<IInteger32> intObj = IInteger32::Probe(obj);


    AutoPtr<IInterface> obj2;
    Get(CaptureResult::STATISTICS_FACE_SCORES, (IInterface**)&obj2);
    AutoPtr<IArrayOf> faceScoresArrray = IArrayOf::Probe(obj2);

    AutoPtr<IInterface> obj3;
    Get(CaptureResult::STATISTICS_FACE_RECTANGLES, (IInterface**)&obj3);
    AutoPtr<IArrayOf> faceRectanglesArrray = IArrayOf::Probe(obj3);


    AutoPtr<IInterface> obj4;
    Get(CaptureResult::STATISTICS_FACE_IDS, (IInterface**)&obj4);
    AutoPtr<IArrayOf> faceIdsArrray = IArrayOf::Probe(obj4);

    AutoPtr<IInterface> obj5;
    Get(CaptureResult::STATISTICS_FACE_LANDMARKS, (IInterface**)&obj5);
    AutoPtr<IArrayOf> faceLandmarksArrray = IArrayOf::Probe(obj5);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(5);
    array->Set(0, intObj);
    array->Set(1, faceScoresArrray);
    array->Set(2, faceRectanglesArrray);
    array->Set(3, faceIdsArrray);
    array->Set(4, faceLandmarksArrray);
    if (AreValuesAllNull(array)) {
        return NULL;
    }

    Int32 faceDetectMode;
    if (intObj == NULL) {
        Logger::W(TAG, "Face detect mode metadata is null, assuming the mode is SIMPLE");
        faceDetectMode = CameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE;
    }
    else {
        intObj->GetValue(&faceDetectMode);
        if (faceDetectMode == CameraMetadata::STATISTICS_FACE_DETECT_MODE_OFF) {
            AutoPtr<ArrayOf<IFace*> > res = ArrayOf<IFace*>::Alloc(0);
            return res;
        }
        if (faceDetectMode != CameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE &&
                faceDetectMode != CameraMetadata::STATISTICS_FACE_DETECT_MODE_FULL) {
            Logger::W(TAG, "Unknown face detect mode: %d", faceDetectMode);
            AutoPtr<ArrayOf<IFace*> > res = ArrayOf<IFace*>::Alloc(0);
            return res;
        }
    }

    // Face scores and rectangles are required by SIMPLE and FULL mode.
    Int32 length = 0;
    Int32 length2 = 0;
    if (faceScoresArrray == NULL || faceRectanglesArrray == NULL) {
        Logger::W(TAG, "Expect face scores and rectangles to be non-null");
        AutoPtr<ArrayOf<IFace*> > res = ArrayOf<IFace*>::Alloc(0);
        return res;
    }
    else {
        faceScoresArrray->GetLength(&length);
        faceRectanglesArrray->GetLength(&length2);
        if (length != length2) {
            Logger::W(TAG, "Face score size(%d) doesn match face rectangle size(%d)!",
                    length, length2);
        }
    }

    // To be safe, make number of faces is the minimal of all face info metadata length.
    Int32 numFaces = Elastos::Core::Math::Min(length, length2);
    // Face id and landmarks are only required by FULL mode.
    if (faceDetectMode == CameraMetadata::STATISTICS_FACE_DETECT_MODE_FULL) {
        if (faceIdsArrray == NULL || faceLandmarksArrray == NULL) {
            Logger::W(TAG, "Expect face ids and landmarks to be non-null for FULL mode,"
                    "fallback to SIMPLE mode");
            faceDetectMode = CameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE;
        }
        else {
            Int32 length3;
            faceIdsArrray->GetLength(&length3);
            Int32 length4;
            faceLandmarksArrray->GetLength(&length4);
            if (length3 != numFaces || length4 != numFaces * FACE_LANDMARK_SIZE) {
                Logger::W(TAG, "Face id size(%d), or face landmark size(%d) don't"
                        "match face number(%d)!",
                        length3, length4 * FACE_LANDMARK_SIZE, numFaces);
            }
            // To be safe, make number of faces is the minimal of all face info metadata length.
            numFaces = Elastos::Core::Math::Min(numFaces, length3);
            numFaces = Elastos::Core::Math::Min(numFaces, length4 / FACE_LANDMARK_SIZE);
        }
    }

    AutoPtr<IArrayList> faceList;
    CArrayList::New((IArrayList**)&faceList);

    if (faceDetectMode == CameraMetadata::STATISTICS_FACE_DETECT_MODE_SIMPLE) {
        for (Int32 i = 0; i < numFaces; i++) {
            AutoPtr<IInterface> tmp;
            faceScoresArrray->Get(i, (IInterface**)&tmp);
            AutoPtr<IByte> byteObj = IByte::Probe(tmp);
            Byte value;
            byteObj->GetValue(&value);
            if (value <= IFace::SCORE_MAX && value >= IFace::SCORE_MIN) {
                AutoPtr<IInterface> tmp2;
                faceRectanglesArrray->Get(i, (IInterface**)&tmp2);
                AutoPtr<IRect> rect = IRect::Probe(tmp2);
                AutoPtr<IFace> face;
                CFace::New(rect, value, (IFace**)&face);
                faceList->Add(TO_IINTERFACE(face));
            }
        }
    }
    else {
        // CaptureResult.STATISTICS_FACE_DETECT_MODE_FULL
        for (Int32 i = 0; i < numFaces; i++) {
            AutoPtr<IInterface> tmp;
            faceScoresArrray->Get(i, (IInterface**)&tmp);
            AutoPtr<IByte> byteObj = IByte::Probe(tmp);
            Byte value;
            byteObj->GetValue(&value);

            AutoPtr<IInterface> tmp2;
            faceIdsArrray->Get(i, (IInterface**)&tmp2);
            AutoPtr<IInteger32> intObj = IInteger32::Probe(tmp);
            Int32 value2;
            intObj->GetValue(&value2);

            if (value <= IFace::SCORE_MAX && value >= IFace::SCORE_MIN && value2 >= 0) {
                AutoPtr<IInterface> tmp3;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE, (IInterface**)&tmp3);
                AutoPtr<IInteger32> intObj3 = IInteger32::Probe(tmp3);
                Int32 value3;
                intObj3->GetValue(&value3);

                AutoPtr<IInterface> tmp4;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE + 1, (IInterface**)&tmp4);
                AutoPtr<IInteger32> intObj4 = IInteger32::Probe(tmp4);
                Int32 value4;
                intObj4->GetValue(&value4);

                AutoPtr<IPoint> leftEye;
                CPoint::New(value3, value4, (IPoint**)&leftEye);

                AutoPtr<IInterface> tmp5;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE + 2, (IInterface**)&tmp5);
                AutoPtr<IInteger32> intObj5 = IInteger32::Probe(tmp5);
                Int32 value5;
                intObj5->GetValue(&value5);

                AutoPtr<IInterface> tmp6;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE + 3, (IInterface**)&tmp6);
                AutoPtr<IInteger32> intObj6 = IInteger32::Probe(tmp6);
                Int32 value6;
                intObj6->GetValue(&value6);

                AutoPtr<IPoint> rightEye;
                CPoint::New(value5, value6, (IPoint**)&rightEye);

                AutoPtr<IInterface> tmp7;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE + 4, (IInterface**)&tmp7);
                AutoPtr<IInteger32> intObj7 = IInteger32::Probe(tmp7);
                Int32 value7;
                intObj7->GetValue(&value7);

                AutoPtr<IInterface> tmp8;
                faceLandmarksArrray->Get(i*FACE_LANDMARK_SIZE + 5, (IInterface**)&tmp8);
                AutoPtr<IInteger32> intObj8 = IInteger32::Probe(tmp8);
                Int32 value8;
                intObj8->GetValue(&value8);

                AutoPtr<IPoint> mouth;
                CPoint::New(value7, value8, (IPoint**)&mouth);


                AutoPtr<IInterface> tmp9;
                faceRectanglesArrray->Get(i, (IInterface**)&tmp9);
                AutoPtr<IRect> rect = IRect::Probe(tmp9);

                AutoPtr<IFace> face;
                CFace::New(rect, value, value2,
                        leftEye, rightEye, mouth, (IFace**)&face);

                faceList->Add(TO_IINTERFACE(face));
            }
        }
    }

    AutoPtr<ArrayOf<IInterface*> > faces;
    faceList->ToArray((ArrayOf<IInterface*>**)&faces);
    AutoPtr<ArrayOf<IFace*> > _faces = ArrayOf<IFace*>::Alloc(faces->GetLength());
    for (Int32 i = 0; i < faces->GetLength(); i++) {
        AutoPtr<IFace> res = IFace::Probe((*faces)[i]);
        _faces->Set(i, res);
    }

    return _faces;
}

AutoPtr<ArrayOf<IRect*> > CameraMetadataNative::GetFaceRectangles()
{
    AutoPtr<IInterface> obj = GetBase(CaptureResult::STATISTICS_FACE_RECTANGLES);
    AutoPtr<IArrayOf> faceRectangles = IArrayOf::Probe(obj);
    if (faceRectangles == NULL) return NULL;

    Int32 length;
    faceRectangles->GetLength(&length);
    AutoPtr<ArrayOf<IRect*> > fixedFaceRectangles = ArrayOf<IRect*>::Alloc(length);

    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> tmp;
        faceRectangles->Get(i, (IInterface**)&tmp);
        AutoPtr<IRect> rect = IRect::Probe(tmp);
        Int32 left;
        rect->GetLeft(&left);
        Int32 top;
        rect->GetTop(&top);
        Int32 right;
        rect->GetRight(&right);
        Int32 bottom;
        rect->GetBottom(&bottom);

        AutoPtr<IRect> res;
        CRect::New(left, top, right - left, bottom - top, (IRect**)&res);
        fixedFaceRectangles->Set(i, res);
    }
    return fixedFaceRectangles;
}

AutoPtr<ILensShadingMap> CameraMetadataNative::GetLensShadingMap()
{
    AutoPtr<IInterface> obj = GetBase(CaptureResult::STATISTICS_LENS_SHADING_MAP);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    AutoPtr<IInterface> obj2;
    Get(CameraCharacteristics::LENS_INFO_SHADING_MAP_SIZE, (IInterface**)&obj2);
    AutoPtr<ISize> s = ISize::Probe(obj2);

    // Do not warn if lsmArray is null while s is not. This is valid.
    if (array == NULL) {
        return NULL;
    }

    if (s == NULL) {
        Logger::W(TAG, "getLensShadingMap - Lens shading map size was null.");
        return NULL;
    }

    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<Float> > lsmArray = ArrayOf<Float>::Alloc(length);
    for (Int32 i = 0; i < length; ++i)
    {
        AutoPtr<IInterface> tmp;
        array->Get(i, (IInterface**)&tmp);
        AutoPtr<IFloat> fObj = IFloat::Probe(tmp);
        Float value;
        fObj->GetValue(&value);
        (*lsmArray)[i] = value;
    }

    Int32 height;
    s->GetHeight(&height);
    Int32 width;
    s->GetWidth(&width);
    AutoPtr<ILensShadingMap> map;
    CLensShadingMap::New(lsmArray, height, width, (ILensShadingMap**)&map);
    return map;
}

AutoPtr<ILocation> CameraMetadataNative::GetGpsLocation()
{
    AutoPtr<IInterface> obj;
    Get(CaptureResult::JPEG_GPS_PROCESSING_METHOD, (IInterface**)&obj);
    AutoPtr<ICharSequence> cstring = ICharSequence::Probe(obj);

    AutoPtr<IInterface> obj2;
    Get(CaptureResult::JPEG_GPS_COORDINATES, (IInterface**)&obj2);
    AutoPtr<IArrayOf> coordsArray = IArrayOf::Probe(obj2);

    AutoPtr<IInterface> obj3;
    Get(CaptureResult::JPEG_GPS_TIMESTAMP, (IInterface**)&obj3);
    AutoPtr<IInteger64> timeObj = IInteger64::Probe(obj3);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(3);
    array->Set(0, cstring);
    array->Set(1, coordsArray);
    array->Set(2, timeObj);
    if (AreValuesAllNull(array)) {
        return NULL;
    }

    String processingMethod;
    cstring->ToString(&processingMethod);

    AutoPtr<ILocation> l;
    CLocation::New(TranslateProcessToLocationProvider(processingMethod), (ILocation**)&l);
    if (timeObj != NULL) {
        Int64 timeStamp;
        timeObj->GetValue(&timeStamp);
        l->SetTime(timeStamp);
    }
    else {
        Logger::W(TAG, "getGpsLocation - No timestamp for GPS location.");
    }

    if (coordsArray != NULL) {
        AutoPtr<IInterface> tmp;
        coordsArray->Get(0, (IInterface**)&tmp);
        AutoPtr<IDouble> valueObj = IDouble::Probe(tmp);
        Double value;
        valueObj->GetValue(&value);
        l->SetLatitude(value);

        AutoPtr<IInterface> tmp2;
        coordsArray->Get(1, (IInterface**)&tmp2);
        AutoPtr<IDouble> valueObj2 = IDouble::Probe(tmp2);
        Double value2;
        valueObj2->GetValue(&value2);
        l->SetLongitude(value2);

        AutoPtr<IInterface> tmp3;
        coordsArray->Get(2, (IInterface**)&tmp3);
        AutoPtr<IDouble> valueObj3 = IDouble::Probe(tmp3);
        Double value3;
        valueObj3->GetValue(&value3);
        l->SetAltitude(value3);
    }
    else {
        Logger::W(TAG, "getGpsLocation - No coordinates for GPS location");
    }

    return l;
}

Boolean CameraMetadataNative::SetGpsLocation(
    /* [in] */ ILocation* l)
{
    if (l == NULL) {
        return FALSE;
    }

    Double latitud;
    l->GetLatitude(&latitud);
    Double longitude;
    l->GetLongitude(&longitude);
    Double altitude;
    l->GetAltitude(&altitude);

    AutoPtr<IArrayOf> coords;
    CArrayOf::New(EIID_IDouble, 3, (IArrayOf**)&coords);
    AutoPtr<IDouble> value = CoreUtils::Convert(latitud);
    AutoPtr<IDouble> value2 = CoreUtils::Convert(longitude);
    AutoPtr<IDouble> value3 = CoreUtils::Convert(altitude);
    coords->Set(0, value);
    coords->Set(1, value2);
    coords->Set(2, value3);

    String provider;
    l->GetProvider(&provider);
    String processMethod = TranslateLocationProviderToProcess(provider);
    Int64 timestamp;
    l->GetTime(&timestamp);

    AutoPtr<IInteger64> timestampObj = CoreUtils::Convert(timestamp);
    Set(CaptureRequest::JPEG_GPS_TIMESTAMP, TO_IINTERFACE(timestampObj));
    Set(CaptureRequest::JPEG_GPS_COORDINATES, TO_IINTERFACE(coords));

    if (processMethod.IsNull()) {
        Logger::W(TAG, "setGpsLocation - No process method, Location is not from a GPS or NETWORK"
                " provider");
    }
    else {
        AutoPtr<ICharSequence> cstring = CoreUtils::Convert(processMethod);
        SetBase(CaptureRequest::JPEG_GPS_PROCESSING_METHOD, TO_IINTERFACE(cstring));
    }
    return TRUE;
}

AutoPtr<IStreamConfigurationMap> CameraMetadataNative::GetStreamConfigurationMap()
{
    AutoPtr<IInterface> obj = GetBase(
            CameraCharacteristics::SCALER_AVAILABLE_STREAM_CONFIGURATIONS);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);
    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<IStreamConfiguration*> > configurations = ArrayOf<IStreamConfiguration*>::Alloc(length);
    for (Int32 i = 0; i < length; ++i)
    {
        AutoPtr<IInterface> tmp;
        array->Get(i, (IInterface**)&tmp);
        AutoPtr<IStreamConfiguration> obj = IStreamConfiguration::Probe(tmp);
        configurations->Set(i, obj);
    }


    AutoPtr<IInterface> obj2 = GetBase(
            CameraCharacteristics::SCALER_AVAILABLE_MIN_FRAME_DURATIONS);
    AutoPtr<IArrayOf> array2 = IArrayOf::Probe(obj2);
    Int32 length2;
    array2->GetLength(&length2);
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > minFrameDurations =
            ArrayOf<IStreamConfigurationDuration*>::Alloc(length2);
    for (Int32 i = 0; i < length2; ++i)
    {
        AutoPtr<IInterface> tmp;
        array2->Get(i, (IInterface**)&tmp);
        AutoPtr<IStreamConfigurationDuration> obj = IStreamConfigurationDuration::Probe(tmp);
        minFrameDurations->Set(i, obj);
    }

    AutoPtr<IInterface> obj3 = GetBase(
            CameraCharacteristics::SCALER_AVAILABLE_STALL_DURATIONS);
    AutoPtr<IArrayOf> array3 = IArrayOf::Probe(obj3);
    Int32 length3;
    array3->GetLength(&length3);
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > stallDurations =
            ArrayOf<IStreamConfigurationDuration*>::Alloc(length3);
    for (Int32 i = 0; i < length3; ++i)
    {
        AutoPtr<IInterface> tmp;
        array3->Get(i, (IInterface**)&tmp);
        AutoPtr<IStreamConfigurationDuration> obj = IStreamConfigurationDuration::Probe(tmp);
        stallDurations->Set(i, obj);
    }

    AutoPtr<IInterface> obj4 = GetBase(
            CameraCharacteristics::CONTROL_AVAILABLE_HIGH_SPEED_VIDEO_CONFIGURATIONS);
    AutoPtr<IArrayOf> array4 = IArrayOf::Probe(obj4);
    Int32 length4;
    array4->GetLength(&length4);
    AutoPtr<ArrayOf<IHighSpeedVideoConfiguration*> > highSpeedVideoConfigurations =
            ArrayOf<IHighSpeedVideoConfiguration*>::Alloc(length4);
    for (Int32 i = 0; i < length4; ++i)
    {
        AutoPtr<IInterface> tmp;
        array4->Get(i, (IInterface**)&tmp);
        AutoPtr<IHighSpeedVideoConfiguration> obj = IHighSpeedVideoConfiguration::Probe(tmp);
        highSpeedVideoConfigurations->Set(i, obj);
    }

    AutoPtr<IStreamConfigurationMap> map;
    CStreamConfigurationMap::New(
            configurations, minFrameDurations, stallDurations,
            highSpeedVideoConfigurations, (IStreamConfigurationMap**)&map);
    return map;
}

ECode CameraMetadataNative::GetMaxRegions(
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ Int32* regions)
{
    VALIDATE_NOT_NULL(regions);
    *regions = 0;

    const Int32 AE = 0;
    const Int32 AWB = 1;
    const Int32 AF = 2;

    // The order of the elements is: (AE, AWB, AF)
    AutoPtr<IInterface> obj = GetBase(CameraCharacteristics::CONTROL_MAX_REGIONS);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    if (array == NULL) {
        return NOERROR;
    }

    Boolean res;
    if (IObject::Probe(key)->Equals(CameraCharacteristics::CONTROL_MAX_REGIONS_AE, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(AE, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(regions);
    }
    else if (IObject::Probe(key)->Equals(CameraCharacteristics::CONTROL_MAX_REGIONS_AWB, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(AWB, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(regions);
    }
    else if (IObject::Probe(key)->Equals(CameraCharacteristics::CONTROL_MAX_REGIONS_AF, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(AF, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(regions);
    }
    else {
        //throw new AssertionError("Invalid key " + key);
        String str;
        IObject::Probe(key)->ToString(&str);
        Logger::E(TAG, "Invalid key %s", str.string());
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

ECode CameraMetadataNative::GetMaxNumOutputs(
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [out] */ Int32* num)
{
    VALIDATE_NOT_NULL(num);
    *num = 0;

    const Int32 RAW = 0;
    const Int32 PROC = 1;
    const Int32 PROC_STALLING = 2;

    // The order of the elements is: (raw, proc+nonstalling, proc+stalling)
    AutoPtr<IInterface> obj = GetBase(CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_STREAMS);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    if (array == NULL) {
        return NOERROR;
    }

    Boolean res;
    if (IObject::Probe(key)->Equals(CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_RAW, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(RAW, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(num);
    }
    else if (IObject::Probe(key)->Equals(CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(PROC, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(num);
    }
    else if (IObject::Probe(key)->Equals(CameraCharacteristics::REQUEST_MAX_NUM_OUTPUT_PROC_STALLING, &res), res) {
        AutoPtr<IInterface> tmp;
        array->Get(PROC_STALLING, (IInterface**)&tmp);
        AutoPtr<IInteger32> valueObj = IInteger32::Probe(tmp);
        return valueObj->GetValue(num);
    }
    else {
        //throw new AssertionError("Invalid key " + key);
        String str;
        IObject::Probe(key)->ToString(&str);
        Logger::E(TAG, "Invalid key %s", str.string());
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

AutoPtr<ITonemapCurve> CameraMetadataNative::GetTonemapCurve()
{
    AutoPtr<IInterface> obj = GetBase(CaptureRequest::TONEMAP_CURVE_RED);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(obj);

    AutoPtr<IInterface> obj2 = GetBase( CaptureRequest::TONEMAP_CURVE_GREEN);
    AutoPtr<IArrayOf> array2 = IArrayOf::Probe(obj2);

    AutoPtr<IInterface> obj3 = GetBase( CaptureRequest::TONEMAP_CURVE_BLUE);
    AutoPtr<IArrayOf> array3 = IArrayOf::Probe(obj3);

    AutoPtr<ArrayOf<IInterface*> > tmp = ArrayOf<IInterface*>::Alloc(3);
    tmp->Set(0, array);
    tmp->Set(1, array2);
    tmp->Set(2, array3);
    if (AreValuesAllNull(tmp)) {
        return NULL;
    }

    Int32 length;
    array->GetLength(&length);
    AutoPtr<ArrayOf<Float> > red = ArrayOf<Float>::Alloc(length);
    for (Int32 i = 0; i < length; ++i)
    {
        AutoPtr<IInterface> tmp;
        array->Get(i, (IInterface**)&tmp);
        AutoPtr<IFloat> obj = IFloat::Probe(tmp);
        Float value;
        obj->GetValue(&value);
        (*red)[i] = value;
    }

    Int32 length2;
    array->GetLength(&length2);
    AutoPtr<ArrayOf<Float> > green = ArrayOf<Float>::Alloc(length2);
    for (Int32 i = 0; i < length2; ++i)
    {
        AutoPtr<IInterface> tmp;
        array2->Get(i, (IInterface**)&tmp);
        AutoPtr<IFloat> obj = IFloat::Probe(tmp);
        Float value;
        obj->GetValue(&value);
        (*green)[i] = value;
    }

    Int32 length3;
    array->GetLength(&length3);
    AutoPtr<ArrayOf<Float> > blue = ArrayOf<Float>::Alloc(length3);
    for (Int32 i = 0; i < length3; ++i)
    {
        AutoPtr<IInterface> tmp;
        array3->Get(i, (IInterface**)&tmp);
        AutoPtr<IFloat> obj = IFloat::Probe(tmp);
        Float value;
        obj->GetValue(&value);
        (*blue)[i] = value;
    }

    if (array == NULL || array2 == NULL || array3 == NULL) {
        Logger::W(TAG, "getTonemapCurve - missing tone curve components");
        return NULL;
    }
    AutoPtr<ITonemapCurve> tc;
    CTonemapCurve::New(red, green, blue, (ITonemapCurve**)&tc);
    return tc;
}

ECode CameraMetadataNative::SetBase(
    /* [in] */ ICameraCharacteristicsKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return SetBase(nativeKey, value);
}

ECode CameraMetadataNative::SetBase(
    /* [in] */ ICaptureResultKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return SetBase(nativeKey, value);
}

ECode CameraMetadataNative::SetBase(
    /* [in] */ ICaptureRequestKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<ICameraMetadataNativeKey> nativeKey;
    key->GetNativeKey((ICameraMetadataNativeKey**)&nativeKey);
    return SetBase(nativeKey, value);
}

ECode CameraMetadataNative::SetBase(
    /* [in] */ ICameraMetadataNativeKey* key,
    /* [in] */ IInterface* value)
{
    Int32 tag;
    key->GetTag(&tag);

    if (value == NULL) {
        // Erase the entry
        return WriteValues(tag, /*src*/NULL);
    } // else update the entry to a new value

    AutoPtr<IMarshaler> marshaler = GetMarshalerForKey(key);
    Int32 size;
    marshaler->CalculateMarshalSize(value, &size);

    // TODO: Optimization. Cache the byte[] and reuse if the size is big enough.
    AutoPtr<ArrayOf<Byte> > values = ArrayOf<Byte>::Alloc(size);
    AutoPtr<IByteBuffer> buffer;
    AutoPtr<IByteBufferHelper> bufferHelper;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bufferHelper);
    bufferHelper->Wrap(values, (IByteBuffer**)&buffer);

    AutoPtr<IByteOrderHelper> orderHelper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&orderHelper);
    ByteOrder bOrder;
    orderHelper->GetNativeOrder(&bOrder);
    buffer->SetOrder(bOrder);
    marshaler->Marshal(value, buffer);

    return WriteValues(tag, values);
}

Boolean CameraMetadataNative::SetAvailableFormats(
    /* [in] */ ArrayOf<Int32>* value)
{
    if (value == NULL) {
        // Let setBase() to handle the null value case.
        return FALSE;
    }

    AutoPtr<ArrayOf<Int32> > newValues = ArrayOf<Int32>::Alloc(value->GetLength());
    for (Int32 i = 0; i < value->GetLength(); i++) {
        (*newValues)[i] = (*value)[i];
        if ((*value)[i] == IImageFormat::JPEG) {
            (*newValues)[i] = NATIVE_JPEG_FORMAT;
        }
    }

    AutoPtr<IArrayOf> array = CoreUtils::Convert(newValues);
    SetBase(CameraCharacteristics::SCALER_AVAILABLE_FORMATS, TO_IINTERFACE(array));
    return TRUE;
}

Boolean CameraMetadataNative::SetFaceRectangles(
    /* [in] */ ArrayOf<IRect*>* faceRects)
{
    if (faceRects == NULL) {
        return FALSE;
    }

    AutoPtr<ArrayOf<IRect*> > newFaceRects = ArrayOf<IRect*>::Alloc(faceRects->GetLength());
    for (Int32 i = 0; i < newFaceRects->GetLength(); i++) {
        AutoPtr<IRect> rect = (*faceRects)[i];
        Int32 left;
        rect->GetLeft(&left);
        Int32 top;
        rect->GetTop(&top);
        Int32 right;
        rect->GetRight(&right);
        Int32 bottom;
        rect->GetBottom(&bottom);

        AutoPtr<IRect> tmp;
        CRect::New(left, top, right + left, bottom + top, (IRect**)&tmp);
        newFaceRects->Set(i, tmp);
    }

    AutoPtr<IArrayOf> array = CoreUtils::Convert(newFaceRects.Get());
    SetBase(CaptureResult::STATISTICS_FACE_RECTANGLES, TO_IINTERFACE(array));
    return TRUE;
}

Boolean CameraMetadataNative::SetTonemapCurve(
    /* [in] */ ITonemapCurve* tc)
{
    if (tc == NULL) {
        return FALSE;
    }

    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Float> > > > curve = ArrayOf<AutoPtr<ArrayOf<Float> > >::Alloc(3);

    for (Int32 i = ITonemapCurve::CHANNEL_RED; i <= ITonemapCurve::CHANNEL_BLUE; i++) {
        Int32 pointCount;
        tc->GetPointCount(i, &pointCount);
        (*curve)[i] = ArrayOf<Float>::Alloc(pointCount * ITonemapCurve::POINT_SIZE);
        tc->CopyColorCurve(i, (*curve)[i], 0);
    }

    AutoPtr<IArrayOf> redArray = CoreUtils::Convert((*curve)[0]);
    SetBase(CaptureRequest::TONEMAP_CURVE_RED, TO_IINTERFACE(redArray));
    AutoPtr<IArrayOf> greenArray = CoreUtils::Convert((*curve)[1]);
    SetBase(CaptureRequest::TONEMAP_CURVE_GREEN, TO_IINTERFACE(greenArray));
    AutoPtr<IArrayOf> blueArray = CoreUtils::Convert((*curve)[2]);
    SetBase(CaptureRequest::TONEMAP_CURVE_BLUE, TO_IINTERFACE(blueArray));

    return TRUE;
}

ECode CameraMetadataNative::Swap(
    /* [in] */ ICameraMetadataNative* other)
{
    NativeSwap(other);
    return NOERROR;
}

ECode CameraMetadataNative::GetEntryCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    return NativeGetEntryCount(count);
}

ECode CameraMetadataNative::IsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    return NativeIsEmpty(result);
}

ECode CameraMetadataNative::GetTag(
    /* [in] */ const String& key,
    /* [out] */ Int32* tag)
{
    VALIDATE_NOT_NULL(tag);

    return NativeGetTagFromKey(key, tag);
}

ECode CameraMetadataNative::GetNativeType(
    /* [in] */ Int32 tag,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);

    return NativeGetTypeFromTag(tag, type);
}

ECode CameraMetadataNative::WriteValues(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte>* src)
{
    NativeWriteValues(tag, src);
    return NOERROR;
}

ECode CameraMetadataNative::ReadValues(
    /* [in] */ Int32 tag,
    /* [out, callee] */ ArrayOf<Byte>** values)
{
    VALIDATE_NOT_NULL(values);

    // TODO: Optimization. Native code returns a ByteBuffer instead.
    return NativeReadValues(tag, values);
}

ECode CameraMetadataNative::DumpToLog()
{
    //try {
    ECode ec = NativeDump();
    //} catch (IOException e) {
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Dump logging failed %d", ec);
    }
    //}
    return NOERROR;
}

AutoPtr<IMarshaler> CameraMetadataNative::GetMarshalerForKey(
    /* [in] */ ICameraMetadataNativeKey* key)
{
    AutoPtr<ITypeReference> ref;
    key->GetTypeReference((ITypeReference**)&ref);
    Int32 tag;
    key->GetTag(&tag);
    Int32 type;
    GetNativeType(tag, &type);
    AutoPtr<IMarshaler> outma;
    assert(0);
    //MarshalRegistry::GetMarshaler(ref, type, (IMarshaler**)&outma);
    return outma;
}

Boolean CameraMetadataNative::AreValuesAllNull(
    /* [in] */ ArrayOf<IInterface*>* objs)
{
    for (Int32 i = 0; i < objs->GetLength(); i++) {
        AutoPtr<IInterface> o = (*objs)[i];
        if (o != NULL) return FALSE;
    }
    return TRUE;
}

android::CameraMetadata* CameraMetadataNative::CameraMetadata_getPointerNoThrow(
    /* [in] */ ICameraMetadataNative* thiz)
{
    if (thiz == NULL) {
        return NULL;
    }

    return reinterpret_cast<android::CameraMetadata*>(mMetadataPtr);
}

ECode CameraMetadataNative::CameraMetadata_getPointerThrow(
    /* [in] */ ICameraMetadataNative* thiz,
    /* [out] */ android::CameraMetadata** data,
    /* [in] */ const char* argName)
{
    VALIDATE_NOT_NULL(data);
    *data = NULL;

    if (thiz == NULL) {
        Logger::E("CameraMetadataNative::CameraMetadata_getPointerThrow for null reference %s", argName);
        return E_NULL_POINTER_EXCEPTION;
    }

    android::CameraMetadata* metadata = CameraMetadata_getPointerNoThrow(thiz);
    if (metadata == NULL) {
        Logger::E("CameraMetadataNative::CameraMetadata_getPointerThrow", "Metadata object was already closed");
        return E_ILLEGAL_STATE_EXCEPTION;;
    }

    *data = metadata;
    return NOERROR;
}

Int64 CameraMetadataNative::NativeAllocate()
{
    Logger::V(TAG, "%s", __FUNCTION__);

    return reinterpret_cast<Int64>(new android::CameraMetadata());
}

android::CameraMetadata* CameraMetadataNative::GetNative()
{
    return reinterpret_cast<android::CameraMetadata*>(mMetadataPtr);
}

ECode CameraMetadataNative::NativeAllocateCopy(
    /* [in] */ ICameraMetadataNative* other,
    /* [out] */ Int64* handle)
{
    VALIDATE_NOT_NULL(handle);
    *handle = 0;

    Logger::V(TAG, "%s", __FUNCTION__);

    android::CameraMetadata* otherMetadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(other, &otherMetadata, "other"))

    // In case of exception, return
    if (otherMetadata == NULL) return NOERROR;

    // Clone native metadata and return new pointer
    *handle = reinterpret_cast<Int64>(new android::CameraMetadata(*otherMetadata));
    return NOERROR;
}

ECode CameraMetadataNative::NativeWriteToParcel(
    /* [in] */ IParcel* parcel)
{
    AutoLock syncLock(this);

    android::CameraMetadata* metadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))
    if (metadata == NULL) {
        return NOERROR;
    }

    android::Parcel* parcelNative;
    parcel->GetElementPayload((Handle32*)&parcelNative);
    if (parcelNative == NULL) {
        //jniThrowNullPointerException(env, "parcel");
        Logger::E(TAG, "parcel");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::status_t err;
    if ((err = metadata->writeToParcel(parcelNative)) != android::OK) {
        Logger::E(TAG, "Failed to write to parcel (error code %d)", err);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode CameraMetadataNative::NativeReadFromParcel(
    /* [in] */ IParcel* source)
{
    AutoLock syncLock(this);
    android::CameraMetadata* metadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))
    if (metadata == NULL) {
        return NOERROR;
    }

    android::Parcel* parcelNative;
    source->GetElementPayload((Handle32*)&parcelNative);
    if (parcelNative == NULL) {
        Logger::E(TAG, "parcel");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::status_t err;
    if ((err = metadata->readFromParcel(parcelNative)) != android::OK) {
        Logger::E(TAG, "Failed to read from parcel (error code %d)", err);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

ECode CameraMetadataNative::NativeSwap(
    /* [in] */ ICameraMetadataNative* other)
{
    AutoLock syncLock(this);

    android::CameraMetadata* metadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))

    // order is important: we can't call another JNI method
    // if there is an exception pending
    if (metadata == NULL) return NOERROR;

    android::CameraMetadata* otherMetadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(other, (android::CameraMetadata**)&otherMetadata, "other"))

    if (otherMetadata == NULL) return NOERROR;

    metadata->swap(*otherMetadata);
    return NOERROR;
}

void CameraMetadataNative::NativeClose()
{
    AutoLock syncLock(this);
    Logger::V(TAG, "%s", __FUNCTION__);

    android::CameraMetadata* metadata = CameraMetadata_getPointerNoThrow(this);

    if (metadata != NULL) {
        delete metadata;
        mMetadataPtr = 0;
    }

    LOG_ALWAYS_FATAL_IF(CameraMetadata_getPointerNoThrow(this) != NULL,
                        "Expected the native ptr to be 0 after #close");
}

ECode CameraMetadataNative::NativeIsEmpty(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Boolean empty = FALSE;
    {
        AutoLock syncLock(this);

        android::CameraMetadata* metadata;
        FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))

        if (metadata == NULL) {
            Logger::W(TAG, "%s: Returning early due to exception being thrown", __FUNCTION__);
            *result = TRUE; // actually throws java exc.
            return NOERROR;
        }

        empty = metadata->isEmpty();

        Logger::V(TAG, "%s: %p IsEmpty returned %d, entry count was %d",
            __FUNCTION__, this, empty, metadata->entryCount());

        {
            Logger::V(TAG, "============TEST==================");
            Int32 tag = 327680;
            camera_metadata_entry entry = metadata->find(tag);
            Logger::V(TAG, "%s: %p-%p Tag %d found %d entries", __FUNCTION__, this, metadata, tag, entry.count);
            if (entry.count == 0 && !metadata->exists(tag)) {
                Logger::V(TAG, "%s: %p-%p Tag %d does not have any entries", __FUNCTION__, this, metadata, tag);
            }
            Logger::V(TAG, "==============TEST===================");

            tag = 524293;
            entry = metadata->find(tag);
            Logger::V(TAG, "%s: %p-%p Tag %d found %d entries", __FUNCTION__, this, metadata, tag, entry.count);
            if (entry.count == 0 && !metadata->exists(tag)) {
                Logger::V(TAG, "%s: %p-%p Tag %d does not have any entries", __FUNCTION__, this, metadata, tag);
            }
            Logger::V(TAG, "==============TEST===================");
        }
    }

    *result = empty;
    return NOERROR;
}

ECode CameraMetadataNative::NativeGetEntryCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = 0;

    Int32 res = 0;
    {
        AutoLock syncLock(this);
        Logger::V(TAG, "%s", __FUNCTION__);

        android::CameraMetadata* metadata;
        FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))

        if (metadata == NULL) {
            *count = 0; // actually throws java exc.
            return NOERROR;
        }

        res = metadata->entryCount();
    }
    *count = res;
    return NOERROR;
}

namespace {
struct Helpers {
    static size_t getTypeSize(uint8_t type) {
        if (type >= NUM_TYPES) {
            Logger::E(TAG, "%s: Invalid type specified (%ud)", __FUNCTION__, type);
            return static_cast<size_t>(-1);
        }

        return camera_metadata_type_size[type];
    }

    static android::status_t updateAny(android::CameraMetadata *metadata,
                          uint32_t tag,
                          uint32_t type,
                          const void *data,
                          size_t dataBytes) {

        if (type >= NUM_TYPES) {
            Logger::E(TAG, "%s: Invalid type specified (%ud)", __FUNCTION__, type);
            return android::INVALID_OPERATION;
        }

        size_t typeSize = getTypeSize(type);

        if (dataBytes % typeSize != 0) {
            Logger::E(TAG, "%s: Expected dataBytes (%ud) to be divisible by typeSize "
                  "(%ud)", __FUNCTION__, dataBytes, typeSize);
            return android::BAD_VALUE;
        }

        size_t dataCount = dataBytes / typeSize;

        switch(type) {
#define METADATA_UPDATE(runtime_type, compile_type)                            \
            case runtime_type: {                                               \
                const compile_type *dataPtr =                                  \
                        static_cast<const compile_type*>(data);                \
                return metadata->update(tag, dataPtr, dataCount);              \
            }                                                                  \

            METADATA_UPDATE(TYPE_BYTE,     uint8_t);
            METADATA_UPDATE(TYPE_INT32,    int32_t);
            METADATA_UPDATE(TYPE_FLOAT,    float);
            METADATA_UPDATE(TYPE_INT64,    int64_t);
            METADATA_UPDATE(TYPE_DOUBLE,   double);
            METADATA_UPDATE(TYPE_RATIONAL, camera_metadata_rational_t);

            default: {
                // unreachable
                Logger::E(TAG, "%s: Unreachable", __FUNCTION__);
                return android::INVALID_OPERATION;
            }
        }

#undef METADATA_UPDATE
    }
};
} // namespace {}

ECode CameraMetadataNative::NativeReadValues(
    /* [in] */ Int32 tag,
    /* [out] */ ArrayOf<Byte>** values)
{
    VALIDATE_NOT_NULL(values);
    *values = NULL;

    AutoPtr<ArrayOf<Byte> > byteArray;
    {
        AutoLock syncLock(this);
        Logger::V(TAG, "%s (tag = %d)", __FUNCTION__, tag);

        android::CameraMetadata* metadata;
        FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))
        if (metadata == NULL) {
            return NOERROR;
        }

        camera_metadata_entry entry = metadata->find(tag);
        if (entry.count == 0) {
             if (!metadata->exists(tag)) {
                Logger::V(TAG, "%s: %p-%p Tag %d does not have any entries", __FUNCTION__, this, metadata, tag);
                *values = NULL;
                return NOERROR;
             }
             else {
                 // OK: we will return a 0-sized array.
                 Logger::V(TAG, "%s: Tag %d had an entry, but it had 0 data", __FUNCTION__, tag);
             }
        }

        int tagType = get_camera_metadata_tag_type(tag);
        if (tagType == -1) {
            Logger::E(TAG, "Tag (%d) did not have a type", tag);
            return NOERROR;
        }

        size_t tagSize = Helpers::getTypeSize(tagType);
        Int32 byteCount = entry.count * tagSize;
        byteArray = ArrayOf<Byte>::Alloc(byteCount);
        memcpy(byteArray->GetPayload(), entry.data.u8, byteCount);
    }

    *values = byteArray;
    REFCOUNT_ADD(*values)
    return NOERROR;
}

ECode CameraMetadataNative::NativeWriteValues(
    /* [in] */ Int32 tag,
    /* [in] */ ArrayOf<Byte>* src)
{
    AutoLock syncLock(this);
    Logger::V(TAG, "%s (tag = %d)", __FUNCTION__, tag);

    android::CameraMetadata* metadata;
    FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))
    if (metadata == NULL) return NOERROR;

    int tagType = get_camera_metadata_tag_type(tag);
    if (tagType == -1) {
        Logger::E(TAG, "Tag (%d) did not have a type", tag);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    size_t UNUSED(tagSize) = Helpers::getTypeSize(tagType);

    android::status_t res;

    if (src == NULL) {
        // If array is NULL, delete the entry
        if (metadata->exists(tag)) {
            res = metadata->erase(tag);
            Logger::V(TAG, "%s: Erase values (res = %d)", __FUNCTION__, res);
        }
        else {
            res = android::OK;
            Logger::V(TAG, "%s: Don't need to erase", __FUNCTION__);
        }
    }
    else {
        // Copy from java array into native array
        //ScopedByteArrayRO arrayReader(env, src);
        if (src == NULL) return NOERROR;

        res = Helpers::updateAny(metadata, static_cast<uint32_t>(tag),
                                 tagType, src->GetPayload(), src->GetLength());

        Logger::V(TAG, "%s: Update values (res = %d)", __FUNCTION__, res);
    }

    if (res == android::OK) {
        return NOERROR;
    }
    else if (res == android::BAD_VALUE) {
        Logger::E(TAG, "Src byte array was poorly formed");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if (res == android::INVALID_OPERATION) {
        Logger::E(TAG, "Internal error while trying to update metadata");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    else {
        Logger::E(TAG,  "Unknown error (%d) while trying to update metadata", res);
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    return NOERROR;
}

struct DumpMetadataParams {
    int writeFd;
    const android::CameraMetadata* metadata;
};

static void* CameraMetadata_writeMetadataThread(
    /* [in] */ void* arg) {
    DumpMetadataParams* p = static_cast<DumpMetadataParams*>(arg);

    /*
     * Write the dumped data, and close the writing side FD.
     */
    p->metadata->dump(p->writeFd, /*verbosity*/2);

    if (close(p->writeFd) < 0) {
        Logger::E(TAG, "%s: Failed to close writeFd (errno = %#x, message = '%s')",
                __FUNCTION__, errno, strerror(errno));
    }

    return NULL;
}

ECode CameraMetadataNative::NativeDump()
{
    {
        AutoLock syncLock(this);
        Logger::V(TAG, "%s", __FUNCTION__);
        android::CameraMetadata* metadata;
        FAIL_RETURN(CameraMetadata_getPointerThrow(this, (android::CameraMetadata**)&metadata))
        if (metadata == NULL) {
            return NOERROR;
        }

        /*
         * Create a socket pair for local streaming read/writes.
         *
         * The metadata will be dumped into the write side,
         * and then read back out (and logged) via the read side.
         */

        int writeFd, readFd;
        {

            int sv[2];
            if (socketpair(AF_LOCAL, SOCK_STREAM, /*protocol*/0, &sv[0]) < 0) {
                // jniThrowExceptionFmt(env, "java/io/IOException",
                //         "Failed to create socketpair (errno = %#x, message = '%s')",
                //         errno, strerror(errno));
                Logger::E(TAG, "Failed to create socketpair (errno = %#x, message = '%s')",
                        errno, strerror(errno));
                return E_IO_EXCEPTION;
            }
            writeFd = sv[0];
            readFd = sv[1];
        }

        /*
         * Create a thread for doing the writing.
         *
         * The reading and writing must be concurrent, otherwise
         * the write will block forever once it exhausts the capped
         * buffer size (from getsockopt).
         */
        pthread_t writeThread;
        DumpMetadataParams params = {
            writeFd,
            metadata
        };

        {
            int threadRet = pthread_create(&writeThread, /*attr*/NULL,
                    CameraMetadata_writeMetadataThread, (void*)&params);

            if (threadRet != 0) {
                close(writeFd);

                // jniThrowExceptionFmt(env, "java/io/IOException",
                //         "Failed to create thread for writing (errno = %#x, message = '%s')",
                //         threadRet, strerror(threadRet));
                Logger::E(TAG, "Failed to create thread for writing (errno = %#x, message = '%s')",
                        threadRet, strerror(threadRet));
                return E_IO_EXCEPTION;

            }
        }

        /*
         * Read out a byte until stream is complete. Write completed lines
         * to ALOG.
         */
        {
            char out[] = {'\0', '\0'}; // large enough to append as a string
            android::String8 logLine;

            // Read one byte at a time! Very slow but avoids complicated \n scanning.
            ssize_t res;
            while ((res = TEMP_FAILURE_RETRY(read(readFd, &out[0], /*count*/1))) > 0) {
                if (out[0] == '\n') {
                    Logger::D(TAG, "%s", logLine.string());
                    logLine.clear();
                }
                else {
                    logLine.append(out);
                }
            }

            if (res < 0) {
                // jniThrowExceptionFmt(env, "java/io/IOException",
                //         "Failed to read from fd (errno = %#x, message = '%s')",
                //         errno, strerror(errno));
                Logger::E(TAG, "Failed to read from fd (errno = %#x, message = '%s')",
                        errno, strerror(errno));
                return E_IO_EXCEPTION;
            }
            else if (!logLine.isEmpty()) {
                Logger::D(TAG, "%s", logLine.string());
            }
        }

        int res;

        // Join until thread finishes. Ensures params/metadata is valid until then.
        if ((res = pthread_join(writeThread, /*retval*/NULL)) != 0) {
            Logger::E(TAG, "%s: Failed to join thread (errno = %#x, message = '%s')",
                    __FUNCTION__, res, strerror(res));
        }
    }
    return NOERROR;
}

ECode CameraMetadataNative::NativeGetTagFromKey(
    /* [in] */ const String& keyName,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    if (VERBOSE) Logger::V(TAG, " >> %s %s", __FUNCTION__, keyName.string());

    const char *key = keyName.string();
    if (key == NULL) {
        Logger::E(TAG, "NativeGetTagFromKey null keyname.");
        *result = 0;
        return NOERROR;
    }
    size_t keyLength = strlen(key);

    Logger::V(TAG, "%s (key = '%s')", __FUNCTION__, key);

    android::sp<android::VendorTagDescriptor> vTags = android::VendorTagDescriptor::getGlobalVendorTagDescriptor();

    android::SortedVector<android::String8> vendorSections;
    size_t vendorSectionCount = 0;

    if (vTags != NULL) {
        vendorSections = vTags->getAllSectionNames();
        vendorSectionCount = vendorSections.size();
    }

    // First, find the section by the longest string match
    const char *section = NULL;
    size_t sectionIndex = 0;
    size_t sectionLength = 0;
    size_t totalSectionCount = ANDROID_SECTION_COUNT + vendorSectionCount;
    for (size_t i = 0; i < totalSectionCount; ++i) {

        const char *str = (i < ANDROID_SECTION_COUNT) ? camera_metadata_section_names[i] :
                vendorSections[i - ANDROID_SECTION_COUNT].string();
        Logger::V(TAG, "%s: Trying to match against section '%s'", __FUNCTION__, str);
        if (strstr(key, str) == key) { // key begins with the section name
            size_t strLength = strlen(str);

            Logger::V(TAG, "%s: Key begins with section name", __FUNCTION__);

            // section name is the longest we've found so far
            if (section == NULL || sectionLength < strLength) {
                section = str;
                sectionIndex = i;
                sectionLength = strLength;

                Logger::V(TAG, "%s: Found new best section (%s)", __FUNCTION__, section);
            }
        }
    }

    // TODO: Make above get_camera_metadata_section_from_name ?

    if (section == NULL) {
        *result = 0;
        Logger::E(TAG, "Could not find section name for key '%s')", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        Logger::V(TAG, "%s: Found matched section '%s' (%d)", __FUNCTION__, section, sectionIndex);
    }

    // Get the tag name component of the key
    const char *keyTagName = key + sectionLength + 1; // x.y.z -> z
    if (sectionLength + 1 >= keyLength) {
        *result = 0;
        Logger::E(TAG, "Key length too short for key '%s')", key);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Match rest of name against the tag names in that section only
    uint32_t tag = 0;
    if (sectionIndex < ANDROID_SECTION_COUNT) {
        // Match built-in tags (typically android.*)
        uint32_t tagBegin, tagEnd; // [tagBegin, tagEnd)
        tagBegin = camera_metadata_section_bounds[sectionIndex][0];
        tagEnd = camera_metadata_section_bounds[sectionIndex][1];

        for (tag = tagBegin; tag < tagEnd; ++tag) {
            const char *tagName = get_camera_metadata_tag_name(tag);

            if (strcmp(keyTagName, tagName) == 0) {
                Logger::V(TAG, "%s: Found matched tag '%s' (%d)",
                      __FUNCTION__, tagName, tag);
                break;
            }
        }

        if (tag == tagEnd) {
            *result = 0;
            Logger::E(TAG, "Could not find tag name for key '%s')", key);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    else if (vTags != NULL) {
        // Match vendor tags (typically com.*)
        const android::String8 sectionName(section);
        const android::String8 tagName(keyTagName);

        android::status_t res = android::OK;
        if ((res = vTags->lookupTag(tagName, sectionName, &tag)) != android::OK) {
            *result = 0;
            Logger::E(TAG, "%s: No vendor tag matches key '%s'", __FUNCTION__, key);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // TODO: Make above get_camera_metadata_tag_from_name ?

    *result = tag;

    if (VERBOSE) Logger::V(TAG, " >> %s %s, tag: %d", __FUNCTION__, keyName.string(), tag);
    return NOERROR;
}

ECode CameraMetadataNative::NativeGetTypeFromTag(
    /* [in] */ Int32 tag,
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = -1;

    Int32 tagType = get_camera_metadata_tag_type(tag);
    if (tagType == -1) {
        // jniThrowExceptionFmt(env, "java/lang/IllegalArgumentException",
        //                      "Tag (%d) did not have a type", tag);
        *type = -1;
        Logger::E(TAG, "Tag (%d) did not have a type", tag);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *type = tagType;
    return NOERROR;
}

void CameraMetadataNative::NativeClassInit()
{
    // // XX: Why do this separately instead of doing it in the register function?
    // Logger::V(TAG, "%s", __FUNCTION__);

    // field fields_to_find[] = {
    //     { CAMERA_METADATA_CLASS_NAME, "mMetadataPtr", "J", &fields.metadata_ptr },
    // };

    // // Do this here instead of in register_native_methods,
    // // since otherwise it will fail to find the fields.
    // if (find_fields(env, fields_to_find, NELEM(fields_to_find)) < 0)
    //     return;

    // jclass clazz = env->FindClass(CAMERA_METADATA_CLASS_NAME);
    return;
}


} // namespace Impl
} // namespace Camera2
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos