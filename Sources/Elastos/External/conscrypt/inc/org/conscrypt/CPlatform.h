
#ifndef __ORG_CONSCRYPT_CPLATFORM_H__
#define __ORG_CONSCRYPT_CPLATFORM_H__

#include "_Org_Conscrypt_CPlatform.h"
#include <elastos/core/Singleton.h>

using Elastos::IO::IFileDescriptor;
using Elastos::Net::ISocket;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Security::IPrivateKey;
using Elastos::Security::Spec::IECParameterSpec;
using Elastosx::Net::Ssl::ISSLParameters;
using Elastosx::Net::Ssl::IX509TrustManager;
// TODO:
// using Org::Conscrypt::IOpenSSLKey;
using Org::Conscrypt::IOpenSSLSocketImpl;

namespace Org {
namespace Conscrypt {

CarClass(CPlatform)
    , public Singleton
    , public IPlatform
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI Setup();

    CARAPI GetFileDescriptor(
        /* [in] */ ISocket* s,
        /* [out] */ IFileDescriptor** result);

    CARAPI GetFileDescriptorFromSSLSocket(
        /* [in] */ IOpenSSLSocketImpl* openSSLSocketImpl,
        /* [out] */ IFileDescriptor** result);

    CARAPI GetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [out] */ String* result);

    CARAPI SetCurveName(
        /* [in] */ IECParameterSpec* spec,
        /* [in] */ const String& curveName);

    CARAPI SetSocketTimeout(
        /* [in] */ ISocket* s,
        /* [in] */ Int64 timeoutMillis);

    CARAPI SetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [in] */ const String& endpointIdentificationAlgorithm);

    CARAPI GetEndpointIdentificationAlgorithm(
        /* [in] */ ISSLParameters* params,
        /* [out] */ String* result);

    CARAPI CheckServerTrusted(
        /* [in] */ IX509TrustManager* x509tm,
        /* [in] */ ArrayOf<IX509Certificate*>* chain,
        /* [in] */ const String& authType,
        /* [in] */ const String& host);

    /**
     * Wraps an old AndroidOpenSSL key instance. This is not needed on platform
     * builds since we didn't backport, so return null. This code is from
     * Chromium's net/android/java/src/org/chromium/net/DefaultAndroidKeyStore.java
     */
// TODO:
    // CARAPI WrapRsaKey(
    //     /* [in] */ IPrivateKey* javaKey,
    //     /* [out] */ IOpenSSLKey** result);

    /**
     * Logs to the system EventLog system.
     */
    CARAPI LogEvent(
        /* [in] */ const String& message);
};

} // namespace Conscrypt
} // namespace Org

#endif //__ORG_CONSCRYPT_CPLATFORM_H__
