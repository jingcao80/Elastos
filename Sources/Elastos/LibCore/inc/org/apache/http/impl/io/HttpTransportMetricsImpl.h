
#ifndef __ORG_APACHE_HTTP_IMPL_IO_HTTPTRANSPORTMETRICSIMPL_H__
#define __ORG_APACHE_HTTP_IMPL_IO_HTTPTRANSPORTMETRICSIMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IO::IHttpTransportMetrics;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace IO {

/**
 * Default implementation of {@link HttpTransportMetrics}.
 */
class HttpTransportMetricsImpl
    : public Object
    , public IHttpTransportMetrics
{
public:
    HttpTransportMetricsImpl();

    CAR_INTERFACE_DECL()

    CARAPI GetBytesTransferred(
        /* [out] */ Int64* number);

    CARAPI_(void) SetBytesTransferred(
        /* [in] */ Int64 count);

    CARAPI_(void) IncrementBytesTransferred(
        /* [in] */ Int64 count);

    CARAPI Reset();

private:
    Int64 mBytesTransferred;
};

} // namespace IO
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_IO_HTTPTRANSPORTMETRICSIMPL_H__
