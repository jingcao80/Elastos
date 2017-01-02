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

#ifndef __ELASTOS_SECURITY_CKEYREP_H__
#define __ELASTOS_SECURITY_CKEYREP_H__

#include "_Elastos_Security_CKeyRep.h"
#include "Object.h"

using Elastos::IO::IObjectInputStream;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

CarClass(CKeyRep)
    , public Object
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyRep();

    /**
     * Constructs a new instance of {@code KeyRep} with the specified arguments.
     * The arguments should be obtained from the {@code Key} object that has to
     * be serialized.
     *
     * @param type
     *            the type of the key.
     * @param algorithm
     *            the algorithm (obtained by {@link Key#getAlgorithm()}).
     * @param format
     *            the format of the key (obtained by {@link Key#getFormat()}).
     * @param encoded
     *            the encoded {@code byte[]} (obtained by
     *            {@link Key#getEncoded()}).
     * @throws NullPointerException
     *             if {@code type, algorithm, format or encoded} is {@code NULL}
     *             .
     */
    CARAPI constructor(
        /* [in] */ KeyRepType type,
        /* [in] */ const String& algorithm,
        /* [in] */ const String& format,
        /* [in] */ ArrayOf<Byte>* encoded);

protected:
    /**
     * Resolves and returns the {@code Key} object. Three {@link Type}|format
     * combinations are supported:
     * <ul>
     * <li> {@code Type.PRIVATE} | "PKCS#8" : returns a {@link PrivateKey}
     * instance, generated from a key factory (suitable for the algorithm) that
     * is initialized with a {@link PKCS8EncodedKeySpec} using the encoded key
     * bytes.
     * <li> {@code Type.SECRET} | "RAW" : returns a {@link SecretKeySpec}
     * instance, created with the encoded key bytes and the algorithm.
     * <li> {@code Type.PUBLIC} | "X.509": returns a {@link PublicKey} instance,
     * generated from a key factory (suitable for the algorithm) that is
     * initialized with a {@link X509EncodedKeySpec} using the encoded key
     * bytes.
     * </ul>
     *
     * @return the resolved {@code Key} object.
     * @throws ObjectStreamException
     *             if the {@code Type}|format combination is not recognized, or
     *             the resolution of any key parameter fails.
     */
    CARAPI ReadResolve(
        /* [out] */ IInterface** result) /*throws ObjectStreamException*/;

private:
    // Makes defensive copy of key encoding
    CARAPI ReadObject(
        /* [in] */ IObjectInputStream* is) /*throws IOException, ClassNotFoundException*/;

private:
    // private static final long serialVersionUID = -4757683898830641853L;
    // Key type
    KeyRepType mType;
    // Key algorithm name
    String mAlgorithm;
    // Key encoding format
    String mFormat;
    // Key encoding
    AutoPtr<ArrayOf<Byte> > mEncoded;
};

}
}

#endif // __ELASTOS_SECURITY_CKEYREP_H__
