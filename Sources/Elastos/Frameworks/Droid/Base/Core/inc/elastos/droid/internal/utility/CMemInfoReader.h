
#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_CMEMINFOREADER_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_CMEMINFOREADER_H__

#include "_Elastos_Droid_Internal_Utility_CMemInfoReader.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CarClass(CMemInfoReader)
    , public Object
    , public IMemInfoReader
{
public:
    CMemInfoReader();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI ReadMemInfo();

    CARAPI GetTotalSize(
        /* [out] */ Int64* size);

    CARAPI GetFreeSize(
        /* [out] */ Int64* size);

    CARAPI GetCachedSize(
        /* [out] */ Int64* size);

    CARAPI GetTotalSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetFreeSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetCachedSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetBuffersSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetShmemSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetSlabSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetSwapTotalSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetSwapFreeSizeKb(
        /* [out] */ Int64* size);

    CARAPI GetZramTotalSizeKb(
        /* [out] */ Int64* size);

public:
    AutoPtr<ArrayOf<Int64> > mInfos;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_CMEMINFOREADER_H__
