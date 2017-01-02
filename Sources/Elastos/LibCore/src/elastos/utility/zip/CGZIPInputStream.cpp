//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "CGZIPInputStream.h"
#include "Memory.h"
#include "CInflater.h"
#include "Arrays.h"
#include "CCRC32.h"
#include "CStreams.h"
#include "CPushbackInputStream.h"
#include <cutils/log.h>

using Libcore::IO::Memory;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;
using Elastos::IO::IPushbackInputStream;
using Elastos::IO::CPushbackInputStream;
using Elastos::IO::ByteOrder_LITTLE_ENDIAN;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 CGZIPInputStream::FCOMMENT;
const Int32 CGZIPInputStream::FEXTRA;
const Int32 CGZIPInputStream::FHCRC;
const Int32 CGZIPInputStream::FNAME;
const Int32 CGZIPInputStream::GZIP_TRAILER_SIZE;

CAR_INTERFACE_IMPL(CGZIPInputStream, InflaterInputStream, IGZIPInputStream)

CAR_OBJECT_IMPL(CGZIPInputStream)

CGZIPInputStream::CGZIPInputStream()
    : mEos(FALSE)
{
    CCRC32::New((ICRC32**)&mCrc);
}

ECode CGZIPInputStream::constructor(
    /* [in] */ IInputStream* is)
{
    return constructor(is, BUF_SIZE);
}

/**
 * Construct a {@code GZIPInputStream} to read from GZIP data from the
 * underlying stream. Set the internal buffer size to {@code size}.
 *
 * @param is
 *            the {@code InputStream} to read data from.
 * @param size
 *            the internal read buffer size.
 * @throws IOException
 *             if an {@code IOException} occurs.
 */
ECode CGZIPInputStream::constructor(
    /* [in] */ IInputStream* is,
    /* [in] */ Int32 size)
{
    AutoPtr<IInflater> inflater;
    CInflater::New(TRUE, (IInflater**)&inflater);
    FAIL_RETURN(InflaterInputStream::constructor(is, inflater, size));

    // try {
    Int16 magic;
    AutoPtr<ArrayOf<Byte> > header;
    FAIL_GOTO(ReadHeader(is, (ArrayOf<Byte>**)&header), _EXIT_)

    magic = Memory::PeekInt16(header, 0, ByteOrder_LITTLE_ENDIAN);
    if (magic != (Int16) IGZIPInputStream::GZIP_MAGIC) {
        ALOGE("IOException: CGZIPInputStream unknown format (magic number %08x", magic);
        // throw new IOException(String.format("unknown format (magic number %x)", magic));
        goto _EXIT_;
    }

    FAIL_GOTO(ParseGzipHeader(is, header, mCrc, mBuf), _EXIT_)
    // } catch (IOException e) {
    //     close(); // release the inflater
    //     throw e;
    // }
_EXIT_:
    Close();    // release the inflater
    return NOERROR;
}

ECode CGZIPInputStream::Close()
{
    mEos = TRUE;
    return InflaterInputStream::Close();
}

ECode CGZIPInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    if (mClosed) {
        ALOGE("IOException: CGZIPInputStream::Read() Stream is closed.");
        return E_IO_EXCEPTION;
    }
    if (mEos) {
        return NOERROR;
    }

    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), offset, byteCount))

    Int32 bytesRead;
    InflaterInputStream::Read(buffer, offset, byteCount, &bytesRead);
    mEos = mEof;    // update eos after every read(), even when it throws

    if (bytesRead != -1) {
        IChecksum::Probe(mCrc)->Update(buffer, offset, bytesRead);
    }

    if (mEos) {
        FAIL_RETURN(VerifyCrc());

        FAIL_RETURN(MaybeReadNextMember(&mEos))
        if (!mEos) {
            IChecksum::Probe(mCrc)->Reset();
            mInf->Reset();
            mEof = FALSE;
            mLen = 0;
        }
    }

    *number = bytesRead;
    return NOERROR;
}

ECode CGZIPInputStream::MaybeReadNextMember(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    // If we have any unconsumed data in the inflater buffer, we have to
    // scan that first. The fact that we've reached here implies we've
    // successfully consumed the GZIP trailer.
    Int32 remaining;
    mInf->GetRemaining(&remaining);
    remaining -= GZIP_TRAILER_SIZE;
    if (remaining > 0) {
        // NOTE: We make sure we create a pushback stream exactly once,
        // even if the input stream contains multiple members.
        //
        // The push back stream we create must therefore be able to contain
        // (worst case) the entire buffer even though there may be fewer bytes
        // remaining when it is first created.
        if (IPushbackInputStream::Probe(mIn) == NULL) {
            AutoPtr<IInputStream> tmp;
            CPushbackInputStream::New(mIn, mBuf->GetLength(), (IInputStream**)&tmp);
            mIn = tmp;
        }

        Int32 offset;
        mInf->GetCurrentOffset(&offset);
        IPushbackInputStream::Probe(mIn)->Unread(mBuf, offset + GZIP_TRAILER_SIZE, remaining);
    }

    AutoPtr<ArrayOf<Byte> > buffer;
    // try {
    ECode ec = ReadHeader(mIn, (ArrayOf<Byte>**)&buffer);
    if (ec == (ECode)E_EOF_EXCEPTION) {
    // } catch (EOFException eof) {
        // We've reached the end of the stream and there are no more members
        // to read. Note that we might also hit this if there are fewer than
        // GZIP_HEADER_LENGTH bytes at the end of a member. We don't care
        // because we're specified to ignore all data at the end of the last
        // gzip record.
        *result = TRUE;
        return NOERROR;
    }

    Int16 magic = Memory::PeekInt16(buffer, 0, ByteOrder_LITTLE_ENDIAN);
    if (magic != (Int16) IGZIPInputStream::GZIP_MAGIC) {
        // Don't throw here because we've already read one valid member
        // from this stream.
        *result = TRUE;
        return NOERROR;
    }

    // We've encountered the gzip magic number, so we assume there's another
    // member in the stream.
    ParseGzipHeader(mIn, buffer, mCrc, mBuf);
    return NOERROR;
}

