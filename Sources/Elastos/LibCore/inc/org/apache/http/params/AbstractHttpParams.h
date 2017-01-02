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

#ifndef __ORG_APACHE_HTTP_PARAMS_ABSTRACTHTTPPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_ABSTRACTHTTPPARAMS_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * Abstract base class for parameter collections.
 * Type specific setters and getters are mapped to the abstract,
 * generic getters and setters.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 *
 * @version $Revision: 542224 $
 */

class AbstractHttpParams
    : public Object
    , public IHttpParams
{
public:
    CAR_INTERFACE_DECL()

    virtual ~AbstractHttpParams() {}

    CARAPI GetInt64Parameter(
        /* [in] */ const String& name,
        /* [in] */ Int64 defaultValue,
        /* [out] */ Int64* int64Parameter);

    CARAPI SetInt64Parameter(
        /* [in] */ const String& name,
        /* [in] */ Int64 value,
        /* [out] */ IHttpParams** httpParams);

    CARAPI GetInt32Parameter(
        /* [in] */ const String& name,
        /* [in] */ Int32 defaultValue,
        /* [out] */ Int32* int32Parameter);

    CARAPI SetInt32Parameter(
        /* [in] */ const String& name,
        /* [in] */ Int32 value,
        /* [out] */ IHttpParams** httpParams);

    CARAPI GetDoubleParameter(
        /* [in] */ const String& name,
        /* [in] */ Double defaultValue,
        /* [out] */ Double* doubleParameter);

    CARAPI SetDoubleParameter(
        /* [in] */ const String& name,
        /* [in] */ Double value,
        /* [out] */ IHttpParams** httpParams);

    CARAPI GetBooleanParameter(
        /* [in] */ const String& name,
        /* [in] */ Boolean defaultValue,
        /* [out] */ Boolean* booleanParameter);

    CARAPI SetBooleanParameter(
        /* [in] */ const String& name,
        /* [in] */ Boolean value,
        /* [out] */ IHttpParams** httpParams);

    CARAPI IsParameterTrue(
        /* [in] */ const String& name,
        /* [out] */ Boolean* isParameterTrue);

    CARAPI IsParameterFalse(
        /* [in] */ const String& name,
        /* [out] */ Boolean* isParameterFalse);
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_ABSTRACTHTTPPARAMS_H_
