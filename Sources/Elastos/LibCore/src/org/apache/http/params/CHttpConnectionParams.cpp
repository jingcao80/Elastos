
#include "org/apache/http/params/CHttpConnectionParams.h"
#include "org/apache/http/params/HttpConnectionParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL_2(CHttpConnectionParams, Object, IHttpConnectionParams, ICoreConnectionPNames)

CAR_SINGLETON_IMPL(CHttpConnectionParams)

ECode CHttpConnectionParams::GetSoTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    return HttpConnectionParams::GetSoTimeout(params, timeout);
}

ECode CHttpConnectionParams::SetSoTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 timeout)
{
    return HttpConnectionParams::SetSoTimeout(params, timeout);
}

ECode CHttpConnectionParams::GetTcpNoDelay(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HttpConnectionParams::GetTcpNoDelay(params, result);
}

ECode CHttpConnectionParams::SetTcpNoDelay(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    return HttpConnectionParams::SetSoTimeout(params, value);
}

ECode CHttpConnectionParams::GetSocketBufferSize(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    return HttpConnectionParams::GetSocketBufferSize(params, size);
}

ECode CHttpConnectionParams::SetSocketBufferSize(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 size)
{
    return HttpConnectionParams::SetSoTimeout(params, size);
}

ECode CHttpConnectionParams::GetLinger(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* linger)
{
    VALIDATE_NOT_NULL(linger)
    return HttpConnectionParams::GetLinger(params, linger);
}

ECode CHttpConnectionParams::SetLinger(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 value)
{
    return HttpConnectionParams::SetLinger(params, value);
}

ECode CHttpConnectionParams::GetConnectionTimeout(
    /* [in] */ IHttpParams* params,
    /* [out] */ Int32* timeout)
{
    VALIDATE_NOT_NULL(timeout)
    return HttpConnectionParams::GetConnectionTimeout(params, timeout);
}

ECode CHttpConnectionParams::SetConnectionTimeout(
    /* [in] */ IHttpParams* params,
    /* [in] */ Int32 timeout)
{
    return HttpConnectionParams::SetConnectionTimeout(params, timeout);
}

ECode CHttpConnectionParams::IsStaleCheckingEnabled(
    /* [in] */ IHttpParams* params,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return HttpConnectionParams::IsStaleCheckingEnabled(params, result);
}

ECode CHttpConnectionParams::SetStaleCheckingEnabled(
    /* [in] */ IHttpParams* params,
    /* [in] */ Boolean value)
{
    return HttpConnectionParams::SetStaleCheckingEnabled(params, value);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
