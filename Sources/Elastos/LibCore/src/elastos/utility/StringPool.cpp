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

#include "elastos/utility/StringPool.h"

namespace Elastos {
namespace Utility {

StringPool::StringPool()
{
    mPool = ArrayOf<String>::Alloc(512);
}

String StringPool::Get(
    /* [in] */ ArrayOf<Char32>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    // Compute an arbitrary hash of the content
    Int32 hashCode = 0;
    for (Int32 i = start; i < start + length; i++) {
        hashCode = (hashCode * 31) + (*array)[i];
    }

    // Pick a bucket using Doug Lea's supplemental secondaryHash function (from HashMap)
    hashCode ^= (hashCode >> 20) ^ (hashCode >> 12);
    hashCode ^= (hashCode >> 7) ^ (hashCode >> 4);
    Int32 index = hashCode & (mPool->GetLength() - 1);

    String pooled = (*mPool)[index];
    if (pooled != NULL && ContentEquals(pooled, array, start, length)) {
        return pooled;
    }

    String result(*array, start, length);
    mPool->Set(index, result);
    return result;
}

Boolean StringPool::ContentEquals(
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<Char32>* chars,
    /* [in] */ Int32 start,
    /* [in] */ Int32 length)
{
    if (s.GetLength() != length) {
        return FALSE;
    }
    AutoPtr<ArrayOf<Char32> > strChars = s.GetChars();
    for (Int32 i = 0; i < length; i++) {
        if ((*chars)[start + i] != (*strChars)[i]) {
            return FALSE;
        }
    }
    return TRUE;
}


} // namespace Utility
} // namespace Elastos
