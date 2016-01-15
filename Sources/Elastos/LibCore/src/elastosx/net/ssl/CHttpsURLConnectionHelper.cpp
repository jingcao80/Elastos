
#include "CHttpsURLConnectionHelper.h"
#include "HttpsURLConnection.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CHttpsURLConnectionHelper, Singleton, IHttpsURLConnectionHelper)

CAR_SINGLETON_IMPL(CHttpsURLConnectionHelper)

ECode CHttpsURLConnectionHelper::SetDefaultHostnameVerifier(
    /* [in] */ IHostnameVerifier* v)
{
    return HttpsURLConnection::SetDefaultHostnameVerifier(v);
}

ECode CHttpsURLConnectionHelper::GetDefaultHostnameVerifier(
    /* [out] */ IHostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)

    return HttpsURLConnection::GetDefaultHostnameVerifier(verifier);
}

ECode CHttpsURLConnectionHelper::SetDefaultSSLSocketFactory(
    /* [in] */ ISSLSocketFactory* sf)
{
    return HttpsURLConnection::SetDefaultSSLSocketFactory(sf);
}

ECode CHttpsURLConnectionHelper::GetDefaultSSLSocketFactory(
    /* [out] */ ISSLSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return HttpsURLConnection::GetDefaultSSLSocketFactory(factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
