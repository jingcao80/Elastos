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

#ifndef __ORG_APACHE_HTTP_CHTTPVERSIONHELPER_H__
#define __ORG_APACHE_HTTP_CHTTPVERSIONHELPER_H__

#include "_Org_Apache_Http_CHttpVersionHelper.h"
#include "elastos/core/Singleton.h"


namespace Org {
namespace Apache {
namespace Http {

/**
 * Represents an HTTP version, as specified in RFC 2616.
 *
 * @author <a href="mailto:oleg@ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 609106 $ $Date: 2008-01-05 01:15:42 -0800 (Sat, 05 Jan 2008) $
 */
CarClass(CHttpVersionHelper)
    , public Object
    , public IHttpVersionHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GerHttpVersion09(
        /* [out] */ IHttpVersion** ver);

    CARAPI GerHttpVersion10(
        /* [out] */ IHttpVersion** ver);

    CARAPI GerHttpVersion11(
        /* [out] */ IHttpVersion** ver);
};

} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CHTTPVERSION_H__
