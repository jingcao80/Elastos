
#include "ZipFile.h"
#include "CInflater.h"
#include "Math.h"
#include "CDataInputStream.h"
#include "CBufferedInputStream.h"
#include "CRandomAccessFile.h"
#include "CFile.h"
#include "CZipEntry.h"
#include "StringBuilder.h"
#include "CStreams.h"
#include "IoUtils.h"
#include "CLinkedHashMap.h"
#include "CString.h"
#include "HeapBufferIterator.h"
#include "CCloseGuardHelper.h"
#include <elastos/core/AutoLock.h>
#include <cutils/log.h>

using Elastos::Core::AutoLock;
using Elastos::Core::Math;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Core::CCloseGuardHelper;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::IO::ByteOrder_LITTLE_ENDIAN;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::CRandomAccessFile;
using Elastos::IO::CFile;
using Elastos::IO::EIID_IDataInput;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::EIID_IFilterInputStream;
using Elastos::IO::EIID_ICloseable;
using Elastos::Core::StringBuilder;
using Libcore::IO::IoUtils;
using Libcore::IO::IStreams;
using Libcore::IO::CStreams;
using Libcore::IO::IBufferIterator;
using Libcore::IO::HeapBufferIterator;

namespace Elastos {
namespace Utility {
namespace Zip {

const Int32 ZipFile::GPBF_DATA_DESCRIPTOR_FLAG = 1 << 3;
const Int32 ZipFile::GPBF_UTF8_FLAG = 1 << 11;
const Int32 ZipFile::GPBF_UNSUPPORTED_MASK = GPBF_DATA_DESCRIPTOR_FLAG;

//=====================================================
//ZipFile::RAFStream
//=====================================================
CAR_INTERFACE_IMPL(ZipFile::RAFStream, InputStream, IZipFileRAFStream)

ZipFile::RAFStream::RAFStream(
    /* [in] */ IRandomAccessFile* raf,
    /* [in] */ Int64 initialOffset)
    : mSharedRaf(raf)
    , mOffset(initialOffset)
    , mEndOffset(0)
{
    raf->GetLength(&mEndOffset);
}

ZipFile::RAFStream::RAFStream(
    /* [in] */ IRandomAccessFile* raf,
    /* [in] */ Int64 initialOffset,
    /* [in] */ Int64 endOffset)
    : mSharedRaf(raf)
    , mOffset(initialOffset)
    , mEndOffset(endOffset)
{
}

ECode ZipFile::RAFStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = (mOffset < mEndOffset ? 1 : 0);
    return NOERROR;
}

ECode ZipFile::RAFStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IStreams> streams;
    CStreams::AcquireSingleton((IStreams**)&streams);
    return streams->ReadSingleByte(this, value);
}

ECode ZipFile::RAFStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = -1;
    VALIDATE_NOT_NULL(buffer);

    AutoLock lock(mSharedRaf);

    Int64 length = mEndOffset - mOffset;
    if (byteCount > length) {
        byteCount = (Int32)length;
    }

    mSharedRaf->Seek(mOffset);
    FAIL_RETURN(mSharedRaf->Read(buffer, byteOffset, byteCount, number));
    if (*number > 0) {
        mOffset += *number;
    }
    else *number = -1;

    return NOERROR;
}

ECode ZipFile::RAFStream::Skip(
    /* [in] */ Int64 byteCount,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);
    if (byteCount > mEndOffset - mOffset) {
        byteCount = mEndOffset - mOffset;
    }
    mOffset += byteCount;
    *number = byteCount;
    return NOERROR;
}

ECode ZipFile::RAFStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = 0;
    VALIDATE_NOT_NULL(buffer);
    return Read(buffer, 0, buffer->GetLength(), number);
}

ECode ZipFile::RAFStream::Fill(
    /* [in] */ IInflater* inflater,
    /* [in] */ Int32 nativeEndBufSize,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    CInflater* inf = (CInflater*)inflater;
    using Elastos::Core::Math;

    {
        AutoLock syncLock(mSharedRaf);
        AutoPtr<IFileDescriptor> fd;
        mSharedRaf->GetFD((IFileDescriptor**)&fd);
        Int32 len = Math::Min((Int32) (mEndOffset - mOffset), nativeEndBufSize);
        Int32 cnt = inf->SetFileInput(fd, mOffset, nativeEndBufSize);
        // setFileInput read from the file, so we need to get the OS and RAFStream back
        // in sync...
        Int64 val;
        Skip(cnt, &val);
        *result = len;
    }
    return NOERROR;
}

