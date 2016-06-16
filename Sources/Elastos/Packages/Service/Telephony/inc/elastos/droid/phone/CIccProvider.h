#ifndef  __ELASTOS_DROID_PHONE_CICCPROVIDER_H__
#define  __ELASTOS_DROID_PHONE_CICCPROVIDER_H__

#include "_Elastos_Droid_Phone_CIccProvider.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * ICC address book content provider.
 */
CarClass(CIccProvider)
    , public IccProvider
    , public IIccProvider
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor();
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CICCPROVIDER_H__