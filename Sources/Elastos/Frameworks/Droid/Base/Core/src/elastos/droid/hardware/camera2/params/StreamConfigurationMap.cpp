
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include "elastos/droid/hardware/camera2/utils/HashCodeHelpers.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include "elastos/droid/graphics/PixelFormat.h"
#include "elastos/droid/graphics/CImageFormat.h"
#include "elastos/droid/os/Debug.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/ClassLoader.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/Objects.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Droid::Hardware::Camera2::Utils::HashCodeHelpers;
using Elastos::Droid::Graphics::IImageFormat;
using Elastos::Droid::Graphics::CImageFormat;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::PixelFormat;
using Elastos::Droid::Os::Debug;
using Elastos::Core::StringBuilder;
using Elastos::Core::ClassLoader;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IIterator;
using Elastos::Utility::Objects;
using Elastos::Utility::ISet;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

const String StreamConfigurationMap::TAG("StreamConfigurationMap");

const Int32 StreamConfigurationMap::HAL_PIXEL_FORMAT_BLOB = 0x21;
const Int32 StreamConfigurationMap::HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED = 0x22;
const Int32 StreamConfigurationMap::HAL_PIXEL_FORMAT_RAW_OPAQUE = 0x24;

const Int32 StreamConfigurationMap::DURATION_MIN_FRAME = 0;
const Int32 StreamConfigurationMap::DURATION_STALL = 1;

List< AutoPtr<IClassInfo> > StreamConfigurationMap::sOutputSupportedClasses;

CAR_INTERFACE_IMPL(StreamConfigurationMap, Object, IStreamConfigurationMap)

StreamConfigurationMap::StreamConfigurationMap()
{
}

StreamConfigurationMap::~StreamConfigurationMap()
{
}

ECode StreamConfigurationMap::constructor()
{
    return NOERROR;
}

ECode StreamConfigurationMap::constructor(
    /* [in] */ ArrayOf<IStreamConfiguration*>* configurations,
    /* [in] */ ArrayOf<IStreamConfigurationDuration*>* minFrameDurations,
    /* [in] */ ArrayOf<IStreamConfigurationDuration*>* stallDurations,
    /* [in] */ ArrayOf<IHighSpeedVideoConfiguration*>* highSpeedVideoConfigurations)
{
    FAIL_RETURN(Preconditions::CheckArrayElementsNotNull((ArrayOf<IInterface*>*)configurations, String("configurations")))
    FAIL_RETURN(Preconditions::CheckArrayElementsNotNull((ArrayOf<IInterface*>*)minFrameDurations, String("minFrameDurations")))
    FAIL_RETURN(Preconditions::CheckArrayElementsNotNull((ArrayOf<IInterface*>*)stallDurations, String("stallDurations")))
    mConfigurations = configurations;
    mMinFrameDurations = minFrameDurations;
    mStallDurations = stallDurations;

    if (highSpeedVideoConfigurations == NULL) {
        mHighSpeedVideoConfigurations = ArrayOf<IHighSpeedVideoConfiguration*>::Alloc(0);
    }
    else {
        FAIL_RETURN(Preconditions::CheckArrayElementsNotNull(
                (ArrayOf<IInterface*>*)highSpeedVideoConfigurations, String("highSpeedVideoConfigurations")))
        mHighSpeedVideoConfigurations = highSpeedVideoConfigurations;
    }

    // For each format, track how many sizes there are available to configure
    for (Int32 i = 0; i < configurations->GetLength(); i++) {
        AutoPtr<IStreamConfiguration> config = (*configurations)[i];

        Boolean result;
        config->IsOutput(&result);
        HashMap<Int32, Int32>& map = result ? mOutputFormats : mInputFormats;

        Int32 format;
        config->GetFormat(&format);
        Int32 count = 0;
        HashMap<Int32, Int32>::Iterator it = map.Find(format);
        if (it != map.End()) {
            count = it->mSecond;
        }
        else {
            count = 0;
        }

        count = count + 1;
        map[format] = count;
    }

    Boolean hasKey;
    HashMap<Int32, Int32>::Iterator it = mOutputFormats.Find(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED);
    if (it != mOutputFormats.End()) {
        hasKey = TRUE;
    }
    else {
        hasKey = FALSE;
    }

    if (!hasKey) {
        // throw new AssertionError(
        //         "At least one stream configuration for IMPLEMENTATION_DEFINED must exist");
        Logger::E(TAG, "At least one stream configuration for IMPLEMENTATION_DEFINED must exist");
        return E_ASSERTION_ERROR;;
    }

    // For each Size/FPS range, track how many FPS range/Size there are available
    for (Int32 i = 0; i < mHighSpeedVideoConfigurations->GetLength(); i++) {
        AutoPtr<IHighSpeedVideoConfiguration> config = (*mHighSpeedVideoConfigurations)[i];

        AutoPtr<ISize> size;
        config->GetSize((ISize**)&size);
        AutoPtr<Range<IInteger32> > fpsRange;
        assert(0 && "TODO");
        // config->GetFpsRange((IInterface**)&fpsRange);
        Int32 fpsRangeCount;
        HashMap<AutoPtr<ISize>, Int32>::Iterator it = mHighSpeedVideoSizeMap.Find(size);
        if (it != mHighSpeedVideoSizeMap.End()) {
            fpsRangeCount = it->mSecond;
        }
        else {
            fpsRangeCount = 0;
        }
        mHighSpeedVideoSizeMap[size] = fpsRangeCount + 1;

        Int32 sizeCount;
        HashMap<AutoPtr<Range<IInteger32> >, Int32>::Iterator _it = mHighSpeedVideoFpsRangeMap.Find(fpsRange);
        if (_it != mHighSpeedVideoFpsRangeMap.End()) {
            sizeCount = _it->mSecond;
        }
        else {
            sizeCount = 0;
        }
        mHighSpeedVideoFpsRangeMap[fpsRange] = sizeCount + 1;
    }
    return NOERROR;
}

