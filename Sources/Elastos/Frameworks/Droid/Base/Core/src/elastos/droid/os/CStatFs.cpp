#include "elastos/droid/os/CStatFs.h"
#include "stdio.h"
#include <elastos/droid/system/Os.h>

using Elastos::Droid::System::Os;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CStatFs, Object, IStatFs)

CAR_OBJECT_IMPL(CStatFs)

ECode CStatFs::Restat(
    /* [in] */ const String& path)
{
    mStat = NULL;
    return DoStat(path, (IStructStatVfs**)&mStat);
}

ECode CStatFs::GetBlockSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size);
    Int64 _size;
    mStat->GetBsize(&_size);
    *size = (Int32)_size;
    return NOERROR;
}

ECode CStatFs::GetBlockSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size);
    mStat->GetBsize(size);
    return NOERROR;
}

ECode CStatFs::GetBlockCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    Int64 _count;
    mStat->GetBlocks(&_count);
    *count = (Int32)_count;
    return NOERROR;
}

ECode CStatFs::GetBlockCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count);
    mStat->GetBlocks(count);
    return NOERROR;
}

ECode CStatFs::GetFreeBlocks(
    /* [out] */ Int32* blocks)
{
    VALIDATE_NOT_NULL(blocks);
    Int64 _blocks;
    mStat->GetBfree(&_blocks);
    *blocks = (Int32)_blocks;
    return NOERROR;
}

ECode CStatFs::GetFreeBlocks(
    /* [out] */ Int64* blocks)
{
    VALIDATE_NOT_NULL(blocks);
    mStat->GetBfree(blocks);
    return NOERROR;
}

ECode CStatFs::GetAvailableBlocks(
    /* [out] */ Int32* blocks)
{
    VALIDATE_NOT_NULL(blocks);
    Int64 _blocks;
    mStat->GetBavail(&_blocks);
    *blocks = (Int32)_blocks;
    return NOERROR;
}

ECode CStatFs::GetAvailableBlocks(
    /* [out] */ Int64* blocks)
{
    VALIDATE_NOT_NULL(blocks);
    mStat->GetBavail(blocks);
    return NOERROR;
}

ECode CStatFs::GetFreeBytes(
    /* [out] */ Int64* bytes)
{
    VALIDATE_NOT_NULL(bytes);
    Int64 block, size;
    mStat->GetBfree(&block);
    mStat->GetBsize(&size);
    *bytes = block * size;
    return NOERROR;
}

ECode CStatFs::GetAvailableBytes(
    /* [out] */ Int64* bytes)
{
    VALIDATE_NOT_NULL(bytes);
    Int64 block, size;
    mStat->GetBavail(&block);
    mStat->GetBsize(&size);
    *bytes = block * size;
    return NOERROR;
}

ECode CStatFs::GetTotalBytes(
    /* [out] */ Int64* bytes)
{
    VALIDATE_NOT_NULL(bytes);
    Int64 block, size;
    mStat->GetBlocks(&block);
    mStat->GetBsize(&size);
    *bytes = block * size;
    return NOERROR;
}

ECode CStatFs::DoStat(
    /* [in] */ const String& path,
    /* [out] */ IStructStatVfs** fs)
{
    VALIDATE_NOT_NULL(fs);
    *fs = NULL;

    return Elastos::Droid::System::Os::StatVfs(path, fs);
}

ECode CStatFs::constructor(
    /* [in] */ const String& path)
{
    return DoStat(path, (IStructStatVfs**)&mStat);
}

} //Os
} //Droid
} //Elastos
