
#include "ZipInputStream.h"
#include "CZipFile.h"
#include "CZipEntry.h"
#include "CCRC32.h"
#include "Memory.h"
#include "Math.h"
#include "CPushbackInputStream.h"
#include "CStreams.h"
#include "CInflater.h"
#include "charset/CModifiedUtf8.h"
#include "Arrays.h"
#include <cutils/log.h>

using Elastos::Core::Math;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::IPushbackInputStream;
using Elastos::IO::CPushbackInputStream;
using Elastos::IO::ByteOrder_LITTLE_ENDIAN;
using Elastos::IO::Charset::IModifiedUtf8;
using Elastos::IO::Charset::CModifiedUtf8;
using Libcore::IO::Memory;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 ZipInputStream::ZIPLocalHeaderVersionNeeded;

CAR_INTERFACE_IMPL(ZipInputStream, InflaterInputStream, IZipInputStream)

ZipInputStream::ZipInputStream()
    : mEntriesEnd(FALSE)
    , mHasDD(FALSE)
    , mEntryIn(0)
    , mInRead(0)
    , mLastRead(0)
{
    mHdrBuf = ArrayOf<Byte>::Alloc(IZipConstants::LOCHDR - IZipConstants::LOCVER);
    CCRC32::New((ICRC32**)&mCrc);
    mStringBytesBuf = ArrayOf<Byte>::Alloc(256);
    mStringCharBuf = ArrayOf<Char32>::Alloc(256);
}

ZipInputStream::~ZipInputStream()
{
}

ECode ZipInputStream::constructor(
    /* [in] */ IInputStream* stream)
{
    if (stream == NULL) {
//        throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<IInputStream> is;
    FAIL_RETURN(CPushbackInputStream::New(stream, BUF_SIZE, (IInputStream**)&is));
    AutoPtr<IInflater> inflater;
    CInflater::New(TRUE, (IInflater**)&inflater);
    return InflaterInputStream::constructor(is.Get(), inflater.Get());
}

ECode ZipInputStream::Close()
{
    if (!mClosed) {
        FAIL_RETURN(CloseEntry()); // Close the current entry
        FAIL_RETURN(InflaterInputStream::Close());
    }
    return NOERROR;
}

ECode ZipInputStream::CloseEntry()
{
    FAIL_RETURN(CheckClosed());
    if (mCurrentEntry == NULL) {
        return NOERROR;
    }

    /*
     * The following code is careful to leave the ZipInputStream in a
     * consistent state, even when close() results in an exception. It does
     * so by:
     *  - pushing bytes back into the source stream
     *  - reading a data descriptor footer from the source stream
     *  - resetting fields that manage the entry being closed
     */

    // Ensure all entry bytes are read
    //try {
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    ECode ec = streams->SkipAll(this);
    // } catch (Exception e) {
    //     failure = e;
    // }

    Int32 inB, out, method;
    mCurrentEntry->GetMethod(&method);
    if (method == IZipEntry::DEFLATED) {
        mInf->GetTotalIn(&inB);
        mInf->GetTotalOut(&out);
    }
    else {
        inB = mInRead;
        out = mInRead;
    }

    Int32 diff = mEntryIn - inB;
    // Pushback any required bytes
    if (diff != 0) {
        IPushbackInputStream::Probe(mIn)->Unread(mBuf, mLen - diff, diff);
    }

    ECode newec = ReadAndVerifyDataDescriptor(inB, out);
    if (SUCCEEDED(ec)) ec = newec; // otherwise we're already going to throw

    mInf->Reset();
    mLastRead = mInRead = mEntryIn = mLen = 0;
    IChecksum::Probe(mCrc)->Reset();
    mCurrentEntry = NULL;

    return ec;
}

ECode ZipInputStream::ReadAndVerifyDataDescriptor(
    /* [in] */ Int32 inB,
    /* [in] */ Int32 out)
{
    if (mHasDD) {
        AutoPtr<IStreams> streams;
        CStreams::AcquireSingleton((IStreams**)&streams);
        FAIL_RETURN(streams->ReadFully(mIn, mHdrBuf, 0, IZipConstants::EXTHDR));
        Int32 sig = Memory::PeekInt32(mHdrBuf, 0, ByteOrder_LITTLE_ENDIAN);
        if (sig != IZipConstants::EXTSIG) {
            ALOGE("ZipException unknown format (EXTSIG=%08x)", sig);
            return E_ZIP_EXCEPTION;
        }

        Int32 temp;
        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::EXTCRC, ByteOrder_LITTLE_ENDIAN);
        mCurrentEntry->SetCrc((Int64)temp & 0xffffffffll);

        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::EXTSIZ, ByteOrder_LITTLE_ENDIAN);
        mCurrentEntry->SetCompressedSize((Int64)temp & 0xffffffffll);

        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::EXTLEN, ByteOrder_LITTLE_ENDIAN);
        mCurrentEntry->SetSize((Int64)temp & 0xffffffffll);
    }

    Int64 checksum;
    IChecksum::Probe(mCrc)->GetValue(&checksum);
    Int64 longVal;
    mCurrentEntry->GetCrc(&longVal);
    if (longVal != checksum) {
         ALOGE("ZipException CRC mismatch");
        return E_ZIP_EXCEPTION;
    }

    Int64 size;
    mCurrentEntry->GetCompressedSize(&longVal);
    mCurrentEntry->GetSize(&size);
    if (longVal != inB || size != out) {
         ALOGE("ZipException Size mismatch");
        return E_ZIP_EXCEPTION;
    }
    return NOERROR;
}

