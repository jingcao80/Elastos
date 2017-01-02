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

#ifndef __ORG_CONSCRYPT_X509PUBLICKEY_H__
#define __ORG_CONSCRYPT_X509PUBLICKEY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPublicKey;

namespace Org {
namespace Conscrypt {

/**
 * A simple but useless key class that holds X.509 public key information when
 * the appropriate KeyFactory for the key algorithm is not available.
 */
class X509PublicKey
    : public Object
    , public IPublicKey
    , public IX509PublicKey
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ String algorithm,
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

private:
    static Int64 mSerialVersionUID;

    String mAlgorithm;

    AutoPtr<ArrayOf<Byte> > mEncoded;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_X509PUBLICKEY_H__