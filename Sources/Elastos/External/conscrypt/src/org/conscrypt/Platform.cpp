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

#include "org/conscrypt/Platform.h"

using Elastos::Security::Cert::ICertificate;

namespace Org {
namespace Conscrypt {

const String Platform::TAG("Conscrypt");
// TODO: Need Method
// Method Platform::m_getCurveName;

Boolean Platform::InitStatic()
{
// TODO:
    // try {
    //     m_getCurveName = ECParameterSpec.class.getDeclaredMethod("getCurveName");
    //     m_getCurveName.setAccessible(true);
    // } catch (Exception ignored) {
    // }
    return TRUE;
}

Boolean Platform::sInit = InitStatic();

ECode Platform::Setup()
{
    return NOERROR;
}

ECode Platform::GetFileDescriptor(
    /* [in] */ ISocket* s,
    /* [out] */ IFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // try {
    //     Field f_impl = Socket.class.getDeclaredField("impl");
    //     f_impl.setAccessible(true);
    //     Object socketImpl = f_impl.get(s);
    //     Class<?> c_socketImpl = Class.forName("java.net.SocketImpl");
    //     Field f_fd = c_socketImpl.getDeclaredField("fd");
    //     f_fd.setAccessible(true);
    //     return (FileDescriptor) f_fd.get(socketImpl);
    // } catch (Exception e) {
    //     throw new RuntimeException("Can't get FileDescriptor from socket", e);
    // }
    return NOERROR;
}

ECode Platform::GetFileDescriptorFromSSLSocket(
    /* [in] */ IOpenSSLSocketImpl* openSSLSocketImpl,
    /* [out] */ IFileDescriptor** result)
{
    VALIDATE_NOT_NULL(result)
// TODO:
    // return GetFileDescriptor(ISocket::Probe(openSSLSocketImpl), result);
    return NOERROR;
}

ECode Platform::GetCurveName(
    /* [in] */ IECParameterSpec* spec,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // if (m_getCurveName == NULL) {
    //     return NULL;
    // }
    // try {
    //     return (String) m_getCurveName.invoke(spec);
    // } catch (Exception e) {
    //     return NULL;
    // }
    return NOERROR;
}

ECode Platform::SetCurveName(
    /* [in] */ IECParameterSpec* spec,
    /* [in] */ const String& curveName)
{
    //assert(0);
    // try {
    //     Method setCurveName = spec.getClass().getDeclaredMethod("setCurveName", String.class);
    //     setCurveName.invoke(spec, curveName);
    // } catch (Exception ignored) {
    // }
    return spec->SetCurveName(curveName);
}

ECode Platform::SetSocketTimeout(
    /* [in] */ ISocket* s,
    /* [in] */ Int64 timeoutMillis)
{
    // TODO: implement this for unbundled
    return NOERROR;
}

ECode Platform::SetEndpointIdentificationAlgorithm(
    /* [in] */ ISSLParameters* params,
    /* [in] */ const String& endpointIdentificationAlgorithm)
{
    // TODO: implement this for unbundled
    return NOERROR;
}

ECode Platform::GetEndpointIdentificationAlgorithm(
    /* [in] */ ISSLParameters* params,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    // TODO: implement this for unbundled
    *result = NULL;
    return NOERROR;
}

ECode Platform::CheckServerTrusted(
    /* [in] */ IX509TrustManager* x509tm,
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    // TODO: use reflection to find whether we have TrustManagerImpl
    /*
    if (x509tm instanceof TrustManagerImpl) {
        TrustManagerImpl tm = (TrustManagerImpl) x509tm;
        tm.checkServerTrusted(chain, authType, host);
    } else {
    */
    return x509tm->CheckServerTrusted(chain, authType);
    /*
    }
    */
}

ECode Platform::WrapRsaKey(
    /* [in] */ IPrivateKey* javaKey,
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0);
    // // This fixup only applies to pre-JB-MR1
    // if (Build.VERSION.SDK_INT >= 17) {
    //     return NULL;
    // }

