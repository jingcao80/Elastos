
#include "elastos/droid/incallui/BaseFragment.h"

namespace Elastos {
namespace Droid {
namespace InCallUI {

ECode BaseFragment::constructor()
{
    mPresenter = CreatePresenter();
    return NOERROR;
}

AutoPtr<IPresenter> BaseFragment::GetPresenter()
{
    return mPresenter;
}

ECode BaseFragment::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    FAIL_RETURN(Fragment::OnActivityCreated(savedInstanceState));
    AutoPtr<IUi> ui = GetUi();
    mPresenter->OnUiReady(ui);
    return NOERROR;
}

ECode BaseFragment::OnDestroyView()
{
    FAIL_RETURN(Fragment::OnDestroyView());
    AutoPtr<IUi> ui = GetUi();
    mPresenter->OnUiDestroy(ui);
    return NOERROR;
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
