
#ifndef __ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__
#define __ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__

#include "Elastos.Droid.Os.h"
#include "_Elastos_Droid_App_CFragmentSavedState.h"
#include <elastos/core/Object.h>

using Elastos::Core::IClassLoader;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CFragmentSavedState)
    , public Object
    , public IFragmentSavedState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBundle* state);

    CARAPI constructor(
        /* [in] */ IParcel* in,
        /* [in] */ IClassLoader* loader);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetState(
        /* [out] */ IBundle** state);

    CARAPI SetState(
        /* [in] */ IBundle* state);

public:
    AutoPtr<IBundle> mState;
    AutoPtr<IClassLoader> mLoader;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CFRAGMENTSAVEDSTATE_H__
