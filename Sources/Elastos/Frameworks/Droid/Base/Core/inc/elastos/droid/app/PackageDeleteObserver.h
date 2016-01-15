
#ifndef __ELASTOS_DROID_APP_PACKAGE_DELETE_OBSERVER_H__
#define __ELASTOS_DROID_APP_PACKAGE_DELETE_OBSERVER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IIPackageDeleteObserver2;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace App {

/** {@hide} */
class PackageDeleteObserver
    : public Object
    , public IPackageDeleteObserver
{
public:
    CARAPI constructor();

    CAR_INTERFACE_DECL()

    virtual ~PackageDeleteObserver() {}

    /** {@hide} */
    CARAPI GetBinder(
        /* [out] */ IIPackageDeleteObserver2** observer);

    CARAPI OnUserActionRequired(
        /* [in] */ IIntent* intent);

    CARAPI OnPackageDeleted(
        /* [in] */ const String& basePackageName,
        /* [in] */ Int32 returnCode,
        /* [in] */ const String& msg);

private:
    AutoPtr<IIPackageDeleteObserver2> mBinder;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_PACKAGE_DELETE_OBSERVER_H__

