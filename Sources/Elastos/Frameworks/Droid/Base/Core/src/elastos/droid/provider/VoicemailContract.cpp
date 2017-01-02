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

#include "Elastos.Droid.Provider.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/provider/VoicemailContract.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriBuilder;
using Elastos::Droid::Net::Uri;

namespace Elastos {
namespace Droid {
namespace Provider {
//============================================================================
//              Voicemails
//============================================================================
static AutoPtr<IUri> initVoicemailsCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IVoicemailContract::AUTHORITY + "/voicemail", (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> VoicemailContract::Voicemails::CONTENT_URI = initVoicemailsCONTENT_URI();

ECode VoicemailContract::Voicemails::BuildSourceUri(
    /* [in] */ const String& packageName,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IUriBuilder> uriBuilder;
    VoicemailContract::Voicemails::CONTENT_URI->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(IVoicemailContract::PARAM_KEY_SOURCE_PACKAGE, packageName);
    return uriBuilder->Build(uri);
}

//============================================================================
//              Status
//============================================================================
static AutoPtr<IUri> initStatusCONTENT_URI()
{
    AutoPtr<IUri> uri;
    Uri::Parse(String("content://") + IVoicemailContract::AUTHORITY + "/status", (IUri**)&uri);
    return uri;
}

const AutoPtr<IUri> VoicemailContract::Status::CONTENT_URI = initStatusCONTENT_URI();

ECode VoicemailContract::Status::BuildSourceUri(
    /* [in] */ const String& packageName,
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    AutoPtr<IUriBuilder> uriBuilder;
    VoicemailContract::Status::CONTENT_URI->BuildUpon((IUriBuilder**)&uriBuilder);
    uriBuilder->AppendQueryParameter(IVoicemailContract::PARAM_KEY_SOURCE_PACKAGE, packageName);
    return uriBuilder->Build(uri);
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
