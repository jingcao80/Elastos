
#include "Elastos.Droid.Content.h"
#include "elastos/droid/server/pm/GrantedPermissions.h"

using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {

GrantedPermissions::GrantedPermissions(
    /* [in] */ Int32 pkgFlags)
    : mGrantedPermissions(30)
{
    SetFlags(pkgFlags);
}

GrantedPermissions::GrantedPermissions(
    /* [in] */ const GrantedPermissions* base)
    : mPkgFlags(base->mPkgFlags)
    , mGrantedPermissions(base->mGrantedPermissions.Begin(),
                             base->mGrantedPermissions.End(), 30)
{
    if (base->mGids != NULL) {
        mGids = base->mGids->Clone();
    }
}

void GrantedPermissions::SetFlags(
    /* [in] */ Int32 pkgFlags)
{
    mPkgFlags = pkgFlags
            & (IApplicationInfo::FLAG_SYSTEM
                    | IApplicationInfo::FLAG_PRIVILEGED
                    | IApplicationInfo::FLAG_FORWARD_LOCK
                    | IApplicationInfo::FLAG_EXTERNAL_STORAGE);
}

} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
