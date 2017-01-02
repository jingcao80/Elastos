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
#include "org/apache/http/impl/conn/AbstractPoolEntry.h"
#include "org/apache/http/conn/routing/CRouteTracker.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Net::IInetAddress;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Conn::Routing::CRouteTracker;
using Org::Apache::Http::Conn::Routing::IRouteInfo;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {

AbstractPoolEntry::AbstractPoolEntry(
    /* [in] */ IClientConnectionOperator* connOperator,
    /* [in] */ IHttpRoute* route)
{
    if (connOperator == NULL) {
        Logger::E("AbstractPoolEntry", "Connection operator may not be null");
        assert(0);
        // throw new IllegalArgumentException("Connection operator may not be null");
    }
    mConnOperator = connOperator;
    connOperator->CreateConnection((IOperatedClientConnection**)&mConnection);
    mRoute = route;
}

AutoPtr<IObject> AbstractPoolEntry::GetState()
{
    return mState;
}

void AbstractPoolEntry::SetState(
    /* [in] */ IObject* state)
{
    mState = state;
}

ECode AbstractPoolEntry::Open(
    /* [in] */ IHttpRoute* route,
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    if (route == NULL) {
        Logger::E("AbstractPoolEntry", "Route must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //@@@ is context allowed to be null? depends on operator?
    if (params == NULL) {
        Logger::E("AbstractPoolEntry", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Boolean isConnected;
    if ((mTracker != NULL) && (mTracker->IsConnected(&isConnected), isConnected)) {
        Logger::E("AbstractPoolEntry", "Connection already open.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // - collect the arguments
    // - call the operator
    // - update the tracking data
    // In this order, we can be sure that only a successful
    // opening of the connection will be tracked.

    //@@@ verify route against planned route?
    mTracker = NULL;
    CRouteTracker::New(route, (IRouteTracker**)&mTracker);
    AutoPtr<IRouteInfo> ri = IRouteInfo::Probe(route);
    AutoPtr<IHttpHost> proxy;
    ri->GetProxyHost((IHttpHost**)&proxy);

    AutoPtr<IHttpHost> host = proxy;
    if (proxy == NULL) {
        ri->GetTargetHost((IHttpHost**)&host);
    }
    AutoPtr<IInetAddress> addr;
    ri->GetLocalAddress((IInetAddress**)&addr);
    mConnOperator->OpenConnection(mConnection, host, addr, context, params);

    AutoPtr<IRouteTracker> localTracker = mTracker; // capture volatile

    // If this tracker was reset while connecting,
    // fail early.
    if (localTracker == NULL) {
        Logger::E("AbstractPoolEntry", "Request aborted");
        return E_IO_EXCEPTION;
    }

    Boolean isSecure;
    mConnection->IsSecure(&isSecure);
    if (proxy == NULL) {
        localTracker->ConnectTarget(isSecure);
    }
    else {
        localTracker->ConnectProxy(proxy, isSecure);
    }
    return NOERROR;
}

ECode AbstractPoolEntry::TunnelTarget(
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    if (params == NULL) {
        Logger::E("AbstractPoolEntry", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //@@@ check for proxy in planned route?
    Boolean isConnected;
    if ((mTracker != NULL) && (mTracker->IsConnected(&isConnected), !isConnected)) {
        Logger::E("AbstractPoolEntry", "Connection not open.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<IRouteInfo> ri = IRouteInfo::Probe(mTracker);
    Boolean isTunnelled;
    if (ri->IsTunnelled(&isTunnelled), isTunnelled) {
        Logger::E("AbstractPoolEntry", "Connection is already tunnelled.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // LOG.debug?

    AutoPtr<IHttpHost> target;
    ri->GetTargetHost((IHttpHost**)&target);
    mConnection->Update(NULL, target, secure, params);
    mTracker->TunnelTarget(secure);
    return NOERROR;
}

ECode AbstractPoolEntry::TunnelProxy(
    /* [in] */ IHttpHost* next,
    /* [in] */ Boolean secure,
    /* [in] */ IHttpParams* params)
{
    if (next == NULL) {
        Logger::E("AbstractPoolEntry", "Next proxy must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (params == NULL) {
        Logger::E("AbstractPoolEntry", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    //@@@ check for proxy in planned route?
    Boolean isConnected;
    if ((mTracker != NULL) && (mTracker->IsConnected(&isConnected), !isConnected)) {
        Logger::E("AbstractPoolEntry", "Connection not open.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // LOG.debug?

    mConnection->Update(NULL, next, secure, params);
    mTracker->TunnelProxy(next, secure);
    return NOERROR;
}

ECode AbstractPoolEntry::LayerProtocol(
    /* [in] */ IHttpContext* context,
    /* [in] */ IHttpParams* params)
{
    //@@@ is context allowed to be null? depends on operator?
    if (params == NULL) {
        Logger::E("AbstractPoolEntry", "Parameters must not be null.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean isConnected;
    if ((mTracker != NULL) && (mTracker->IsConnected(&isConnected), !isConnected)) {
        Logger::E("AbstractPoolEntry", "Connection not open.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean isTunnelled;
    if (IRouteInfo::Probe(mTracker)->IsTunnelled(&isTunnelled), !isTunnelled) {
        Logger::E("AbstractPoolEntry", "Protocol layering without a tunnel not supported.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    Boolean isLayered;
    if (IRouteInfo::Probe(mTracker)->IsLayered(&isLayered), isLayered) {
        Logger::E("AbstractPoolEntry", "Multiple protocol layering not supported.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    // - collect the arguments
    // - call the operator
    // - update the tracking data
    // In this order, we can be sure that only a successful
    // layering on top of the connection will be tracked.

    AutoPtr<IHttpHost> target;
    IRouteInfo::Probe(mTracker)->GetTargetHost((IHttpHost**)&target);

    mConnOperator->UpdateSecureConnection(mConnection, target, context, params);

    Boolean isSecure;
    mConnection->IsSecure(&isSecure);
    mTracker->LayerProtocol(isSecure);
    return NOERROR;
}

void AbstractPoolEntry::ShutdownEntry()
{
    mTracker = NULL;
}

} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org