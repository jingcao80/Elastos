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

#ifndef __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_
#define __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_

#include "_Org_Apache_Http_Cookie_CCookieOrigin.h"
#include "elastos/core/Object.h"

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Http {
namespace Cookie {

/**
 * This cookie comparator can be used to compare identity of cookies.
 *
 * <p>
 *  Cookies are considered identical if their names are equal and
 *  their domain attributes match ignoring case.
 * </p>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
CarClass(CCookieOrigin)
    , public Object
    , public ICookieOrigin
{
public:
    CCookieOrigin();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI GetHost(
        /* [out] */ String* host);

    CARAPI GetPath(
        /* [out] */ String* path);

    CARAPI GetPort(
        /* [out] */ Int32* port);

    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ Int32 port,
        /* [in] */ const String& path,
        /* [in] */ Boolean secure);

private:
    String mHost;
    Int32 mPort;
    String mPath;
    Boolean mSecure;

};

} // namespace Cookie
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_COOKIE_CCOOKIEORIGIN_H_
