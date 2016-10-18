
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableParcelable.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::IPair;
using Elastos::Utility::Logging::Logger;
using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableParcelable::MarshalerParcelable::MarshalerParcelable(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableParcelable* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    assert(0);
    // mClass = (Class<T>)typeReference.getRawType();
    // Field creatorField;
    // try {
    //     creatorField = mClass.getDeclaredField(FIELD_CREATOR);
    // } catch (NoSuchFieldException e) {
    //     // Impossible. All Parcelable implementations must have a 'CREATOR' static field
    //     throw new AssertionError(e);
    // }

    // try {
    //     mCreator = (Parcelable.Creator<T>)creatorField.get(null);
    // } catch (IllegalAccessException e) {
    //     // Impossible: All 'CREATOR' static fields must be public
    //     throw new AssertionError(e);
    // } catch (IllegalArgumentException e) {
    //     // Impossible: This is a static field, so null must be ok
    //     throw new AssertionError(e);
    // }
}

ECode MarshalQueryableParcelable::MarshalerParcelable::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    if (VERBOSE) {
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::V("MarshalQueryableParcelable", "marshal %s", str.string());
    }

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    AutoPtr<ArrayOf<Byte> > parcelContents;

    //try {
    ECode ec;
    ec = IParcelable::Probe(value)->WriteToParcel(parcel);
    FAIL_GOTO(ec, EXIT);

    Boolean res;
    parcel->HasFileDescriptors(&res);
    if (res) {
        // throw new UnsupportedOperationException(
        //         "Parcelable " + value + " must not have file descriptors");
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::E("MarshalQueryableParcelable", "Parcelable %s must not have file descriptors",str.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    parcel->Marshall((ArrayOf<Byte>**)&parcelContents);
    //}
    //finally {
EXIT:
    // parcel->Recycle();
    //}

    if (parcelContents->GetLength() == 0) {
        //throw new AssertionError("No data marshaled for " + value);
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::E(TAG, "No data marshaled for %s", str.string());
        return E_ASSERTION_ERROR;
    }

    return buffer->Put(parcelContents);
}

ECode MarshalQueryableParcelable::MarshalerParcelable::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    if (VERBOSE) {
        Int32 remaining;
        IBuffer::Probe(buffer)->GetRemaining(&remaining);
        Logger::V(TAG, "unmarshal, buffer remaining %d", remaining);
    }

    /*
     * Quadratically slow when marshaling an array of parcelables.
     *
     * Read out the entire byte buffer as an array, then copy it into the parcel.
     *
     * Once we unparcel the entire object, advance the byte buffer by only how many
     * bytes the parcel actually used up.
     *
     * Future: If we ever do need to use parcelable arrays, we can do this a little smarter
     * by reading out a chunk like 4,8,16,24 each time, but not sure how to detect
     * parcels being too short in this case.
     *
     * Future: Alternatively use Parcel#obtain(long) directly into the native
     * pointer of a ByteBuffer, which would not copy if the ByteBuffer was direct.
     */
    IBuffer::Probe(buffer)->Mark();

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    //try {
    ECode ec;
    AutoPtr<IInterface> value;
    AutoPtr<ArrayOf<Byte> > remaining;
    Int32 maxLength;
    ec = IBuffer::Probe(buffer)->GetRemaining(&maxLength);
    FAIL_GOTO(ec, EXIT);

    remaining = ArrayOf<Byte>::Alloc(maxLength);
    ec = buffer->Get(remaining);
    FAIL_GOTO(ec, EXIT);

    ec = parcel->Unmarshall(remaining, /*offset*/0, maxLength);
    FAIL_GOTO(ec, EXIT);
    ec = parcel->SetDataPosition(/*pos*/0);
    FAIL_GOTO(ec, EXIT);

    assert(0);
    // value = mCreator.createFromParcel(parcel);
    Int32 actualLength;
    ec = parcel->GetDataPosition(&actualLength);
    FAIL_GOTO(ec, EXIT);

    if (actualLength == 0) {
        //throw new AssertionError("No data marshaled for " + value);
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::E("MarshalQueryableParcelable", "No data marshaled for %s", str.string());
        return E_ASSERTION_ERROR;
    }

    // set the position past the bytes the parcelable actually used
    ec = IBuffer::Probe(buffer)->Reset();
    FAIL_GOTO(ec, EXIT);
    Int32 pos;
    ec = IBuffer::Probe(buffer)->GetPosition(&pos);
    FAIL_GOTO(ec, EXIT);
    ec = IBuffer::Probe(buffer)->SetPosition(pos + actualLength);
    FAIL_GOTO(ec, EXIT);

    if (VERBOSE) {
        Logger::V(TAG, "unmarshal, parcel length was %d", actualLength);
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::V(TAG, "unmarshal, value is %s", str.string());
    }

    *outface = value;
    REFCOUNT_ADD(*outface);
    //} finally {
EXIT:
    // /parcel.recycle();
    //}
    return NOERROR;
}

ECode MarshalQueryableParcelable::MarshalerParcelable::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = IMarshaler::NATIVE_SIZE_DYNAMIC;
    return NOERROR;
}

ECode MarshalQueryableParcelable::MarshalerParcelable::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    *outvalue = 0;

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    //try {
    ECode ec;
    Int32 length;
    AutoPtr<ArrayOf<Byte> > bytes;
    ec = IParcelable::Probe(value)->WriteToParcel(parcel);
    FAIL_GOTO(ec, EXIT);
    ec = parcel->Marshall((ArrayOf<Byte>**)&bytes);
    FAIL_GOTO(ec, EXIT);
    length = bytes->GetLength();

    if (VERBOSE) {
        String str;
        IObject::Probe(value)->ToString(&str);
        Logger::V("MarshalQueryableParcelable", "calculateMarshalSize, length when "
                "parceling %s is %d", str.string(), length);
    }

    *outvalue = length;
    //} finally {
EXIT:
    //return parcel->Recycle();
    //}
    return NOERROR;
}

const String MarshalQueryableParcelable::TAG("MarshalParcelable");
const Boolean MarshalQueryableParcelable::VERBOSE = FALSE; //Log.isLoggable(TAG, Log.VERBOSE);

const String MarshalQueryableParcelable::FIELD_CREATOR("CREATOR");

CAR_INTERFACE_IMPL_2(MarshalQueryableParcelable, Object,
        IMarshalQueryableParcelable, IMarshalQueryable)

ECode MarshalQueryableParcelable::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerParcelable(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableParcelable::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    // assert(0);
    //return Parcelable.class.isAssignableFrom(managedType.getRawType());
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