    // // First, check that this is a proper instance of OpenSSLRSAPrivateKey
    // // or one of its sub-classes.
    // Class<?> superClass;
    // try {
    //     superClass = Class
    //             .forName("org.apache.harmony.xnet.provider.jsse.OpenSSLRSAPrivateKey");
    // } catch (Exception e) {
    //     // This may happen if the target device has a completely different
    //     // implementation of the java.security APIs, compared to vanilla
    //     // Android. Highly unlikely, but still possible.
    //     Log.e(TAG, "Cannot find system OpenSSLRSAPrivateKey class: " + e);
    //     return NULL;
    // }
    // if (!superClass.isInstance(javaKey)) {
    //     // This may happen if the PrivateKey was not created by the
    //     // "AndroidOpenSSL"
    //     // provider, which should be the default. That could happen if an
    //     // OEM decided
    //     // to implement a different default provider. Also highly unlikely.
    //     Log.e(TAG, "Private key is not an OpenSSLRSAPrivateKey instance, its class name is:"
    //             + javaKey.getClass().getCanonicalName());
    //     return NULL;
    // }

    // try {
    //     // Use reflection to invoke the 'getOpenSSLKey()' method on
    //     // the private key. This returns another Java object that wraps
    //     // a native EVP_PKEY. Note that the method is final, so calling
    //     // the superclass implementation is ok.
    //     Method getKey = superClass.getDeclaredMethod("getOpenSSLKey");
    //     getKey.setAccessible(true);
    //     Object opensslKey = NULL;
    //     try {
    //         opensslKey = getKey.invoke(javaKey);
    //     } finally {
    //         getKey.setAccessible(false);
    //     }
    //     if (opensslKey == NULL) {
    //         // Bail when detecting OEM "enhancement".
    //         Log.e(TAG, "Could not getOpenSSLKey on instance: " + javaKey.toString());
    //         return NULL;
    //     }

    //     // Use reflection to invoke the 'getPkeyContext' method on the
    //     // result of the getOpenSSLKey(). This is an 32-bit integer
    //     // which is the address of an EVP_PKEY object. Note that this
    //     // method these days returns a 64-bit long, but since this code
    //     // path is used for older Android versions, it may still return
    //     // a 32-bit int here. To be on the safe side, we cast the return
    //     // value via Number rather than directly to Integer or Long.
    //     Method getPkeyContext;
    //     try {
    //         getPkeyContext = opensslKey.getClass().getDeclaredMethod("getPkeyContext");
    //     } catch (Exception e) {
    //         // Bail here too, something really not working as expected.
    //         Log.e(TAG, "No getPkeyContext() method on OpenSSLKey member:" + e);
    //         return NULL;
    //     }
    //     getPkeyContext.setAccessible(true);
    //     long evp_pkey = 0;
    //     try {
    //         evp_pkey = ((Number) getPkeyContext.invoke(opensslKey)).longValue();
    //     } finally {
    //         getPkeyContext.setAccessible(false);
    //     }
    //     if (evp_pkey == 0) {
    //         // The PrivateKey is probably rotten for some reason.
    //         Log.e(TAG, "getPkeyContext() returned NULL");
    //         return NULL;
    //     }
    //     return new OpenSSLKey(evp_pkey);
    // } catch (Exception e) {
    //     Log.e(TAG, "Error during conversion of privatekey instance: " + javaKey.toString(), e);
    //     return NULL;
    // }
    return NOERROR;
}

ECode Platform::LogEvent(
    /* [in] */ const String& message)
{
    assert(0);
    // try {
    //     Class processClass = Class.forName("android.os.Process");
    //     Object processInstance = processClass.newInstance();
    //     Method myUidMethod = processClass.getMethod("myUid", (Class[]) NULL);
    //     int uid = (Integer) myUidMethod.invoke(processInstance);

    //     Class eventLogClass = Class.forName("android.util.EventLog");
    //     Object eventLogInstance = eventLogClass.newInstance();
    //     Method writeEventMethod = eventLogClass.getMethod("writeEvent",
    //             new Class[] { Integer.TYPE, Object[].class });
    //     writeEventMethod.invoke(eventLogInstance, 0x534e4554 /* SNET */,
    //             new Object[] { "conscrypt", uid, message });
    // } catch (Exception e) {
    //     // Fail silently
    // }
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