//=====================================================
//ZipFile::ZipInflaterInputStream
//=====================================================

ZipFile::ZipInflaterInputStream::ZipInflaterInputStream(
    /* [in] */ IInputStream* is,
    /* [in] */ IInflater* inf,
    /* [in] */ Int32 bsize,
    /* [in] */ IZipEntry* entry)
    : mEntry(entry)
    , mBytesRead(0)
{
    ASSERT_SUCCEEDED(InflaterInputStream::constructor(is, inf, bsize));
}

//@Override
ECode ZipFile::ZipInflaterInputStream::Read(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 byteOffset,
    /* [in] */ Int32 byteCount,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;
    Int32 i;
    FAIL_RETURN(InflaterInputStream::Read(buffer, byteOffset, byteCount, &i));
    if (i == -1) {
        Int64 size;
        mEntry->GetSize(&size);
        if (size != mBytesRead) {
            ALOGE("IOException: Size mismatch on inflated file: %lld vs %lld", mBytesRead, size);
            return E_IO_EXCEPTION;
        }
    }
    else {
        mBytesRead += i;
    }

    *number = i;
    return NOERROR;
}

//@Override
ECode ZipFile::ZipInflaterInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    if (mClosed) {
        // Our superclass will throw an exception, but there's a jtreg test that
        // explicitly checks that the InputStream returned from ZipFile.getInputStream
        // returns 0 even when closed.
        *number = 0;
        return NOERROR;
    }

    FAIL_RETURN(InflaterInputStream::Available(number));
    if (*number != 0) {
        Int64 size;
        mEntry->GetSize(&size);
        *number = size - mBytesRead;
    }
    return NOERROR;
}

//====================================================================
// ZipFile::Enumeration::
//====================================================================
CAR_INTERFACE_IMPL(ZipFile::Enumeration, Object, IEnumeration)

ZipFile::Enumeration::Enumeration(
    /* [in] */ IIterator* it,
    /* [in] */ ZipFile* zipFile)
    : mHost(zipFile)
{
    mIt = it;
}

ECode ZipFile::Enumeration::HasMoreElements(
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    FAIL_RETURN(mHost->CheckNotClosed())
    return mIt->HasNext(value);
}

ECode ZipFile::Enumeration::GetNextElement(
    /* [out] */ IInterface ** inter)
{
    VALIDATE_NOT_NULL(inter);
    FAIL_RETURN(mHost->CheckNotClosed())
    return mIt->GetNext(inter);
}

//====================================================================
// ZipFile::
//====================================================================

CAR_INTERFACE_IMPL_2(ZipFile, Object, IZipFile, ICloseable)

ZipFile::ZipFile()
{
    CLinkedHashMap::New((IMap**)&mEntries);
    AutoPtr<ICloseGuardHelper> cgHelper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&cgHelper);
    cgHelper->Get((ICloseGuard**)&mGuard);
}

ZipFile::~ZipFile()
{
    if (mGuard != NULL) {
        mGuard->WarnIfOpen();
    }
    mEntries->Clear();
}

ECode ZipFile::constructor(
    /* [in] */ const String& name)
{
    AutoPtr<IFile> file;
    FAIL_RETURN(CFile::New(name, (IFile**)&file));
    return constructor(file, IZipFile::OPEN_READ);
}

ECode ZipFile::constructor(
    /* [in] */ IFile* file)
{
    return constructor(file, IZipFile::OPEN_READ);
}

