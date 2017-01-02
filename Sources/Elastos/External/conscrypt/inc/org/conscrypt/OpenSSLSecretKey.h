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

#ifndef __ORG_CONSCRYPT_OPENSSLSECRETKEY_H__
#define __ORG_CONSCRYPT_OPENSSLSECRETKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IKey;
using Elastosx::Crypto::ISecretKey;
using Elastos::IO::IObjectInputStream;
using Elastos::IO::IObjectOutputStream;

namespace Org {
namespace Conscrypt {

class OpenSSLSecretKey
    : public Object
    , public IKey
    , public ISecretKey
    , public IOpenSSLSecretKey
    , public IOpenSSLKeyHolder
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ String algorithm,
        /* [in] */ ArrayOf<Byte>* encoded);

    CARAPI constructor(
        /* [in] */ String algorithm,
        /* [in] */ IOpenSSLKey* key);

    static CARAPI GetInstance(
        /* [in] */ ISecretKey* key,
        /* [out] */ IOpenSSLKey** result);

    CARAPI GetAlgorithm(
        /* [out] */ String* result);

    CARAPI GetFormat(
        /* [out] */ String* result);

    CARAPI GetEncoded(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetOpenSSLKey(
        /* [out] */ IOpenSSLKey** result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

private:
    void ReadObject(
        /* [in] */ IObjectInputStream* stream);

    void WriteObject(
        /* [in] */ IObjectOutputStream* stream);

private:
    static Int64 mSerialVersionUID;

    String mAlgorithm;
    Int32 mType;
    AutoPtr<ArrayOf<Byte> > mEncoded;

    AutoPtr<IOpenSSLKey> mKey;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLSECRETKEY_H__
