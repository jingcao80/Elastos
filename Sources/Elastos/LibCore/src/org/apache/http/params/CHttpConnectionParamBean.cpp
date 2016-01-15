
#include "org/apache/http/params/CHttpConnectionParamBean.h"
#include "org/apache/http/params/HttpConnectionParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL(CHttpConnectionParamBean, Object, IHttpConnectionParamBean)

CAR_OBJECT_IMPL(CHttpConnectionParamBean)

ECode CHttpConnectionParamBean::SetSoTimeout(
    /* [in] */ Int32 soTimeout)
{
    return HttpConnectionParams::SetSoTimeout(mParams, soTimeout);
}

ECode CHttpConnectionParamBean::SetTcpNoDelay(
    /* [in] */ Boolean tcpNoDelay)
{
    return HttpConnectionParams::SetTcpNoDelay(mParams, tcpNoDelay);
}

ECode CHttpConnectionParamBean::SetSocketBufferSize(
    /* [in] */ Int32 socketBufferSize)
{
    return HttpConnectionParams::SetSocketBufferSize(mParams, socketBufferSize);
}

ECode CHttpConnectionParamBean::SetLinger(
    /* [in] */ Int32 linger)
{
    return HttpConnectionParams::SetLinger(mParams, linger);
}

ECode CHttpConnectionParamBean::SetConnectionTimeout(
    /* [in] */ Int32 connectionTimeout)
{
    return HttpConnectionParams::SetConnectionTimeout(mParams, connectionTimeout);
}

ECode CHttpConnectionParamBean::SetStaleCheckingEnabled(
    /* [in] */ Boolean staleCheckingEnabled)
{
    return HttpConnectionParams::SetStaleCheckingEnabled(mParams, staleCheckingEnabled);
}

ECode CHttpConnectionParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
