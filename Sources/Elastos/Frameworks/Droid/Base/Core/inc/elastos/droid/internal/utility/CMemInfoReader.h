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
