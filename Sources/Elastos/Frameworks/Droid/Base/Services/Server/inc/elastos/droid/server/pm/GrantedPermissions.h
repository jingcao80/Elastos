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

#ifndef __ELASTOS_DROID_SERVER_PM_GRANTEDPERMISSIONS_H__
#define __ELASTOS_DROID_SERVER_PM_GRANTEDPERMISSIONS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class GrantedPermissions : public Object
{
public:
    GrantedPermissions(
        /* [in] */ Int32 pkgFlags);

    GrantedPermissions(
        /* [in] */ const GrantedPermissions* base);

    CARAPI_(void) SetFlags(
        /* [in] */ Int32 pkgFlags);

public:
    Int32 mPkgFlags;

    HashSet<String> mGrantedPermissions;

    AutoPtr<ArrayOf<Int32> > mGids;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_GRANTEDPERMISSIONS_H__
