
#include "elastos/droid/server/pm/PackageSetting.h"
#include "elastos/droid/server/pm/SharedUserSetting.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
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
        return reinterpret_cast<PInterface>((PackageSetting*)this);;
    }
    return PackageSettingBase::Probe(riid);
}

ECode PackageSetting::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 hashCode;
    sys->IdentityHashCode((IInterface*)(IObject*)this, &hashCode);
    *str = String("PackageSetting{") + StringUtils::ToHexString(hashCode) + " "
            + mName + "/" + StringUtils::ToString(mAppId) + "}";
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