ECode StreamConfigurationMap::GetOutputFormats(
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr)

    return GetPublicFormats(/*output*/TRUE, outarr);
}

ECode StreamConfigurationMap::GetInputFormats(
    /* [out, callee] */ ArrayOf<Int32>** outarr)
{
    VALIDATE_NOT_NULL(outarr)

    return GetPublicFormats(/*output*/FALSE, outarr);
}

ECode StreamConfigurationMap::GetInputSizes(
    /* [in] */ Int32 format,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr)

    return GetPublicFormatSizes(format, /*output*/FALSE, outarr);
}

ECode StreamConfigurationMap::IsOutputSupportedFor(
    /* [in] */ Int32 format,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Int32 _format;
    FAIL_RETURN(CheckArgumentFormat(format, &_format))
    Int32 tmp;
    FAIL_RETURN(ImageFormatToInternal(_format, &tmp))
    _format = tmp;
    HashMap<Int32, Int32> formatsMap = GetFormatsMap(/*output*/TRUE);
    HashMap<Int32, Int32>::Iterator it = formatsMap.Find(_format);
    if (it != formatsMap.End()) {
        *value = TRUE;
    }
    else {
        *value = FALSE;
    }
    return NOERROR;
}

ECode StreamConfigurationMap::IsOutputSupportedFor(
    /* [in] */ IClassInfo* klass,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    FAIL_RETURN(Preconditions::CheckNotNull(klass, String("klass must not be null")))

    if (sOutputSupportedClasses.IsEmpty()) {
        AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
        AutoPtr<IClassInfo> irClass;
        String classes[6];
        classes[0] = String("Elastos.Droid.Media.CImageReader");
        classes[1] = String("Elastos.Droid.Media.CMediaRecorder");
        classes[2] = String("Elastos.Droid.Media.CMediaCodec");
        classes[3] = String("Elastos.Droid.Renderscript.CAllocation");
        classes[4] = String("Elastos.Droid.View.CSurfaceHolder");
        classes[5] = String("Elastos.Droid.Graphics.CSurfaceTexture");

        for (Int32 i = 0; i < 6; ++i) {
            AutoPtr<IClassInfo> info;
            cl->LoadClass(classes[i], (IClassInfo**)&info);
            sOutputSupportedClasses.PushBack(info);
        }
    }

    AutoPtr<IClassInfo> temp = klass;
    List< AutoPtr<IClassInfo> >::Iterator it = Find(
        sOutputSupportedClasses.Begin(), sOutputSupportedClasses.End(), temp);
    if (it != sOutputSupportedClasses.End()) {
        *value = TRUE;
    }

    return NOERROR;
}

