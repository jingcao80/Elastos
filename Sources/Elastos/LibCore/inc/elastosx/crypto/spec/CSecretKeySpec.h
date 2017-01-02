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

#ifndef __ELASTOSX_CRYPTO_SPEC_CSECRETKEYSPEC_H__
#define __ELASTOSX_CRYPTO_SPEC_CSECRETKEYSPEC_H__

#include "_Elastosx_Crypto_Spec_CSecretKeySpec.h"
#include <elastos/core/Object.h>

using Elastos::IO::ISerializable;
using Elastos::IO::EIID_ISerializable;
using Elastosx::Crypto::ISecretKey;
using Elastos::Security::IKey;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::EIID_IKeySpec;

namespace Elastosx {
namespace Crypto {
namespace Spec {

CarClass(CSecretKeySpec)
    , public Object
    , public ISecretKeySpec
    , public ISecretKey
    , public IKey
    , public IKeySpec
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSecretKeySpec();

    /**
     * Creates a new <code>SecretKeySpec</code> for the specified key data and
     * algorithm name.
     *
     * @param key
     *            the key data.
     * @param algorithm
     *            the algorithm name.
     * @throws IllegalArgumentException
     *             if the key data or the algorithm name is null or if the key
     *             data is empty.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * key,
        /* [in] */ const String& algorithm);

    /**
     * Creates a new <code>SecretKeySpec</code> for the key data from the
     * specified buffer <code>key</code> starting at <code>offset</code> with
     * length <code>len</code> and the specified <code>algorithm</code> name.
     *
     * @param key
     *            the key data.
     * @param offset
     *            the offset.
     * @param len
     *            the size of the key data.
     * @param algorithm
     *            the algorithm name.
     * @throws IllegalArgumentException
     *             if the key data or the algorithm name is null, the key data
     *             is empty or <code>offset</code> and <code>len</code> do not
     *             specify a valid chunk in the buffer <code>key</code>.
     * @throws ArrayIndexOutOfBoundsException
     *             if <code>offset</code> or <code>len</code> is negative.
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<Byte> * key,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len,
        /* [in] */ const String& algorithm);

    /**
     * Returns the algorithm name.
     *
     * @return the algorithm name.
     */
    CARAPI GetAlgorithm(
        /* [out] */ String * value);

    /**
     * Returns the name of the format used to encode the key.
     *
     * @return the format name "RAW".
     */
    CARAPI GetFormat(
        /* [out] */ String * value);

    /**
     * Returns the encoded form of this secret key.
     *
     * @return the encoded form of this secret key.
     */
    CARAPI GetEncoded(
        /* [out, callee] */ ArrayOf<Byte> ** value);

    /**
     * Returns the hash code of this <code>SecretKeySpec</code> object.
     *
     * @return the hash code.
     */
    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    /**
     * Compares the specified object with this <code>SecretKeySpec</code>
     * instance.
     *
     * @param obj
     *            the object to compare.
     * @return true if the algorithm name and key of both object are equal,
     *         otherwise false.
     */
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    // The 5.0 spec. doesn't declare this serialVersionUID field
    // In order to be compatible it is explicitly declared here
    // for details see HARMONY-233
    static Int64 sSerialVersionUID; // = 6577238317307289933L;

    AutoPtr<ArrayOf<Byte> > mKey;
    String mAlgorithm;

};

}
}
}

#endif // __ELASTOSX_CRYPTO_SPEC_CSECRETKEYSPEC_H__
