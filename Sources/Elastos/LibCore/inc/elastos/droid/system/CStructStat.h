
#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTSTAT_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTSTAT_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructStat.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * File information returned by fstat(2), lstat(2), and stat(2). Corresponds to C's
 * {@code struct stat} from
 * <a href="http://www.opengroup.org/onlinepubs/000095399/basedefs/sys/stat.h.html">&lt;stat.h&gt;</a>
 */
CarClass(CStructStat)
    , public Object
    , public IStructStat
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetDev(
        /* [out] */ Int64* dev);

    CARAPI GetIno(
        /* [out] */ Int64* ino);

    CARAPI GetMode(
        /* [out] */ Int32* mode);

    CARAPI GetNlink(
        /* [out] */ Int64* nlink);

    CARAPI GetUid(
        /* [out] */ Int32* uid);

    CARAPI GetGid(
        /* [out] */ Int32* gid);

    CARAPI GetRdev(
        /* [out] */ Int64* rdev);

    CARAPI GetSize(
        /* [out] */ Int64* size);

    CARAPI GetAtime(
        /* [out] */ Int64* atime);

    CARAPI GetMtime(
        /* [out] */ Int64* mtime);

    CARAPI GetCtime(
        /* [out] */ Int64* ctime);

    CARAPI GetBlksize(
        /* [out] */ Int64* blksize);

    CARAPI GetBlocks(
        /* [out] */ Int64* blocks);

    CARAPI constructor(
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
        /* [in] */ Int64 blocks);

private:
    /** Device ID of device containing file. */
    Int64 mSt_dev; /*dev_t*/

    /** File serial number (inode). */
    Int64 mSt_ino; /*ino_t*/

    /** Mode (permissions) of file. */
    Int32 mSt_mode; /*mode_t*/

    /** Number of hard links to the file. */
    Int64 mSt_nlink; /*nlink_t*/

    /** User ID of file. */
    Int32 mSt_uid; /*uid_t*/

    /** Group ID of file. */
    Int32 mSt_gid; /*gid_t*/

    /** Device ID (if file is character or block special). */
    Int64 mSt_rdev; /*dev_t*/

    /**
     * For regular files, the file size in bytes.
     * For symbolic links, the length in bytes of the pathname contained in the symbolic link.
     * For a shared memory object, the length in bytes.
     * For a typed memory object, the length in bytes.
     * For other file types, the use of this field is unspecified.
     */
    Int64 mSt_size; /*off_t*/

    /** Time of last access. */
    Int64 mSt_atime; /*time_t*/

    /** Time of last data modification. */
    Int64 mSt_mtime; /*time_t*/

    /** Time of last status change. */
    Int64 mSt_ctime; /*time_t*/

    /**
     * A file system-specific preferred I/O block size for this object.
     * For some file system types, this may vary from file to file.
     */
    Int64 mSt_blksize; /*blksize_t*/

    /** Number of blocks allocated for this object. */
    Int64 mSt_blocks; /*blkcnt_t*/
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTSTAT_H__