ECode CGZIPInputStream::ReadHeader(
    /* [in] */ IInputStream* in,
    /* [out, callee] */ ArrayOf<Byte>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = NULL;

    AutoPtr<ArrayOf<Byte> > header = ArrayOf<Byte>::Alloc(10);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    streams->ReadFully(in, header, 0, header->GetLength());
    *array = header;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode CGZIPInputStream::ParseGzipHeader(
    /* [in] */ IInputStream* in,
    /* [in] */ ArrayOf<Byte>* header,
    /* [in] */ ICRC32* crc,
    /* [in] */ ArrayOf<Byte>* scratch)
{
    VALIDATE_NOT_NULL(header)
    VALIDATE_NOT_NULL(scratch)

    Byte flags = (*header)[3];
    Boolean hcrc = (flags & FHCRC) != 0;
    if (hcrc) {
        IChecksum::Probe(crc)->Update(header, 0, header->GetLength());
    }
    if ((flags & FEXTRA) != 0) {
        AutoPtr<IStreams> streams;
        CStreams::AcquireSingleton((IStreams**)&streams);
        FAIL_RETURN(streams->ReadFully(in, header, 0, 2))
        if (hcrc) {
            IChecksum::Probe(crc)->Update(header, 0, 2);
        }
        Int32 length = Memory::PeekInt16(scratch, 0, ByteOrder_LITTLE_ENDIAN) & 0xffff;
        Int32 result, max;
        while (length > 0) {
            max = length > scratch->GetLength() ? scratch->GetLength() : length;
            FAIL_RETURN(in->Read(scratch, 0, max, &result))
            if (result == -1) {
                // throw new EOFException();
                return E_EOF_EXCEPTION;
            }
            if (hcrc) {
                IChecksum::Probe(crc)->Update(scratch, 0, result);
            }
            length -= result;
        }
    }

    if ((flags & FNAME) != 0) {
        FAIL_RETURN(ReadZeroTerminated(in, crc, hcrc))
    }

    if ((flags & FCOMMENT) != 0) {
        FAIL_RETURN(ReadZeroTerminated(in, crc, hcrc))
    }

    if (hcrc) {
        AutoPtr<IStreams> streams;
        CStreams::AcquireSingleton((IStreams**)&streams);
        FAIL_RETURN(streams->ReadFully(in, header, 0, 2))
        Int16 crc16 = Memory::PeekInt16(scratch, 0, ByteOrder_LITTLE_ENDIAN);
        Int64 val;
        IChecksum::Probe(crc)->GetValue(&val);
        if ((Int16)val != crc16) {
            // throw new IOException("CRC mismatch");
            return E_IO_EXCEPTION;
        }
        IChecksum::Probe(crc)->Reset();
    }
    return NOERROR;
}

ECode CGZIPInputStream::VerifyCrc()
{
    // Get non-compressed bytes read by fill
    Int32 size;
    mInf->GetRemaining(&size);
    const Int32 trailerSize = 8; // crc (4 bytes) + total out (4 bytes)
    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(trailerSize);
    Int32 copySize = (size > trailerSize) ? trailerSize : size;

    mBuf->Copy(mLen - size, b, 0, copySize);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    FAIL_RETURN(streams->ReadFully(mIn, b, copySize, trailerSize - copySize))

    Int64 checksum;
    IChecksum::Probe(mCrc)->GetValue(&checksum);

    Int32 temp = Memory::PeekInt32(b, 0, ByteOrder_LITTLE_ENDIAN);
    if (temp != (Int32)checksum) {
//        throw new IOException("CRC mismatch");
        return E_IO_EXCEPTION;
    }

    Int32 value;
    mInf->GetTotalOut(&value);
    temp = Memory::PeekInt32(b, 4, ByteOrder_LITTLE_ENDIAN);
    if (temp != value) {
//        throw new IOException("Size mismatch");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CGZIPInputStream::ReadZeroTerminated(
    /* [in] */ IInputStream* in,
    /* [in] */ ICRC32* crc,
    /* [in] */ Boolean hcrc)
{
    Int32 result;
    FAIL_RETURN(in->Read(&result));
    while (result > 0) {
        if (hcrc) {
            IChecksum::Probe(crc)->Update(result);
        }
        FAIL_RETURN(in->Read(&result));
    }
    if (result == -1) {
//        throw new EOFException();
        return E_EOF_EXCEPTION;
    }
    // Add the zero
    if (hcrc) {
        IChecksum::Probe(crc)->Update(result);
    }
    return NOERROR;
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