ECode ZipFile::constructor(
    /* [in] */ IFile* file,
    /* [in] */ Int32 mode)
{
    VALIDATE_NOT_NULL(file);
    file->GetPath(&mFileName);
    if (mode != IZipFile::OPEN_READ
        && mode != (IZipFile::OPEN_READ | IZipFile::OPEN_DELETE)) {
//        throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if ((mode & IZipFile::OPEN_DELETE) != 0) {
        mFileToDeleteOnClose = file;
        mFileToDeleteOnClose->DeleteOnExit();
    }
    else {
        mFileToDeleteOnClose = NULL;
    }

    FAIL_RETURN(CRandomAccessFile::New(file, String("r"), (IRandomAccessFile**)&mRaf));

    // Make sure to close the RandomAccessFile if reading the central directory fails.
    Boolean mustCloseFile = TRUE;
    // try {
    ECode ec = ReadCentralDir();
    FAIL_GOTO(ec, _EXIT_)
        // Read succeeded so do not close the underlying RandomAccessFile.
        mustCloseFile = FALSE;
    // } finally {
_EXIT_:
        if (mustCloseFile) {
            IoUtils::CloseQuietly(ICloseable::Probe(mRaf));
        }
    // }

    return mGuard->Open(String("ZipFile::Close"));
}


ECode ZipFile::Close()
{
    mGuard->Close();
    AutoPtr<IRandomAccessFile> raf = mRaf;

    if (raf != NULL) { // Only close initialized instances
        {
            AutoLock lock(raf);
            mRaf = NULL;
            ICloseable::Probe(raf)->Close();
        }
        if (mFileToDeleteOnClose != NULL) {
            Boolean result;
            mFileToDeleteOnClose->Delete(&result);
            mFileToDeleteOnClose = NULL;
        }
    }
    return NOERROR;
}

ECode ZipFile::CheckNotClosed()
{
   if (mRaf == NULL) {
       // throw new IllegalStateException("Zip file closed");
       return E_ILLEGAL_STATE_EXCEPTION;
   }
   return NOERROR;
}

ECode ZipFile::GetEntries(
    /* [out] */ IEnumeration** entries)
{
    VALIDATE_NOT_NULL(entries);
    *entries = NULL;
    FAIL_RETURN(CheckNotClosed());

    AutoPtr<ICollection> vals;
    mEntries->GetValues((ICollection**)&vals);
    AutoPtr<IIterator> iterator;
    vals->GetIterator((IIterator**)&iterator);
    AutoPtr<Enumeration> res = new Enumeration(iterator, this);
    *entries = res.Get();
    REFCOUNT_ADD(*entries);
    return NOERROR;
}

ECode ZipFile::GetEntry(
    /* [in] */ const String& entryName,
    /* [out] */ IZipEntry** entry)
{
    VALIDATE_NOT_NULL(entry)
    *entry = NULL;
    FAIL_RETURN(CheckNotClosed());

    if (entryName.IsNull()) {
         return E_NULL_POINTER_EXCEPTION;
//         throw new NullPointerException("entryName == null");
    }

    AutoPtr<ICharSequence> csq;
    CString::New(entryName, (ICharSequence**)&csq);
    AutoPtr<IInterface> obj;
    mEntries->Get(TO_IINTERFACE(csq), (IInterface**)&obj);
    AutoPtr<IZipEntry> ze = IZipEntry::Probe(obj);
    if (ze == NULL) {
        String name(entryName);
        name += "/";
        csq = NULL;
        CString::New(name, (ICharSequence**)&csq);
        obj = NULL;
        mEntries->Get(TO_IINTERFACE(csq), (IInterface**)&obj);
        ze = IZipEntry::Probe(obj);
    }

    *entry = ze;
    REFCOUNT_ADD(*entry)
    return NOERROR;
}

ECode ZipFile::ZipFile::GetComment(
    /* [out] */ String* comment)
{
    VALIDATE_NOT_NULL(comment)
    *comment = mComment;
    return NOERROR;
}

