
#include "elastos/droid/server/pm/SharedUserSetting.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

// {34B3DE70-FA92-4C61-BA7E-756E654D081A}
extern const InterfaceID EIID_SharedUserSetting =
    { 0x34b3de70, 0xfa92, 0x4c61, { 0xba, 0x7e, 0x75, 0x6e, 0x65, 0x4d, 0x8, 0x1a } };

SharedUserSetting::SharedUserSetting(
    /* [in] */ const String& name,
    /* [in] */ Int32 pkgFlags)
    : GrantedPermissions(pkgFlags)
    , mName(name)
    , mUserId(0)
    , mUidFlags(0)
    , mPackages(10)
    , mSignatures(new PackageSignatures())
{}

PInterface SharedUserSetting::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_SharedUserSetting) {
        return reinterpret_cast<PInterface>((SharedUserSetting*)this);;
    }
    return GrantedPermissions::Probe(riid);
}

ECode SharedUserSetting::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int32 hashCode;
    sys->IdentityHashCode((IInterface*)this, &hashCode);
    *str = String("SharedUserSetting{") + StringUtils::ToHexString(hashCode) + " "
            + mName + "/" + mUserId + "}";
}

void SharedUserSetting::RemovePackage(
    /* [in]  */ PackageSetting* packageSetting)
{
    HashSet<AutoPtr<PackageSetting> >::Iterator it = mPackages.Find(packageSetting);
    if (it != mPackages.End()) {
        mPackages.Erase(it);
        // recalculate the pkgFlags for this shared user if needed
        if ((mPkgFlags & packageSetting->mPkgFlags) != 0) {
            Int32 aggregatedFlags = mUidFlags;
            it = mPackages.Begin();
            for (; it != mPackages.End(); ++it) {
                aggregatedFlags |= ps->mPkgFlags;
            }
            SetFlags(aggregatedFlags);
        }
    }
}

void SharedUserSetting::AddPackage(
    /* [in]  */ PackageSetting* packageSetting)
{
    if (mPackages.Find(packageSetting) == mPackages.End()) {
        mPackages.Insert(packageSetting);
        SetFlags(mPkgFlags | packageSetting->mPkgFlags);
    }
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
