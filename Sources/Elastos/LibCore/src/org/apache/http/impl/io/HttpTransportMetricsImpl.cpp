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