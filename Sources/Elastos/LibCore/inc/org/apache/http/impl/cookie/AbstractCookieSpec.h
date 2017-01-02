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

#ifndef __ORG_APACHE_HTTP_IMPL_COOKIE_ABSTRACTCOOKIESPEC_H__
#define __ORG_APACHE_HTTP_IMPL_COOKIE_ABSTRACTCOOKIESPEC_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;
using Elastos::Utility::ICollection;
using Org::Apache::Http::Cookie::ICookieSpec;
using Org::Apache::Http::Cookie::ICookieAttributeHandler;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Cookie {

class AbstractCookieSpec
    : public Object
    , public ICookieSpec
{
public:
    AbstractCookieSpec();

    CAR_INTERFACE_DECL()

    CARAPI RegisterAttribHandler(
        /* [in] */ const String& name,
        /* [in] */ ICookieAttributeHandler* handler);

protected:
    /**
     * Finds an attribute handler {@link CookieAttributeHandler} for the
     * given attribute. Returns <tt>null</tt> if no attribute handler is
     * found for the specified attribute.
     *
     * @param name attribute name. e.g. Domain, Path, etc.
     * @return an attribute handler or <tt>null</tt>
     */
    CARAPI FindAttribHandler(
        /* [in] */ const String& name,
        /* [out] */ ICookieAttributeHandler** handler);

    /**
     * Gets attribute handler {@link CookieAttributeHandler} for the
     * given attribute.
     *
     * @param name attribute name. e.g. Domain, Path, etc.
     * @throws IllegalStateException if handler not found for the
     *          specified attribute.
     */
    CARAPI GetAttribHandler(
        /* [in] */ const String& name,
        /* [out] */ ICookieAttributeHandler** handler);

    CARAPI GetAttribHandlers(
        /* [out] */ ICollection** collection);

private:
    /**
    * Stores attribute name -> attribute handler mappings
    */
    AutoPtr<IMap> mAttribHandlerMap;
};

} // namespace Cookie
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_COOKIE_ABSTRACTCOOKIESPEC_H__
