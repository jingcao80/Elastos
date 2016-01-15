#ifndef __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__
#define __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__

#include "_Elastos_Droid_App_CFragmentManagerState.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentManagerState)
    , public Object
    , public IFragmentManagerState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFragmentManagerState();

    CARAPI constructor();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
public:
    AutoPtr< ArrayOf<IFragmentState*> > mActive;
    AutoPtr< ArrayOf<Int32> > mAdded;
    AutoPtr< ArrayOf<IBackStackState*> > mBackStack;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CFRAGMENTMANAGERSTATE_H__
