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

#ifndef __ORG_APACHE_HTTP_PARAMS_HTTPABSTRACTPARAMBEAN_H_
#define __ORG_APACHE_HTTP_PARAMS_HTTPABSTRACTPARAMBEAN_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

class HttpAbstractParamBean : public Object
{
public:
    virtual ~HttpAbstractParamBean() {}

protected:
    CARAPI Init(
        /* [in] */ IHttpParams* params);

protected:
    AutoPtr<IHttpParams> mParams;
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_HTTPABSTRACTPARAMBEAN_H_
