
#include "DataInputStream.h"
#include "CPushbackInputStream.h"
#include "Math.h"
#include "StringBuilder.h"
#include "CModifiedUtf8.h"

using Elastos::Core::StringBuilder;
using Elastos::Core::Math;
using Elastos::IO::Charset::IModifiedUtf8;
using Elastos::IO::Charset::CModifiedUtf8;

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL_2(DataInputStream, FilterInputStream, IDataInputStream, IDataInput)

DataInputStream::DataInputStream()
{
    mBuff = ArrayOf<Byte>::Alloc(8);
    assert(mBuff);
}

DataInputStream::~DataInputStream()
{
}

ECode DataInputStream::constructor(
    /* [in] */ IInputStream* in)
{
    return FilterInputStream::constructor(in);
}

ECode DataInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(buffer)
    return InputStream::Read(buffer, number);
}

ECode DataInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    VALIDATE_NOT_NULL(buffer)

    return mIn->Read(buffer, byteOffset, byteCount, number);
}

ECode DataInputStream::ReadBoolean(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Int32 temp;
    FAIL_RETURN(mIn->Read(&temp));
    if (temp < 0) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    *value = temp != 0;
    return NOERROR;
}

ECode DataInputStream::ReadByte(
    /* [out] */ Byte* value)
{
    VALIDATE_NOT_NULL(value)
    *value = '\0';

    Int32 temp;
    FAIL_RETURN(mIn->Read(&temp));
    if (temp < 0) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    *value = (Byte)temp;
    return NOERROR;
}

ECode DataInputStream::ReadChar(
    /* [out] */ Char32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = -1;

    Int32 number;
    FAIL_RETURN(ReadToBuff(4, &number));
    if (number < 0) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    *value = String((const char*)mBuff->GetPayload()).GetChar(0);

    return NOERROR;
}

ECode DataInputStream::ReadToBuff(
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    Int32 offset = 0;
    while(offset < count) {
        FAIL_RETURN(Read(mBuff, offset, count - offset, number));
        if(*number == -1) return NOERROR;
        offset += *number;
    }

    return NOERROR;
}

ECode DataInputStream::ReadDouble(
    /* [out] */ Double* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int64 int64Value;
    FAIL_RETURN(ReadInt64(&int64Value));
    *value = Elastos::Core::Math::Int64BitsToDouble(int64Value);
    return NOERROR;
}

ECode DataInputStream::ReadFloat(
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 int32Value;
    FAIL_RETURN(ReadInt32(&int32Value));
    *value = Elastos::Core::Math::Int32BitsToFloat(int32Value);
    return NOERROR;
}

ECode DataInputStream::ReadFully(
    /* [out] */ ArrayOf<byte> * buffer)
{
    VALIDATE_NOT_NULL(buffer)
    return ReadFully(buffer, 0, buffer->GetLength());
}

ECode DataInputStream::ReadFully(
    /* [out] */ ArrayOf<byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length)
{
    VALIDATE_NOT_NULL(buffer)
    // BEGIN android-removed
    // if (length < 0) {
    //     throw new IndexOutOfBoundsException();
    // }
    // END android-removed
    if(length == 0) return NOERROR;

    if ((mIn == NULL) || (buffer == NULL)) {
        return E_NULL_POINTER_EXCEPTION;
    }

    // BEGIN android-changed
    // Exception priorities (in case of multiple errors) differ from
    // RI, but are spec-compliant.
    // used (offset | length) < 0 instead of separate (offset < 0) and
    // (length < 0) check to safe one operation
    if ((offset | length) < 0 || offset > buffer->GetLength() - length) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    // END android-changed
    Int32 number;
    while (length > 0) {
        FAIL_RETURN(mIn->Read(buffer, offset, length, &number));
        if (number < 0) return E_EOF_EXCEPTION;
        offset += number;
        length -= number;
    }
    return NOERROR;
}

ECode DataInputStream::ReadInt32(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 number;
    FAIL_RETURN(ReadToBuff(4, &number));
    if (number < 0) return E_EOF_EXCEPTION;
    *value = (((*mBuff)[0] & 0xff) << 24) | (((*mBuff)[1] & 0xff) << 16) |
             (((*mBuff)[2] & 0xff) << 8) | ((*mBuff)[3] & 0xff);

    return NOERROR;
}

