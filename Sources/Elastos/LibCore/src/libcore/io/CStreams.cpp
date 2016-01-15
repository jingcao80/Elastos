
#include "CStreams.h"
#include "Math.h"
#include "StringBuilder.h"
#include "CByteArrayOutputStream.h"
#include "CStringWriter.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::IWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::IStringWriter;
using Elastos::Core::IStringBuffer;

namespace Libcore {
namespace IO {

CAR_INTERFACE_IMPL(CStreams, Singleton, IStreams)

CAR_SINGLETON_IMPL(CStreams)

AutoPtr<ArrayOf<Byte> > CStreams::mSkipBuffer;

ECode CStreams::ReadSingleByte(
    /* [in] */ IInputStream* in,
    /* [out] */ Int32* singleByte)
{
    VALIDATE_NOT_NULL(singleByte);
    *singleByte = '\0';
    VALIDATE_NOT_NULL(in);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    Int32 result;
    FAIL_RETURN(in->Read(buffer.Get(), 0, 1, &result));
    *singleByte = (result != -1) ? (*buffer)[0] & 0xff : -1;
    return NOERROR;
}

ECode CStreams::WriteSingleByte(
    /* [in] */ IOutputStream* out,
    /* [in] */ Int32 b)
{
    VALIDATE_NOT_NULL(out);

    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1);
    (*buffer)[0] = (Byte)(b & 0xff);
    return out->Write(buffer);
}

ECode CStreams::ReadFully(
    /* [in] */ IInputStream* in,
    /* [out, callee] */ ArrayOf<Byte>** dst)
{
    VALIDATE_NOT_NULL(dst);
    *dst = NULL;
    VALIDATE_NOT_NULL(in);
    ECode ec = ReadFullyNoClose(in, dst);
    FAIL_RETURN(ICloseable::Probe(in)->Close())
    return ec;
}

ECode CStreams::ReadFully(
    /* [in] */ IInputStream* in,
    /* [in] */ ArrayOf<Byte>* dst,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount)
{
    if (dst == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (byteCount == 0) {
        return NOERROR;
    }
    if (in == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if ((offset | byteCount) < 0 || offset > dst->GetLength() ||
            dst->GetLength() - offset < byteCount) {
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 bytesRead;
    while (byteCount > 0) {
        FAIL_RETURN(in->Read(dst, offset, byteCount, &bytesRead));
        if (bytesRead < 0) {
            return E_EOF_EXCEPTION;
        }
        offset += bytesRead;
        byteCount -= bytesRead;
    }

    return NOERROR;
}

ECode CStreams::ReadFullyCloseWhenDone(
    /* [in] */ IInputStream* in,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes);
    *bytes = NULL;
    VALIDATE_NOT_NULL(in);

    ECode ec = ReadFullyNoClose(in, bytes);
    ICloseable::Probe(in)->Close();
    return ec;
}

ECode CStreams::ReadFullyNoClose(
    /* [in] */ IInputStream* in,
    /* [out, callee] */ ArrayOf<Byte>** byteArray)
{
    VALIDATE_NOT_NULL(byteArray);
    *byteArray = NULL;
    VALIDATE_NOT_NULL(in);

    AutoPtr<IByteArrayOutputStream> bytes;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&bytes);
    AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(1024);
    Int32 count;
    FAIL_RETURN(in->Read(buffer, &count));
    while (count != -1) {
        FAIL_RETURN(IOutputStream::Probe(bytes)->Write(buffer, 0, count));
        FAIL_RETURN(in->Read(buffer, &count));
    }
    return bytes->ToByteArray(byteArray);
}

ECode CStreams::ReadFully(
    /* [in] */ IReader* reader,
    /* [out] */ String* result)//todo: how to write simple when fail??????
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(reader);

    AutoPtr<ICloseable> ic = (ICloseable*)reader->Probe(EIID_ICloseable);
    //try {
    AutoPtr<IStringWriter> writer;
    CStringWriter::New((IStringWriter**)&writer);
    AutoPtr<ArrayOf<Char32> > buffer = ArrayOf<Char32>::Alloc(1024);
    AutoPtr<IStringBuffer> strBuf;
    Int32 count;
    ECode ec = reader->Read(buffer.Get(), &count);
    if (FAILED(ec)) {
        goto finally;
    }
    while (count != -1) {
        ec = IWriter::Probe(writer)->Write(buffer, 0, count);
        if (FAILED(ec)) {
            goto finally;
        }
        ec = reader->Read(buffer, &count);
        if (FAILED(ec)) {
            goto finally;
        }
    }

    ec = writer->GetBuffer((IStringBuffer**)&strBuf);
    if (FAILED(ec)) {
        goto finally;
    }

    ec = ICharSequence::Probe(strBuf)->ToString(result);

finally:
    if (ic != NULL) {
        ic->Close();
    }
    return ec;
}

ECode CStreams::SkipAll(
    /* [in] */ IInputStream* in)
{
    VALIDATE_NOT_NULL(in);
    using Elastos::Core::Math;
    Int32 count;
    Int64 skipped;
    do {
        FAIL_RETURN(in->Skip(Math::INT64_MAX_VALUE, &skipped));
        FAIL_RETURN(in->Read(&count));
    }
    while (count != -1);
    return NOERROR;
}

ECode CStreams::SkipByReading(
    /* [in] */ IInputStream* in,
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    VALIDATE_NOT_NULL(in);

    using Elastos::Core::Math;
    if (mSkipBuffer.Get() == NULL) {
        mSkipBuffer = ArrayOf<Byte>::Alloc(4096);
    }
    Int64 skipBufferLength = mSkipBuffer->GetLength();
    Int64 skipped = 0;
    Int32 toRead, read;
    ECode ec = NOERROR;
    while (skipped < byteCount) {
        toRead = (Int32)Min((Int64)(byteCount - skipped), skipBufferLength);
        // FAIL_RETURN(in->Read(mSkipBuffer, 0, toRead, &read));
        in->Read(mSkipBuffer, 0, toRead, &read);
        if (read == -1) {
            break;
        }
        skipped += read;
        if (read < toRead) {
            break;
        }
    }

    *result = skipped;
    return ec;
}

ECode CStreams::Copy(
    /* [in] */ IInputStream* in,
    /* [in] */ IOutputStream* out,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(in);
    VALIDATE_NOT_NULL(out);

    Int32 total = 0;
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(8192);
    Int32 c;
    FAIL_RETURN(in->Read(buffer, &c));
    while (c != -1) {
        total += c;
        FAIL_RETURN(out->Write(buffer, 0, c));
        FAIL_RETURN(in->Read(buffer, &c));
    }

    *number = total;
    return NOERROR;
}

ECode CStreams::ReadAsciiLine(
    /* [in] */ IInputStream* in,
    /* [out] */ String* characters)
{
    VALIDATE_NOT_NULL(characters);
    *characters = String(NULL);
    VALIDATE_NOT_NULL(in);
    //TODO: support UTF-8 here instead

    StringBuilder result(80);
    while (TRUE) {
        Int32 c;
        FAIL_RETURN(in->Read(&c));
        if (c == -1) {
            return E_EOF_EXCEPTION;
//            throw new EOFException();
        }
        else if (c == '\n') {
            break;
        }

        result.AppendChar((Char32)c);
    }
    Int32 length = 0;
    result.GetLength(&length);
    if (length > 0) {
        Char32 ch = result.GetCharAt(length - 1);
        if (ch == '\r') {
            result.DeleteCharAt(length - 1);
        }
    }

    result.ToString(characters);
    return NOERROR;
}

} // namespace IO
} // namespace Libcore
