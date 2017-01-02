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

#include "org/apache/http/auth/params/CAuthParams.h"
#include "org/apache/http/auth/params/AuthParams.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CAR_INTERFACE_IMPL(CAuthParams, Object, IAuthParams)

CAR_SINGLETON_IMPL(CAuthParams)

ECode CAuthParams::GetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    return AuthParams::GetCredentialCharset(params, charset);
}

ECode CAuthParams::SetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    return AuthParams::SetCredentialCharset(params, charset);
}

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
