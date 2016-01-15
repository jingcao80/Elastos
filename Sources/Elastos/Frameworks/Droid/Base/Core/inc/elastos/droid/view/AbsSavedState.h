
#ifndef __ELASTOS_DROID_VIEW_ABSSAVEDSTATE_H__
#define __ELASTOS_DROID_VIEW_ABSSAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace View {

class ECO_PUBLIC AbsSavedState
    : public Object
    , public IAbsSavedState
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    AbsSavedState();

    virtual ~AbsSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** p);

public:
    CARAPI_(AutoPtr<IParcelable>) GetSuperState();

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

public:
    static const AutoPtr<IAbsSavedState> EMPTY_STATE;

private:
    AutoPtr<IParcelable> mSuperState;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_VIEW_ABSSAVEDSTATE_H__
