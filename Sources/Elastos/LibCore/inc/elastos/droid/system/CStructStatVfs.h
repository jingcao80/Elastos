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

#ifndef __ELASTOS_DROID_SYSTEM_CSTRUCTSTATVFS_H__
#define __ELASTOS_DROID_SYSTEM_CSTRUCTSTATVFS_H__

#include "core/Object.h"
#include "_Elastos_Droid_System_CStructStatVfs.h"


namespace Elastos {
namespace Droid {
namespace System {

/**
 * File information returned by fstatfs(2) and statfs(2).
 *
 * TODO: this should be {@code struct statvfs}, but Bionic doesn't support that yet.
 * @hide until the TODO is fixed.
 */
CarClass(CStructStatVfs)
    , public Object
    , public IStructStatVfs
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetBsize(
        /* [out] */ Int64* bsize);

    CARAPI GetBlocks(
        /* [out] */ Int64* blocks);

    CARAPI GetBfree(
        /* [out] */ Int64* bfree);

    CARAPI GetBavail(
        /* [out] */ Int64* bavail);

    CARAPI GetFiles(
        /* [out] */ Int64* files);

    CARAPI GetFfree(
        /* [out] */ Int64* ffree);

    CARAPI GetNamemax(
        /* [out] */ Int64* namemax);

    CARAPI GetFavail(
        /* [out] */ Int64* favail);

    CARAPI GetFsid(
        /* [out] */ Int64* fsid);

    CARAPI GetFlag(
        /* [out] */ Int64* flag);

    CARAPI GetFrsize(
        /* [out] */ Int64* frsize);

    CARAPI constructor(
        /* [in] */ Int64 f_bsize,
        /* [in] */ Int64 f_frsize,
        /* [in] */ Int64 f_blocks,
        /* [in] */ Int64 f_bfree,
        /* [in] */ Int64 f_bavail,
        /* [in] */ Int64 f_files,
        /* [in] */ Int64 f_ffree,
        /* [in] */ Int64 f_favail,
        /* [in] */ Int64 f_fsid,
        /* [in] */ Int64 f_flag,
        /* [in] */ Int64 f_namemax);

private:
  /** File system block size (used for block counts). */
    Int64 mF_bsize; /*unsigned long*/

  /** Fundamental file system block size. */
    Int64 mF_frsize; /*unsigned long*/

  /** Total block count. */
    Int64 mF_blocks; /*fsblkcnt_t*/

  /** Free block count. */
    Int64 mF_bfree; /*fsblkcnt_t*/

  /** Free block count available to non-root. */
    Int64 mF_bavail; /*fsblkcnt_t*/

  /** Total file (inode) count. */
    Int64 mF_files; /*fsfilcnt_t*/

  /** Free file (inode) count. */
    Int64 mF_ffree; /*fsfilcnt_t*/

  /** Free file (inode) count available to non-root. */
    Int64 mF_favail; /*fsfilcnt_t*/

  /** File system id. */
    Int64 mF_fsid; /*unsigned long*/

  /** Bit mask of ST_* flags. */
    Int64 mF_flag; /*unsigned long*/

  /** Maximum filename length. */
    Int64 mF_namemax; /*unsigned long*/
};

} // namespace System
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEM_CSTRUCTSTATVFS_H__