ECode StreamConfigurationMap::IsOutputSupportedFor(
    /* [in] */ ISurface* surface,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    FAIL_RETURN(Preconditions::CheckNotNull(surface, String("surface must not be null")))
    return E_UNSUPPORTED_OPERATION_EXCEPTION;

    // TODO: JNI function that checks the Surface's IGraphicBufferProducer state
}

ECode StreamConfigurationMap::GetOutputSizes(
    /* [in] */ IClassInfo* klass,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    // Image reader is "supported", but never for implementation-defined formats; return empty
    Boolean isAssignable = FALSE;
    Int32 itfCount;
    klass->GetInterfaceCount(&itfCount);
    Logger::I(TAG, " >> GetOutputSizes: interface count: %d", itfCount);
    AutoPtr< ArrayOf<IInterfaceInfo *> > itfes = ArrayOf<IInterfaceInfo *>::Alloc(itfCount);
    if (itfes == NULL) {
        Logger::E(TAG, "GetOutputSizes: E_OUT_OF_MEMORY.");
        return E_OUT_OF_MEMORY;
    }
    klass->GetAllInterfaceInfos(itfes.Get());
    String name, ns;
    for (Int32 i = 0; i < itfCount; ++i) {
        (*itfes)[i]->GetName(&name);
        (*itfes)[i]->GetNamespace(&ns);
        if (name.Equals("IImageReader") && ns.Equals("Elastos.Droid.Media")) {
            isAssignable = TRUE;
            break;
        }
    }

    if (isAssignable) {
        AutoPtr< ArrayOf<ISize*> > array = ArrayOf<ISize*>::Alloc(0);
        *outarr = array;
        REFCOUNT_ADD(*outarr)
        return NOERROR;
    }

    Boolean result;
    FAIL_RETURN(IsOutputSupportedFor(klass, &result))
    if (result == FALSE) {
        *outarr = NULL;
    }

    return GetInternalFormatSizes(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED, /*output*/TRUE, outarr);
}

ECode StreamConfigurationMap::GetOutputSizes(
    /* [in] */ Int32 format,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    return GetPublicFormatSizes(format, /*output*/TRUE, outarr);
}

