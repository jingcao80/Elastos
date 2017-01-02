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

#ifndef __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMBEAN_H_
#define __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMBEAN_H_

#include "_Org_Apache_Http_Params_CHttpConnectionParamBean.h"
#include "org/apache/http/params/HttpAbstractParamBean.h"
#include "elastos/core/Object.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CarClass(CHttpConnectionParamBean)
    , public IHttpConnectionParamBean
    , public HttpAbstractParamBean
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI SetSoTimeout(
        /* [in] */ Int32 soTimeout);

    CARAPI SetTcpNoDelay(
        /* [in] */ Boolean tcpNoDelay);

    CARAPI SetSocketBufferSize(
        /* [in] */ Int32 socketBufferSize);

    CARAPI SetLinger(
        /* [in] */ Int32 linger);

    CARAPI SetConnectionTimeout(
        /* [in] */ Int32 connectionTimeout);

    CARAPI SetStaleCheckingEnabled(
        /* [in] */ Boolean staleCheckingEnabled);

    CARAPI constructor(
        /* [in] */ IHttpParams* params);

};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CHTTPCONNECTIONPARAMBEAN_H_
