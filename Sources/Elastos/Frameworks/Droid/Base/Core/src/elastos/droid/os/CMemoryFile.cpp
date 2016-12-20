
#include "elastos/droid/os/CMemoryFile.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

#include <cutils/ashmem.h>
#include <unistd.h>
#include <sys/mman.h>

using Elastos::IO::CFileDescriptor;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {


//=============================================================
// CMemoryFile::MemoryInputStream
//=============================================================

CMemoryFile::MemoryInputStream::MemoryInputStream(
    /* [in] */ CMemoryFile* host)
    : mMark(0)
    , mOffset(0)
    , mHost(host)
{
}

ECode CMemoryFile::MemoryInputStream::Available(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;

    if (mOffset >= mHost->mLength) {
        return NOERROR;
    }

    *number = mHost->mLength - mOffset;
    return NOERROR;
}

ECode CMemoryFile::MemoryInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported)
    *supported = TRUE;
    return NOERROR;
}

ECode CMemoryFile::MemoryInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    mMark = mOffset;
    return NOERROR;
}

ECode CMemoryFile::MemoryInputStream::Reset()
{
    mOffset = mMark;
    return NOERROR;
}

ECode CMemoryFile::MemoryInputStream::Read(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    if (mSingleByte == NULL) {
        mSingleByte = ArrayOf<Byte>::Alloc(1);
    }
    Int32 result;
    Read(mSingleByte, 0, 1, &result);
    if (result != 1) {
        *value = -1;
        return NOERROR;
    }

    *value = (*mSingleByte)[0];
    return NOERROR;
}

ECode CMemoryFile::MemoryInputStream::Read(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    if (offset < 0 || count < 0 || offset + count > buffer->GetLength()) {
        // readBytes() also does this check, but we need to do it before
        // changing count.
        // throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32 available;
    Available(&available);
    count = Elastos::Core::Math::Min(count, available);
    if (count < 1) {
        return NOERROR;
    }

    Int32 result;
    FAIL_RETURN(mHost->ReadBytes(buffer, mOffset, offset, count, &result))
    if (result > 0) {
        mOffset += result;
    }
    *number = result;
    return NOERROR;
}


ECode CMemoryFile::MemoryInputStream::Skip(
    /* [in] */ Int64 n,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    if (mOffset + n > mHost->mLength) {
        n = mHost->mLength - mOffset;
    }
    mOffset += n;
    *number = n;
    return NOERROR;
}

//=============================================================
// CMemoryFile::MemoryOutputStream
//=============================================================
CMemoryFile::MemoryOutputStream::MemoryOutputStream(
    /* [in] */ CMemoryFile* host)
    : mOffset(0)
    , mHost(host)
{
}

ECode CMemoryFile::MemoryOutputStream::Write(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 count)
{
    FAIL_RETURN(mHost->WriteBytes(buffer, offset, mOffset, count))
    mOffset += count;
    return NOERROR;
}

ECode CMemoryFile::MemoryOutputStream::Write(
    /* [in] */ Int32 oneByte)
{
    if (mSingleByte == NULL) {
        mSingleByte = ArrayOf<Byte>::Alloc(1);
    }
    mSingleByte->Set(0, (Byte)oneByte);
    Write(mSingleByte, 0, 1);

    return NOERROR;
}

//=============================================================
//
//=============================================================
const String CMemoryFile::TAG("CMemoryFile");

// const Int32 CMemoryFile::PROT_READ = 0x1;
// const Int32 CMemoryFile::PROT_WRITE = 0x2;

CAR_INTERFACE_IMPL(CMemoryFile, Object, IMemoryFile)

CAR_OBJECT_IMPL(CMemoryFile)

CMemoryFile::CMemoryFile()
    : mAddress(0)
    , mLength(0)
    , mAllowPurging(FALSE)
{
}

ECode CMemoryFile::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 length)
{
    mLength = length;
    if (length >= 0) {
        native_open(name, length, (IFileDescriptor**)&mFD);
    }
    else {
        //throw new IOException("Invalid length: " + length);
        return E_IO_EXCEPTION;
    }

    if (length > 0) {
        return native_mmap(mFD, length, PROT_READ | PROT_WRITE, &mAddress);
    }
    else {
        mAddress = 0;
    }
    return NOERROR;
}

