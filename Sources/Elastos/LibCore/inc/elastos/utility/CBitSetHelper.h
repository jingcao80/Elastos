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

#ifndef __ELASTOS_UTILITY_CBITSETHELPER_H__
#define __ELASTOS_UTILITY_CBITSETHELPER_H__

#include "_Elastos_Utility_CBitSetHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;
using Elastos::IO::IInt64Buffer;
using Elastos::IO::IByteBuffer;

namespace Elastos {
namespace Utility {

CarClass(CBitSetHelper)
    , public Singleton
    , public IBitSetHelper
{
public:
    CAR_SINGLETON_DECL()
    CAR_INTERFACE_DECL()

	/**
     * Equivalent to {@code BitSet.valueOf(LongBuffer.wrap(longs))}, but likely to be faster.
     * This is likely to be the fastest way to create a {@code BitSet} because it's closest
     * to the internal representation.
     * @hide 1.7
     */
    CARAPI ValueOf(
        /* [in] */ ArrayOf<Int64>* int64Arr,
        /* [out] */ IBitSet** bs);

    /**
     * Returns a {@code BitSet} corresponding to {@code longBuffer}, interpreted as a little-endian
     * sequence of bits. This method does not alter the {@code LongBuffer}.
     * @hide 1.7
     */
    CARAPI ValueOf(
        /* [in] */ IInt64Buffer* int64Arr,
        /* [out] */ IBitSet** bs);

    /**
     * Equivalent to {@code BitSet.valueOf(ByteBuffer.wrap(bytes))}.
     * @hide 1.7
     */
    CARAPI ValueOf(
        /* [in] */ ArrayOf<Byte>* byteArr,
        /* [out] */ IBitSet** bs);

    /**
     * Returns a {@code BitSet} corresponding to {@code byteBuffer}, interpreted as a little-endian
     * sequence of bits. This method does not alter the {@code ByteBuffer}.
     * @hide 1.7
     */
    CARAPI ValueOf(
        /* [in] */ IByteBuffer* byteArr,
        /* [out] */ IBitSet** bs);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CBITSETHELPER_H__
