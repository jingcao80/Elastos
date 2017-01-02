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

#ifndef __ELASTOS_DROID_SERVER_PM_SHAREDUSERSETTING_H__
#define __ELASTOS_DROID_SERVER_PM_SHAREDUSERSETTING_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/GrantedPermissions.h"
#include "elastos/droid/server/pm/PackageSignatures.h"
#include "elastos/droid/server/pm/PackageSetting.h"
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::HashSet;

// namespace Elastos {
// namespace Droid {
// namespace Server {
// namespace Pm {

// class PackageSetting;

// } // namespace Pm
// } // namespace Server
// } // namespace Droid
// } // namespace Elastos

// _ETL_NAMESPACE_BEGIN
// template<> struct Hash<Elastos::Droid::Server::Pm::PackageSetting *>
// {
//     size_t operator()(const Elastos::Droid::Server::Pm::PackageSetting * s) const { return (size_t)s; }
// };
// _ETL_NAMESPACE_END

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

extern const InterfaceID EIID_SharedUserSetting;

class SharedUserSetting : public GrantedPermissions
{
public:
    SharedUserSetting(
        /* [in] */ const String& name,
        /* [in] */ Int32 pkgFlags);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(void) RemovePackage(
        /* [in]  */ PackageSetting* packageSetting);

    CARAPI_(void) AddPackage(
        /* [in]  */ PackageSetting* packageSetting);

public:
    String mName;

    Int32 mUserId;

    // flags that are associated with this uid, regardless of any package flags
    Int32 mUidFlags;

    HashSet<AutoPtr<PackageSetting> > mPackages; // TODO using IWeakReference

    AutoPtr<PackageSignatures> mSignatures;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_SHAREDUSERSETTING_H__
