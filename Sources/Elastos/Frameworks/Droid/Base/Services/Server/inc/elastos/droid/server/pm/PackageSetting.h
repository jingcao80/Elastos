#ifndef __ELASTOS_DROID_SERVER_PM_PACKAGESETTING_H__
#define __ELASTOS_DROID_SERVER_PM_PACKAGESETTING_H__

#include "elastos/droid/server/pm/PackageSettingBase.h"
#include "elastos/droid/content/pm/PackageParser.h"

using Elastos::Droid::Content::Pm::PackageParser;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

class SharedUserSetting;

extern const InterfaceID EIID_PackageSetting;

class PackageSetting : public PackageSettingBase
{
public:
    PackageSetting(
        /* [in] */ const String& name,
        /* [in] */ const String& realName,
        /* [in] */ IFile* codePath,
        /* [in] */ IFile* resourcePath,
        /* [in] */ const String& legacyNativeLibraryPathString,
        /* [in] */ const String& primaryCpuAbiString,
        /* [in] */ const String& secondaryCpuAbiString,
        /* [in] */ const String& cpuAbiOverrideString,
        /* [in] */ Int32 pVersionCode,
        /* [in] */ Int32 pkgFlags);

    PackageSetting(
        /* [in] */ const PackageSetting* orig);

    CARAPI_(PInterface) Probe(
        /* [in]  */ REIID riid);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI_(AutoPtr<ArrayOf<Int32> >) GetGids();

    CARAPI_(Boolean) IsPrivileged();

public:
    Int32 mAppId;
    AutoPtr<PackageParser::Package> mPkg;
    AutoPtr<SharedUserSetting> mSharedUser;
};

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Server::Pm::PackageSetting)

#endif //__ELASTOS_DROID_SERVER_PM_PACKAGESETTING_H__
