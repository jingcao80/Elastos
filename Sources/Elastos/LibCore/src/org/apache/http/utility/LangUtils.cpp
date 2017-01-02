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

#include "org/apache/http/utility/LangUtils.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Utility {

const Int32 LangUtils::HASH_SEED;
const Int32 LangUtils::HASH_OFFSET;

Int32 LangUtils::HashCode(
    /* [in] */ Int32 seed,
    /* [in] */ Int32 hashcode)
{
    return seed * HASH_OFFSET + hashcode;
}

Int32 LangUtils::HashCode(
    /* [in] */ Int32 seed,
    /* [in] */ Boolean b)
{
    return HashCode(seed, b ? 1 : 0);
}

Int32 LangUtils::HashCode(
    /* [in] */ Int32 seed,
    /* [in] */ IObject* obj)
{
    Int32 hashcode = 0;
    if (obj != NULL) {
        obj->GetHashCode(&hashcode);
    }
    return HashCode(seed, hashcode);
}

Boolean LangUtils::Equals(
    /* [in] */ IObject* obj1,
    /* [in] */ IObject* obj2)
{
    Boolean result = FALSE;
    if (obj1 == NULL) {
        result = obj2 == NULL;
    }
    else {
        obj1->Equals(obj2, &result);
    }
    return result;
}

Boolean LangUtils::Equals(
    /* [in] */ ArrayOf<IObject*>* a1,
    /* [in] */ ArrayOf<IObject*>* a2)
{
    if (a1 == NULL) {
        if (a2 == NULL) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else {
        if (a2 != NULL && a1->GetLength() == a2->GetLength()) {
            for (Int32 i = 0; i < a1->GetLength(); i++) {
                if (!Equals((*a1)[i], (*a2)[i])) {
                    return FALSE;
                }
            }
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
}

} // namespace Utility
} // namespace Http
} // namespace Apache
} // namespace Org
