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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CUriHelper.h"
#include "elastos/droid/net/Uri.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CUriHelper, Singleton, IUriHelper)

CAR_SINGLETON_IMPL(CUriHelper)

ECode CUriHelper::Parse(
    /* [in] */ const String& uriString,
    /* [out] */ IUri** result)
{
    return Uri::Parse(uriString, result);
}

ECode CUriHelper::FromFile(
    /* [in] */ IFile* file,
    /* [out] */ IUri** result)
{
    return Uri::FromFile(file, result);
}

ECode CUriHelper::FromParts(
    /* [in] */ const String& scheme,
    /* [in] */ const String& ssp,
    /* [in] */ const String& fragment,
    /* [out] */ IUri** result)
{
    return Uri::FromParts(scheme, ssp, fragment, result);
}

ECode CUriHelper::Encode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return Uri::Encode(s, result);
}

ECode CUriHelper::Encode(
    /* [in] */ const String& s,
    /* [in] */ const String& allow,
    /* [out] */ String* result)
{
    return Uri::Encode(s, allow, result);
}

ECode CUriHelper::Decode(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    return Uri::Decode(s, result);
}

ECode CUriHelper::WithAppendedPath(
    /* [in] */ IUri* baseUri,
    /* [in] */ const String& pathSegment,
    /* [out] */ IUri** result)
{
    return Uri::WithAppendedPath(baseUri, pathSegment, result);
}

ECode CUriHelper::GetEMPTY(
    /* [out] */ IUri** result)
{
    return Uri::GetEMPTY(result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos

