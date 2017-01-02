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

#ifndef __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CADLER32_H__
#define __ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CADLER32_H__

#include "_Elastos_Utility_Zip_CAdler32.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {
namespace Zip {

CarClass(CAdler32)
    , public Object
    , public IAdler32
    , public IChecksum
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAdler32();

    /**
     * Returns the {@code Adler32} checksum for all input received.
     *
     * @return The checksum for this instance.
     */
    CARAPI GetValue(
        /* [out] */ Int64* value);

    /**
     * Reset this instance to its initial checksum.
     */
    CARAPI Reset();

    /**
     * Update this {@code Adler32} checksum with the single byte provided as
     * argument.
     *
     * @param i
     *            the byte to update checksum with.
     */
    CARAPI Update(
        /* [in] */ Int32 i);

    /**
     * Update this {@code Adler32} checksum using the contents of {@code buf}.
     *
     * @param buf
     *            bytes to update checksum with.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* buf);

    /**
     * Update this {@code Adler32} checksum with the contents of {@code buf},
     * starting from {@code offset} and reading {@code byteCount} bytes of data.
     */
    CARAPI Update(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount);

private:
    CARAPI_(Int64) UpdateImpl(
        /* [in] */ ArrayOf<Byte>* buf,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 byteCount,
        /* [in] */ Int64 adler);

    CARAPI_(Int64) UpdateByteImpl(
        /* [in] */ Int32 val,
        /* [in] */ Int64 adler);

private:
    Int64 mAdler;
};

} // namespace Zip
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ZIP_ELASTOS_UTILITY_CADLER32_H__
