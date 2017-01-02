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