ECode ZipInputStream::GetNextEntry(
    /* [out] */ IZipEntry** entry)
{
    VALIDATE_NOT_NULL(entry);
    *entry = NULL;

    FAIL_RETURN(CloseEntry());
    if (mEntriesEnd) {
        return NOERROR;
    }

    // Read the signature to see whether there's another local file header.
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    FAIL_RETURN(streams->ReadFully(mIn, mHdrBuf, 0, 4));

    Int32 hdr = Memory::PeekInt32(mHdrBuf, 0, ByteOrder_LITTLE_ENDIAN);
    if (hdr == IZipConstants::CENSIG) {
        mEntriesEnd = TRUE;
        return NOERROR;
    }
    else if (hdr != IZipConstants::LOCSIG) {
        return NOERROR;
    }

    // Read the local file header.
    FAIL_RETURN(streams->ReadFully(mIn, mHdrBuf, 0, (IZipConstants::LOCHDR - IZipConstants::LOCVER)));
    Int32 version = PeekShort(0) & 0xff;
    if (version > ZIPLocalHeaderVersionNeeded) {
        ALOGE("ZipException Cannot read local header version %d", version);
        return E_ZIP_EXCEPTION;
    }

    Int32 flags = PeekShort(IZipConstants::LOCFLG - IZipConstants::LOCVER);
    if ((flags & ZipFile::GPBF_UNSUPPORTED_MASK) != 0) {
        ALOGE("ZipException Invalid General Purpose Bit Flag: %d", flags);
        return E_ZIP_EXCEPTION;
    }

    mHasDD = ((flags & ZipFile::GPBF_DATA_DESCRIPTOR_FLAG) != 0);
    Int32 ceLastModifiedTime = PeekShort(IZipConstants::LOCTIM - IZipConstants::LOCVER);
    Int32 ceLastModifiedDate = PeekShort(IZipConstants::LOCTIM - IZipConstants::LOCVER + 2);
    Int32 ceCompressionMethod = PeekShort(IZipConstants::LOCHOW - IZipConstants::LOCVER);

    Int64 ceCrc = 0, ceCompressedSize = 0, ceSize = -1;
    if (!mHasDD) {
        Int32 temp;
        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::LOCCRC - IZipConstants::LOCVER, ByteOrder_LITTLE_ENDIAN);
        ceCrc = temp & 0xffffffffL;

        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::LOCSIZ - IZipConstants::LOCVER, ByteOrder_LITTLE_ENDIAN);
        ceCompressedSize = temp & 0xffffffffL;

        temp = Memory::PeekInt32(mHdrBuf, IZipConstants::LOCLEN - IZipConstants::LOCVER, ByteOrder_LITTLE_ENDIAN);
        ceSize = temp & 0xffffffffL;
    }

    Int32 nameLength = PeekShort( IZipConstants::LOCNAM - IZipConstants::LOCVER);
    if (nameLength == 0) {
        ALOGE("ZipException: Entry is not named");
        return E_ZIP_EXCEPTION;
    }
    Int32 extraLength = PeekShort(IZipConstants::LOCEXT - IZipConstants::LOCVER);

    String name = ReadString(nameLength);
    mCurrentEntry = CreateZipEntry(name);
    mCurrentEntry->SetTime(ceLastModifiedTime);
    ((ZipEntry*)mCurrentEntry.Get())->mModDate = ceLastModifiedDate;
    mCurrentEntry->SetMethod(ceCompressionMethod);
    if (ceSize != -1) {
        mCurrentEntry->SetCrc(ceCrc);
        mCurrentEntry->SetSize(ceSize);
        mCurrentEntry->SetCompressedSize(ceCompressedSize);
    }

    if (extraLength > 0) {
        AutoPtr<ArrayOf<Byte> > extraData = ArrayOf<Byte>::Alloc(extraLength);
        FAIL_RETURN(streams->ReadFully(mIn, extraData, 0, extraLength));
        mCurrentEntry->SetExtra(extraData);
    }

    *entry = mCurrentEntry;
    REFCOUNT_ADD(*entry);
    return NOERROR;
}

