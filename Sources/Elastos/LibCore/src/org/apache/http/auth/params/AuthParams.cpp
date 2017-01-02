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

#include "org/apache/http/auth/params/AuthParams.h"
#include "elastos/core/CString.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Logging::Logger;
using Org::Apache::Http::Protocol::IHTTP;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

AuthParams::AuthParams()
{}

ECode AuthParams::GetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [out] */ String* charset)
{
    VALIDATE_NOT_NULL(charset)
    *charset = String(NULL);

    if (params == NULL) {
        Logger::E("AuthParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInterface> o;
    params->GetParameter(IAuthPNames::CREDENTIAL_CHARSET, (IInterface**)&o);
    String cs;
    IObject::Probe(o)->ToString(&cs);
    //@@@ TODO: inconsistent with JavaDoc in AuthPNames,
    //@@@ TODO: check HTTP_ELEMENT_CHARSET first, or fix JavaDocs
    if (cs.IsNull()) {
        cs = IHTTP::DEFAULT_PROTOCOL_CHARSET;
    }
    *charset = cs;
    return NOERROR;
}

ECode AuthParams::SetCredentialCharset(
    /* [in] */ IHttpParams* params,
    /* [in] */ const String& charset)
{
    if (params == NULL) {
        Logger::E("AuthParams", "HTTP parameters may not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ICharSequence> cs;
    CString::New(charset, (ICharSequence**)&cs);
    AutoPtr<IHttpParams> p;
    return params->SetParameter(IAuthPNames::CREDENTIAL_CHARSET, cs, (IHttpParams**)&p);
}

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org
