
#include "Elastos.Droid.Os.h"
#include "elastos/droid/internal/utility/CMemInfoReader.h"
#include "elastos/droid/os/Debug.h"
#include "elastos/droid/os/CStrictMode.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Debug;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CMemInfoReader, Object, IMemInfoReader)

CAR_OBJECT_IMPL(CMemInfoReader)

CMemInfoReader::CMemInfoReader()
{
}

ECode CMemInfoReader::constructor()
{
    mInfos = ArrayOf<Int64>::Alloc(IDebug::MEMINFO_COUNT);
    return NOERROR;
}

ECode CMemInfoReader::ReadMemInfo()
{
    // Permit disk reads here, as /proc/meminfo isn't really "on
    // disk" and should be fast.  TODO: make BlockGuard ignore
    // /proc/ and /sys/ files perhaps?
    AutoPtr<IStrictMode> strictMode;
    CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
    AutoPtr<IStrictModeThreadPolicy> savedPolicy;
    strictMode->AllowThreadDiskReads((IStrictModeThreadPolicy**)&savedPolicy);
    ECode ec = Debug::GetMemInfo(mInfos);
    strictMode->SetThreadPolicy(savedPolicy);
    return ec;
}

ECode CMemInfoReader::GetTotalSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_TOTAL] * 1024;
    return NOERROR;
}

ECode CMemInfoReader::GetFreeSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_FREE] * 1024;
    return NOERROR;
}

ECode CMemInfoReader::GetCachedSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_CACHED] * 1024;
    return NOERROR;
}

ECode CMemInfoReader::GetTotalSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_TOTAL];
    return NOERROR;
}

ECode CMemInfoReader::GetFreeSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_FREE];
    return NOERROR;
}

ECode CMemInfoReader::GetCachedSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_CACHED];
    return NOERROR;
}

ECode CMemInfoReader::GetBuffersSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_BUFFERS];
    return NOERROR;
}

ECode CMemInfoReader::GetShmemSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_SHMEM];
    return NOERROR;
}

ECode CMemInfoReader::GetSlabSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_SLAB];
    return NOERROR;
}

ECode CMemInfoReader::GetSwapTotalSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_SWAP_TOTAL];
    return NOERROR;
}

ECode CMemInfoReader::GetSwapFreeSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_SWAP_FREE];
    return NOERROR;
}

ECode CMemInfoReader::GetZramTotalSizeKb(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size)
    *size = (*mInfos)[IDebug::MEMINFO_ZRAM_TOTAL];
    return NOERROR;
}
} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
