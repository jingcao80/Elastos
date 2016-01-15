
#include "CStructStat.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructStat)

CAR_INTERFACE_IMPL(CStructStat, Object, IStructStat)

ECode CStructStat::GetDev(
    /* [out] */ Int64* dev)
{
    VALIDATE_NOT_NULL(dev);

    *dev = mSt_dev;
    return NOERROR;
}

ECode CStructStat::GetIno(
    /* [out] */ Int64* ino)
{
    VALIDATE_NOT_NULL(ino);

    *ino = mSt_ino;
    return NOERROR;
}

ECode CStructStat::GetMode(
    /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);

    *mode = mSt_mode;
    return NOERROR;
}

ECode CStructStat::GetNlink(
    /* [out] */ Int64* nlink)
{
    VALIDATE_NOT_NULL(nlink);

    *nlink = mSt_nlink;
    return NOERROR;
}

ECode CStructStat::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mSt_uid;
    return NOERROR;
}

ECode CStructStat::GetGid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid);

    *gid = mSt_gid;
    return NOERROR;
}

ECode CStructStat::GetRdev(
    /* [out] */ Int64* rdev)
{
    VALIDATE_NOT_NULL(rdev);

    *rdev = mSt_rdev;
    return NOERROR;
}

ECode CStructStat::GetSize(
    /* [out] */ Int64* size)
{
    VALIDATE_NOT_NULL(size);

    *size = mSt_size;
    return NOERROR;
}

ECode CStructStat::GetAtime(
    /* [out] */ Int64* atime)
{
    VALIDATE_NOT_NULL(atime);

    *atime = mSt_atime;
    return NOERROR;
}

ECode CStructStat::GetMtime(
    /* [out] */ Int64* mtime)
{
    VALIDATE_NOT_NULL(mtime);

    *mtime = mSt_mtime;
    return NOERROR;
}

ECode CStructStat::GetCtime(
    /* [out] */ Int64* ctime)
{
    VALIDATE_NOT_NULL(ctime);

    *ctime = mSt_ctime;
    return NOERROR;
}

ECode CStructStat::GetBlksize(
    /* [out] */ Int64* blksize)
{
    VALIDATE_NOT_NULL(blksize);

    *blksize = mSt_blksize;
    return NOERROR;
}

ECode CStructStat::GetBlocks(
    /* [out] */ Int64* blocks)
{
    VALIDATE_NOT_NULL(blocks);

    *blocks = mSt_blocks;
    return NOERROR;
}

ECode CStructStat::constructor(
    /* [in] */ Int64 dev,
    /* [in] */ Int64 ino,
    /* [in] */ Int32 mode,
    /* [in] */ Int64 nlink,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ Int64 rdev,
    /* [in] */ Int64 size,
    /* [in] */ Int64 atime,
    /* [in] */ Int64 mtime,
    /* [in] */ Int64 ctime,
    /* [in] */ Int64 blksize,
    /* [in] */ Int64 blocks)
{
    mSt_dev = dev;
    mSt_ino = ino;
    mSt_mode = mode;
    mSt_nlink = nlink;
    mSt_uid = uid;
    mSt_gid = gid;
    mSt_rdev = rdev;
    mSt_size = size;
    mSt_atime = atime;
    mSt_mtime = mtime;
    mSt_ctime = ctime;
    mSt_blksize = blksize;
    mSt_blocks = blocks;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
