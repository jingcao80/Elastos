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

#ifndef __ORG_CONSCRYPT_OPENSSLMAC_H__
#define __ORG_CONSCRYPT_OPENSSLMAC_H__

#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include "elastosx/crypto/MacSpi.h"
#include <elastos/core/Object.h>

using Elastosx::Crypto::MacSpi;
using Elastos::Security::IKey;
using Elastos::Security::Spec::IAlgorithmParameterSpec;

namespace Org {
namespace Conscrypt {

class OpenSSLMac
    : public MacSpi //    : public MacSpi
    , public IOpenSSLMac
{
public :
    CAR_INTERFACE_DECL()

    CARAPI EngineGetMacLength(
        /* [out] */ Int32* result);

    CARAPI EngineInit(
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params);

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineDoFinal(
        /* [out, callee] */ ArrayOf<Byte>** input);

    CARAPI EngineReset();

    CARAPI constructor(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size,
        /* [in] */ Int32 evp_pkey_type);

    void ResetContext();

private:
    AutoPtr<IOpenSSLDigestContext> mCtx;

    /**
     * Holds the EVP_MD for the hashing algorithm, e.g.
     * EVP_get_digestbyname("sha1");
     */
    Int64 mEvp_md;

    /**
     * The key type of the secret key.
     */
    Int32 mEvp_pkey_type;

    /**
     * The secret key used in this keyed MAC.
     */
    AutoPtr<IOpenSSLKey> mMacKey;

    /**
     * Holds the output size of the message digest.
     */
    Int32 mSize;

    /**
     * Holds a dummy buffer for writing single bytes to the digest.
     */
    AutoPtr<ArrayOf<Byte> > mSingleByte;
};

class HmacMD5
    : public OpenSSLMac
{
public:
    HmacMD5();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class HmacSHA1
    : public OpenSSLMac
{
public:
    HmacSHA1();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class HmacSHA224
    : public OpenSSLMac
{
public:
    HmacSHA224();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class HmacSHA256
    : public OpenSSLMac
{
public:
    HmacSHA256();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class HmacSHA384
    : public OpenSSLMac
{
public:
    HmacSHA384();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class HmacSHA512
    : public OpenSSLMac
{
public:
    HmacSHA512();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLMAC_H__
