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

#include "_Elastos.Droid.Core.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include "_Org.Conscrypt.h"
#include "elastos/droid/net/http/CertificateChainValidator.h"
#include "elastos/droid/net/http/CCertificateChainValidator.h"
#include "elastos/droid/net/http/CDelegatingSSLSessionCertificateWrap.h"
#include "elastos/droid/net/http/CSslCertificate.h"
#include "elastos/droid/net/http/CSslError.h"
#include "elastos/droid/net/http/HttpLog.h"
#include "elastos/droid/net/http/HttpsConnection.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::ISlog;

using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::Net::CURI;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IKeyStore;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;
using Elastosx::Net::Ssl::CTrustManagerFactoryHelper;
using Elastosx::Net::Ssl::IHostnameVerifier;
using Elastosx::Net::Ssl::IHttpsURLConnection;
using Elastosx::Net::Ssl::IHttpsURLConnectionHelper;
using Elastosx::Net::Ssl::ISSLSession;
using Elastosx::Net::Ssl::ISSLSocket;
using Elastosx::Net::Ssl::ITrustManager;
using Elastosx::Net::Ssl::ITrustManagerFactory;
using Elastosx::Net::Ssl::ITrustManagerFactoryHelper;
using Elastosx::Net::Ssl::IX509TrustManager;

using Org::Conscrypt::CSSLParametersImplHelper;
using Org::Conscrypt::ISSLParametersImpl;
using Org::Conscrypt::ISSLParametersImplHelper;
using Org::Conscrypt::ITrustManagerImpl;

