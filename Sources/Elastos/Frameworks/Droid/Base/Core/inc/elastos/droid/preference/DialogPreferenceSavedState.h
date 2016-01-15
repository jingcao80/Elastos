
#ifndef __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__
#define __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/preference/PreferenceBaseSavedState.h"

using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Preference {

 class DialogPreferenceSavedState
    : public PreferenceBaseSavedState
    , public IDialogPreferenceSavedState
{
public:
    CAR_INTERFACE_DECL()

    DialogPreferenceSavedState();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IParcelable* superState);

    CARAPI GetIsDialogShowing(
        /* [out] */ Boolean* isShow);

    CARAPI SetIsDialogShowing(
        /* [in] */ Boolean isShow);

    CARAPI GetDialogBundle(
        /* [out] */ IBundle** bundle);

    CARAPI SetDialogBundle(
        /* [in] */ IBundle* bundle);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetSuperState(
        /* [out] */ IParcelable** superState);

private:
    Boolean mIsDialogShowing;
    AutoPtr<IBundle> mDialogBundle;
};

}
}
}

#endif // __ELASTOS_DROID_PREFERENCE_DIALOGPREFERENCESAVEDSTATE_H__
