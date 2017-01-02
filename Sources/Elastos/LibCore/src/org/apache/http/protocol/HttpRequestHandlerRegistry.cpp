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

#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/HttpRequestHandlerRegistry.h"
#include "org/apache/http/protocol/CUriPatternMatcher.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL_2(HttpRequestHandlerRegistry, Object, IHttpRequestHandlerRegistry, IHttpRequestHandlerResolver)

ECode HttpRequestHandlerRegistry::Init()
{
    return CUriPatternMatcher::New((IUriPatternMatcher**)&mMatcher);
}

ECode HttpRequestHandlerRegistry::Register(
    /* [in] */ const String& pattern,
    /* [in] */ IHttpRequestHandler* handler)
{
    return mMatcher->Register(pattern, handler);
}

ECode HttpRequestHandlerRegistry::Unregister(
    /* [in] */ const String& pattern)
{
    return mMatcher->Unregister(pattern);
}

ECode HttpRequestHandlerRegistry::SetHandlers(
    /* [in] */ IMap* map)
{
    return mMatcher->SetHandlers(map);
}

ECode HttpRequestHandlerRegistry::Lookup(
    /* [in] */ const String& requestURI,
    /* [out] */ IHttpRequestHandler** handler)
{
    VALIDATE_NOT_NULL(handler)
    AutoPtr<IInterface> value;
    mMatcher->Lookup(requestURI, (IInterface**)&value);
    *handler = IHttpRequestHandler::Probe(value);
    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode HttpRequestHandlerRegistry::MatchUriRequestPattern(
    /* [in] */ const String& pattern,
    /* [in] */ const String& requestUri,
    /* [out] */ Boolean* isMatch)
{
    VALIDATE_NOT_NULL(isMatch)
    return mMatcher->MatchUriRequestPattern(pattern, requestUri, isMatch);
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
