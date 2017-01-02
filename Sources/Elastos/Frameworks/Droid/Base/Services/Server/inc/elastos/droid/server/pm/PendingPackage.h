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

#ifndef __ELASTOS_DROID_SERVER_PM_PENDINGPACKAGE_H__
#define __ELASTOS_DROID_SERVER_PM_PENDINGPACKAGE_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/pm/PackageSettingBase.h"

using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class PendingPackage : public PackageSettingBase
{
public:
    PendingPackage(
        /* [in] */ const String& name,
        /* [in] */ const String& realName,
        /* [in] */ IFile* codePath,
        /* [in] */ IFile* resourcePath,
        /* [in] */ const String& legacyNativeLibraryPathString,
        /* [in] */ const String& primaryCpuAbiString,
        /* [in] */ const String& secondaryCpuAbiString,
        /* [in] */ const String& cpuAbiOverrideString,
        /* [in] */ Int32 sharedId,
        /* [in] */ Int32 pVersionCode,
        /* [in] */ Int32 pkgFlags)
        : PackageSettingBase(name, realName, codePath, resourcePath,
                legacyNativeLibraryPathString, primaryCpuAbiString,
                secondaryCpuAbiString, cpuAbiOverrideString, pVersionCode, pkgFlags)
        , mSharedId(sharedId)
    {}

public:
    Int32 mSharedId;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_PM_PENDINGPACKAGE_H__
