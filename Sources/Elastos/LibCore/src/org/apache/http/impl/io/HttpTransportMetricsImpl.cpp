
#include "org/apache/http/impl/io/HttpTransportMetricsImpl.h"

using Org::Apache::Http::IO::EIID_IHttpTransportMetrics;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

HttpTransportMetricsImpl::HttpTransportMetricsImpl()
    : mBytesTransferred(0)
{}

CAR_INTERFACE_IMPL(HttpTransportMetricsImpl, Object, IHttpTransportMetrics)

ECode HttpTransportMetricsImpl::GetBytesTransferred(
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number)
    *number = mBytesTransferred;
    return NOERROR;
}

void HttpTransportMetricsImpl::SetBytesTransferred(
    /* [in] */ Int64 count)
{
    mBytesTransferred = count;
}

void HttpTransportMetricsImpl::IncrementBytesTransferred(
    /* [in] */ Int64 count)
{
    mBytesTransferred += count;
}

ECode HttpTransportMetricsImpl::Reset()
{
    mBytesTransferred = 0;
    return NOERROR;
}

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org