String ZipInputStream::ReadString(
    /* [in] */ Int32 byteLength)
{
    if (byteLength > mStringBytesBuf->GetLength()) {
        mStringBytesBuf = ArrayOf<Byte>::Alloc(byteLength);
    }
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    streams->ReadFully(mIn, mStringBytesBuf, 0, byteLength);
    // The number of chars will always be less than or equal to the number of bytes. It's
    // fine if this buffer is too long.
    if (byteLength > mStringCharBuf->GetLength()) {
        mStringCharBuf = ArrayOf<Char32>::Alloc(byteLength);
    }

    AutoPtr<IModifiedUtf8> muhelper;
    CModifiedUtf8::AcquireSingleton((IModifiedUtf8**)&muhelper);
    String result;
    muhelper->Decode(mStringBytesBuf, mStringCharBuf, 0, byteLength, &result);
    return result;
}

Int32 ZipInputStream::PeekShort(
    /* [in] */ Int32 offset)
{
    Int16 value = Memory::PeekInt16(mHdrBuf, offset, ByteOrder_LITTLE_ENDIAN);
    return (value & 0xffff);
}

ECode ZipInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    VALIDATE_NOT_NULL(buffer)

    FAIL_RETURN(CheckClosed());
    FAIL_RETURN(Arrays::CheckOffsetAndCount(buffer->GetLength(), byteOffset, byteCount))

    Boolean finished;
    mInf->Finished(&finished);
    if (finished || mCurrentEntry == NULL) {
        *number = -1;
        return NOERROR;
    }

    Int32 method;
    mCurrentEntry->GetMethod(&method);
    Int64 size;
    if (method == IZipEntry::STORED) {
        mCurrentEntry->GetSize(&size);
        Int32 csize = (Int32)size;
        if (mInRead >= csize) {
            *number = -1;
            return NOERROR;
        }
        if (mLastRead >= mLen) {
            mLastRead = 0;
            FAIL_RETURN(mIn->Read(mBuf, &mLen));
            if (mLen == -1) {
                mEof = TRUE;
                *number = -1;
                return NOERROR;
            }
            mEntryIn += mLen;
        }
        Int32 toRead = byteCount > (mLen - mLastRead) ? mLen - mLastRead : byteCount;
        if ((csize - mInRead) < toRead) {
            toRead = csize - mInRead;
        }
        buffer->Copy(byteOffset, mBuf, mLastRead, toRead);
        mLastRead += toRead;
        mInRead += toRead;
        IChecksum::Probe(mCrc)->Update(buffer, byteOffset, toRead);
        *number = toRead;
        return NOERROR;
    }

    Boolean value;
    mInf->NeedsInput(&value);
    if (value) {
        FAIL_RETURN(Fill());
        if (mLen > 0) {
            mEntryIn += mLen;
        }
    }
    Int32 read;
    if (FAILED(mInf->Inflate(buffer, byteOffset, byteCount, &read))) {
        return E_ZIP_EXCEPTION;
    }
    mInf->Finished(&value);
    if (read == 0 && value) {
        *number = -1;
        return NOERROR;
    }
    IChecksum::Probe(mCrc)->Update(buffer, byteOffset, read);
    *number = read;
    return NOERROR;
}

ECode ZipInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    FAIL_RETURN(CheckClosed());
    // The InflaterInputStream contract says we must only return 0 or 1.
    Int64 size = 0;
    if (mCurrentEntry != NULL) {
        mCurrentEntry->GetSize(&size);
    }
    *number = (mCurrentEntry == NULL || mInRead < size) ? 1 : 0;
    return NOERROR;
}

AutoPtr<IZipEntry> ZipInputStream::CreateZipEntry(
    /* [in] */ const String& name)
{
    AutoPtr<IZipEntry> entry;
    CZipEntry::New(name, (IZipEntry**)&entry);
    return entry;
}

ECode ZipInputStream::CheckClosed()
{
    if (mClosed) {
        ALOGE("IOException Stream is closed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
