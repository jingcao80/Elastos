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

#include "Elastos.CoreLibrary.Net.h"
#include "org/apache/http/impl/client/RoutedRequest.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

RoutedRequest::RoutedRequest(
    /* [in] */ RequestWrapper* request,
    /* [in] */ IHttpRoute* route)
    : mRequest(request)
    , mRoute(route)
{}

AutoPtr<RequestWrapper> RoutedRequest::GetRequest()
{
    return mRequest;
}

AutoPtr<IHttpRoute> RoutedRequest::GetRoute()
{
    return mRoute;
}

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org