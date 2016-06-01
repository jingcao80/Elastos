#ifndef __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__
#define __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__

#include "_Elastos_Droid_Utility_CParcelableList.h"
#include <elastos/utility/ArrayList.h>

using Elastos::Utility::ArrayList;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Utility {

CarClass(CParcelableList)
    , public ArrayList
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    using ArrayList::constructor;

    CARAPI constructor(
        /* [in] */ IList* list);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI_(String) GetClassName() { return String("CParcelableList"); }
};

} // namespace Utility
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_UTILITY_CPARCELABLELIST_H__
