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

#include "org/apache/http/client/methods/HttpRequestBase.h"
#include "org/apache/http/client/utils/CloneUtils.h"
#include "org/apache/http/params/HttpProtocolParams.h"
#include "org/apache/http/message/CBasicRequestLine.h"
#include "elastos/utility/concurrent/locks/CReentrantLock.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::Message::CBasicRequestLine;
using Org::Apache::Http::Message::IHeaderGroup;
using Org::Apache::Http::Client::Utils::CloneUtils;
using Org::Apache::Http::Params::IHttpParams;
using Org::Apache::Http::Params::HttpProtocolParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

HttpRequestBase::HttpRequestBase()
    : AbstractHttpMessage()
    , mAborted(FALSE)
{
    AutoPtr<IReentrantLock> l;
    CReentrantLock::New((IReentrantLock**)&l);
    mAbortLock = ILock::Probe(l);
}

CAR_INTERFACE_IMPL_4(HttpRequestBase, AbstractHttpMessage, IHttpRequest, IHttpUriRequest, IAbortableHttpRequest, ICloneable)

ECode HttpRequestBase::GetProtocolVersion(
    /* [out] */ IProtocolVersion** ver)
{
    VALIDATE_NOT_NULL(ver)
    AutoPtr<IHttpParams> params;
    GetParams((IHttpParams**)&params);
    return HttpProtocolParams::GetVersion(params, ver);
}

ECode HttpRequestBase::GetURI(
    /* [out] */ IURI** uri)
{
    VALIDATE_NOT_NULL(uri)
    *uri = mUri;
    REFCOUNT_ADD(*uri)
    return NOERROR;
}

ECode HttpRequestBase::GetRequestLine(
    /* [out] */ IRequestLine** line)
{
    VALIDATE_NOT_NULL(line)
    String method;
    GetMethod(&method);
    AutoPtr<IProtocolVersion> ver;
    GetProtocolVersion((IProtocolVersion**)&ver);
    AutoPtr<IURI> uri;
    GetURI((IURI**)&uri);
    String uritext = String(NULL);
    if (uri != NULL) {
        uri->ToASCIIString(&uritext);
    }
    if (uritext.IsNull() || uritext.GetLength() == 0) {
        uritext = String("/");
    }
    return CBasicRequestLine::New(method, uritext, ver, line);
}

ECode HttpRequestBase::SetURI(
    /* [in] */ IURI* uri)
{
    mUri = uri;
    return NOERROR;
}

ECode HttpRequestBase::SetConnectionRequest(
    /* [in] */ IClientConnectionRequest* connRequest)
{
    mAbortLock->Lock();
    // try {
    if (mAborted) {
        Logger::E("HttpRequestBase", "Request already aborted");
        mAbortLock->UnLock();
        return E_IO_EXCEPTION;
    }

    mReleaseTrigger = NULL;
    mConnRequest = connRequest;
    // } finally {
    //     this.abortLock.unlock();
    // }
    mAbortLock->UnLock();
    return NOERROR;
}

ECode HttpRequestBase::SetReleaseTrigger(
    /* [in] */ IConnectionReleaseTrigger* releaseTrigger)
{
    mAbortLock->Lock();
    // try {
    if (mAborted) {
        Logger::E("HttpRequestBase", "Request already aborted");
        mAbortLock->UnLock();
        return E_IO_EXCEPTION;
    }

    mConnRequest = NULL;
    mReleaseTrigger = releaseTrigger;
    // } finally {
    //     this.abortLock.unlock();
    // }
    mAbortLock->UnLock();
    return NOERROR;
}

ECode HttpRequestBase::Abort()
{
    AutoPtr<IClientConnectionRequest> localRequest;
    AutoPtr<IConnectionReleaseTrigger> localTrigger;

    mAbortLock->Lock();
    // try {
    if (mAborted) {
        mAbortLock->UnLock();
        return NOERROR;
    }
    mAborted = TRUE;

    localRequest = mConnRequest;
    localTrigger = mReleaseTrigger;
    mAbortLock->UnLock();
    // } finally {
    //     this.abortLock.unlock();
    // }

    // Trigger the callbacks outside of the lock, to prevent
    // deadlocks in the scenario where the callbacks have
    // their own locks that may be used while calling
    // setReleaseTrigger or setConnectionRequest.
    if (localRequest != NULL) {
        localRequest->AbortRequest();
    }
    if (localTrigger != NULL) {
        // try {
        localTrigger->AbortConnection();
        // } catch (IOException ex) {
        //     // ignore
        // }
    }
    return NOERROR;
}

ECode HttpRequestBase::IsAborted(
    /* [out] */ Boolean* isAborted)
{
    VALIDATE_NOT_NULL(isAborted)
    *isAborted = mAborted;
    return NOERROR;
}

ECode HttpRequestBase::CloneImpl(
    /* [in] */ HttpRequestBase* clone)
{
    AutoPtr<IReentrantLock> l;
    CReentrantLock::New((IReentrantLock**)&l);
    clone->mAbortLock = ILock::Probe(l);
    clone->mAborted = FALSE;
    clone->mReleaseTrigger = NULL;
    clone->mConnRequest = NULL;
    AutoPtr<IObject> o;
    CloneUtils::Clone(IObject::Probe(mHeadergroup), (IObject**)&o);
    clone->mHeadergroup = IHeaderGroup::Probe(o);
    o = NULL;
    CloneUtils::Clone(IObject::Probe(mParams), (IObject**)&o);
    clone->mParams = IHttpParams::Probe(o);
    return NOERROR;
}

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org