ECode DataInputStream::ReadLine(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("");

    AutoPtr<StringBuilder> line = new StringBuilder(80);
    Boolean foundTerminator = FALSE;

    while (TRUE) {
        Int32 nextByte;
        FAIL_RETURN(mIn->Read(&nextByte));
        switch (nextByte) {
            case -1:
                if (line->GetLength() == 0 && !foundTerminator) {
                    *str = NULL;
                    return NOERROR;
                }
                *str = line->ToString();
                return NOERROR;
            case (Byte)'\r':
                if (foundTerminator) {
                    IPushbackInputStream::Probe(mIn)->Unread(nextByte);///////////not implemented
                    *str = line->ToString();
                    return NOERROR;
                }
                foundTerminator = TRUE;
                /* Have to be able to peek ahead one byte */ ///////////////not implemented
                if (!IPushbackInputStream::Probe(mIn)) {
                    AutoPtr<IPushbackInputStream> in;
                    CPushbackInputStream::New(mIn, (IPushbackInputStream**)&in);
                    mIn = IInputStream::Probe(in);
                }
                break;
            case (Byte)'\n':
                *str = line->ToString();
                return NOERROR;
            default:
                if (foundTerminator) {
                   IPushbackInputStream::Probe(mIn)->Unread(nextByte);//////////////not implemented
                    *str = line->ToString();
                    return NOERROR;
                }
                line->AppendChar((Char32)nextByte);
        }
    }
    return NOERROR;
}

ECode DataInputStream::ReadInt64(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 number;
    FAIL_RETURN(ReadToBuff(8, &number));
    if (number < 0) return E_EOF_EXCEPTION;
    Int32 i1 = (((*mBuff)[0] & 0xff) << 24) | (((*mBuff)[1] & 0xff) << 16) |
               (((*mBuff)[2] & 0xff) << 8) | ((*mBuff)[3] & 0xff);
    Int32 i2 = (((*mBuff)[4] & 0xff) << 24) | (((*mBuff)[5] & 0xff) << 16) |
               (((*mBuff)[6] & 0xff) << 8) | ((*mBuff)[7] & 0xff);

    *value = ((i1 & 0xffffffffLL) << 32) | (i2 & 0xffffffffLL);

    return NOERROR;
}

ECode DataInputStream::ReadInt16(
    /* [out] */ Int16* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 number;
    FAIL_RETURN(ReadToBuff(2, &number));
    if (number < 0) return E_EOF_EXCEPTION;
    *value = (Int16)((((*mBuff)[0] & 0xff) << 8) | ((*mBuff)[1] & 0xff));

    return NOERROR;
}

ECode DataInputStream::ReadUnsignedByte(
    /* [out] */ Int32* value)
{
    FAIL_RETURN(mIn->Read(value));
    return *value < 0? E_EOF_EXCEPTION : NOERROR;
}

ECode DataInputStream::ReadUnsignedInt16(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;

    Int32 number;
    FAIL_RETURN(ReadToBuff(2, &number));
    if (number < 0) return E_EOF_EXCEPTION;
    *value = (UInt16)((((*mBuff)[0] & 0xff) << 8) | ((*mBuff)[1] & 0xff));

    return NOERROR;
}

ECode DataInputStream::ReadUTF(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String(NULL);

    Int32 value = 0;
    FAIL_RETURN(ReadUnsignedInt16(&value));
    return DecodeUTF(value, str);
}

ECode DataInputStream::SkipBytes(
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    Int32 skipped = 0;
    Int64 skip;

    while (skipped < count && (mIn->Skip(count - skipped, &skip), skip != 0)) {
        skipped += skip;
    }
    // BEGIN android-removed
    // if (skipped < 0) {
    //     return E_IO_EXCEPTION;//throw new EOFException();
    // }
    // END android-removed
    *number = skipped;

    return NOERROR;
}

String DataInputStream::ReadUTF(
    /* [in] */ IDataInput* dataInput)
{
    assert(dataInput);

    Int32 value = 0;
    dataInput->ReadUnsignedInt16(&value);

    return DecodeUTF(value, dataInput);
}

ECode DataInputStream::DecodeUTF(
    /* [in] */ Int32 utfSize,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    *str = DecodeUTF(utfSize, THIS_PROBE(IDataInput));
    return NOERROR;
}

String DataInputStream::DecodeUTF(
    /* [in] */ Int32 utfSize,
    /* [in] */ IDataInput* in)
{
    assert(in);

    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(utfSize);
    in->ReadFully(buf, 0, utfSize);


    AutoPtr<IModifiedUtf8> mu;
    CModifiedUtf8::AcquireSingleton((IModifiedUtf8**)&mu);
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(utfSize);
    String result;
    mu->Decode(buf, chars, 0, utfSize, &result);
    return result;
}

} // namespace IO
} // namespace Elastos
