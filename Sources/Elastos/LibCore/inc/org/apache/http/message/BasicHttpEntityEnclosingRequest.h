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

#ifndef __ORG_APACHE_HTTP_MESSAGE_BASICHTTPENTITYENCLOSINGREQUEST_H_
#define __ORG_APACHE_HTTP_MESSAGE_BASICHTTPENTITYENCLOSINGREQUEST_H_

#include "org/apache/http/message/BasicHttpRequest.h"

using Org::Apache::Http::IHttpEntity;
using Org::Apache::Http::IRequestLine;
using Org::Apache::Http::IProtocolVersion;
using Org::Apache::Http::IHttpEntityEnclosingRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Message {

/**
 * Basic implementation of a request with an entity that can be modified.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618017 $
 *
 * @since 4.0
 */
class BasicHttpEntityEnclosingRequest
    : public BasicHttpRequest
    , public IHttpEntityEnclosingRequest
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetEntity(
        /* [out] */ IHttpEntity** entity);

    CARAPI SetEntity(
        /* [in] */ IHttpEntity* entity);

    CARAPI ExpectContinue(
        /* [out] */ Boolean* result);

protected:
    CARAPI Init(
        /* [in] */ const String& method,
        /* [in] */ const String& uri);

    CARAPI Init(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* ver);

    CARAPI Init(
        /* [in] */ IRequestLine* requestline);

private:
    AutoPtr<IHttpEntity> mEntity;
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_BASICHTTPENTITYENCLOSINGREQUEST_H_
