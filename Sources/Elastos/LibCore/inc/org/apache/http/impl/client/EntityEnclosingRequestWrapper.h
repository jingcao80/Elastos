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

#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_ENTITYENCLOSINGREQUESTWRAPPER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_ENTITYENCLOSINGREQUESTWRAPPER_H__

#include "org/apache/http/impl/client/RequestWrapper.h"

using Org::Apache::Http::IHttpEntityEnclosingRequest;
using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpMessage;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * A wrapper class for {@link HttpEntityEnclosingRequest}s that can
 * be used to change properties of the current request without
 * modifying the original object.
 * </p>
 * This class is also capable of resetting the request headers to
 * the state of the original request.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 674186 $
 *
 * @since 4.0
 */
class EntityEnclosingRequestWrapper
    : public RequestWrapper
    , public IHttpEntityEnclosingRequest
{
public:
    EntityEnclosingRequestWrapper(
        /* [in] */ IHttpEntityEnclosingRequest* request);

    CAR_INTERFACE_DECL()

    CARAPI SetEntity(
        /* [in] */ IHttpEntity* entity);

    CARAPI GetEntity(
        /* [out] */ IHttpEntity** entity);

    CARAPI ExpectContinue(
        /* [out] */ Boolean* result);

    CARAPI_(Boolean) IsRepeatable();

private:
    AutoPtr<IHttpEntity> mEntity;
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_REQUESTWRAPPER_H__
