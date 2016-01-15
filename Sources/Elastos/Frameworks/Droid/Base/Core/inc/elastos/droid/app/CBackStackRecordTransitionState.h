#ifndef __ELASTOS_DROID_APP_CBACKSTACKRECORD_TRANSITIONSTATE_H__
#define __ELASTOS_DROID_APP_CBACKSTACKRECORD_TRANSITIONSTATE_H__

#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_App_CBackStackRecordTransitionState.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Utility::IArrayMap;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CBackStackRecordTransitionState)
    , public Object
    , public IBackStackRecordTransitionState
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBackStackRecordTransitionState();

    CARAPI GetNameOverrides(
        /* [out] */ IArrayMap** map);

    CARAPI SetNameOverrides(
        /* [in] */ IArrayMap* map);

    CARAPI GetEnteringEpicenterView(
        /* [out] */ IView** result);

    CARAPI SetEnteringEpicenterView(
        /* [in] */ IView* result);

    CARAPI GetNonExistentView(
        /* [out] */ IView** result);

    CARAPI SetNonExistentView(
        /* [in] */ IView* result);

public:
    AutoPtr<IArrayMap> mNameOverrides;// = new ArrayMap<String, String>();
    AutoPtr<IView> mEnteringEpicenterView;
    AutoPtr<IView> mNonExistentView;
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif
