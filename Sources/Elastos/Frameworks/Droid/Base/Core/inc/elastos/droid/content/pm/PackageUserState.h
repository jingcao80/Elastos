
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEUSERSTATE_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEUSERSTATE_H__

#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::HashSet;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Per-user state information about a package.
 * @hide
 */
class ECO_PUBLIC PackageUserState
    : public Object
{
public:
    PackageUserState();

    PackageUserState(
        /* [in] */ PackageUserState* o);

    ~PackageUserState();

public:
    Boolean mStopped;
    Boolean mNotLaunched;
    Boolean mInstalled;
    Boolean mHidden; // Is the app restricted by owner / admin
    Int32 mEnabled;
    Boolean mBlockUninstall;

    String mLastDisableAppCaller;
    AutoPtr<HashSet<String> > mDisabledComponents;
    AutoPtr<HashSet<String> > mEnabledComponents;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGEUSERSTATE_H__
