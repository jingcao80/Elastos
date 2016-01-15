
#include "org/apache/http/params/CHttpProtocolParams.h"
#include "org/apache/http/params/HttpProtocolParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL_2(CHttpProtocolParams, Object, IHttpProtocolParams, ICoreProtocolPNames)

CAR_SINGLETON_IMPL(CHttpProtocolParams)

ECode CHttpProtocolParams::GetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    return HttpProtocolParams::GetHttpElementCharset(params, charset);
}

ECode CHttpProtocolParams::SetHttpElementCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return HttpProtocolParams::SetHttpElementCharset(params, charset);
}

ECode CHttpProtocolParams::GetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    return HttpProtocolParams::GetContentCharset(params, charset);
}

ECode CHttpProtocolParams::SetContentCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return HttpProtocolParams::SetContentCharset(params, charset);
}

ECode CHttpProtocolParams::GetVersion(
    /* [in] */ IHttpParams* params,
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    return HttpProtocolParams::GetVersion(params, ver);
}

ECode CHttpProtocolParams::SetVersion(
    /* [in] */ IHttpParams* params,
    /* [in] */ IProtocolVersion* version)
{
    return HttpProtocolParams::SetVersion(params, version);
}

ECode CHttpProtocolParams::GetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* agent)
{
    VALIDATE_NOT_NULL(agent)
    return HttpProtocolParams::GetUserAgent(params, agent);
}

ECode CHttpProtocolParams::SetUserAgent(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& useragent)
{
    return HttpProtocolParams::SetUserAgent(params, useragent);
}

ECode CHttpProtocolParams::UseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HttpProtocolParams::UseExpectContinue(params, result);
}

ECode CHttpProtocolParams::SetUseExpectContinue(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean b)
{
    return HttpProtocolParams::SetUseExpectContinue(params, b);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
