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

#ifndef __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPREQUESTBASE_H__
#define __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPREQUESTBASE_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Net.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "org/apache/http/message/AbstractHttpMessage.h"

using Elastos::Core::ICloneable;
using Elastos::Net::IURI;
using Elastos::Utility::Concurrent::Locks::ILock;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::Message::AbstractHttpMessage;
using Org::Apache::Http::Conn::IClientConnectionRequest;
using Org::Apache::Http::Conn::IConnectionReleaseTrigger;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Methods {

/**
 * Basic implementation of an HTTP request that can be modified.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 674186 $
 *
 * @since 4.0
 */
class HttpRequestBase
    : public AbstractHttpMessage
    , public IHttpRequest
    , public IHttpUriRequest
    , public IAbortableHttpRequest
    , public ICloneable
{
public:
    HttpRequestBase();

    virtual ~HttpRequestBase() {}

    CAR_INTERFACE_DECL()

    virtual CARAPI GetMethod(
        /* [out] */ String* method) = 0;

    CARAPI GetProtocolVersion(
        /* [out] */ IProtocolVersion** ver);

    CARAPI GetURI(
        /* [out] */ IURI** URI);

    CARAPI GetRequestLine(
        /* [out] */ IRequestLine** line);

    CARAPI SetURI(
        /* [in] */ IURI* uri);

    CARAPI SetConnectionRequest(
        /* [in] */ IClientConnectionRequest* connRequest);

    CARAPI SetReleaseTrigger(
        /* [in] */ IConnectionReleaseTrigger* releaseTrigger);

    CARAPI Abort();

    CARAPI IsAborted(
        /* [out] */ Boolean* isAborted);

protected:
    CARAPI CloneImpl(
        /* [in] */ HttpRequestBase* clone);

private:
    AutoPtr<ILock> mAbortLock;

    Boolean mAborted;

    AutoPtr<IURI> mUri;
    AutoPtr<IClientConnectionRequest> mConnRequest;
    AutoPtr<IConnectionReleaseTrigger> mReleaseTrigger;

};

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPREQUESTBASE_H__
