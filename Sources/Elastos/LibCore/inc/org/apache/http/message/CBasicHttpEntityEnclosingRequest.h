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

#ifndef __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPENTITYENCLOSINGREQUEST_H_
#define __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPENTITYENCLOSINGREQUEST_H_

#include "_Org_Apache_Http_Message_CBasicHttpEntityEnclosingRequest.h"
#include "org/apache/http/message/BasicHttpEntityEnclosingRequest.h"

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
CarClass(CBasicHttpEntityEnclosingRequest) , public BasicHttpEntityEnclosingRequest
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ const String& uri);

    CARAPI constructor(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [in] */ IProtocolVersion* ver);

    CARAPI constructor(
        /* [in] */ IRequestLine* requestline);

    CARAPI constructor();
};

} // namespace Message
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_MESSAGE_CBASICHTTPENTITYENCLOSINGREQUEST_H_
