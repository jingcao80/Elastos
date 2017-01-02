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

#include "org/apache/http/params/CHttpConnectionParamBean.h"
#include "org/apache/http/params/HttpConnectionParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

CAR_INTERFACE_IMPL(CHttpConnectionParamBean, Object, IHttpConnectionParamBean)

CAR_OBJECT_IMPL(CHttpConnectionParamBean)

ECode CHttpConnectionParamBean::SetSoTimeout(
    /* [in] */ Int32 soTimeout)
{
    return HttpConnectionParams::SetSoTimeout(mParams, soTimeout);
}

ECode CHttpConnectionParamBean::SetTcpNoDelay(
    /* [in] */ Boolean tcpNoDelay)
{
    return HttpConnectionParams::SetTcpNoDelay(mParams, tcpNoDelay);
}

ECode CHttpConnectionParamBean::SetSocketBufferSize(
    /* [in] */ Int32 socketBufferSize)
{
    return HttpConnectionParams::SetSocketBufferSize(mParams, socketBufferSize);
}

ECode CHttpConnectionParamBean::SetLinger(
    /* [in] */ Int32 linger)
{
    return HttpConnectionParams::SetLinger(mParams, linger);
}

ECode CHttpConnectionParamBean::SetConnectionTimeout(
    /* [in] */ Int32 connectionTimeout)
{
    return HttpConnectionParams::SetConnectionTimeout(mParams, connectionTimeout);
}

ECode CHttpConnectionParamBean::SetStaleCheckingEnabled(
    /* [in] */ Boolean staleCheckingEnabled)
{
    return HttpConnectionParams::SetStaleCheckingEnabled(mParams, staleCheckingEnabled);
}

ECode CHttpConnectionParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org
