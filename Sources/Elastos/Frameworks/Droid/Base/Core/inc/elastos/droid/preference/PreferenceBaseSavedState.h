
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Preference.h"
#include "elastos/droid/view/AbsSavedState.h"

using Elastos::Droid::Preference::IPreferenceBaseSavedState;
using Elastos::Droid::View::AbsSavedState;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceBaseSavedState
    : public AbsSavedState
    , public IPreferenceBaseSavedState
{
public:
    CAR_INTERFACE_DECL()

    PreferenceBaseSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* source);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    virtual CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);
};

} // Preference
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_PREFERENCE_PREFERENCEBASESAVEDSTATE_H__
