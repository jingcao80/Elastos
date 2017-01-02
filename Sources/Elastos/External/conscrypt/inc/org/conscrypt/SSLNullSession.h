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

#ifndef __ORG_CONSCRYPT_SSLNULLSESSION_H__
#define __ORG_CONSCRYPT_SSLNULLSESSION_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Extensions.h"
#include "_Org.Conscrypt.h"
#include <elastos/core/Object.h>

using Elastos::Core::ICloneable;
using Elastos::Security::IPrincipal;
using Elastos::Security::Cert::ICertificate;
using Elastos::Utility::IHashMap;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSessionContext;

namespace Org {
namespace Conscrypt {

class SSLNullSession
    : public Object
    , public ISSLNullSession
    , public ISSLSession
    , public ICloneable
{
private:
    /*
     * Holds default instances so class preloading doesn't create an instance of
     * it.
     */
    class DefaultHolder
    {
    public:
        static AutoPtr<ISSLNullSession> NULL_SESSION;
    };

public:
    CAR_INTERFACE_DECL()

    static AutoPtr<ISSLSession> GetNullSession();

    CARAPI constructor();

    CARAPI GetApplicationBufferSize(
        /* [out] */ Int32* result);

    CARAPI GetCipherSuite(
        /* [out] */ String* result);

    CARAPI GetCreationTime(
        /* [out] */ Int64* result);

    CARAPI GetId(
        /* [out] */ ArrayOf<Byte>** result);

    CARAPI GetLastAccessedTime(
        /* [out] */ Int64* result);

    CARAPI GetLocalCertificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    CARAPI GetLocalPrincipal(
        /* [out] */ IPrincipal** result);

    CARAPI GetPacketBufferSize(
        /* [out] */ Int32* result);

    CARAPI GetPeerCertificateChain(
        /* [out] */ ArrayOf<Elastosx::Security::Cert::IX509Certificate*>** result);

    CARAPI GetPeerCertificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    CARAPI GetPeerHost(
        /* [out] */ String* result);

    CARAPI GetPeerPort(
        /* [out] */ Int32* result);

    CARAPI GetPeerPrincipal(
        /* [out] */ IPrincipal** result);

    CARAPI GetProtocol(
        /* [out] */ String* result);

    CARAPI GetSessionContext(
        /* [out] */ ISSLSessionContext** result);

    CARAPI GetValue(
        /* [in] */ const String& name,
        /* [out] */ IInterface** result);

    CARAPI GetValueNames(
        /* [out] */ ArrayOf<String>** result);

    CARAPI Invalidate();

    CARAPI IsValid(
        /* [out] */ Boolean* result);

    CARAPI PutValue(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value);

    CARAPI RemoveValue(
        /* [in] */ const String& name);

    CARAPI Clone(
        /* [out] */ IInterface** result);

public:
    Int64 mCreationTime;
    Int64 mLastAccessedTime;

private:
    AutoPtr<IHashMap> mValues;
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_SSLNULLSESSION_H__