ECode StreamConfigurationMap::GetHighSpeedVideoSizes(
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    AutoPtr<ArrayOf<ISize*> > _outarr = ArrayOf<ISize*>::Alloc(mHighSpeedVideoSizeMap.GetSize());
    HashMap<AutoPtr<ISize>, Int32>::Iterator it;
    Int32 i = 0;
    for (it = mHighSpeedVideoSizeMap.Begin(); it != mHighSpeedVideoSizeMap.End(); ++it) {
        AutoPtr<ISize> key = it->mFirst;
        _outarr->Set(i++, key.Get());
    }
    *outarr = _outarr;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode StreamConfigurationMap::GetHighSpeedVideoFpsRangesFor(
    /* [in] */ ISize* size,
    /* [out, callee] */ ArrayOf<IInterface*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    Int32 fpsRangeCount;
    HashMap<AutoPtr<ISize>, Int32>::Iterator it = mHighSpeedVideoSizeMap.Find(size);
    if (it == mHighSpeedVideoSizeMap.End() || it->mSecond == 0) {
        Logger::E(TAG, "Size %s does not support high speed video recording", TO_CSTR(size));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        fpsRangeCount = it->mSecond;
    }

    assert(0 && "TODO");
    //@SuppressWarnings("unchecked")
    // AutoPtr<ArrayOf<Range<IInteger32>* > > fpsRanges= ArrayOf<Range<IInteger32>* >::Alloc(fpsRangeCount);
    // for (Int32 i = 0; i < mHighSpeedVideoConfigurations->GetLength(); i++) {
    //     AutoPtr<IHighSpeedVideoConfiguration> config = (*mHighSpeedVideoConfigurations)[i];
    //     AutoPtr<ISize> _size;
    //     config->GetSize((ISize**)&_size);
    //     Boolean result;
    //     size->Equals(_size, &result);
    //     if (result) {
    //         AutoPtr<Range<IInteger32> > fpsRange;
    //         config->GetFpsRange((Handle32*)&fpsRange);
    //         fpsRanges->Set(i, fpsRange.Get());
    //     }
    // }

    // *outarr = fpsRanges;
    // REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode StreamConfigurationMap::GetHighSpeedVideoFpsRanges(
    /* [out, callee] */ ArrayOf<IInterface*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    assert(0 && "TODO");
    // AutoPtr<ArrayOf<Range<IInteger32>* > > _outarr = ArrayOf<Range<IInteger32>* >::Alloc(mHighSpeedVideoFpsRangeMap.GetSize());
    // HashMap<AutoPtr<Range<IInteger32> >, Int32>::Iterator it;
    // Int32 i = 0;
    // for (it = mHighSpeedVideoFpsRangeMap.Begin(); it != mHighSpeedVideoFpsRangeMap.End(); ++it) {
    //     AutoPtr<Range<IInteger32> > key = it->mFirst;
    //     _outarr->Set(i++, key.Get());
    // }
    // *outarr = _outarr;
    // REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode StreamConfigurationMap::GetHighSpeedVideoSizesFor(
    /* [in] */ IInterface* fpsRange,
    /* [out, callee] */ ArrayOf<ISize*>** outarr)
{
    VALIDATE_NOT_NULL(outarr);
    *outarr = NULL;

    Int32 sizeCount;
    AutoPtr<Range<IInteger32> > range = (Range<IInteger32>*)IObject::Probe(fpsRange);
    HashMap<AutoPtr<Range<IInteger32> >, Int32>::Iterator it = mHighSpeedVideoFpsRangeMap.Find(range);
    if (it == mHighSpeedVideoFpsRangeMap.End() || it->mSecond == 0) {
        Logger::E(TAG, "FpsRange %s does not support high speed video recording", TO_CSTR(fpsRange));
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        sizeCount = it->mSecond;
    }

    AutoPtr<ArrayOf<ISize*> > sizes = ArrayOf<ISize*>::Alloc(sizeCount);
    for (Int32 i = 0; i < mHighSpeedVideoConfigurations->GetLength(); i++) {
        AutoPtr<IHighSpeedVideoConfiguration> config = (*mHighSpeedVideoConfigurations)[i];
        AutoPtr<Range<IInteger32> > _range;
        config->GetFpsRange((IInterface**)&_range);
        Boolean result;
        range->Equals(TO_IINTERFACE(_range), &result);
        if (result) {
            AutoPtr<ISize> _size;
            config->GetSize((ISize**)&_size);
            sizes->Set(i, _size.Get());
        }
    }
    *outarr = sizes;
    REFCOUNT_ADD(*outarr);
    return NOERROR;
}

ECode StreamConfigurationMap::GetOutputMinFrameDuration(
    /* [in] */ Int32 format,
    /* [in] */ ISize* size,
    /* [out] */Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    FAIL_RETURN(Preconditions::CheckNotNull(size, String("size must not be null")))

    Int32 result;
    FAIL_RETURN(CheckArgumentFormatSupported(format, /*output*/TRUE, &result))

    Int32 internal;
    FAIL_RETURN(ImageFormatToInternal(format, &internal))
    return GetInternalFormatDuration(internal, size, DURATION_MIN_FRAME, value);
}

ECode StreamConfigurationMap::GetOutputMinFrameDuration(
    /* [in] */ IClassInfo* klass,
    /* [in] */ ISize* size,
    /* [out] */Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Boolean result;
    FAIL_RETURN(IsOutputSupportedFor(klass, &result))
    if (!result) {
        Logger::E(TAG, "klass was not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return GetInternalFormatDuration(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED,
            size, DURATION_MIN_FRAME, value);
}

ECode StreamConfigurationMap::GetOutputStallDuration(
    /* [in] */ Int32 format,
    /* [in] */ ISize* size,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Int32 result;
    FAIL_RETURN(CheckArgumentFormatSupported(format, /*output*/TRUE, &result))

    Int32 internal;
    FAIL_RETURN(ImageFormatToInternal(format, &internal))
    return GetInternalFormatDuration(internal,
            size, DURATION_STALL, value);
}

ECode StreamConfigurationMap::GetOutputStallDuration(
    /* [in] */ IClassInfo* klass,
    /* [in] */ ISize* size,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    Boolean result;
    FAIL_RETURN(IsOutputSupportedFor(klass, &result))
    if (!result) {
        Logger::E(TAG, "klass was not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return GetInternalFormatDuration(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED,
            size, DURATION_STALL, value);
}

ECode StreamConfigurationMap::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* equal)
{
    VALIDATE_NOT_NULL(equal);

    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }
    else if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    else if (IStreamConfigurationMap::Probe(obj) != NULL) {
        const AutoPtr<StreamConfigurationMap> other = (StreamConfigurationMap*)IStreamConfigurationMap::Probe(obj);
        // XX: do we care about order?
        *equal = (Arrays::Equals(mConfigurations, other->mConfigurations)) &&
        (Arrays::Equals(mMinFrameDurations, other->mMinFrameDurations)) &&
        (Arrays::Equals(mStallDurations, other->mStallDurations)) &&
        (Arrays::Equals(mHighSpeedVideoConfigurations, other->mHighSpeedVideoConfigurations));
        return NOERROR;
    }
    *equal = FALSE;
    return NOERROR;
}

ECode StreamConfigurationMap::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    // XX: do we care about order?
    Int32 h1, h2, h3, h4;
    HashCodeHelpers::GetHashCode(mConfigurations.Get(), &h1);
    HashCodeHelpers::GetHashCode(mMinFrameDurations.Get(), &h2);
    HashCodeHelpers::GetHashCode(mStallDurations.Get(), &h3);
    HashCodeHelpers::GetHashCode(mHighSpeedVideoConfigurations.Get(), &h4);
    return HashCodeHelpers::GetHashCode(h1, h2, h3, h4, value);
}