ECode CMemoryFile::Close()
{
    Deactivate();
    if (!IsClosed()) {
        native_close(mFD);
    }
    return NOERROR;
}

ECode CMemoryFile::Deactivate()
{
    if (!IsDeactivated()) {
    // try {
        FAIL_RETURN(native_munmap(mAddress, mLength))
        mAddress = 0;
    // } catch (IOException ex) {
    //     Log.e(TAG, ex.toString());
    // }
    }
    return NOERROR;
}

Boolean CMemoryFile::IsDeactivated()
{
    return mAddress == 0;
}

Boolean CMemoryFile::IsClosed()
{
    Boolean result;
    mFD->Valid(&result);
    return !result;
}

void CMemoryFile::Finalize()
{
    if (!IsClosed()) {
        Logger::E(TAG, "MemoryFile.finalize() called while ashmem still open");
        Close();
    }
}

ECode CMemoryFile::GetLenth(
    /* [out] */ Int32* len)
{
    VALIDATE_NOT_NULL(len)
    *len = mLength;
    return NOERROR;
}

ECode CMemoryFile::IsPurgingAllowed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAllowPurging;
    return NOERROR;
}

ECode CMemoryFile::AllowPurging(
    /* [in] */ Boolean allowPurging,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean oldValue = mAllowPurging;
    if (oldValue != allowPurging) {
        FAIL_RETURN(native_pin(mFD, !allowPurging))
        mAllowPurging = allowPurging;
    }
    *result = oldValue;
    return NOERROR;
}

ECode CMemoryFile::GetInputStream(
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream)
    AutoPtr<IInputStream> is = new MemoryInputStream(this);
    *stream = is;
    REFCOUNT_ADD(*stream)
    return NOERROR;
}

ECode CMemoryFile::GetOutputStream(
    /* [out] */ IOutputStream** stream)
{
    VALIDATE_NOT_NULL(stream)
    AutoPtr<IOutputStream> is = new MemoryOutputStream(this);
    *stream = is;
    REFCOUNT_ADD(*stream)
    return NOERROR;
}

