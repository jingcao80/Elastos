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

#include "org/conscrypt/ByteArray.h"
#include <elastos/utility/Arrays.h>

using Elastos::Utility::Arrays;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(ByteArray, Object, IByteArray)

ByteArray::ByteArray(
    /* [in] */ ArrayOf<Byte>* bytes)
{
    mBytes = bytes;
    mHashCode = Arrays::GetHashCode(bytes);
}

ECode ByteArray::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHashCode;
    return NOERROR;
}

ECode ByteArray::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IByteArray::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<IByteArray> lhs = IByteArray::Probe(obj);
    *result = Arrays::Equals(mBytes, ((ByteArray*)lhs.Get())->mBytes);
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
