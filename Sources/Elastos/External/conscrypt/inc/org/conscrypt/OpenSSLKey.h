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

#ifndef __ORG_CONSCRYPT_OPENSSLKEY_H__
#define __ORG_CONSCRYPT_OPENSSLKEY_H__

#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Security::IPrivateKey;
using Elastos::Security::IPublicKey;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::IPKCS8EncodedKeySpec;
using Elastosx::Crypto::ISecretKey;

namespace Org {
namespace Conscrypt {

class OpenSSLKey
    : public Object
    , public IOpenSSLKey
{
public:
    virtual ~OpenSSLKey();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI constructor(
        /* [in] */ Int64 ctx,
        /* [in] */ IOpenSSLEngine* engine,
        /* [in] */ const String& alias);

    /**
     * Returns the raw pointer to the EVP_PKEY context for use in JNI calls. The
     * life cycle of this native pointer is managed by the {@code OpenSSLKey}
     * instance and must not be destroyed or freed by users of this API.
     */
    CARAPI GetPkeyContext(
        /* [out] */ Int64* result);

    CARAPI GetEngine(
        /* [out] */ IOpenSSLEngine** result);

    CARAPI IsEngineBased(
        /* [out] */ Boolean* result);

    CARAPI GetAlias(
        /* [out] */ String* result);

    static AutoPtr<IOpenSSLKey> FromPrivateKey(
        /* [in] */ IPrivateKey* key);

    static AutoPtr<IOpenSSLKey> FromPublicKey(
        /* [in] */ IPublicKey* key);

    CARAPI GetPublicKey(
        /* [out] */ IPublicKey** result);

    static AutoPtr<IPublicKey> GetPublicKey(
        /* [in] */ IX509EncodedKeySpec* keySpec,
        /* [in] */ Int32 type);

    CARAPI GetPrivateKey(
        /* [out] */ IPrivateKey** result);

    static AutoPtr<IPrivateKey> GetPrivateKey(
        /* [in] */ IPKCS8EncodedKeySpec* keySpec,
        /* [in] */ Int32 type);

    CARAPI GetSecretKey(
        /* [in] */ const String& algorithm,
        /* [out] */ ISecretKey** result);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

private:
    static AutoPtr<IOpenSSLKey> WrapPrivateKey(
        /* [in] */ IPrivateKey* key);

private:
    Int64 mCtx;

    AutoPtr<IOpenSSLEngine> mEngine;

    String mAlias;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLKEY_H__