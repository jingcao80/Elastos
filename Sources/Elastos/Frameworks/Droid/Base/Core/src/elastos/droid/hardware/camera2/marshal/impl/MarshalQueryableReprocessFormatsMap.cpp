
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableReprocessFormatsMap.h"
#include "elastos/droid/hardware/camera2/params/CReprocessFormatsMap.h"
#include "elastos/droid/hardware/camera2/params/StreamConfigurationMap.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Hardware::Camera2::Params::IReprocessFormatsMap;
using Elastos::Droid::Hardware::Camera2::Params::CReprocessFormatsMap;
using Elastos::Droid::Hardware::Camera2::Params::ECLSID_CReprocessFormatsMap;
using Elastos::Droid::Hardware::Camera2::Params::StreamConfigurationMap;
using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableReprocessFormatsMap::MarshalerReprocessFormatsMap::MarshalerReprocessFormatsMap(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableReprocessFormatsMap* host)
{
    Marshaler::constructor(host, typeReference, nativeType);
}

ECode MarshalQueryableReprocessFormatsMap::MarshalerReprocessFormatsMap::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    /*
     * // writing (static example, DNG+ZSL)
     * int32_t[] contents = {
     *   RAW_OPAQUE, 3, RAW16, YUV_420_888, BLOB,
     *   RAW16, 2, YUV_420_888, BLOB,
     *   ...,
     *   INPUT_FORMAT, OUTPUT_FORMAT_COUNT, [OUTPUT_0, OUTPUT_1, ..., OUTPUT_FORMAT_COUNT-1]
     * };
     */
    AutoPtr<IReprocessFormatsMap> map = IReprocessFormatsMap::Probe(value);
    AutoPtr<ArrayOf<Int32> > inputs;
    map->GetInputs((ArrayOf<Int32>**)&inputs);
    StreamConfigurationMap::ImageFormatToInternal(inputs);
    for (Int32 i = 0; i < inputs->GetLength(); i++) {
        Int32 input = (*inputs)[i];
        // INPUT_FORMAT
        buffer->PutInt32(input);

        AutoPtr<ArrayOf<Int32> > outputs;
        map->GetOutputs(input, (ArrayOf<Int32>**)&outputs);
        StreamConfigurationMap::ImageFormatToInternal(outputs);
        // OUTPUT_FORMAT_COUNT
        buffer->PutInt32(outputs->GetLength());

        // [OUTPUT_0, OUTPUT_1, ..., OUTPUT_FORMAT_COUNT-1]
        for (Int32 j = 0; j < outputs->GetLength(); j++) {
            Int32 output = (*outputs)[j];
            buffer->PutInt32(output);
        }
    }
    return NOERROR;
}

ECode MarshalQueryableReprocessFormatsMap::MarshalerReprocessFormatsMap::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 remaining;
    IBuffer::Probe(buffer)->GetRemaining(&remaining);
    Int32 len = remaining / IMarshalHelpers::SIZEOF_INT32;
    if (remaining % IMarshalHelpers::SIZEOF_INT32 != 0) {
        //throw new AssertionError("ReprocessFormatsMap was not TYPE_INT32");
        Logger::E("MarshalQueryableReprocessFormatsMap", "ReprocessFormatsMap was not TYPE_INT32");
        return E_ASSERTION_ERROR;
    }

    AutoPtr<ArrayOf<Int32> > entries = ArrayOf<Int32>::Alloc(len);

    AutoPtr<IInt32Buffer> intBuffer;
    buffer->AsInt32Buffer((IInt32Buffer**)&intBuffer);
    intBuffer->Get(entries);

    // TODO: consider moving rest of parsing code from ReprocessFormatsMap to here
    AutoPtr<IReprocessFormatsMap> map;
    CReprocessFormatsMap::New(entries, (IReprocessFormatsMap**)&map);
    *outface = TO_IINTERFACE(map);
    REFCOUNT_ADD(*outface);
    return NOERROR;
}

ECode MarshalQueryableReprocessFormatsMap::MarshalerReprocessFormatsMap::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = Marshaler::NATIVE_SIZE_DYNAMIC;
    return NOERROR;
}

ECode MarshalQueryableReprocessFormatsMap::MarshalerReprocessFormatsMap::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    AutoPtr<IReprocessFormatsMap> map = IReprocessFormatsMap::Probe(value);

    /*
     * // writing (static example, DNG+ZSL)
     * int32_t[] contents = {
     *   RAW_OPAQUE, 3, RAW16, YUV_420_888, BLOB,
     *   RAW16, 2, YUV_420_888, BLOB,
     *   ...,
     *   INPUT_FORMAT, OUTPUT_FORMAT_COUNT, [OUTPUT_0, OUTPUT_1, ..., OUTPUT_FORMAT_COUNT-1]
     * };
     */
    Int32 length = 0;

    AutoPtr<ArrayOf<Int32> > inputs;
    map->GetInputs((ArrayOf<Int32>**)&inputs);
    for (Int32 i = 0; i < inputs->GetLength(); i++) {
        Int32 input = (*inputs)[i];

        length += 1; // INPUT_FORMAT
        length += 1; // OUTPUT_FORMAT_COUNT

        AutoPtr<ArrayOf<Int32> > outputs;
        map->GetOutputs(input, (ArrayOf<Int32>**)&outputs);
        length += outputs->GetLength(); // [OUTPUT_0, OUTPUT_1, ..., OUTPUT_FORMAT_COUNT-1]
    }

    *outvalue = length * IMarshalHelpers::SIZEOF_INT32;
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(MarshalQueryableReprocessFormatsMap, Object,
        IMarshalQueryableReprocessFormatsMap, IMarshalQueryable)

ECode MarshalQueryableReprocessFormatsMap::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerReprocessFormatsMap(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableReprocessFormatsMap::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (nativeType == ICameraMetadataNative::TYPE_INT32) {
        ClassID cls;
        managedType->GetClassType(&cls);
        if (cls == ECLSID_CReprocessFormatsMap) {
            *value = TRUE;
            return NOERROR;
        }
    }
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
