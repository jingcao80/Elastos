
#ifndef __ELASTOS_DROID_WIDGET_INTERNAL_TRANSPORTCONTROLVIEWSAVEDSTATE_H__
#define __ELASTOS_DROID_WIDGET_INTERNAL_TRANSPORTCONTROLVIEWSAVEDSTATE_H__

#include "elastos/droid/view/ViewBaseSavedState.h"

using Elastos::Droid::View::ViewBaseSavedState;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

class TransportControlViewSavedState : public ViewBaseSavedState
{
public:

    TransportControlViewSavedState();

    TransportControlViewSavedState(
        /* [in] */ IParcelable* superState);

    virtual CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    virtual CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

protected:
    virtual CARAPI Init(
        /* [in] */ IParcelable* superState);

    virtual CARAPI Init();

    Boolean wasShowing;
};

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
#endif
