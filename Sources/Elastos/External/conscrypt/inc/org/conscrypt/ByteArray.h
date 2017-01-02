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

#ifndef __ORG_CONSCRYPT_BYTEARRAY_H__
#define __ORG_CONSCRYPT_BYTEARRAY_H__

#include <elastos/coredef.h>
#include <elastos/core/Object.h>
#include "_Org.Conscrypt.h"

namespace Org {
namespace Conscrypt {

/**
 * Byte array wrapper for hashtable use. Implements equals() and hashCode().
 */
class ByteArray
    : public IByteArray
    , public Object
{
public:
    CAR_INTERFACE_DECL()

    ByteArray(
        /* [in] */ ArrayOf<Byte>* bytes);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    AutoPtr<ArrayOf<Byte> > mBytes;
    Int32 mHashCode;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_BYTEARRAY_H__
