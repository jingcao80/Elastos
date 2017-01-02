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

#include "CBitSetHelper.h"
#include "CBitSet.h"

namespace Elastos {
namespace Utility {

CAR_SINGLETON_IMPL(CBitSetHelper)
CAR_INTERFACE_IMPL(CBitSetHelper, Singleton, IBitSetHelper)


ECode CBitSetHelper::ValueOf(
    /* [in] */ ArrayOf<Int64>* int64Arr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(int64Arr, bs);
}

/**
 * Returns a {@code BitSet} corresponding to {@code longBuffer}, interpreted as a little-endian
 * sequence of bits. This method does not alter the {@code LongBuffer}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ IInt64Buffer* int64Arr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(int64Arr, bs);
}

/**
 * Equivalent to {@code BitSet.valueOf(ByteBuffer.wrap(bytes))}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ ArrayOf<Byte>* byteArr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(byteArr, bs);
}

/**
 * Returns a {@code BitSet} corresponding to {@code byteBuffer}, interpreted as a little-endian
 * sequence of bits. This method does not alter the {@code ByteBuffer}.
 * @hide 1.7
 */
ECode CBitSetHelper::ValueOf(
    /* [in] */ IByteBuffer* byteArr,
    /* [out] */ IBitSet** bs)
{
    return CBitSet::ValueOf(byteArr, bs);
}

} // namespace Utility
} // namespace Elastos