ECode CMemoryFile::ReadBytes(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 destOffset,
    /* [in] */ Int32 count,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    if (IsDeactivated()) {
        // throw new IOException("Can't read from deactivated memory file.");
        return E_IO_EXCEPTION;
    }

    if (destOffset < 0 || destOffset > buffer->GetLength() || count < 0
        || count > buffer->GetLength() - destOffset
        || srcOffset < 0 || srcOffset > mLength
        || count > mLength - srcOffset) {
        // throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return native_read(mFD, mAddress, buffer, srcOffset, destOffset, count, mAllowPurging, number);
}

ECode CMemoryFile::WriteBytes(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 destOffset,
    /* [in] */ Int32 count)
{
    if (IsDeactivated()) {
        // throw new IOException("Can't write to deactivated memory file.");
        return E_IO_EXCEPTION;
    }
    if (srcOffset < 0 || srcOffset > buffer->GetLength() || count < 0
        || count > buffer->GetLength() - srcOffset
        || destOffset < 0 || destOffset > mLength
        || count > mLength - destOffset) {
        // throw new IndexOutOfBoundsException();
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return native_write(mFD, mAddress, buffer, srcOffset, destOffset, count, mAllowPurging);
}

ECode CMemoryFile::GetFileDescriptor(
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = mFD;
    REFCOUNT_ADD(*fd)
    return NOERROR;
}

ECode CMemoryFile::GetSize(
    /* [in] */ IFileDescriptor* fd,
    /* [out] */ Int32* size)
{
    return native_get_size(fd, size);
}

//=======================================================
// native methods
//=======================================================
ECode CMemoryFile::native_open(
    /* [in] */ const String& name,
    /* [in] */ Int32 length,
    /* [out] */ IFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd)
    *fd = NULL;
    Int32 result = ashmem_create_region(name.string(), length);

    if (result < 0) {
        // jniThrowException(env, "java/io/IOException", "ashmem_create_region failed");
        return E_IO_EXCEPTION;
    }

    AutoPtr<IFileDescriptor> fdObj;
    CFileDescriptor::New((IFileDescriptor**)&fdObj);
    fdObj->SetDescriptor(result);
    *fd = fdObj;
    REFCOUNT_ADD(*fd);
    return NOERROR;
}

ECode CMemoryFile::native_mmap(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Int32 length,
    /* [in] */ Int32 prot,
    /* [out] */ Handle64* addr)
{
    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    void* result = mmap(NULL, length, prot, MAP_SHARED, fd, 0);
    if (result == MAP_FAILED) {
        // jniThrowException(env, "java/io/IOException", "mmap failed");
        return E_IO_EXCEPTION;
    }
    *addr = reinterpret_cast<Handle64>(result);
    return NOERROR;
}

ECode CMemoryFile::native_munmap(
    /* [in] */ Handle64 addr,
    /* [in] */ Int32 length)
{
    Int32 result = munmap(reinterpret_cast<void *>(addr), length);
    if (result < 0) {
        // jniThrowException(env, "java/io/IOException", "munmap failed");
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CMemoryFile::native_close(
    /* [in] */ IFileDescriptor* fileDescriptor)
{
    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    if (fd >= 0) {
        fileDescriptor->SetDescriptor(-1);
        close(fd);
    }
    return NOERROR;
}

ECode CMemoryFile::native_read(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Handle64 address,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 destOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean unpinned,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = -1;

    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    if (unpinned && ashmem_pin_region(fd, 0, 0) == ASHMEM_WAS_PURGED) {
        ashmem_unpin_region(fd, 0, 0);
        // jniThrowException(env, "java/io/IOException", "ashmem region was purged");
        return E_IO_EXCEPTION;
    }

    buffer->Copy(destOffset, (const Byte*)address + srcOffset, count);
    //env->SetByteArrayRegion(buffer, destOffset, count, (const jbyte *)address + srcOffset);

    if (unpinned) {
        ashmem_unpin_region(fd, 0, 0);
    }
    *number = count;
    return NOERROR;
}

ECode CMemoryFile::native_write(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Handle64 address,
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 srcOffset,
    /* [in] */ Int32 destOffset,
    /* [in] */ Int32 count,
    /* [in] */ Boolean unpinned)
{
    // VALIDATE_NOT_NULL(number)
    // *number = -1;

    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    if (unpinned && ashmem_pin_region(fd, 0, 0) == ASHMEM_WAS_PURGED) {
        ashmem_unpin_region(fd, 0, 0);
        // jniThrowException(env, "java/io/IOException", "ashmem region was purged");
        return E_IO_EXCEPTION;
    }

    memcpy((Byte*)address + destOffset, (Byte*)buffer->GetPayload() + srcOffset, count);
    // env->GetByteArrayRegion(buffer, srcOffset, count, (jbyte *)address + destOffset);

    if (unpinned) {
        ashmem_unpin_region(fd, 0, 0);
    }

    // *number = count;
    return NOERROR;
}

ECode CMemoryFile::native_pin(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [in] */ Boolean pin)
{
    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    Int32 result = (pin ? ashmem_pin_region(fd, 0, 0) : ashmem_unpin_region(fd, 0, 0));
    if (result < 0) {
        // jniThrowException(env, "java/io/IOException", NULL);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CMemoryFile::native_get_size(
    /* [in] */ IFileDescriptor* fileDescriptor,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = -1;

    Int32 fd;
    fileDescriptor->GetDescriptor(&fd);
    // Use ASHMEM_GET_SIZE to find out if the fd refers to an ashmem region.
    // ASHMEM_GET_SIZE should succeed for all ashmem regions, and the kernel
    // should return ENOTTY for all other valid file descriptors
    Int32 result = ashmem_get_size_region(fd);
    if (result < 0) {
        if (errno == ENOTTY) {
            // ENOTTY means that the ioctl does not apply to this object,
            // i.e., it is not an ashmem region.
            return NOERROR;
        }
        // Some other error, throw exception
        // jniThrowIOException(env, errno);
        return E_IO_EXCEPTION;
    }
    *size = result;
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos