#include "CMemoryMappedFile.h"
#include "CLibcore.h"
#include "droid/system/OsConstants.h"
#include "io/CByteOrderHelper.h"
#include "NioBufferIterator.h"
#include "CLibcore.h"
#include "AutoLock.h"

using Elastos::IO::CByteOrderHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::ByteOrder;
using Elastos::IO::ByteOrder_BIG_ENDIAN;
using Elastos::IO::ByteOrder_LITTLE_ENDIAN;
using Elastos::IO::EIID_ICloseable;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::System::IStructStat;
using Elastos::IO::IFileDescriptor;

namespace Libcore {
namespace IO {

CAR_OBJECT_IMPL(CMemoryMappedFile)

CAR_INTERFACE_IMPL_2(CMemoryMappedFile, Object, IMemoryMappedFile, ICloseable)

ECode CMemoryMappedFile::constructor(
    /* [in] */ Int64 address,
    /* [in] */ Int64 size)
{
    mAddress = address;
    mSize = size;
    return NOERROR;
}

/**
 * Returns a new iterator that treats the mapped data as big-endian.
 */
ECode CMemoryMappedFile::BigEndianIterator(
    /* [out] */ IBufferIterator** it)
{
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder order;
    helper->GetNativeOrder(&order);
    *it = new NioBufferIterator(mAddress, (Int32) mSize, order != ByteOrder_BIG_ENDIAN);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

/**
 * Returns a new iterator that treats the mapped data as little-endian.
 */
ECode CMemoryMappedFile::LittleEndianIterator(
    /* [out] */ IBufferIterator** it)
{
    AutoPtr<IByteOrderHelper> helper;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&helper);
    ByteOrder order;
    helper->GetNativeOrder(&order);
    *it = new NioBufferIterator(mAddress, (Int32) mSize, order != ByteOrder_LITTLE_ENDIAN);
    REFCOUNT_ADD(*it)
    return NOERROR;
}

/**
 * Returns the size in bytes of the memory-mapped region.
 */
ECode CMemoryMappedFile::Size(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = mSize;
    return NOERROR;
}

ECode CMemoryMappedFile::Close()
{
    AutoLock lock(this);
    if (mAddress != 0) {
        CLibcore::sOs->Munmap(mAddress, mSize);
        mAddress = 0;
    }
    return NOERROR;
}

ECode CMemoryMappedFile::MmapRO(
    /* [in] */ const String& path,
    /* [in] */ IMemoryMappedFile** rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = NULL;

    AutoPtr<IFileDescriptor> fd;
    AutoPtr<IOs> os = CLibcore::sOs;
    if (os == NULL) {
        // TODO upgrade: delete this
        return NOERROR;
    }
    FAIL_RETURN(os->Open(path, OsConstants::_O_RDONLY, 0, (IFileDescriptor**)&fd))
    AutoPtr<IStructStat> structState;
    FAIL_RETURN(os->Fstat(fd, (IStructStat**)&structState))
    Int64 size;
    FAIL_RETURN(structState->GetSize(&size))
    Int64 address;
    FAIL_RETURN(os->Mmap(0L, size, OsConstants::_PROT_READ, OsConstants::_MAP_SHARED, fd, 0, &address))
    FAIL_RETURN(os->Close(fd))
    return CMemoryMappedFile::New(address, size, rst);
}

} // namespace IO
} // namespace Libcore