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

#ifndef __ELASTOS_SECURITY_CKEYPAIR_H__
#define __ELASTOS_SECURITY_CKEYPAIR_H__

#include "_Elastos_Security_CKeyPair.h"
#include "Object.h"

using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

CarClass(CKeyPair)
    , public Object
    , public IKeyPair
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Constructs a new instance of {@code KeyPair} with a public key and the
     * corresponding private key.
     *
     * @param publicKey
     *            the public key.
     * @param privateKey
     *            the private key.
     */
    CARAPI constructor(
        /* [in] */ IPublicKey* publicKey,
        /* [in] */ IPrivateKey* privateKey);

    /**
     * Returns the private key.
     *
     * @return the private key.
     */
    CARAPI GetPrivate(
        /* [out] */ IPrivateKey** result);

    /**
     * Returns the public key.
     *
     * @return the public key.
     */
    CARAPI GetPublic(
        /* [out] */ IPublicKey** result);

private:
    // private static final long serialVersionUID = -7565189502268009837L;
    AutoPtr<IPrivateKey> mPrivateKey;
    AutoPtr<IPublicKey> mPublicKey;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYPAIR_H__
