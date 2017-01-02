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

#include "CKeyManagerFactory.h"
#include "CTrustManagerFactory.h"
#include "CHandshakeCompletedEvent.h"
#include "CCertPathTrustManagerParameters.h"
#include "CDefaultSSLServerSocketFactory.h"
#include "CDefaultSSLSocketFactory.h"
#include "CSSLEngineResult.h"
#include "CSSLSessionBindingEvent.h"
#include "CSSLContext.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_OBJECT_IMPL(CKeyManagerFactory)

CAR_OBJECT_IMPL(CTrustManagerFactory)

CAR_OBJECT_IMPL(CHandshakeCompletedEvent)

CAR_OBJECT_IMPL(CCertPathTrustManagerParameters)

CAR_OBJECT_IMPL(CDefaultSSLServerSocketFactory)

CAR_OBJECT_IMPL(CDefaultSSLSocketFactory)

CAR_OBJECT_IMPL(CSSLEngineResult)

CAR_OBJECT_IMPL(CSSLSessionBindingEvent)

CAR_OBJECT_IMPL(CSSLContext)

} // namespace Ssl
} // namespace Net
} // namespace Elastosx