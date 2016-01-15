
#include "elastos/droid/internal/app/NavItemSelectedListener.h"

using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(NavItemSelectedListener, Object, IAdapterViewOnItemSelectedListener)

ECode  NavItemSelectedListener::constructor(
    /* [in] */ IActionBarOnNavigationListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode  NavItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (mListener != NULL) {
        Boolean res;
        mListener->OnNavigationItemSelected(position, id, &res);
    }
    return NOERROR;
}

ECode  NavItemSelectedListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // Do nothing
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
