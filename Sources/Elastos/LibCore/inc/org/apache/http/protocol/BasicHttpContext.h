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

#ifndef __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Default implementation of the {@link HttpContext HttpContext}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 654882 $
 *
 * @since 4.0
 */
class BasicHttpContext
    : public Object
    , public IHttpContext
{
public:
    CAR_INTERFACE_DECL()

    virtual ~BasicHttpContext() {}

    CARAPI GetAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attr);

    CARAPI SetAttribute(
        /* [in] */ const String& id,
        /* [in] */ IInterface* obj);

    CARAPI RemoveAttribute(
        /* [in] */ const String& id,
        /* [out] */ IInterface** attrs);

protected:
    CARAPI Init();

    CARAPI Init(
        /* [in] */ IHttpContext* parentContext);

private:
    AutoPtr<IHttpContext> mParentContext;
    AutoPtr<IMap> mMap;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_BASICHTTPCONTEXT_H_
