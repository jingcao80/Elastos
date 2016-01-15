
#ifndef __ELASTOS_DROID_PREFERENCE_PREFERENCESCREENSAVEDSTATE_H__
#define  __ELASTOS_DROID_PREFERENCE_PREFERENCESCREENSAVEDSTATE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Preference {

class PreferenceScreenSavedState
    : public PreferenceBaseSavedState
    , public IPreferenceScreenSavedState
{
public:
    CAR_INTERFACE_DECL()

    PreferenceScreenSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* source);

    CARAPI IsDialogShowing(
        /* [out] */ Boolean* isDialogShowing);

    CARAPI SetDialogShowing(
        /* [in] */ Boolean isDialogShowing);

    CARAPI SetDialogBundle(
        /* [in] */ IBundle* b);

    CARAPI GetDialogBundle(
        /* [out] */ IBundle** b);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    Boolean mIsDialogShowing;
    AutoPtr<IBundle> mDialogBundle;
};

} // Preference
} // Droid
} // Elastos

#endif  // __ELASTOS_DROID_PREFERENCE_PREFERENCESCREENSAVEDSTATE_H__
