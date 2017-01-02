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

#include "CSSLPermission.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_OBJECT_IMPL(CSSLPermission)

ECode CSSLPermission::constructor(
    /* [in] */ const String& name)
{
    assert(0 && "TODO");
    //TODO return BasicPermission::constructor(String(""));
    return NOERROR;
}

ECode CSSLPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    assert(0 && "TODO");
    //TODO return BasicPermission::constructor(String(""), String(""));
    return NOERROR;
}

ECode CSSLPermission::GetActions(
    /* [out] */ String* action)
{
    VALIDATE_NOT_NULL(action)

    *action = String(NULL);
    return NOERROR;
}

ECode CSSLPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
