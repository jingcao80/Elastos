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

#include "org/apache/http/conn/params/CConnConnectionParamBean.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace Params {

CAR_INTERFACE_IMPL(CConnConnectionParamBean, Object, IConnConnectionParamBean)

CAR_OBJECT_IMPL(CConnConnectionParamBean)

ECode CConnConnectionParamBean::SetMaxStatusLineGarbage(
    /* [in] */ Int32 maxStatusLineGarbage)
{
    AutoPtr<IHttpParams> p;
    return mParams->SetInt32Parameter(IConnConnectionPNames::MAX_STATUS_LINE_GARBAGE,
            maxStatusLineGarbage, (IHttpParams**)&p);
}

ECode CConnConnectionParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(mParams);
}

} // namespace Params
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org
