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

#include "org/conscrypt/CClientSessionContext.h"
#include <elastos/core/AutoLock.h>

using Elastos::Utility::CHashMap;

namespace Org {
namespace Conscrypt {

CAR_INTERFACE_IMPL(CClientSessionContext::HostAndPort, Object,
        IClientSessionContextHostAndPort)

CClientSessionContext::HostAndPort::HostAndPort(
    /* [in] */ const String& host,
    /* [in] */ Int32 port)
    : mHost(host)
    , mPort(port)
{
}

ECode CClientSessionContext::HostAndPort::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost.GetHashCode() * 31 + mPort;
    return NOERROR;
}

ECode CClientSessionContext::HostAndPort::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IClientSessionContextHostAndPort::Probe(obj) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<HostAndPort> lhs = (HostAndPort*)IClientSessionContextHostAndPort::Probe(obj);
    *result = mHost.Equals(lhs->mHost) && mPort == lhs->mPort;
    return NOERROR;
}

CAR_OBJECT_IMPL(CClientSessionContext)

CAR_INTERFACE_IMPL(CClientSessionContext, AbstractSessionContext,
        IClientSessionContext)

CClientSessionContext::CClientSessionContext()
{
    CHashMap::New((IMap**)&mSessionsByHostAndPort);
}

ECode CClientSessionContext::constructor()
{
    return AbstractSessionContext::constructor(10, 0);
}

ECode CClientSessionContext::Size(
    /* [out] */  Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mSessionsByHostAndPort->GetSize(result);
}

ECode CClientSessionContext::SetPersistentCache(
    /* [in] */  ISSLClientSessionCache* persistentCache)
{
    mPersistentCache = persistentCache;
    return NOERROR;
}

ECode CClientSessionContext::GetSession(
    /* [in] */  const String& host,
    /* [in] */  Int32 port,
    /* [out] */  ISSLSession** result)
{
    VALIDATE_NOT_NULL(result)

    if (host == NULL) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ISSLSession> session;
    AutoPtr<HostAndPort> hostAndPortKey = new HostAndPort(host, port);
    {
        AutoLock lock(mSessionsByHostAndPort);
        AutoPtr<IInterface> obj;
        mSessionsByHostAndPort->Get((IInterface*)(IObject*)hostAndPortKey.Get(),
                (IInterface**)&obj);
        session = ISSLSession::Probe(obj);
    }
    Boolean b;
    if (session != NULL && (session->IsValid(&b), b)) {
        *result = session;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    // Look in persistent cache.
    if (mPersistentCache != NULL) {
        AutoPtr<ArrayOf<Byte> > data;
        mPersistentCache->GetSessionData(host, port, (ArrayOf<Byte>**)&data);
        if (data != NULL) {
            ToSession(data, host, port, (ISSLSession**)&session);

            if (session != NULL && (session->IsValid(&b), b)) {
                AbstractSessionContext::PutSession(session);
                {
                    AutoLock lock(mSessionsByHostAndPort);
                    mSessionsByHostAndPort->Put(
                            (IInterface*)(IObject*)hostAndPortKey, session);
                }
                *result = session;
                REFCOUNT_ADD(*result)
                return NOERROR;
            }
        }
    }

    *result = NULL;
    return NOERROR;
}

ECode CClientSessionContext::PutSession(
    /* [in] */ ISSLSession* session)
{
    AbstractSessionContext::PutSession(session);

    String host;
    session->GetPeerHost(&host);
    Int32 port;
    session->GetPeerPort(&port);
    if (host == NULL) {
        return NOERROR;
    }

    AutoPtr<HostAndPort> hostAndPortKey = new HostAndPort(host, port);
    {
        AutoLock lock(mSessionsByHostAndPort);
        mSessionsByHostAndPort->Put(
                (IInterface*)(IObject*)hostAndPortKey, session);
    }

    // TODO: This in a background thread.
    if (mPersistentCache != NULL) {
        AutoPtr<ArrayOf<Byte> > data;
        ToBytes(session, (ArrayOf<Byte>**)&data);
        if (data != NULL) {
            mPersistentCache->PutSessionData(session, data);
        }
    }
    return NOERROR;
}

void CClientSessionContext::SessionRemoved(
    /* [in] */ ISSLSession* session)
{
    String host;
    session->GetPeerHost(&host);
    Int32 port;
    session->GetPeerPort(&port);
    if (host == NULL) {
        return;
    }
    AutoPtr<HostAndPort> hostAndPortKey = new HostAndPort(host, port);
    AutoLock lock(mSessionsByHostAndPort);
    mSessionsByHostAndPort->Remove((IInterface*)(IObject*)hostAndPortKey);
}

} // namespace Conscrypt
} // namespace Org
