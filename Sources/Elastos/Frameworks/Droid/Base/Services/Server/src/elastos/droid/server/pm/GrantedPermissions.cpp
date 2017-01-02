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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/pm/GrantedPermissions.h"

using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

GrantedPermissions::GrantedPermissions(
    /* [in] */ Int32 pkgFlags)
    : mGrantedPermissions(30)
{
    SetFlags(pkgFlags);
}

GrantedPermissions::GrantedPermissions(
    /* [in] */ const GrantedPermissions* base)
    : mPkgFlags(base->mPkgFlags)
    , mGrantedPermissions(base->mGrantedPermissions.Begin(),
                             base->mGrantedPermissions.End(), 30)
{
    if (base->mGids != NULL) {
        mGids = base->mGids->Clone();
    }
}

void GrantedPermissions::SetFlags(
    /* [in] */ Int32 pkgFlags)
{
    mPkgFlags = pkgFlags
            & (IApplicationInfo::FLAG_SYSTEM
                    | IApplicationInfo::FLAG_PRIVILEGED
                    | IApplicationInfo::FLAG_FORWARD_LOCK
                    | IApplicationInfo::FLAG_EXTERNAL_STORAGE);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
