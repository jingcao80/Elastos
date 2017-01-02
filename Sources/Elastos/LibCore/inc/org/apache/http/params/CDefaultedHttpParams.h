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

#ifndef __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_
#define __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_

#include "_Org_Apache_Http_Params_CDefaultedHttpParams.h"
#include "org/apache/http/params/AbstractHttpParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * {@link HttpParams} implementation that delegates resolution of a parameter
 * to the given default {@link HttpParams} instance if the parameter is not
 * present in the local one. The state of the local collection can be mutated,
 * whereas the default collection is treated as read-only.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 610763 $
 */
CarClass(CDefaultedHttpParams)
    , public AbstractHttpParams
    , public IDefaultedHttpParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a copy of the local collection with the same default
     */
    CARAPI Copy(
        /* [out] */ IHttpParams** httpParams);

    /**
     * Retrieves the value of the parameter from the local collection and, if the
     * parameter is not set locally, delegates its resolution to the default
     * collection.
     */
    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ IInterface** parameter);

    /**
     * Attempts to remove the parameter from the local collection. This method
     * <i>does not</i> modify the default collection.
     */
    CARAPI RemoveParameter(
        /* [in] */ const String& name,
        /* [out] */ Boolean* result);

    /**
     * Sets the parameter in the local collection. This method <i>does not</i>
     * modify the default collection.
     */
    CARAPI SetParameter(
        /* [in] */ const String& name,
        /* [in] */ IInterface* value,
        /* [out] */ IHttpParams** httpParams);

    /**
     * Copies the locally defined parameters to the argument parameters.
     * This method is called from {@link #copy()}.
     *
     * @param target    the parameters to which to copy
     */
    CARAPI GetDefaults(
        /* [out] */ IHttpParams** defaults);

    CARAPI constructor(
        /* [in] */ IHttpParams* local,
        /* [in] */ IHttpParams* defaults);

private:
    AutoPtr<IHttpParams> mLocal;
    AutoPtr<IHttpParams> mDefaults;
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CDefaultedHttpParams_H_
