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
