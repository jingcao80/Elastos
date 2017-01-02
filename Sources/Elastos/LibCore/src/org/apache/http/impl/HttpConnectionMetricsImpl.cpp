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

#include "org/apache/http/impl/HttpConnectionMetricsImpl.h"
#include "elastos/core/CString.h"
#include "elastos/core/CInteger64.h"
#include "elastos/utility/CHashMap.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IInteger64;
using Elastos::Core::CInteger64;
using Elastos::Utility::CHashMap;
using Elastos::Utility::IMap;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

const String HttpConnectionMetricsImpl::REQUEST_COUNT("http.request-count");
const String HttpConnectionMetricsImpl::RESPONSE_COUNT("http.response-count");
const String HttpConnectionMetricsImpl::SENT_BYTES_COUNT("http.sent-bytes-count");
const String HttpConnectionMetricsImpl::RECEIVED_BYTES_COUNT("http.received-bytes-count");

HttpConnectionMetricsImpl::HttpConnectionMetricsImpl(
    /* [in] */ IHttpTransportMetrics* inTransportMetric,
    /* [in] */ IHttpTransportMetrics* outTransportMetric)
    : mInTransportMetric(inTransportMetric)
    , mOutTransportMetric(outTransportMetric)
    , mRequestCount(0)
    , mResponseCount(0)
{}

CAR_INTERFACE_IMPL(HttpConnectionMetricsImpl, Object, IHttpConnectionMetrics)

ECode HttpConnectionMetricsImpl::GetReceivedBytesCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count)
    if (mInTransportMetric != NULL) {
        return mInTransportMetric->GetBytesTransferred(count);
    }
    else {
        *count = -1;
        return NOERROR;
    }
}

ECode HttpConnectionMetricsImpl::GetSentBytesCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count)
    if (mOutTransportMetric != NULL) {
        return mOutTransportMetric->GetBytesTransferred(count);
    }
    else {
        *count = -1;
        return NOERROR;
    }
}

ECode HttpConnectionMetricsImpl::GetRequestCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mRequestCount;
    return NOERROR;
}

void HttpConnectionMetricsImpl::IncrementRequestCount()
{
    mRequestCount++;
}

ECode HttpConnectionMetricsImpl::GetResponseCount(
    /* [out] */ Int64* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mResponseCount;
    return NOERROR;
}

void HttpConnectionMetricsImpl::IncrementResponseCount()
{
    mResponseCount++;
}

ECode HttpConnectionMetricsImpl::GetMetric(
    /* [in] */ const String& metricName,
    /* [out] */ IObject** metric)
{
    VALIDATE_NOT_NULL(metric)

    if (mMetricsCache != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(metricName, (ICharSequence**)&cs);
        AutoPtr<IMap> map = IMap::Probe(mMetricsCache);
        AutoPtr<IInterface> i;
        map->Get(cs, (IInterface**)&i);
        *metric = IObject::Probe(i);
        REFCOUNT_ADD(*metric)
    }
    if (*metric == NULL) {
        if (REQUEST_COUNT.Equals(metricName)) {
            AutoPtr<IInteger64> integer;
            CInteger64::New(mRequestCount, (IInteger64**)&integer);
            *metric = IObject::Probe(integer);
            REFCOUNT_ADD(*metric)
        }
        else if (RESPONSE_COUNT.Equals(metricName)) {
            AutoPtr<IInteger64> integer;
            CInteger64::New(mResponseCount, (IInteger64**)&integer);
            *metric = IObject::Probe(integer);
            REFCOUNT_ADD(*metric)
        }
        else if (RECEIVED_BYTES_COUNT.Equals(metricName)) {
            if (mInTransportMetric != NULL) {
                Int64 bytes;
                mInTransportMetric->GetBytesTransferred(&bytes);
                AutoPtr<IInteger64> integer;
                CInteger64::New(bytes, (IInteger64**)&integer);
                *metric = IObject::Probe(integer);
                REFCOUNT_ADD(*metric)
            }
            else {
                *metric = NULL;
                return NOERROR;
            }
        }
        else if (SENT_BYTES_COUNT.Equals(metricName)) {
            if (mOutTransportMetric != NULL) {
                Int64 bytes;
                mOutTransportMetric->GetBytesTransferred(&bytes);
                AutoPtr<IInteger64> integer;
                CInteger64::New(bytes, (IInteger64**)&integer);
                *metric = IObject::Probe(integer);
                REFCOUNT_ADD(*metric)
            }
            else {
                *metric = NULL;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

ECode HttpConnectionMetricsImpl::SetMetric(
    /* [in] */ const String& metricName,
    /* [in] */ IObject* metric)
{
    if (mMetricsCache == NULL) {
        CHashMap::New((IHashMap**)&mMetricsCache);
    }
    AutoPtr<ICharSequence> cs;
    CString::New(metricName, (ICharSequence**)&cs);
    IMap::Probe(mMetricsCache)->Put(cs, metric);
    return NOERROR;
}

ECode HttpConnectionMetricsImpl::Reset()
{
    if (mOutTransportMetric != NULL) {
        mOutTransportMetric->Reset();
    }
    if (mInTransportMetric != NULL) {
        mInTransportMetric->Reset();
    }
    mRequestCount = 0;
    mResponseCount = 0;
    mMetricsCache = NULL;
    return NOERROR;
}

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org