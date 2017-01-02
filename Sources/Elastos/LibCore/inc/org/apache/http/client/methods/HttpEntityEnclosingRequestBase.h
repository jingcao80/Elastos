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

#ifndef __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPENTITYENCLOSINGREQUESTBASE_H__
#define __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPENTITYENCLOSINGREQUESTBASE_H__

#include "org/apache/http/client/methods/HttpRequestBase.h"

using Org::Apache::Http::IHttpEntity;

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
class HttpEntityEnclosingRequestBase
    : public HttpRequestBase
    , public IHttpEntityEnclosingRequest
{
public:
    HttpEntityEnclosingRequestBase();

    virtual ~HttpEntityEnclosingRequestBase() {}

    CAR_INTERFACE_DECL()

    CARAPI SetEntity(
        /* [in] */ IHttpEntity* entity);

    CARAPI GetEntity(
        /* [out] */ IHttpEntity** entity);

    CARAPI ExpectContinue(
        /* [out] */ Boolean* result);

protected:
    CARAPI CloneImpl(
        /* [in] */ HttpEntityEnclosingRequestBase* dst);

private:
    AutoPtr<IHttpEntity> mEntity;
};

} // namespace Methods
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_METHODS_HTTPENTITYENCLOSINGREQUESTBASE_H__
