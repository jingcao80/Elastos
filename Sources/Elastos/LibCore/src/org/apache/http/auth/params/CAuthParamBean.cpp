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

#include "org/apache/http/auth/params/CAuthParamBean.h"
#include "org/apache/http/auth/params/AuthParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CAR_INTERFACE_IMPL(CAuthParamBean, Object, IAuthParamBean)

CAR_OBJECT_IMPL(CAuthParamBean)

ECode CAuthParamBean::SetCredentialCharset(
    /* [in] */ const String& charset)
{
    return AuthParams::SetCredentialCharset(mParams, charset);
}

ECode CAuthParamBean::constructor(
    /* [in] */ IHttpParams* params)
{
    return Init(params);
}

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
