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

#ifndef __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPEC_H__

#include "_Elastosx_Crypto_Spec_CDESedeKeySpec.h"
#include <elastos/core/Object.h>

using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::EIID_IKeySpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CDESedeKeySpec)
    , public Object
    , public IDESedeKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CDESedeKeySpec();

    /**
     * Creates a new <code>DESedeKeySpec</code> instance from the first 24 (
     * {@link #DES_EDE_KEY_LEN}) bytes of the specified key data.
     *
     * @param key
     *            the key data.
     * @throws InvalidKeyException
     *             if the length of the key data is less than 24.
     * @throws NullPointerException
     *             if the key data is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * key);

    /**
     * Creates a new <code>DESedeKeySpec</code> instance from the first 24 (
     * {@link #DES_EDE_KEY_LEN} ) bytes of the specified key data starting at
     * <code>offset</code>.
     *
     * @param key
     *            the key data
     * @param offset
     *            the offset to start at.
     * @throws InvalidKeyException
     *             if the length of the key data starting at offset is less than
     *             24.
     * @throws NullPointerException
     *             if the key data is null.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * key,
        /* [in] */ Int32 offset);

    /**
     * Returns a copy of the key.
     *
     * @return a copy of the key.
     */
    CARAPI GetKey(
        /* [out, callee] */ ArrayOf<Byte> ** key);

    /**
     * Returns whether the specified key data starting at <code>offset</code> is
     * <i>parity-adjusted</i>.
     *
     * @param key
     *            the key data.
     * @param offset
     *            the offset to start checking at.
     * @return {@code true} if the specified key data is parity-adjusted,
     *            {@code false} otherwise.
     * @throws InvalidKeyException
     *             if the length of the key data starting at offset is less than
     *             24.
     */
    static CARAPI IsParityAdjusted(
        /* [in] */ ArrayOf<Byte> * key,
        /* [in] */ Int32 offset,
        /* [out] */ Boolean * result);

private:
    AutoPtr<ArrayOf<Byte> > mKey;
};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CDESEDEKEYSPEC_H__
