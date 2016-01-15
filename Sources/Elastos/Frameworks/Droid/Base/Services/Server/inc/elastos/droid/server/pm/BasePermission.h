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
