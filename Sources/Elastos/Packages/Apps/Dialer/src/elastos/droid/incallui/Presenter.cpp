
#include "elastos/droid/incallui/Presenter.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

CAR_INTERFACE_IMPL(Presenter, Object, IPresenter);

ECode Presenter::OnUiReady(
    /* [in] */ IUi* ui)
{
    mUi = ui;
    return NOERROR;
}

ECode Presenter::OnUiDestroy(
    /* [in] */ IUi* ui)
{
    FAIL_RETURN(OnUiUnready(ui));
    mUi = NULL;
    return NOERROR;
}

ECode Presenter::OnUiUnready(
    /* [in] */ IUi* ui)
{
    return NOERROR;
}

ECode Presenter::GetUi(
    /* [out] */ IUi** ui)
{
    VALIDATE_NOT_NULL(ui);
    *ui = mUi;
    REFCOUNT_ADD(*ui);
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
