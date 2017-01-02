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

#ifndef __ELASTOS_DROID_SERVER_PM_BASEPERMISSION_H__
#define __ELASTOS_DROID_SERVER_PM_BASEPERMISSION_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/PackageSettingBase.h"
#include "elastos/droid/content/pm/PackageParser.h"

using Elastos::Droid::Content::Pm::PackageParser;
using Elastos::Droid::Content::Pm::IPermissionInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class BasePermission : public Object
{
public:
    BasePermission(
        /* [in] */ const String& name,
        /* [in] */ const String& sourcePackage,
        /* [in] */ Int32 type);

    CARAPI ToString(
        /* [out] */ String* str);
public:
    static const Int32 TYPE_NORMAL = 0;

    static const Int32 TYPE_BUILTIN = 1;

    static const Int32 TYPE_DYNAMIC = 2;

    String mName;

    String mSourcePackage;

    AutoPtr<PackageSettingBase> mPackageSetting;

    Int32 mType;

    Int32 mProtectionLevel;

    AutoPtr<PackageParser::Permission> mPerm;

    AutoPtr<IPermissionInfo> mPendingInfo;

    Int32 mUid;

    AutoPtr< ArrayOf<Int32> > mGids;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_BASEPERMISSION_H__
