
#ifndef __ACTIONBARVIEWSAVEDSTATE_H__
#define __ACTIONBARVIEWSAVEDSTATE_H__


#include "elastos/droid/view/ViewBaseSavedState.h"

class ActionBarViewSavedState : public ViewBaseSavedState
{

public:
    ActionBarViewSavedState(
        /* [in] */ IParcelable* superState);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int32 flags);

private:
    ActionBarViewSavedState(
        /* [in] */ IParcel* in);

    Int32 expandedMenuItemId;

    Boolean isOverflowOpen;

    /*public static final Parcelable.Creator<SavedState> CREATOR =
        new Parcelable.Creator<SavedState>() {
            public SavedState createFromParcel(Parcel in) {
                return new SavedState(in);
            }

            public SavedState[] newArray(int size) {
                return new SavedState[size];
            }
    };*/
};

#endif
