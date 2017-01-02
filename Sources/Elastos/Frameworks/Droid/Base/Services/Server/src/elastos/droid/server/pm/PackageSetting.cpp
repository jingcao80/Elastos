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

#include "elastos/droid/server/pm/PackageSetting.h"
#include "elastos/droid/server/pm/SharedUserSetting.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

// {93602C16-4192-4A01-9355-56A477D9EFE8}
extern const InterfaceID EIID_PackageSetting =
    { 0x93602c16, 0x4192, 0x4a01, { 0x93, 0x55, 0x56, 0xa4, 0x77, 0xd9, 0xef, 0xe8 } };

PackageSetting::PackageSetting(
    /* [in] */ const String& name,
    /* [in] */ const String& realName,
    /* [in] */ IFile* codePath,
    /* [in] */ IFile* resourcePath,
    /* [in] */ const String& legacyNativeLibraryPathString,
    /* [in] */ const String& primaryCpuAbiString,
    /* [in] */ const String& secondaryCpuAbiString,
    /* [in] */ const String& cpuAbiOverrideString,
    /* [in] */ Int32 pVersionCode,
    /* [in] */ Int32 pkgFlags)
    : PackageSettingBase(name, realName, codePath, resourcePath,
            legacyNativeLibraryPathString, primaryCpuAbiString,
            secondaryCpuAbiString ,cpuAbiOverrideString, pVersionCode, pkgFlags)
    , mAppId(0)
{}

PackageSetting::PackageSetting(
    /* [in] */ const PackageSetting* orig)
    : PackageSettingBase(orig)
    , mAppId(orig->mAppId)
    , mPkg(orig->mPkg)
    , mSharedUser(orig->mSharedUser)
{}

PInterface PackageSetting::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_PackageSetting) {
        return reinterpret_cast<PInterface>(this);
    }
    return PackageSettingBase::Probe(riid);
}

ECode PackageSetting::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("PackageSetting{");
    sb += StringUtils::ToHexString((Int32)this);
    sb += " ";
    sb += mName;
    sb += "/";
    sb += mAppId;
    sb += "}";
    *str = sb.ToString();
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > PackageSetting::GetGids()
{
    return mSharedUser != NULL ? mSharedUser->mGids : mGids;
}

Boolean PackageSetting::IsPrivileged()
{
    return (mPkgFlags & IApplicationInfo::FLAG_PRIVILEGED) != 0;
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
