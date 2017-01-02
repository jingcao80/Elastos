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

#ifndef __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_

#include "_Org_Apache_Http_Protocol_CDefaultedHttpContext.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * {@link HttpContext} implementation that delegates resolution of an attribute
 * to the given default {@link HttpContext} instance if the attribute is not
 * present in the local one. The state of the local context can be mutated,
 * whereas the default context is treated as read-only.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 654882 $
 */
CarClass(CDefaultedHttpContext)
    , public Object
    , public IDefaultedHttpContext
    , public IHttpContext
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attr);

    CARAPI RemoveAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attrs);

    CARAPI SetAttribute(
        /* [in] */ const String& id,
        /* [in] */ IInterface* obj);

    CARAPI GetDefaults(
        /* [out] */ IHttpContext** defaults);

    CARAPI constructor(
        /* [in] */ IHttpContext* local,
        /* [in] */ IHttpContext* defaults);

private:
    AutoPtr<IHttpContext> mLocal;
    AutoPtr<IHttpContext> mDefaults;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CDEFAULTEDHTTPCONTEXT_H_
