
#include "elastos/droid/incallui/CConferenceManagerFragment.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace InCallUI {

//================================================================
// CConferenceManagerFragment::EndButtonClickListener
//================================================================
CAR_INTERFACE_IMPL(CConferenceManagerFragment::EndButtonClickListener, Object, IViewOnClickListener);

ECode CConferenceManagerFragment::EndButtonClickListener::OnClick(
    /* [in] */ IView* v)
{
    getPresenter().endConferenceConnection(rowId);
}

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos
