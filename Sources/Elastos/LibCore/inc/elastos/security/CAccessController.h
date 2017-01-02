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

#ifndef __ELASTOS_SECURITY_CACCESSCONTROLLER_H__
#define __ELASTOS_SECURITY_CACCESSCONTROLLER_H__

#include "_Elastos_Security_CAccessController.h"
#include "Singleton.h"

namespace Elastos {
namespace Security {

CarClass(CAccessController)
    , public Singleton
    , public IAccessController
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivileged(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [in] */ IAccessControlContext *context,
        /* [out] */ IInterface **result);

    CARAPI DoPrivilegedWithCombiner(
        /* [in] */ IPrivilegedAction *action,
        /* [out] */ IInterface **result);

    CARAPI DoPrivilegedWithCombiner(
        /* [in] */ IPrivilegedExceptionAction *action,
        /* [out] */ IInterface **result);

    CARAPI CheckPermission(
        /* [in] */ IPermission *permission);

    CARAPI GetContext(
        /* [out] */ IAccessControlContext **context);
};

}
}

#endif // __ELASTOS_SECURITY_CACCESSCONTROLLER_H__
