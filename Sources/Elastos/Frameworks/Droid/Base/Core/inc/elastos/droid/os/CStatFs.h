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