ECode StreamConfigurationMap::CheckArgumentFormatSupported(
    /* [in] */ Int32 format,
    /* [in] */ Boolean output,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    Int32 _format;
    FAIL_RETURN(CheckArgumentFormat(format, &_format))

    AutoPtr<ArrayOf<Int32> > formats;
    if (output) {
        FAIL_RETURN(GetOutputFormats((ArrayOf<Int32>**)&formats))
    }
    else {
        FAIL_RETURN(GetInputFormats((ArrayOf<Int32>**)&formats))
    }

    for (Int32 i = 0; i < formats->GetLength(); ++i) {
        if (_format == (*formats)[i]) {
            *result = _format;
            return NOERROR;
        }
    }

    Logger::E(TAG, "format %08x is not supported by this %s stream configuration map %s",
        format, output ? "output" : "input", Arrays::ToString(formats.Get()).string());
    Debug::DumpBacktrace();
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode StreamConfigurationMap::CheckArgumentFormatInternal(
    /* [in] */ Int32 format,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
        case HAL_PIXEL_FORMAT_BLOB:
        case HAL_PIXEL_FORMAT_RAW_OPAQUE:
            *result = format;
            return NOERROR;
        case IImageFormat::JPEG:
            Logger::E(TAG, "ImageFormat.JPEG is an unknown internal format");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        default:
            return CheckArgumentFormat(format, result);
    }
    return NOERROR;
}

