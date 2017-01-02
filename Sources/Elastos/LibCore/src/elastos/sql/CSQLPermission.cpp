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

#include "CSQLPermission.h"

namespace Elastos {
namespace Sql {

//TODO: to delete
CAR_INTERFACE_IMPL(CSQLPermission, Object, ISQLPermission)

CAR_OBJECT_IMPL(CSQLPermission)

ECode CSQLPermission::constructor(
    /* [in] */ const String& name)
{
    // super("");
    //TODO
    assert(0);
    // BasicPermission::Init(String(""));
    return NOERROR;
}

ECode CSQLPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    // super("", "");
    //TODO
    assert(0);
    // BasicPermission::Init(String(""), String(""));
    return NOERROR;
}

ECode CSQLPermission::GetActions(
    /* [out] */ String * str)
{
    *str = String(NULL);
    return NOERROR;
}

ECode CSQLPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    *value = TRUE;
    return NOERROR;
}

} // namespace Sql
} // namespace Elastos
