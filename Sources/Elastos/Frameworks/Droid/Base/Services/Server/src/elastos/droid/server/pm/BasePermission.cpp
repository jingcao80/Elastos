
#include "elastos/droid/server/pm/BasePermission.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

const Int32 BasePermission::TYPE_NORMAL;
const Int32 BasePermission::TYPE_BUILTIN;
const Int32 BasePermission::TYPE_DYNAMIC;

BasePermission::BasePermission(
    /* [in] */ const String& name,
    /* [in] */ const String& sourcePackage,
    /* [in] */ Int32 type)
    : mName(name)
    , mSourcePackage(sourcePackage)
    , mType(type)
    // Default to most conservative protection level.
    , mProtectionLevel(IPermissionInfo::PROTECTION_SIGNATURE)
    , mUid(0)
{}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