ECode StreamConfigurationMap::CheckArgumentFormat(
    /* [in] */ Int32 format,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    AutoPtr<IImageFormat> helper;
    CImageFormat::AcquireSingleton((IImageFormat**)&helper);
    Boolean res;
    helper->IsPublicFormat(format, &res);
    if (!res && !(PixelFormat::IsPublicFormat(format))) {
        Logger::E(TAG, "format 0x%x was not defined in either ImageFormat or PixelFormat", format);
        Debug::DumpBacktrace();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = format;
    return NOERROR;
}

ECode StreamConfigurationMap::ImageFormatToPublic(
    /* [in] */ Int32 format,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    switch (format) {
        case HAL_PIXEL_FORMAT_BLOB:
            *result = IImageFormat::JPEG;
            return NOERROR;
        case IImageFormat::JPEG:
            Logger::E(TAG, "ImageFormat.JPEG is an unknown internal format");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
            Logger::E(TAG, "IMPLEMENTATION_DEFINED must not leak to public API");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        default:
            *result = format;
            return NOERROR;
    }
    return NOERROR;
}

ECode StreamConfigurationMap::ImageFormatToPublic(
    /* [in] */ ArrayOf<Int32>* formats)
{
    if (formats == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < formats->GetLength(); ++i) {
        FAIL_RETURN(ImageFormatToPublic((*formats)[i], &((*formats)[i])))
    }
    return NOERROR;
}

ECode StreamConfigurationMap::ImageFormatToInternal(
    /* [in] */ Int32 format,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    switch (format) {
        case IImageFormat::JPEG:
            *result = HAL_PIXEL_FORMAT_BLOB;
            return NOERROR;
        case HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED:
            Logger::E(TAG, "IMPLEMENTATION_DEFINED is not allowed via public API");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        default:
            *result = format;
            return NOERROR;
    }
    return NOERROR;
}

ECode StreamConfigurationMap::ImageFormatToInternal(
    /* [in] */ ArrayOf<Int32>* formats)
{
    if (formats == NULL) {
        return NOERROR;
    }

    for (Int32 i = 0; i < formats->GetLength(); ++i) {
        FAIL_RETURN(ImageFormatToInternal((*formats)[i], &((*formats)[i])))
    }
    return NOERROR;
}

ECode StreamConfigurationMap::GetPublicFormatSizes(
    /* [in] */ Int32 format,
    /* [in] */ Boolean output,
    /* [out, callee] */ ArrayOf<ISize*>** sizes)
{
    VALIDATE_NOT_NULL(sizes);
    *sizes = NULL;

    //try {
    Int32 result;
    ECode ec = CheckArgumentFormatSupported(format, output, &result);
    //} catch (IllegalArgumentException e) {
    if (FAILED(ec)) {
        return ec;
    }
    //}

    Int32 tmp;
    FAIL_RETURN(ImageFormatToInternal(format, &tmp))
    format = tmp;

    return GetInternalFormatSizes(format, output, sizes);
}

ECode StreamConfigurationMap::GetInternalFormatSizes(
    /* [in] */ Int32 format,
    /* [in] */ Boolean output,
    /* [out, callee] */ ArrayOf<ISize*>** _sizes)
{
    VALIDATE_NOT_NULL(_sizes);
    *_sizes = NULL;

    HashMap<Int32, Int32> formatsMap = GetFormatsMap(output);
    Int32 sizesCount;
    HashMap<Int32, Int32>::Iterator it = formatsMap.Find(format);
    if (it == formatsMap.End()) {
        Logger::E(TAG, "format not available");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else {
        sizesCount = it->mSecond;
    }

    Int32 len = sizesCount;
    AutoPtr<ArrayOf<ISize*> > sizes = ArrayOf<ISize*>::Alloc(len);
    Int32 sizeIndex = 0;
    for (Int32 i = 0; i < mConfigurations->GetLength(); i++) {
        AutoPtr<IStreamConfiguration> config = (*mConfigurations)[i];
        Int32 _format;
        Boolean _output;
        config->GetFormat(&_format);
        config->IsOutput(&_output);
        if (_format == format && _output == output) {
            AutoPtr<ISize> size;
            config->GetSize((ISize**)&size);
            sizes->Set(sizeIndex++, size);
        }
    }

    if (sizeIndex != len) {
        Logger::E(TAG, "Too few sizes (expected %d, actual %d)", len, sizeIndex);
        return E_ASSERTION_ERROR;
    }

    *_sizes = sizes;
    REFCOUNT_ADD(*_sizes);
    return NOERROR;
}

ECode StreamConfigurationMap::GetPublicFormats(
    /* [in] */ Boolean output,
    /* [out, callee] */ ArrayOf<Int32>** _formats)
{
    VALIDATE_NOT_NULL(_formats);
    *_formats = NULL;

    Int32 count;
    GetPublicFormatCount(output, &count);
    AutoPtr<ArrayOf<Int32> > formats = ArrayOf<Int32>::Alloc(count);

    Int32 i = 0;

    AutoPtr<ISet> lkeyset;
    HashMap<Int32, Int32> map = GetFormatsMap(output);
    HashMap<Int32, Int32>::Iterator it;
    for (it = map.Begin(); it != map.End(); ++it) {
        Int32 format = it->mFirst;
        if (format != HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
            (*formats)[i++] = format;
        }
    }

    if (formats->GetLength() != i) {
        Logger::E(TAG, "Too few formats %d, expected %d", i, formats->GetLength());
        return E_ASSERTION_ERROR;
    }

    FAIL_RETURN(ImageFormatToPublic(formats))
    *_formats = formats;
    REFCOUNT_ADD(*_formats);
    return NOERROR;
}

HashMap<Int32, Int32>& StreamConfigurationMap::GetFormatsMap(
    /* [in] */ Boolean output)
{
    return output ? mOutputFormats : mInputFormats;
}

ECode StreamConfigurationMap::GetInternalFormatDuration(
    /* [in] */ Int32 format,
    /* [in] */ ISize* size,
    /* [in] */ Int32 duration,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    // assume format is already checked, since its internal
    AutoPtr<ArrayOf<ISize*> > sizes;
    GetInternalFormatSizes(format, /*output*/TRUE, (ArrayOf<ISize*>**)&sizes);

    if (!ArrayContains((ArrayOf<IInterface*>*)sizes.Get(), TO_IINTERFACE(size))) {
        Logger::E(TAG, "size was not supported");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > durations;
    GetDurations(duration, (ArrayOf<IStreamConfigurationDuration*>**)&durations);
    Int32 cFormat, cWidth, cHeight, sWidth, sHeight;
    IStreamConfigurationDuration* configurationDuration;
    for (Int32 i = 0; i < durations->GetLength(); i++) {
        configurationDuration = (*durations)[i];
        configurationDuration->GetFormat(&cFormat);
        configurationDuration->GetWidth(&cWidth);
        configurationDuration->GetHeight(&cHeight);
        size->GetWidth(&sWidth);
        size->GetHeight(&sHeight);
        if (cFormat == format && cWidth == sWidth &&  cHeight == sHeight) {
            return configurationDuration->GetDuration(result);
        }

    }
    // Default duration is '0' (unsupported/no extra stall)
    *result = 0;
    return NOERROR;
}

ECode StreamConfigurationMap::GetDurations(
    /* [in] */ Int32 duration,
    /* [out, callee] */ ArrayOf<IStreamConfigurationDuration*>** durations)
{
    VALIDATE_NOT_NULL(durations);
    *durations = NULL;

    switch (duration) {
        case DURATION_MIN_FRAME:
            *durations = mMinFrameDurations;
            REFCOUNT_ADD(*durations);
            return NOERROR;
        case DURATION_STALL:
            *durations =  mStallDurations;
            REFCOUNT_ADD(*durations);
            return NOERROR;
        default:
            Logger::E(TAG, "duration was invalid");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode StreamConfigurationMap::GetPublicFormatCount(
    /* [in] */ Boolean output,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    HashMap<Int32, Int32> formatsMap = GetFormatsMap(output);

    Int32 size = formatsMap.GetSize();
    HashMap<Int32, Int32>::Iterator it = formatsMap.Find(HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED);
    if (it != mOutputFormats.End()) {
        size -= 1;
    }
    *result = size;
    return NOERROR;
}

Boolean StreamConfigurationMap::ArrayContains(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ IInterface* element)
{
    if (array == NULL) {
        return FALSE;
    }

    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IInterface> el = (*array)[i];
        if (Objects::Equals(el.Get(), element)) {
            return TRUE;
        }
    }

    return FALSE;
}

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos