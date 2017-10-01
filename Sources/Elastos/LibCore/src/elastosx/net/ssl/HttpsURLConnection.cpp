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

#include "Elastos.CoreLibrary.Security.h"
#include "HttpsURLConnection.h"
#include "HttpURLConnection.h"
#include "CDefaultHostnameVerifier.h"
#include "SSLSocketFactory.h"

using Elastos::Net::EIID_IURLConnection;
using Elastos::Net::EIID_IHttpURLConnection;
using Elastos::Net::HttpURLConnection;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::Cert::EIID_IX509Certificate;
using Elastosx::Security::Auth::X500::IX500Principal;
using Elastosx::Net::Ssl::CDefaultHostnameVerifier;

namespace Elastosx {
namespace Net {
namespace Ssl {

static AutoPtr<IHostnameVerifier> InitDefaultHostnameVerifier()
{
    AutoPtr<IHostnameVerifier> nameVerifier;
    CDefaultHostnameVerifier::New((IHostnameVerifier**)&nameVerifier);
    return nameVerifier;
}

static AutoPtr<ISSLSocketFactory> InitDefaultSSLSocketFactory()
{
    AutoPtr<ISocketFactory> factory;
    SSLSocketFactory::GetDefault((ISocketFactory**)&factory);
    return ISSLSocketFactory::Probe(factory);
}

AutoPtr<IHostnameVerifier> HttpsURLConnection::NoPreloadHolder::sDefaultHostnameVerifier = InitDefaultHostnameVerifier();

AutoPtr<ISSLSocketFactory> HttpsURLConnection::NoPreloadHolder::sDefaultSSLSocketFactory = InitDefaultSSLSocketFactory();

CAR_INTERFACE_IMPL(HttpsURLConnection, HttpURLConnection, IHttpsURLConnection)

ECode HttpsURLConnection::SetDefaultHostnameVerifier(
    /* [in] */ IHostnameVerifier* v)
{
    if (v == NULL) {
        //throw new IllegalArgumentException("HostnameVerifier is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NoPreloadHolder::sDefaultHostnameVerifier = v;
    return NOERROR;
}

ECode HttpsURLConnection::GetDefaultHostnameVerifier(
    /* [out] */ IHostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)

    *verifier = NoPreloadHolder::sDefaultHostnameVerifier;
    REFCOUNT_ADD(*verifier);
    return NOERROR;
}


ECode HttpsURLConnection::SetDefaultSSLSocketFactory(
    /* [in] */ ISSLSocketFactory* sf)
{
    if (sf == NULL) {
        //throw new IllegalArgumentException("SSLSocketFactory is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NoPreloadHolder::sDefaultSSLSocketFactory = sf;
    return NOERROR;
}

ECode HttpsURLConnection::GetDefaultSSLSocketFactory(
    /* [out] */ ISSLSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    *factory = NoPreloadHolder::sDefaultSSLSocketFactory;
    REFCOUNT_ADD(*factory);
    return NOERROR;
}

ECode HttpsURLConnection::constructor(
    /* [in] */ IURL* url)
{
    FAIL_RETURN(HttpURLConnection::constructor(url))
    mHostnameVerifier = NoPreloadHolder::sDefaultHostnameVerifier;
    mSslSocketFactory = NoPreloadHolder::sDefaultSSLSocketFactory;
    return NOERROR;
}

ECode HttpsURLConnection::GetPeerPrincipal(
    /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)

    AutoPtr<ArrayOf<AutoPtr<ICertificate> > > certs;
    FAIL_RETURN(GetServerCertificates((ArrayOf<ICertificate*>**)&certs))
    if (certs == NULL || certs->GetLength() == 0 || (((*certs)[0]->Probe(EIID_IX509Certificate) == NULL))) {
        //throw new SSLPeerUnverifiedException("No server's end-entity certificate");
        return E_SSL_PEER_UNVERIFIED_EXCEPTION;
    }

    AutoPtr<IX500Principal> prin;
    ((IX509Certificate*)(*certs)[0]->Probe(EIID_IX509Certificate))->GetSubjectX500Principal((IX500Principal**)&prin);
    *principal = IPrincipal::Probe(prin);
    REFCOUNT_ADD(*principal);
    return NOERROR;
}

ECode HttpsURLConnection::GetLocalPrincipal(
   /* [out] */ IPrincipal** principal)
{
    VALIDATE_NOT_NULL(principal)

    AutoPtr<ArrayOf<AutoPtr<ICertificate> > > certs;
    FAIL_RETURN(GetLocalCertificates((ArrayOf<ICertificate*>**)&certs))
    if (certs == NULL || certs->GetLength() == 0 || (((*certs)[0]->Probe(EIID_IX509Certificate) == NULL))) {
        *principal = NULL;
        return NOERROR;
    }

    AutoPtr<IX500Principal> prin;
    ((IX509Certificate*)(*certs)[0]->Probe(EIID_IX509Certificate))->GetSubjectX500Principal((IX500Principal**)&prin);
    *principal = IPrincipal::Probe(prin);
    REFCOUNT_ADD(*principal);
    return NOERROR;
}

ECode HttpsURLConnection::SetHostnameVerifier(
    /* [in] */ IHostnameVerifier* v)
{
    if (v == NULL) {
        //throw new IllegalArgumentException("HostnameVerifier is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mHostnameVerifier = v;
    return NOERROR;
}

ECode HttpsURLConnection::GetHostnameVerifier(
    /* [out] */ IHostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)

    *verifier = mHostnameVerifier;
    REFCOUNT_ADD(*verifier);
    return NOERROR;
}

ECode HttpsURLConnection::SetSSLSocketFactory(
    /* [in] */ ISSLSocketFactory* sf)
{
    if (sf == NULL) {
        //throw new IllegalArgumentException("SSLSocketFactory is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSslSocketFactory = sf;
    return NOERROR;
}

ECode HttpsURLConnection::GetSSLSocketFactory(
    /* [out] */ ISSLSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    *factory = mSslSocketFactory;
    REFCOUNT_ADD(*factory);
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
