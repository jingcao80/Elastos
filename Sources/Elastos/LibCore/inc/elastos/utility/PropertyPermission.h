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

#ifndef __ELASTOS_UTILITY_PROPERTYPERMISSION_H__
#define __ELASTOS_UTILITY_PROPERTYPERMISSION_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "security/BasicPermission.h"

using Elastos::Security::BasicPermission;
using Elastos::Security::IPermissionCollection;

namespace Elastos {
namespace Utility {

class PropertyPermission
    : public BasicPermission
    , public IPropertyPermission
{
public:
    CAR_INTERFACE_DECL()

    PropertyPermission();

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI NewPermissionCollection(
        /* [out] */ IPermissionCollection** permissions);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_PROPERTYPERMISSION_H__