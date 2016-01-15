
#ifndef __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFODISPLAYNAMECOMPARATOR_H__
#define __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFODISPLAYNAMECOMPARATOR_H__

#include "_Elastos_Droid_Content_Pm_CPackageItemInfoDisplayNameComparator.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;
using Elastos::Text::ICollator;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CPackageItemInfoDisplayNameComparator)
    , public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IPackageManager* pm);

    CARAPI Compare(
        /* [in] */ IInterface* aa,
        /* [in] */ IInterface* ab,
        /* [out] */ Int32* result);

private:
    static AutoPtr<ICollator> sCollator;
    AutoPtr<IPackageManager> mPM;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CPACKAGEITEMINFODISPLAYNAMECOMPARATOR_H__
