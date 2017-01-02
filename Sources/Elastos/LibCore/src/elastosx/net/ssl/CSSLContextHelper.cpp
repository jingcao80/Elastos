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

#include "CSSLContextHelper.h"
#include "SSLContext.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLContextHelper, Singleton, ISSLContextHelper)

CAR_SINGLETON_IMPL(CSSLContextHelper)

ECode CSSLContextHelper::GetDefault(
    /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    return SSLContext::GetDefault(context);
}

ECode CSSLContextHelper::SetDefault(
    /* [in] */ ISSLContext* sslContext)
{
    return SSLContext::SetDefault(sslContext);
}

ECode CSSLContextHelper::GetInstance(
    /* [in] */ const String& protocol,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)

    return SSLContext::GetInstance(protocol, instance);
}

ECode CSSLContextHelper::GetInstance(
    /* [in] */ const String& protocol,
    /* [in] */ const String& provider,
    /* [out] */ ISSLContext** instance)
{
    VALIDATE_NOT_NULL(instance)

    return SSLContext::GetInstance(protocol, provider, instance);
}

ECode CSSLContextHelper::GetInstance(
  /* [in] */ const String& protocol,
  /* [in] */ IProvider* provider,
  /* [out] */ ISSLContext** context)
{
    VALIDATE_NOT_NULL(context)

    return SSLContext::GetInstance(protocol, provider, context);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
