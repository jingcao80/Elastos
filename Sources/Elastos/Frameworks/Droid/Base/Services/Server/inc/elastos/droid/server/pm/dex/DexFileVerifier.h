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

#ifndef __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__

#include "elastos/droid/server/pm/DexFile.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Server::Pm::DexFile;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class DexFileVerifier : public Object
{
public:
    static CARAPI_(Boolean) Verify(
        /* [in] */ const DexFile* dex_file,
        /* [in] */ const Byte* begin,
        /* [in] */ size_t size,
        /* [in] */ const char* location,
        /* [out] */ String* error_msg);

    CARAPI_(String) FailureReason() const
    {
        return mFailureReason;
    }

private:
    DexFileVerifier(
        /* [in] */ const DexFile* dex_file,
        /* [in] */ const Byte* begin,
        /* [in] */ size_t size,
        /* [in] */ const char* location)
        : mDexFile(dex_file)
        , mBegin(begin)
        , mSize(size)
        , mLocation(location)
        , mHeader(&mDexFile->GetHeader())
        , mPtr(NULL)
        , mPreviousItem(NULL)
    {}

    CARAPI_(Boolean) Verify();

    CARAPI_(Boolean) CheckListSize(
        /* [in] */ const void* start,
        /* [in] */ size_t count,
        /* [in] */ size_t element_size,
        /* [in] */ const char* label);

    // Checks whether the offset is zero (when size is zero) or that the offset falls within the area
    // claimed by the file.
    CARAPI_(Boolean) CheckValidOffsetAndSize(
        /* [in] */ uint32_t offset,
        /* [in] */ uint32_t size,
        /* [in] */ const char* label);

    CARAPI_(Boolean) CheckHeader();

    CARAPI_(Boolean) CheckMap();

    CARAPI_(Boolean) CheckPadding(
        /* [in] */ size_t offset,
        /* [in] */ uint32_t aligned_offset);

    CARAPI_(Boolean) CheckIntraSectionIterate(
        /* [in] */ size_t offset,
        /* [in] */ uint32_t count,
        /* [in] */ uint16_t type);

    CARAPI_(Boolean) CheckIntraIdSection(
        /* [in] */ size_t offset,
        /* [in] */ uint32_t count,
        /* [in] */ uint16_t type);

    CARAPI_(Boolean) CheckIntraDataSection(
        /* [in] */ size_t offset,
        /* [in] */ uint32_t count,
        /* [in] */ uint16_t type);

    CARAPI_(Boolean) CheckIntraSection();

    CARAPI_(Boolean) CheckInterSection();

    CARAPI_(void) ErrorStringPrintf(const char* fmt, ...)
            __attribute__((__format__(__printf__, 2, 3)));

private:
    const DexFile* const mDexFile;
    const Byte* const mBegin;
    const size_t mSize;
    const char* const mLocation;
    const DexFile::Header* const mHeader;

    const Byte* mPtr;
    const void* mPreviousItem;

    String mFailureReason;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_DEXFILEVERIFIER_H__
