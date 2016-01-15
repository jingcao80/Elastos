
#ifndef __ORG_APACHE_HTTP_IMPL_HTTPCONNECTIONMETRICSIMPL_H__
#define __ORG_APACHE_HTTP_IMPL_HTTPCONNECTIONMETRICSIMPL_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IHashMap;
using Org::Apache::Http::IHttpConnectionMetrics;
using Org::Apache::Http::IO::IHttpTransportMetrics;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Implementation of the metrics interface.
 */
class HttpConnectionMetricsImpl
    : public Object
    , public IHttpConnectionMetrics
{
public:
    HttpConnectionMetricsImpl(
        /* [in] */ IHttpTransportMetrics* inTransportMetric,
        /* [in] */ IHttpTransportMetrics* outTransportMetric);

    CAR_INTERFACE_DECL()

    CARAPI GetReceivedBytesCount(
        /* [out] */ Int64* count);

    CARAPI GetSentBytesCount(
        /* [out] */ Int64* count);

    CARAPI GetRequestCount(
        /* [out] */ Int64* count);

    CARAPI_(void) IncrementRequestCount();

    CARAPI GetResponseCount(
        /* [out] */ Int64* count);

    CARAPI_(void) IncrementResponseCount();

    CARAPI GetMetric(
        /* [in] */ const String& metricName,
        /* [out] */ IObject** metric);

    CARAPI SetMetric(
        /* [in] */ const String& metricName,
        /* [in] */ IObject* metric);

    CARAPI Reset();

public:
    static const String REQUEST_COUNT;
    static const String RESPONSE_COUNT;
    static const String SENT_BYTES_COUNT;
    static const String RECEIVED_BYTES_COUNT;

private:
    AutoPtr<IHttpTransportMetrics> mInTransportMetric;
    AutoPtr<IHttpTransportMetrics> mOutTransportMetric;
    Int64 mRequestCount;
    Int64 mResponseCount;

    /**
     * The cache map for all metrics values.
     */
    AutoPtr<IHashMap> mMetricsCache;
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_HTTPCONNECTIONMETRICSIMPL_H__
