#include "org/conscrypt/CPlatform.h"
#include "org/conscrypt/Platform.h"

namespace Org {
namespace Conscrypt {

CAR_SINGLETON_IMPL(CPlatform)

CAR_INTERFACE_IMPL(CPlatform, Singleton, IPlatform)

ECode CPlatform::Setup()
{
    return Platform::Setup();
}

ECode CPlatform::GetFileDescriptor(
    /* [in] */ ISocket* s,
    /* [out] */ IFileDescriptor** result)
{
    return Platform::GetFileDescriptor(s, result);
}

ECode CPlatform::GetFileDescriptorFromSSLSocket(
    /* [in] */ IOpenSSLSocketImpl* openSSLSocketImpl,
    /* [out] */ IFileDescriptor** result)
{
    return Platform::GetFileDescriptorFromSSLSocket(openSSLSocketImpl, result);
}

ECode CPlatform::GetCurveName(
    /* [in] */ IECParameterSpec* spec,
    /* [out] */ String* result)
{
    return Platform::GetCurveName(spec, result);
}

ECode CPlatform::SetCurveName(
    /* [in] */ IECParameterSpec* spec,
    /* [in] */ const String& curveName)
{
    return Platform::SetCurveName(spec, curveName);
}

ECode CPlatform::SetSocketTimeout(
    /* [in] */ ISocket* s,
    /* [in] */ Int64 timeoutMillis)
{
    return Platform::SetSocketTimeout(s, timeoutMillis);
}

ECode CPlatform::SetEndpointIdentificationAlgorithm(
    /* [in] */ ISSLParameters* params,
    /* [in] */ const String& endpointIdentificationAlgorithm)
{
    return Platform::SetEndpointIdentificationAlgorithm(params, endpointIdentificationAlgorithm);
}

ECode CPlatform::GetEndpointIdentificationAlgorithm(
    /* [in] */ ISSLParameters* params,
    /* [out] */ String* result)
{
    return Platform::GetEndpointIdentificationAlgorithm(params, result);
}

ECode CPlatform::CheckServerTrusted(
    /* [in] */ IX509TrustManager* x509tm,
    /* [in] */ ArrayOf<IX509Certificate*>* chain,
    /* [in] */ const String& authType,
    /* [in] */ const String& host)
{
    return Platform::CheckServerTrusted(x509tm, chain, authType, host);
}

ECode CPlatform::WrapRsaKey(
    /* [in] */ IPrivateKey* javaKey,
    /* [out] */ IOpenSSLKey** result)
{
    return Platform::WrapRsaKey(javaKey, result);
}

ECode CPlatform::LogEvent(
    /* [in] */ const String& message)
{
    return Platform::LogEvent(message);
}

} // namespace Conscrypt
} // namespace Org