ECode ZipFile::GetInputStream(
    /* [in] */ IZipEntry* entry,
    /* [out] */ IInputStream** is)
{
    VALIDATE_NOT_NULL(is);
    *is = NULL;
    VALIDATE_NOT_NULL(entry);

    // Make sure this ZipEntry is in this Zip file.  We run it through the name lookup.
    String name;
    entry->GetName(&name);
    AutoPtr<IZipEntry> ze;
    GetEntry(name, (IZipEntry**)&ze);
    if (ze == NULL) {
        *is = NULL;
        return NOERROR;
    }

    using Elastos::Core::Math;

    // Create an InputStream at the right part of the file.
    AutoPtr<IRandomAccessFile> raf = mRaf;
    CRandomAccessFile* craf = (CRandomAccessFile*)raf.Get();

    AutoLock lock(craf);
    // We don't know the entry data's start position. All we have is the
    // position of the entry's local header.
    // http://www.pkware.com/documents/casestudies/APPNOTE.TXT
    AutoPtr<RAFStream> rafStream = new RAFStream(raf, ((CZipEntry*)ze.Get())->mLocalHeaderRelOffset + 28);
    AutoPtr<IDataInputStream> dis;
    CDataInputStream::New((IInputStream*)rafStream, (IDataInputStream**)&dis);

    AutoPtr<IDataInput> di = IDataInput::Probe(dis);
    Int32 localMagic;
    di->ReadInt32(&localMagic);
    localMagic = Math::ReverseBytes(localMagic);
    if (localMagic != IZipConstants::LOCSIG) {
        ALOGE("ZipException: ZipFile::GetInputStream, Local File Header %08x", localMagic);
        return E_ZIP_EXCEPTION;
    }

    Int32 skipped;
    di->SkipBytes(2, &skipped);

    // At position 6 we find the General Purpose Bit Flag.
    Int16 shortVal;
    di->ReadInt16(&shortVal);
    Int32 gpbf = Math::ReverseBytes(shortVal) & 0xffff;
    if ((gpbf & ZipFile::GPBF_UNSUPPORTED_MASK) != 0) {
        ALOGE("ZipException: ZipFile::GetInputStream, Invalid General Purpose Bit Flag: %08x", gpbf);
        return E_ZIP_EXCEPTION;
    }

    // Offset 26 has the file name length, and offset 28 has the extra field length.
    // These lengths can differ from the ones in the central header.
    di->SkipBytes(18, &skipped);
    di->ReadInt16(&shortVal);
    Int32 fileNameLength = Math::ReverseBytes(shortVal) & 0xffff;
    di->ReadInt16(&shortVal);
    Int32 extraFieldLength = Math::ReverseBytes(shortVal) & 0xffff;
    ICloseable::Probe(dis)->Close();

    // Skip the variable-size file name and extra field data.
    Int64 longVal;
    rafStream->Skip(fileNameLength + extraFieldLength, &longVal);

    Int32 compressionMethod;
    Int64 size, compressedSize;
    entry->GetMethod(&compressionMethod);
    if (compressionMethod == IZipEntry::STORED) {
        entry->GetSize(&size);
        rafStream->mEndOffset = rafStream->mOffset + size;
        *is = (IInputStream*)rafStream.Get();
        REFCOUNT_ADD(*is)
    }
    else {
        entry->GetCompressedSize(&compressedSize);
        rafStream->mEndOffset = rafStream->mOffset + compressedSize;
        entry->GetSize(&size);
        Int32 bufSize = Math::Max(1024, (Int32) Math::Min(size, 65535L));
        AutoPtr<IInflater> inf;
        CInflater::New(TRUE, (IInflater**)&inf);

        *is = (IInputStream*)new ZipInflaterInputStream(rafStream, inf, bufSize, ze);
        REFCOUNT_ADD(*is)
    }

    return NOERROR;
}

ECode ZipFile::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mFileName;
    return NOERROR;
}

ECode ZipFile::GetSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    *size = -1;
    FAIL_RETURN(CheckNotClosed());
    return mEntries->GetSize(size);
}

