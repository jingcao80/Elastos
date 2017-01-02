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