namespace Elastos {
namespace Droid {
namespace Net {
namespace Http {

//==============================================================================
// CertificateChainValidator::NoPreloadHolder
//==============================================================================
const AutoPtr<ICertificateChainValidator> CertificateChainValidator::NoPreloadHolder::sInstance = InitInstance();
const AutoPtr<IHostnameVerifier> CertificateChainValidator::NoPreloadHolder::sVerifier = InitVerifier();

AutoPtr<ICertificateChainValidator> CertificateChainValidator::NoPreloadHolder::InitInstance()
{
    AutoPtr<ICertificateChainValidator> ret;
    // TODO
    // CCertificateChainValidator::New((ICertificateChainValidator**)&ret);
    return ret;
}

AutoPtr<IHostnameVerifier> CertificateChainValidator::NoPreloadHolder::InitVerifier()
{
    AutoPtr<IHostnameVerifier> ret;
    AutoPtr<IHttpsURLConnectionHelper> huc;

    // TODO: Waiting for CHttpsURLConnectionHelper
    // assert(0);
    // CHttpsURLConnectionHelper::AcquireSingleton((IHttpsURLConnectionHelper**)&huc);
    // huc->GetDefaultHostnameVerifier(&ret);

    return ret;
}

//==============================================================================
// CertificateChainValidator
//==============================================================================
CAR_INTERFACE_IMPL(CertificateChainValidator, Object, ICertificateChainValidator)

const String CertificateChainValidator::TAG("CertificateChainValidator");

CertificateChainValidator::CertificateChainValidator()
{}

ECode CertificateChainValidator::GetInstance(
    /* [out] */ ICertificateChainValidator** result)
{
    VALIDATE_NOT_NULL(result)

    *result = NoPreloadHolder::sInstance;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CertificateChainValidator::constructor()
{
        // try {
    AutoPtr<ITrustManagerFactoryHelper> helper;
    CTrustManagerFactoryHelper::AcquireSingleton((ITrustManagerFactoryHelper**)&helper);
    AutoPtr<ITrustManagerFactory> tmf;
    ECode ec = helper->GetInstance(String("X.509"), (ITrustManagerFactory**)&tmf);
    if (FAILED(ec)) {
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            Logger::E(TAG, "X.509 TrustManagerFactory must be available %d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        if (ec == (ECode)E_KEY_STORE_EXCEPTION) {
            Logger::E(TAG, "X.509 TrustManagerFactory cannot be initialized %d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        return ec;
    }
    ec = tmf->Init((IKeyStore*) NULL);
    if (FAILED(ec)) {
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            Logger::E(TAG, "X.509 TrustManagerFactory must be available %d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        if (ec == (ECode)E_KEY_STORE_EXCEPTION) {
            Logger::E(TAG, "X.509 TrustManagerFactory cannot be initialized %d", ec);
            return E_RUNTIME_EXCEPTION;
        }
        return ec;
    }
    AutoPtr<ArrayOf<ITrustManager*> > trustManagers;
    tmf->GetTrustManagers((ArrayOf<ITrustManager*>**)&trustManagers);
    for (Int32 i = 0; i < trustManagers->GetLength(); ++i) {
        if (IX509TrustManager::Probe((*trustManagers)[i]) != NULL) {
            mTrustManager = IX509TrustManager::Probe((*trustManagers)[i]);
        }
    }
        // } catch (NoSuchAlgorithmException e) {
        // } catch (KeyStoreException e) {
        // }
    if (mTrustManager == NULL) {
        Logger::E(TAG, "None of the X.509 TrustManagers are X509TrustManager");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CertificateChainValidator::DoHandshakeAndValidateServerCertificates(
    /* [in] */ IHttpsConnection* connection,
    /* [in] */ ISSLSocket* sslSocket,
    /* [in] */ const String& domain,
    /* [out] */ ISslError** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // get a valid SSLSession, close the socket if we fail
    AutoPtr<ISSLSession> sslSession;
    sslSocket->GetSession((ISSLSession**)&sslSession);
    Boolean valid;
    if (sslSession->IsValid(&valid), !valid) {
        return CloseSocketThrowException(sslSocket, String("failed to perform SSL handshake"));
    }
    // retrieve the chain of the server peer certificates
    AutoPtr<ArrayOf<ICertificate*> > peerCertificates;
    sslSession->GetPeerCertificates((ArrayOf<ICertificate*>**)&peerCertificates);
    if (peerCertificates == NULL || peerCertificates->GetLength() == 0) {
        return CloseSocketThrowException(sslSocket, String("failed to retrieve peer certificates"));
    }
    else {
        // update the SSL certificate associated with the connection
        if (connection != NULL) {
            if ((*peerCertificates)[0] != NULL) {
                AutoPtr<ISslCertificate> newSslCertificate;
                CSslCertificate::New(IX509Certificate::Probe((*peerCertificates)[0]), (ISslCertificate**)&newSslCertificate);
                ((HttpsConnection*)connection)->SetCertificate(newSslCertificate);
            }
        }
    }

    AutoPtr<ArrayOf<IX509Certificate*> > x509Certificates = ArrayOf<IX509Certificate*>::Alloc(peerCertificates->GetLength());
    for (Int32 i = 0; i < peerCertificates->GetLength(); i++) {
        x509Certificates->Set(i, IX509Certificate::Probe((*peerCertificates)[i]));
    }
    return VerifyServerDomainAndCertificates(x509Certificates, domain, String("RSA"), result);
}

ECode CertificateChainValidator::VerifyServerCertificates(
    /* [in] */ ArrayOf<IArrayOf*>* certChain,
    /* [in] */ const String& domain,
    /* [in] */ const String& authType,
    /* [out] */ ISslError** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (certChain == NULL || certChain->GetLength() == 0) {
        Logger::E(TAG, "bad certificate chain");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ArrayOf<IX509Certificate*> > serverCertificates = ArrayOf<IX509Certificate*>::Alloc(certChain->GetLength());
    // try {
    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    AutoPtr<ICertificateFactory> cf;
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);
    for (Int32 i = 0; i < certChain->GetLength(); ++i) {
        AutoPtr<IByteArrayInputStream> newByteArrayInputStream;
        AutoPtr<IArrayOf> oldArray = (*certChain)[i];
        Int32 length;
        oldArray->GetLength(&length);
        AutoPtr<ArrayOf<Byte> > newArray = ArrayOf<Byte>::Alloc(length);
        for (Int32 j = 0; j < newArray->GetLength(); ++j) {
            AutoPtr<IInterface> obj;
            oldArray->Get(j, (IInterface**)&obj);
            Byte b;
            IByte::Probe(obj)->GetValue(&b);
            newArray->Set(j, b);
        }
        CByteArrayInputStream::New(newArray, (IByteArrayInputStream**)&newByteArrayInputStream);
        AutoPtr<ICertificate> certificate;
        ECode ec = cf->GenerateCertificate(IInputStream::Probe(newByteArrayInputStream), (ICertificate**)&certificate);
        if (FAILED(ec)) {
            if (ec == (ECode)E_CERTIFICATE_EXCEPTION) {
                Logger::E(TAG, "can't read certificate %d", ec);
                return E_IO_EXCEPTION;
            }
            return ec;
        }
        serverCertificates->Set(i, IX509Certificate::Probe(certificate));
    }
    // } catch (CertificateException e) {
    // }
    return VerifyServerDomainAndCertificates(serverCertificates, domain, authType, result);
}

ECode CertificateChainValidator::HandleTrustStorageUpdate()
{
    AutoPtr<ITrustManagerFactory> tmf;
    // try {
    AutoPtr<ITrustManagerFactoryHelper> helper;
    CTrustManagerFactoryHelper::AcquireSingleton((ITrustManagerFactoryHelper**)&helper);
    ECode ec = helper->GetInstance(String("X.509"), (ITrustManagerFactory**)&tmf);
    if (FAILED(ec)) {
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            Slogger::W(TAG, "Couldn't find default X.509 TrustManagerFactory");
            return NOERROR;
        }
        if (ec == (ECode)E_KEY_STORE_EXCEPTION) {
            Slogger::W(TAG, "Couldn't initialize default X.509 TrustManagerFactory %d", ec);
            return NOERROR;
        }
        return ec;
    }
    ec = tmf->Init((IKeyStore*) NULL);
    if (FAILED(ec)) {
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            Slogger::W(TAG, "Couldn't find default X.509 TrustManagerFactory");
            return NOERROR;
        }
        if (ec == (ECode)E_KEY_STORE_EXCEPTION) {
            Slogger::W(TAG, "Couldn't initialize default X.509 TrustManagerFactory %d", ec);
            return NOERROR;
        }
        return ec;
    }
    // } catch (NoSuchAlgorithmException e) {
    // } catch (KeyStoreException e) {
    // }
    AutoPtr<ArrayOf<ITrustManager*> > tms;
    tmf->GetTrustManagers((ArrayOf<ITrustManager*>**)&tms);
    Boolean sentUpdate = FALSE;
    for (Int32 i = 0; i < tms->GetLength(); ++i) {
        AutoPtr<ITrustManager> tm = (*tms)[i];
        // try {
        // TODO: Waiting for method to get method from object
        assert(0);
            // Method updateMethod = tm.getClass().getDeclaredMethod("handleTrustStorageUpdate");
            // updateMethod.setAccessible(TRUE);
            // updateMethod.invoke(tm);
            // sentUpdate = TRUE;
        // } catch (Exception e) {
        // }
    }
    if (!sentUpdate) {
        Slogger::W(TAG, "Didn't find a TrustManager to handle CA list update");
    }
    return NOERROR;
}

ECode CertificateChainValidator::VerifyServerDomainAndCertificates(
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& domain,
    /* [in] */ const String& authType,
    /* [out] */ ISslError** result)
{
    VALIDATE_NOT_NULL(result);

    // check if the first certificate in the chain is for this site
    AutoPtr<IX509Certificate> currCertificate = (*chain)[0];
    if (currCertificate == NULL) {
        Logger::E(TAG, "certificate for this site is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean verifier;
    AutoPtr<IDelegatingSSLSessionCertificateWrap> newDelegatingSSLSessionCertificateWrap;
    CDelegatingSSLSessionCertificateWrap::New(ICertificate::Probe(currCertificate), (IDelegatingSSLSessionCertificateWrap**)&newDelegatingSSLSessionCertificateWrap);
    NoPreloadHolder::sVerifier->Verify(domain, ISSLSession::Probe(newDelegatingSSLSessionCertificateWrap), &verifier);
    Boolean valid = !domain.IsNullOrEmpty() && verifier;
    if (!valid) {
        if (HttpLog::LOGV) {
            HttpLog::V("certificate not for this host: %s", domain.string());
        }
        CSslError::New(ISslError::SSL_IDMISMATCH, currCertificate, result);
        return NOERROR;
    }

    AutoPtr<ISSLParametersImplHelper> helper;
    CSSLParametersImplHelper::AcquireSingleton((ISSLParametersImplHelper**)&helper);
    AutoPtr<IX509TrustManager> x509TrustManager;
    ECode ec;
    ec = helper->GetDefaultX509TrustManager((IX509TrustManager**)&x509TrustManager);
    if(FAILED(ec)) {
        if (ec == (ECode)E_GENERAL_SECURITY_EXCEPTION) {
            if (HttpLog::LOGV) {
                HttpLog::V("failed to validate the certificate chain, error: %d", ec);
            }
            CSslError::New(ISslError::SSL_UNTRUSTED, currCertificate, result);
            return NOERROR;
        }
        return ec;
    }
    AutoPtr<ITrustManagerImpl> tmImpl = ITrustManagerImpl::Probe(x509TrustManager);
    if (tmImpl != NULL) {
        // TODO: Waiting for ITrustManagerImpl
        assert(0);
        // ec = tmImpl->CheckServerTrusted(chain, authType, domain);
    } else {
        ec = x509TrustManager->CheckServerTrusted(chain, authType);
    }
    if(FAILED(ec)) {
        if (ec == (ECode)E_GENERAL_SECURITY_EXCEPTION) {
            if (HttpLog::LOGV) {
                HttpLog::V("failed to validate the certificate chain, error: %d", ec);
            }
            CSslError::New(ISslError::SSL_UNTRUSTED, currCertificate, result);
            return NOERROR;
        }
        return ec;
    }
    *result = NULL;
    return NOERROR;
}

ECode CertificateChainValidator::GetTrustManager(
    /* [out] */ IX509TrustManager** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mTrustManager;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CertificateChainValidator::CloseSocketThrowException(
    /* [in] */ ISSLSocket* socket,
    /* [in] */ const String& errorMessage,
    /* [in] */ const String& defaultErrorMessage)
{
    return CloseSocketThrowException(
            socket, !errorMessage.IsNullOrEmpty() ? errorMessage : defaultErrorMessage);
}

ECode CertificateChainValidator::CloseSocketThrowException(
    /* [in] */ ISSLSocket* socket,
    /* [in] */ const String& errorMessage)
{
    if (HttpLog::LOGV) {
        HttpLog::V("validation error: %s", errorMessage.string());
    }
    if (socket != NULL) {
        AutoPtr<ISSLSession> session;
        socket->GetSession((ISSLSession**)&session);
        if (session != NULL) {
            session->Invalidate();
        }
        ICloseable::Probe(socket)->Close();
    }
    Logger::E(TAG, errorMessage.string());
    return E_SSL_HANDSHAKE_EXCEPTION;
}

} // namespace Http
} // namespace Net
} // namespace Droid
} // namespace Elastos