ECode ZipFile::ReadCentralDir()
{
    // Scan back, looking for the End Of Central Directory field. If the zip file doesn't
    // have an overall comment (unrelated to any per-entry comments), we'll hit the EOCD
    // on the first try.
    // No need to synchronize raf here -- we only do this when we first open the zip file.
    Int64 len;
    mRaf->GetLength(&len);
    Int64 scanOffset = len - IZipConstants::ENDHDR;
    if (scanOffset < 0) {
        ALOGE("ZipException: File too short to be a zip file: %lld", len);
        return E_ZIP_EXCEPTION;
    }
    mRaf->Seek(0);

    using Elastos::Core::Math;

    Int32 headerMagic;
    IDataInput::Probe(mRaf)->ReadInt32(&headerMagic);
    headerMagic = Math::ReverseBytes(headerMagic);
    if (headerMagic == IZipConstants::ENDSIG) {
        ALOGE("ZipException: Empty zip archive not supported");
        return E_ZIP_EXCEPTION;
    }
    if (headerMagic != IZipConstants::LOCSIG) {
        ALOGE("ZipException: Not a zip archive");
        return E_ZIP_EXCEPTION;
    }

    Int64 stopOffset = scanOffset - 65536;
    if (stopOffset < 0) {
        stopOffset = 0;
    }

    Int32 intVal;
    while (TRUE) {
        mRaf->Seek(scanOffset);
        IDataInput::Probe(mRaf)->ReadInt32(&intVal);
        intVal = Math::ReverseBytes(intVal);
        if (intVal == IZipConstants::ENDSIG) {
            break;
        }

        scanOffset--;
        if (scanOffset < stopOffset) {
            ALOGE("ZipException: End Of Central Directory signature not found");
            return E_ZIP_EXCEPTION;
        }
    }

    // Read the End Of Central Directory. ENDHDR includes the signature bytes,
    // which we've already read.
    AutoPtr<ArrayOf<Byte> > eocd = ArrayOf<Byte>::Alloc(IZipConstants::ENDHDR - 4);
    IDataInput::Probe(mRaf)->ReadFully(eocd);

    // Pull out the information we need.
    AutoPtr<IBufferIterator> it = HeapBufferIterator::Iterator(
        eocd, 0, eocd->GetLength(), ByteOrder_LITTLE_ENDIAN);
    Int16 shortVal;
    it->ReadInt16(&shortVal);
    Int32 diskNumber = shortVal & 0xffff;
    it->ReadInt16(&shortVal);
    Int32 diskWithCentralDir = shortVal & 0xffff;
    it->ReadInt16(&shortVal);
    Int32 numEntries = shortVal & 0xffff;
    it->ReadInt16(&shortVal);
    Int32 totalNumEntries = shortVal & 0xffff;
    it->Skip(4); // Ignore centralDirSize.
    it->ReadInt32(&intVal);
    Int64 centralDirOffset = ((Int64) intVal) & 0xffffffffL;
    it->ReadInt16(&shortVal);
    Int32 commentLength = shortVal & 0xffff;

    if (numEntries != totalNumEntries || diskNumber != 0 || diskWithCentralDir != 0) {
        ALOGE("ZipException: Spanned archives not supported");
        return E_ZIP_EXCEPTION;
    }

    if (commentLength > 0) {
        AutoPtr<ArrayOf<Byte> > commentBytes = ArrayOf<Byte>::Alloc(commentLength);
        IDataInput::Probe(mRaf)->ReadFully(commentBytes);
        mComment = String((const char*)commentBytes->GetPayload(), commentBytes->GetLength());//, StandardCharsets.UTF_8);
    }

    // Seek to the first CDE and read all entries.
    // We have to do this now (from the constructor) rather than lazily because the
    // public API doesn't allow us to throw IOException except from the constructor
    // or from getInputStream.
    AutoPtr<RAFStream> rafStream = new RAFStream(mRaf, centralDirOffset);
    AutoPtr<IInputStream> bufferedStream;
    FAIL_RETURN(CBufferedInputStream::New((IInputStream*)rafStream.Get(), 4096,
            (IInputStream**)&bufferedStream));

    AutoPtr<ICharset> charset; //StandardCharsets.UTF_8
    AutoPtr<ArrayOf<Byte> > hdrBuf = ArrayOf<Byte>::Alloc(IZipConstants::CENHDR); // Reuse the same buffer for each entry.
    for (Int32 i = 0; i < numEntries; ++i) {
        AutoPtr<IZipEntry> newEntry;
        FAIL_RETURN(CZipEntry::New(hdrBuf, bufferedStream, charset, (IZipEntry**)&newEntry));

        if (((CZipEntry*)newEntry.Get())->mLocalHeaderRelOffset >= centralDirOffset) {
            ALOGE("Local file header offset is after central directory");
            return E_ZIP_EXCEPTION;
        }
        String entryName;
        newEntry->GetName(&entryName);
        AutoPtr<ICharSequence> csq;
        CString::New(entryName, (ICharSequence**)&csq);
        AutoPtr<IInterface> oldValue;
        mEntries->Put(TO_IINTERFACE(csq), TO_IINTERFACE(newEntry), (IInterface**)&oldValue);
        if (oldValue.Get() != NULL) {
            ALOGE("Duplicate entry name: %s", entryName.string());
            return E_ZIP_EXCEPTION;
        }
    }

    return NOERROR;
}


} // namespace Zip
} // namespace Utility
} // namespace Elastos
