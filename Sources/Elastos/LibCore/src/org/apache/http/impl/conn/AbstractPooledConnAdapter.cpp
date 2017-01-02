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

#include "org/apache/http/impl/conn/AbstractPooledConnAdapter.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

AbstractPooledConnAdapter::AbstractPooledConnAdapter(
    /* [in] */ IClientConnectionManager* manager,
    /* [in] */ AbstractPoolEntry* entry)
    : AbstractClientConnAdapter(manager, entry->mConnection)
    , mPoolEntry(entry)
{}

ECode AbstractPooledConnAdapter::AssertAttached()
{
    if (mPoolEntry == NULL) {
        Logger::E("AbstractPooledConnAdapter", "Adapter is detached.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    return NOERROR;
}

void AbstractPooledConnAdapter::Detach()
{
    AbstractClientConnAdapter::Detach();
    mPoolEntry = NULL;
}

ECode AbstractPooledConnAdapter::GetRoute(
    /* [out] */ IHttpRoute** route)
{
    VALIDATE_NOT_NULL(route)
    *route = NULL;
    FAIL_RETURN(AssertAttached())
    if (mPoolEntry->mTracker != NULL) {
        return mPoolEntry->mTracker->ToRoute(route);
    }
    return NOERROR;
}

ECode AbstractPooledConnAdapter::Open(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertAttached())
    return mPoolEntry->Open(route, context, params);
}

ECode AbstractPooledConnAdapter::TunnelTarget(
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertAttached())
    return mPoolEntry->TunnelTarget(secure, params);
}

ECode AbstractPooledConnAdapter::TunnelProxy(
    /* [in] */ IHttpHost* next,
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertAttached())
    return mPoolEntry->TunnelProxy(next, secure, params);
}

ECode AbstractPooledConnAdapter::LayerProtocol(
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    FAIL_RETURN(AssertAttached())
    return mPoolEntry->LayerProtocol(context, params);
}

ECode AbstractPooledConnAdapter::Close()
{
    if (mPoolEntry != NULL)
        mPoolEntry->ShutdownEntry();

    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    if (conn != NULL) {
        IHttpConnection::Probe(conn)->Close();
    }
    return NOERROR;
}

ECode AbstractPooledConnAdapter::Shutdown()
{
    if (mPoolEntry != NULL)
        mPoolEntry->ShutdownEntry();

    AutoPtr<IOperatedClientConnection> conn = GetWrappedConnection();
    if (conn != NULL) {
        IHttpConnection::Probe(conn)->Shutdown();
    }
    return NOERROR;
}

ECode AbstractPooledConnAdapter::GetState(
    /* [out] */ IObject** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;
    FAIL_RETURN(AssertAttached())
    AutoPtr<IObject> temp = mPoolEntry->GetState();
    *state = temp;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode AbstractPooledConnAdapter::SetState(
    /* [in] */ IObject* state)
{
    FAIL_RETURN(AssertAttached())
    mPoolEntry->SetState(state);
    return NOERROR;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org