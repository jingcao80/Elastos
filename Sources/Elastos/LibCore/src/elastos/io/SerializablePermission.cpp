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

#include "SerializablePermission.h"

namespace Elastos {
namespace IO {

ECode SerializablePermission::constructor(
    /* [in] */ const String& permissionName)
{
    return BasicPermission::constructor(String(""));
}

ECode SerializablePermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::constructor(String(""), String(""));
}

ECode SerializablePermission::GetActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode SerializablePermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
