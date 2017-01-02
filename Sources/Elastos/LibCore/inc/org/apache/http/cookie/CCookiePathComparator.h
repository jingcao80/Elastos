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

#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_

#include "_Org_Apache_Http_Cookie_CCookiePathComparator.h"
#include "elastos/core/Object.h"

using Elastos::Core::IComparator;
using Elastos::IO::ISerializable;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

/**
 * This cookie comparator ensures that multiple cookies satisfying
 * a common criteria are ordered in the <tt>Cookie</tt> header such
 * that those with more specific Path attributes precede those with
 * less specific.
 *
 * <p>
 * This comparator assumes that Path attributes of two cookies
 * path-match a commmon request-URI. Otherwise, the result of the
 * comparison is undefined.
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
CarClass(CCookiePathComparator)
    , public Object
    , public IComparator
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Compare(
        /* [in] */ IInterface* lhs,
        /* [in] */ IInterface* rhs,
        /* [out] */ Int32* result);

private:
    CARAPI_(String) NormalizePath(
        /* [in] */ ICookie* cookie);

private:
    static const Int64 serialVersionUID = 7523645369616405818L;

};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIEPATHCOMPARATOR_H_
