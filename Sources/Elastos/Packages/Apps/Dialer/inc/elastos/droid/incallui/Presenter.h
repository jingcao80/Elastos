
#ifndef __ELASTOS_DROID_INCALLUI_PRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_PRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class Presenter
    : public Object
    , public IPresenter
{
public:
    CAR_INTERFACE_DECL()

    /**
     * Called after the UI view has been created.  That is when fragment.onViewCreated() is called.
     *
     * @param ui The Ui implementation that is now ready to be used.
     */
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    /**
     * Called when the UI view is destroyed in Fragment.onDestroyView().
     */
    CARAPI OnUiDestroy(
        /* [in] */ IUi* ui);

    /**
     * To be overriden by Presenter implementations.  Called when the fragment is being
     * destroyed but before ui is set to null.
     */
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    CARAPI GetUi(
        /* [out] */ IUi** ui);

protected:
    Presenter() {}

private:
    AutoPtr<IUi> mUi;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_PRESENTER_H__
