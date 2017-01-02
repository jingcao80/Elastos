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

#ifndef __ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__
#define __ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__

#include "Org.Conscrypt.h"
#include <elastos/core/Object.h>
#include "elastos/security/MessageDigestSpi.h"

using Elastos::Core::Object;
using Elastos::Core::ICloneable;
using Elastos::Security::MessageDigestSpi;

namespace Org {
namespace Conscrypt {

/**
 * Implements the JDK MessageDigest interface using OpenSSL's EVP API.
 */
class OpenSSLMessageDigestJDK
    : public MessageDigestSpi
    , public IOpenSSLMessageDigestJDK
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI EngineReset();

    CARAPI EngineGetDigestLength(
        /* [out] */ Int32* len);

    CARAPI EngineUpdate(
        /* [in] */ Byte input);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 len);

    CARAPI EngineDigest(
        /* [out, callee] */ ArrayOf<Byte>** ed);

    /**
     * Creates a new OpenSSLMessageDigest instance for the given algorithm name.
     */
    CARAPI constructor(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size);

    CARAPI constructor(
        /* [in] */ Int64 evp_md,
        /* [in] */ Int32 size,
        /* [in] */ IOpenSSLDigestContext* ctx);

    void ResetContext();

private:
    AutoPtr<IOpenSSLDigestContext> mCtx;

    /**
     * Holds the EVP_MD for the hashing algorithm, e.g. EVP_get_digestbyname("sha1");
     */
    Int64 mEvp_md;

    /**
     * Holds the output size of the message digest.
     */
    Int32 mSize;

    /**
     * Holds a dummy buffer for writing single bytes to the digest.
     */
    AutoPtr<ArrayOf<Byte> > mSingleByte;
};

class OpenSSLMessageDigestJDKMD5
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKMD5
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class OpenSSLMessageDigestJDKSHA1
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKSHA1
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class OpenSSLMessageDigestJDKSHA224
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKSHA224
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class OpenSSLMessageDigestJDKSHA256
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKSHA256
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class OpenSSLMessageDigestJDKSHA384
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKSHA384
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

class OpenSSLMessageDigestJDKSHA512
    : public OpenSSLMessageDigestJDK
    , public IOpenSSLMessageDigestJDKSHA512
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

public:
    static Int64 EVP_MD;
    static Int32 SIZE;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_OPENSSLMESSAGEDIGESTJDK_H__