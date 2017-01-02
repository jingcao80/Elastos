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

#ifndef __ELASTOS_SQL_CSQLPERMISSION_H__
#define __ELASTOS_SQL_CSQLPERMISSION_H__

#include "_Elastos_Sql_CSQLPermission.h"
#include "Object.h"
//#include "BasicPermission.h"

using Elastos::Core::Object;
//TODO: temp.
//using Elastos::Security::BasicPermission;
using Elastos::Security::IPermission;

namespace Elastos {
namespace Sql {
//TODO need the class BasicPermission.
CarClass(CSQLPermission)
    // , public BasicPermission
    , public Object
    , public ISQLPermission
{
public:
    //TODO: to delete
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetActions(
        /* [out] */ String * str);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean * value);
};

} // namespace Sql
} // namespace Elastos

#endif // __ELASTOS_SQL_CSQLPERMISSION_H__
