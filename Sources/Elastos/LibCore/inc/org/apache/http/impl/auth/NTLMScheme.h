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

#ifndef __ORG_APACHE_HTTP_IMPL_AUTH_NTLMSCHEME_H__
#define __ORG_APACHE_HTTP_IMPL_AUTH_NTLMSCHEME_H__

#include "org/apache/http/impl/auth/AuthSchemeBase.h"

using Elastos::Utility::IMap;
using Org::Apache::Http::IHeader;
using Org::Apache::Http::Auth::ICredentials;
using Org::Apache::Http::Utility::ICharArrayBuffer;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Auth {

class NTLMScheme : public AuthSchemeBase
{
public:
    NTLMScheme(
        /* [in] */ INTLMEngine* engine);

    CARAPI GetSchemeName(
        /* [out] */ String* name);

    CARAPI GetParameter(
        /* [in] */ const String& name,
        /* [out] */ String* param);

    CARAPI GetRealm(
        /* [out] */ String* realm);

    CARAPI IsConnectionBased(
        /* [out] */ Boolean* connectionBased);

    CARAPI Authenticate(
        /* [in] */ ICredentials* credentials,
        /* [in] */ IHttpRequest* request,
        /* [out] */ IHeader** header);

    CARAPI IsComplete(
        /* [out] */ Boolean* isComplete);

protected:
    CARAPI ParseChallenge(
        /* [in] */ ICharArrayBuffer* buffer,
        /* [in] */ Int32 pos,
        /* [in] */ Int32 len);

private:
    enum State {
        UNINITIATED,
        CHALLENGE_RECEIVED,
        MSG_TYPE1_GENERATED,
        MSG_TYPE2_RECEVIED,
        MSG_TYPE3_GENERATED,
        FAILED,
    };

private:
    AutoPtr<INTLMEngine> mEngine;

    State mState;
    String mChallenge;
};

} // namespace Auth
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_AUTH_NTLMSCHEME_H__
