
#ifndef __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFODISPLAYNAMECOMPARATOR_H__
#define __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFODISPLAYNAMECOMPARATOR_H__

#include "_Elastos_Droid_Content_Pm_CApplicationInfoDisplayNameComparator.h"
#include <elastos/core/Object.h>

using Elastos::Core::IComparator;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CarClass(CApplicationInfoDisplayNameComparator)
    , public Object
    , public IComparator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IPackageManager * pm);

    CARAPI Compare(
        /* [in] */  IInterface* a,
        /* [in] */  IInterface* b,
        /* [out] */ Int32* result);

private:
    // private final Collator   sCollator = Collator.getInstance();
    AutoPtr<IPackageManager> mPM;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_CAPPLICATIONINFODISPLAYNAMECOMPARATOR_H__
