#ifndef __ELASTOS_DROID_OS_CSTATFS_H__
#define __ELASTOS_DROID_OS_CSTATFS_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Os_CStatFs.h"
#include <elastos/core/Object.h>

using Elastos::Droid::System::IStructStatVfs;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStatFs)
    , public Object
    , public IStatFs
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Restat(
        /* [in] */ const String& path);

    CARAPI GetBlockSize(
        /* [out] */ Int32* size);

    CARAPI GetBlockSize(
        /* [out] */ Int64* size);

    CARAPI GetBlockCount(
        /* [out] */ Int32* count);

    CARAPI GetBlockCount(
        /* [out] */ Int64* count);

    CARAPI GetFreeBlocks(
        /* [out] */ Int32* blocks);

    CARAPI GetFreeBlocks(
        /* [out] */ Int64* blocks);

   /**
     * The number of bytes that are free on the file system, including reserved
     * blocks (that are not available to normal applications). Most applications
     * will want to use {@link #getAvailableBytes()} instead.
     */
    CARAPI GetFreeBytes(
        /* [out] */ Int64* blocks);

    CARAPI GetAvailableBlocks(
        /* [out] */ Int32* blocks);

    CARAPI GetAvailableBlocks(
        /* [out] */ Int64* blocks);

    CARAPI constructor(
        /* [in] */ const String& path);

    /**
     * The number of bytes that are free on the file system and available to
     * applications.
     */
    CARAPI GetAvailableBytes(
        /* [out] */ Int64* blocks);

    /**
     * The total number of bytes supported by the file system.
     */
    CARAPI GetTotalBytes(
        /* [out] */ Int64* blocks);

private:
    static CARAPI DoStat(
        /* [in] */ const String& path,
        /* [out] */ IStructStatVfs** fs);

private:
    AutoPtr<IStructStatVfs> mStat;
};

} //Os
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_OS_CSTATFS_H__
